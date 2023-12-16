#define _GNU_SOURCE
#include <stdio.h>
#include <argp.h>
#include <stdlib.h>  // Include this header for EXIT_SUCCESS
#include <unistd.h>  // Include this for pause()
#include "liberun/error.h"
#include "liberun/utils.h"
#include "erun.h"
#include "container.h"
#include "create.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <errno.h>
#include <arpa/inet.h>

#include <string.h>
#include <openssl/ssl.h>
#include <openssl/err.h>
#include "http/http_client.h"




// int ReadHttpStatus(int sock)
// {
//   char buff[1024] = "", *ptr = buff + 1;
//   int bytes_received;
//   printf("Begin Response ..\n");
//   while ((bytes_received = recv(sock, ptr, 1, 0))) // Added parentheses
//   {
//     if (bytes_received == -1)
//     {
//       perror("ReadHttpStatus");
//       exit(1);
//     }

//     if ((ptr[-1] == '\r') && (*ptr == '\n'))
//       break;
//     ptr++;
//   }
//   *ptr = 0;
//   ptr = buff + 1;

//   int status; // Moved declaration here
//   sscanf(ptr, "%*s %d ", &status);

//   printf("%s\n", ptr);
//   printf("status=%d\n", status);
//   printf("End Response ..\n");
//   return (bytes_received > 0) ? status : 0;
// }

// int ParseHeader_HTTP(int sock) {
//   char buff[1024] = "", *ptr = buff + 4;
//   int bytes_received;
//   printf("Begin HEADER ..\n");
//   while ((bytes_received = recv(sock, ptr, 1, 0))) // Added parentheses
//   {
//     if (bytes_received == -1)
//     {
//       perror("Parse Header");
//       exit(1);
//     }

//     if (
//         (ptr[-3] == '\r') && (ptr[-2] == '\n') &&
//         (ptr[-1] == '\r') && (*ptr == '\n'))
//       break;
//     ptr++;
//   }

//   *ptr = 0;
//   ptr = buff + 4;

//   if (bytes_received)
//   {
//     ptr = strstr(ptr, "Content-Length:");
//     if (ptr)
//     {
//       sscanf(ptr, "%*s %d", &bytes_received);
//     }
//     else
//       bytes_received = -1; // unknown size

//     printf("Content-Length: %d\n", bytes_received);
//   }
//   printf("End HEADER ..\n");
//   return bytes_received;
// }


// int ParseHeader_HTTPs(SSL *ssl) {
//     char buff[1024] = "", *ptr = buff + 4;
//     int bytes_received;
//     printf("Begin HEADER ..\n");
//     while ((bytes_received = SSL_read(ssl, ptr, 1)) > 0) { // Use SSL_read
//         if (
//             (ptr[-3] == '\r') && (ptr[-2] == '\n') &&
//             (ptr[-1] == '\r') && (*ptr == '\n')
//         ) break;
//         ptr++;
//     }

//     *ptr = 0;
//     ptr = buff + 4;

//     if (bytes_received) {
//         ptr = strstr(ptr, "Content-Length:");
//         if (ptr) {
//             sscanf(ptr, "%*s %d", &bytes_received);
//         } else {
//             bytes_received = -1; // Unknown size
//         }

//         printf("Content-Length: %d\n", bytes_received);
//     }
//     printf("End HEADER ..\n");
//     return bytes_received;
// }

// void handle_error(const char *message, int error_code)
// {
//   perror(message);
//   exit(error_code);
// }

// struct hostent *resolve_hostname(const char *domain)
// {
//   struct hostent *he = gethostbyname(domain);
//   if (he == NULL)
//   {
//     handle_error("Error in gethostbyname", 1);
//   }
//   return he;
// }

// int create_socket()
// {
//   int sock = socket(AF_INET, SOCK_STREAM, 0);
//   if (sock == -1)
//   {
//     handle_error("Error in Socket", 2);
//   }
//   return sock;
// }

// void connect_to_server(int sock, struct hostent *he, int port)
// {
//   struct sockaddr_in server_addr;
//   server_addr.sin_family = AF_INET;
//   server_addr.sin_port = htons(port);
//   server_addr.sin_addr = *((struct in_addr *)he->h_addr);
//   bzero(&(server_addr.sin_zero), 8);

//   if (connect(sock, (struct sockaddr *)&server_addr, sizeof(struct sockaddr)) == -1)
//   {
//     handle_error("Error in Connect", 3);
//   }
// }

// void receive_response(int sock, const char *destination, SSL *ssl, int use_ssl)
// {
//   char recv_data[1024];
//   int bytes_received;
//   int content_length;

//   if (use_ssl) {
//     content_length = ParseHeader_HTTPs(ssl);
//   } else {
//     content_length = ParseHeader_HTTP(sock);
//   }

//   FILE *fd = fopen(destination, "wb");
//   if (!fd) {
//     handle_error("Error opening file", 5);
//   }

//   printf("Receiving response: Destination file [%s], Expected Content-Length: %d\n", destination, content_length);

//   int bytes = 0;
//   while (use_ssl ? ((bytes_received = SSL_read(ssl, recv_data, sizeof(recv_data))) > 0) :
//                    ((bytes_received = recv(sock, recv_data, sizeof(recv_data), 0)) > 0)) {
//     fwrite(recv_data, 1, bytes_received, fd);
//     bytes += bytes_received;
//     printf("Received %d bytes (Total: %d/%d)\n", bytes_received, bytes, content_length);
//     if (bytes >= content_length) break;
//   }

//   printf("Response fully received. Total bytes received: %d\n", bytes);
//   fclose(fd);
// }

// int initialize_ssl_library(void)
// {
//   SSL_library_init();
//   OpenSSL_add_all_algorithms();
//   SSL_load_error_strings();
//   return 0;
// }

// int initialize_ssl_connection(int sock, SSL_CTX **ctx, SSL **ssl)
// {
//     const SSL_METHOD *method = TLS_client_method();
//     *ctx = SSL_CTX_new(method);
//     if (!(*ctx)) {
//         perror("Unable to create SSL context");
//         ERR_print_errors_fp(stderr);
//         return -1; // Indicate error
//     }

//     *ssl = SSL_new(*ctx);
//     if (!(*ssl)) {
//         perror("Unable to create SSL structure");
//         ERR_print_errors_fp(stderr);
//         SSL_CTX_free(*ctx);
//         return -1; // Indicate error
//     }

//     SSL_set_fd(*ssl, sock);

//     if (SSL_connect(*ssl) != 1) {
//         perror("SSL_connect failed");
//         ERR_print_errors_fp(stderr);
//         SSL_free(*ssl);
//         SSL_CTX_free(*ctx);
//         return -1; // Indicate error
//     }

//     return 0; // Success
// }

// void ssl_write(SSL *ssl, const char *data)
// {
//     if (!ssl) {
//         perror("SSL pointer is null");
//         exit(EXIT_FAILURE);
//     }

//     if (SSL_write(ssl, data, strlen(data)) <= 0) {
//         perror("SSL_write failed");
//         ERR_print_errors_fp(stderr); // Print SSL error messages
//         SSL_shutdown(ssl);
//         SSL_free(ssl); // Free the SSL object
//         // Note: Do not free the SSL context here as it might be used by other connections
//         exit(EXIT_FAILURE);
//     }
// }



// void send_request(int sock, const char *domain, const char *path, SSL_CTX *ctx, SSL *ssl, int use_ssl)
// {
//   char send_data[1024];
//   snprintf(send_data, sizeof(send_data), "GET /%s HTTP/1.1\r\nHost: %s\r\n\r\n", path, domain);
//   if (use_ssl) {
//     ssl_write(ssl, send_data);
//   } else if (send(sock, send_data, strlen(send_data), 0) == -1)
//   {
//     handle_error("Error in Send", 4);
//   }
// }


// int curl(char *domain, char *path, int port, const char *destination, int use_ssl)
// {
//   SSL_CTX *ctx = NULL;
//   SSL *ssl = NULL;
//   int ret = 0;

//   struct hostent *he = resolve_hostname(domain);
//   int sock = create_socket();
//   connect_to_server(sock, he, port);
//   if (use_ssl)
//   {
//     initialize_ssl_library();
//     ret = initialize_ssl_connection(sock, &ctx, &ssl);
    
//   }

//   send_request(sock, domain, path, ctx, ssl, use_ssl);
//   receive_response(sock, destination, ssl, use_ssl);


//   close(sock);
//   printf("Program finished successfully\n");

//   return ret;
// }



static struct erun_global_arguments arguments;

int init_libcrun_context(liberun_context_t *con, const char *id, struct erun_global_arguments *glob, liberun_error_t *err)
{
    int ret;

    con->id = id;
    con->argc = glob->argc;
    con->argv = glob->argv;

    if (con->bundle == NULL)
        con->bundle = ".";

    return 0;
}

enum
{
    COMMAND_CREATE = 1000,
};

struct commands_s
{
    int value;
    const char *name;
    int (*handler)(struct erun_global_arguments *, int, char **, liberun_error_t *);
};

struct commands_s commands[] = {{COMMAND_CREATE, "create", erun_command_create},
                                {
                                    0,
                                }};

static char doc[] = "\nCOMMANDS:\n"
                    "\tcreate      - create a container\n";
static char args_doc[] = "COMMAND [OPTION...]";



static struct commands_s *get_command(const char *arg) {
    struct commands_s *it;
    for (it = commands; it->value; it++)
        if (strcmp(it->name, arg) == 0)
            return it;
    return NULL;
}

static struct argp_option options[] = {{
    0,
}};

static error_t parse_opt(int key, char *arg, struct argp_state *state)
{
    switch (key)
    {
    default:
        return ARGP_ERR_UNKNOWN;
    }
}

static struct commands_s *command;

char *argp_mandatory_argument(char *arg, struct argp_state *state)
{
    if (arg)
        return arg;
    return state->argv[state->next++];
}

static struct argp argp = {options, parse_opt, args_doc, doc, NULL, NULL, NULL};

int execute_command(char **argv, int argc) {
    int ret, first_argument = 0;
    liberun_error_t err = NULL;
    arguments.argc = argc;
    arguments.argv = argv;

    struct commands_s *command = get_command(argv[first_argument]);
    if (command == NULL) {
        liberun_fail_with_error(0, "unknown command %s", argv[first_argument]);
        return -1;
    }
    ret = command->handler(&arguments, argc - first_argument, argv + first_argument, &err);
    if (ret && err) {
        liberun_fail_with_error(err->status, "%s", err->msg);
        return -1;
    }
    return 0;
}

int main() {
  printf("Curling\n");
  int result;
  result = system("tar -czf file.tar.gz file.txt");
    if (result == 0) {
        printf("Compression successful.\n");
    } else {
        // Print the error message
        printf("An error occurred during compression: %s\n", strerror(errno));
    }



  char *domain = "10.35.15.175";
  char *path = "working-with-image-spec/images/buldah_image/manifest.json";
  int port = 443;
  const char *destination = "config.json";
  int use_ssl = 1; // Set to 0 for non-SSL connection

  return curl(domain, path, port, destination, use_ssl);

    for (;;)
        pause(); // Infinite loop to keep the parent process alive

    exit(EXIT_SUCCESS);
}