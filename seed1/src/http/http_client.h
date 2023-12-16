#ifndef HTTP_CLIENT_H
#define HTTP_CLIENT_H

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netdb.h>
#include <errno.h>
#include <arpa/inet.h>
#include <openssl/ssl.h>
#include <openssl/err.h>

// Function Prototypes

int ReadHttpStatus(int sock);
int ParseHeader_HTTP(int sock);
int ParseHeader_HTTPs(SSL *ssl);
void handle_error(const char *message, int error_code);
struct hostent *resolve_hostname(const char *domain);
int create_socket();
void connect_to_server(int sock, struct hostent *he, int port);
int receive_response(int sock, const char *destination, SSL *ssl, int use_ssl);
int initialize_ssl_library(void);
int initialize_ssl_connection(int sock, SSL_CTX **ctx, SSL **ssl);
int ssl_write(SSL *ssl, const char *data);
int send_request(int sock, const char *domain, const char *path, SSL_CTX *ctx, SSL *ssl, int use_ssl);
int curl(char *domain, char *path, int port, const char *destination, int use_ssl);

#endif // HTTP_CLIENT_H