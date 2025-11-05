#include "scanner.h"

#include "banner.h"
#include "utils.h"

#include <arpa/inet.h>
#include <errno.h>
#include <fcntl.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <unistd.h>

#define BANNER_BUFFER 256

static int resolve_target_ipv4(const char *host, char *out_ip, size_t out_len) {
    if (!host || !out_ip) {
        return -1;
    }

    struct addrinfo hints;
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;

    struct addrinfo *result = NULL;
    int ret = getaddrinfo(host, NULL, &hints, &result);
    if (ret != 0) {
        fprintf(stderr, "Error al resolver %s: %s\n", host, gai_strerror(ret));
        return -1;
    }

    struct sockaddr_in *addr = (struct sockaddr_in *)result->ai_addr;
    const char *converted = inet_ntop(AF_INET, &(addr->sin_addr), out_ip, (socklen_t)out_len);
    freeaddrinfo(result);

    if (!converted) {
        perror("inet_ntop");
        return -1;
    }
    return 0;
}

int tcp_connect_with_timeout(const char *host, int port, int timeout_ms) {
    if (!host || port <= 0 || port > 65535) {
        return -1;
    }

    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        perror("socket");
        return -1;
    }

    int flags = fcntl(sockfd, F_GETFL, 0);
    if (flags == -1) {
        perror("fcntl");
        close(sockfd);
        return -1;
    }
    if (fcntl(sockfd, F_SETFL, flags | O_NONBLOCK) == -1) {
        perror("fcntl");
        close(sockfd);
        return -1;
    }

    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons((uint16_t)port);

    if (inet_pton(AF_INET, host, &addr.sin_addr) != 1) {
        char resolved[INET_ADDRSTRLEN] = {0};
        if (resolve_target_ipv4(host, resolved, sizeof(resolved)) != 0 ||
            inet_pton(AF_INET, resolved, &addr.sin_addr) != 1) {
            fprintf(stderr, "No se pudo resolver la dirección: %s\n", host);
            close(sockfd);
            return -1;
        }
    }

    int res = connect(sockfd, (struct sockaddr *)&addr, sizeof(addr));
    if (res == -1 && errno != EINPROGRESS) {
        close(sockfd);
        return -1;
    }

    if (res != 0) {
        fd_set write_fds;
        FD_ZERO(&write_fds);
        FD_SET(sockfd, &write_fds);

        struct timeval tv;
        tv.tv_sec = timeout_ms / 1000;
        tv.tv_usec = (timeout_ms % 1000) * 1000;

        int sel = select(sockfd + 1, NULL, &write_fds, NULL, (timeout_ms > 0) ? &tv : NULL);
        if (sel <= 0) {
            close(sockfd);
            return -1;
        }

        int sock_err = 0;
        socklen_t len = sizeof(sock_err);
        if (getsockopt(sockfd, SOL_SOCKET, SO_ERROR, &sock_err, &len) < 0 || sock_err != 0) {
            close(sockfd);
            return -1;
        }
    }

    if (fcntl(sockfd, F_SETFL, flags) == -1) {
        perror("fcntl");
        close(sockfd);
        return -1;
    }
    return sockfd;
}

int run_scan(const scan_opts_t *opts) {
    if (!opts || !opts->target || !opts->ports || opts->port_count == 0) {
        fprintf(stderr, "Opciones de escaneo inválidas.\n");
        return -1;
    }

    char ip_buffer[INET_ADDRSTRLEN] = {0};
    if (resolve_target_ipv4(opts->target, ip_buffer, sizeof(ip_buffer)) != 0) {
        return -1;
    }

    printf("[*] Objetivo: %s (%s)\n", opts->target, ip_buffer);
    printf("[*] Total de puertos a comprobar: %zu\n", opts->port_count);
    if (opts->concurrency > 1) {
        printf("[!] Concurrencia solicitada: %d (no implementada en esta versión secuencial)\n",
               opts->concurrency);
    }

    for (size_t i = 0; i < opts->port_count; ++i) {
        int port = opts->ports[i];
        ms_sleep(10);
        int sockfd = tcp_connect_with_timeout(ip_buffer, port, opts->timeout_ms);
        if (sockfd >= 0) {
            printf("[+] Puerto %d/tcp -> ABIERTO (OPEN)\n", port);
            if (opts->verbose) {
                char banner[BANNER_BUFFER] = {0};
                if (grab_banner(opts->target, port, opts->timeout_ms, banner, sizeof(banner)) == 0) {
                    printf("    Banner: %s\n", banner);
                } else {
                    printf("    Sin banner disponible.\n");
                }
            }
            close(sockfd);
        } else if (opts->verbose) {
            printf("[-] Puerto %d/tcp -> cerrado o filtrado.\n", port);
        }
    }

    return 0;
}
