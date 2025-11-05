#include "banner.h"

#include "scanner.h"
#include "utils.h"

#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <unistd.h>

static const char *select_probe(int port) {
    switch (port) {
        case 21:
            return "FEAT\r\n";
        case 25:
        case 587:
            return "EHLO secscanner\r\n";
        case 80:
        case 8000:
        case 8080:
        case 8081:
            return "HEAD / HTTP/1.0\r\nHost: secscanner\r\n\r\n";
        case 110:
            return "CAPA\r\n";
        case 143:
            return ". CAPABILITY\r\n";
        case 443:
            return "HEAD / HTTP/1.0\r\nHost: secscanner\r\n\r\n";
        case 990:
        case 992:
            return "FEAT\r\n";
        default:
            return NULL;
    }
}

int grab_banner(const char *host, int port, int timeout_ms, char *outbuf, size_t outlen) {
    if (!host || !outbuf || outlen < 2) {
        return -1;
    }

    int sockfd = tcp_connect_with_timeout(host, port, timeout_ms);
    if (sockfd < 0) {
        return -1;
    }

    const char *probe = select_probe(port);
    if (probe) {
        ssize_t sent = send(sockfd, probe, strlen(probe), 0);
        if (sent < 0) {
            close(sockfd);
            return -1;
        }
    }

    fd_set read_fds;
    FD_ZERO(&read_fds);
    FD_SET(sockfd, &read_fds);

    struct timeval tv;
    if (timeout_ms <= 0) {
        timeout_ms = 1000;
    }
    tv.tv_sec = timeout_ms / 1000;
    tv.tv_usec = (timeout_ms % 1000) * 1000;

    int ready = select(sockfd + 1, &read_fds, NULL, NULL, &tv);
    if (ready <= 0) {
        close(sockfd);
        return -1;
    }

    ssize_t received = recv(sockfd, outbuf, outlen - 1, 0);
    if (received <= 0) {
        close(sockfd);
        return -1;
    }

    outbuf[received] = '\0';
    trim_newline(outbuf);
    close(sockfd);
    return 0;
}
