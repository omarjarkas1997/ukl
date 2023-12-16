#include "http_client.h"
#include "../liberun/error.h"


#include <openssl/ssl.h>
#include <openssl/err.h>

int ReadHttpStatus(int sock)
{
    char buff[1024] = "", *ptr = buff + 1;
    int bytes_received;
    printf("Begin Response ..\n");
    while ((bytes_received = recv(sock, ptr, 1, 0))) // Added parentheses
    {
        if (bytes_received == -1)
        {
            perror("ReadHttpStatus");
            exit(1);
        }

        if ((ptr[-1] == '\r') && (*ptr == '\n'))
            break;
        ptr++;
    }
    *ptr = 0;
    ptr = buff + 1;

    int status; // Moved declaration here
    sscanf(ptr, "%*s %d ", &status);

    printf("%s\n", ptr);
    printf("status=%d\n", status);
    printf("End Response ..\n");
    return (bytes_received > 0) ? status : 0;
}

int ParseHeader_HTTP(int sock)
{
    char buff[1024] = "", *ptr = buff + 4;
    int bytes_received;
    printf("Begin HEADER ..\n");
    while ((bytes_received = recv(sock, ptr, 1, 0))) // Added parentheses
    {
        if (bytes_received == -1)
        {
            perror("Parse Header");
            exit(1);
        }

        if (
            (ptr[-3] == '\r') && (ptr[-2] == '\n') &&
            (ptr[-1] == '\r') && (*ptr == '\n'))
            break;
        ptr++;
    }

    *ptr = 0;
    ptr = buff + 4;

    if (bytes_received)
    {
        ptr = strstr(ptr, "Content-Length:");
        if (ptr)
        {
            sscanf(ptr, "%*s %d", &bytes_received);
        }
        else
            bytes_received = -1; // unknown size

        printf("Content-Length: %d\n", bytes_received);
    }
    printf("End HEADER ..\n");
    return bytes_received;
}

int ParseHeader_HTTPs(SSL *ssl)
{
    char buff[1024] = "", *ptr = buff + 4;
    int bytes_received;
    printf("Begin HEADER ..\n");
    while ((bytes_received = SSL_read(ssl, ptr, 1)) > 0)
    { // Use SSL_read
        if (
            (ptr[-3] == '\r') && (ptr[-2] == '\n') &&
            (ptr[-1] == '\r') && (*ptr == '\n'))
            break;
        ptr++;
    }

    *ptr = 0;
    ptr = buff + 4;

    if (bytes_received)
    {
        ptr = strstr(ptr, "Content-Length:");
        if (ptr)
        {
            sscanf(ptr, "%*s %d", &bytes_received);
        }
        else
        {
            bytes_received = -1; // Unknown size
        }

        printf("Content-Length: %d\n", bytes_received);
    }
    printf("End HEADER ..\n");
    return bytes_received;
}

struct hostent *resolve_hostname(const char *domain)
{
    struct hostent *he = gethostbyname(domain);
    if (he == NULL)
    {
        liberun_fail_with_error(errno,"Error in gethostbyname");
    }
    return he;
}

int create_socket()
{
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == -1)
    {
        liberun_fail_with_error(errno,"Error in Socket");
    }
    return sock;
}

void connect_to_server(int sock, struct hostent *he, int port)
{
    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    server_addr.sin_addr = *((struct in_addr *)he->h_addr);
    bzero(&(server_addr.sin_zero), 8);

    if (connect(sock, (struct sockaddr *)&server_addr, sizeof(struct sockaddr)) == -1)
    {
        liberun_fail_with_error(errno,"Error in Connect");
    }
}

int receive_response(int sock, const char *destination, SSL *ssl, int use_ssl)
{
    char recv_data[1024];
    int bytes_received;
    int content_length;

    if (use_ssl)
    {
        content_length = ParseHeader_HTTPs(ssl);
    }
    else
    {
        content_length = ParseHeader_HTTP(sock);
    }

    if (content_length < 0)
    {
        liberun_fail_with_error(errno, "Failed to parse HTTP header\n");
        return 1; // Added return to exit the function in case of failure
    }
    FILE *fd = fopen(destination, "wb");
    if (!fd)
    {
        liberun_fail_with_error(errno, "Error opening file"); // Using perror for more detailed error message
        return 1;                     // Exit if file cannot be opened
    }

    printf("Receiving response: Destination file [%s], Expected Content-Length: %d\n", destination, content_length);
    int bytes = 0;
    while (use_ssl ? ((bytes_received = SSL_read(ssl, recv_data, sizeof(recv_data))) > 0) : ((bytes_received = recv(sock, recv_data, sizeof(recv_data), 0)) > 0))
    {
        size_t written = fwrite(recv_data, 1, bytes_received, fd);
        if (written < bytes_received)
        {
            liberun_fail_with_error(errno, "Failed to write all received bytes to file\n");
            break; // Exit the loop if not all bytes are written
        }
        bytes += bytes_received;
        printf("Received %d bytes (Total: %d/%d)\n", bytes_received, bytes, content_length);
        if (bytes >= content_length)
            break;
    }
    if (use_ssl && bytes_received < 0)
    {
        liberun_fail_with_error(errno, "SSL read error\n");
        return 1;
    }
    else if (!use_ssl && bytes_received < 0)
    {
        liberun_fail_with_error(errno, "Socket read error"); // Print socket error messages
        return 1;
    }

    printf("Response fully received. Total bytes received: %d\n", bytes);
    fclose(fd);
    return 0;
}

int initialize_ssl_library(void)
{
    SSL_library_init();
    OpenSSL_add_all_algorithms();
    SSL_load_error_strings();
    return 0;
}

int initialize_ssl_connection(int sock, SSL_CTX **ctx, SSL **ssl)
{
    const SSL_METHOD *method = TLS_client_method();
    *ctx = SSL_CTX_new(method);
    if (!(*ctx))
    {
        liberun_fail_with_error(errno, "Unable to create SSL context");
        return -1;
    }
    *ssl = SSL_new(*ctx);
    if (!(*ssl))
    {
        liberun_fail_with_error(errno, "Unable to create SSL structure");
        SSL_CTX_free(*ctx);
        return -1;
    }
    SSL_set_fd(*ssl, sock);
    if (SSL_connect(*ssl) != 1)
    {
        liberun_fail_with_error(errno, "SSL_connect failed");
        SSL_free(*ssl);
        SSL_CTX_free(*ctx);
        return -1;
    }
    return 0;
}

int ssl_write(SSL *ssl, const char *data)
{
    int bytes;
    if (!ssl)
    {
        liberun_fail_with_error(errno, "SSL pointer is null");
        return -1;
    }
    bytes = SSL_write(ssl, data, strlen(data));
    if (bytes <= 0)
    {
        perror("SSL_write failed");
        ERR_print_errors_fp(stderr);
        int ssl_error = SSL_get_error(ssl, bytes);
        switch (ssl_error)
        {
        case SSL_ERROR_ZERO_RETURN:
            liberun_fail_with_error(errno, "SSL_write: The connection was closed.\n");
            break;
        case SSL_ERROR_WANT_READ:
        case SSL_ERROR_WANT_WRITE:
            liberun_fail_with_error(errno, "SSL_write: The operation did not complete; more data needed.\n");
            break;
        default:
            liberun_fail_with_error(errno, "SSL_write: A serious error occurred.\n");
            SSL_shutdown(ssl);
            SSL_free(ssl);
            break;
        }
        return -2;
    }
    printf("Successfully sent %d bytes over SSL.\n", bytes);
    return 0;
}

int send_request(int sock, const char *domain, const char *path, SSL_CTX *ctx, SSL *ssl, int use_ssl)
{
    char send_data[1024];
    snprintf(send_data, sizeof(send_data), "GET /%s HTTP/1.1\r\nHost: %s\r\n\r\n", path, domain);
    printf("Preparing to send request:\n%s\n", send_data);
    if (use_ssl)
    {
        printf("Sending request over SSL...\n");
        int request = ssl_write(ssl, send_data);
        if (request != 0)
        {
            liberun_fail_with_error(errno, "SSL_write: A serious error occurred.\n");
            return 1;
        }
    }
    else
    {
        printf("Sending request over plain socket...\n");
        ssize_t bytes_sent = send(sock, send_data, strlen(send_data), 0);
        if (bytes_sent == -1)
        {
            liberun_fail_with_error(errno, "Failed to send request. Error code: %d\n", errno);
            return 1;
        }
        else
        {
            printf("Sent %zd bytes to %s\n", bytes_sent, domain);
        }
    }
    return 0;
}

int curl(char *domain, char *path, int port, const char *destination, int use_ssl)
{
    
    SSL_CTX *ctx = NULL; 
    SSL *ssl = NULL;
    int ret = 0;

    printf("Starting curl function\n");
    printf("Parameters: domain=%s, path=%s, port=%d, destination=%s, use_ssl=%d\n", domain, path, port, destination, use_ssl);

    printf("Resolving hostname: %s\n", domain);
    // Resolve hostname
    struct hostent *he = resolve_hostname(domain); 
    if (!he) {
        liberun_fail_with_error(errno, "Hostname resolution failed for %s\n", domain);
        return -1;
    }

    printf("Creating socket...\n");
    int sock = create_socket();
    if (sock < 0) {
        liberun_fail_with_error(errno, "Socket creation failed\n");
        return -1;
    }
    printf("Connecting to server: %s:%d\n", domain, port);
    connect_to_server(sock, he, port);
    if (use_ssl) {
        printf("Initializing SSL library...\n");
        initialize_ssl_library();
        printf("Initializing SSL connection...\n");
        ret = initialize_ssl_connection(sock, &ctx, &ssl);
        if (ret != 0)
        {
            liberun_fail_with_error(errno, "SSL connection initialization failed with code: %d\n", ret);
            close(sock);
            return ret;
        }
    }

    printf("Sending request to %s%s\n", domain, path);
    send_request(sock, domain, path, ctx, ssl, use_ssl);
    printf("Receiving response...\n");
    receive_response(sock, destination, ssl, use_ssl);
    printf("Closing socket\n");
    close(sock);
    printf("curl function completed with return code: %d\n", ret);
    return ret;
}