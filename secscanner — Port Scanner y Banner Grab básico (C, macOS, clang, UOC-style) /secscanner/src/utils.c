#include "utils.h"

#include <ctype.h>
#include <errno.h>
#include <limits.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

static int is_valid_port(int port) {
    return port > 0 && port <= 65535;
}

int str_to_int(const char *str, int *out) {
    if (!str || !out) {
        return -1;
    }

    char *endptr = NULL;
    errno = 0;
    long value = strtol(str, &endptr, 10);

    if (errno != 0 || endptr == str || *endptr != '\0') {
        return -1;
    }
    if (value < INT_MIN || value > INT_MAX) {
        return -1;
    }

    *out = (int)value;
    return 0;
}

void trim_newline(char *str) {
    if (!str) {
        return;
    }
    size_t len = strlen(str);
    while (len > 0 && (str[len - 1] == '\n' || str[len - 1] == '\r')) {
        str[len - 1] = '\0';
        len--;
    }
}

void ms_sleep(int ms) {
    if (ms <= 0) {
        return;
    }
    struct timespec req;
    req.tv_sec = ms / 1000;
    req.tv_nsec = (ms % 1000) * 1000000L;
    while (nanosleep(&req, &req) == -1 && errno == EINTR) {
        continue;
    }
}

static void trim_spaces(char *str) {
    if (!str) {
        return;
    }
    while (*str == ' ' || *str == '\t') {
        memmove(str, str + 1, strlen(str));
    }
    size_t len = strlen(str);
    while (len > 0 && (str[len - 1] == ' ' || str[len - 1] == '\t')) {
        str[len - 1] = '\0';
        len--;
    }
}

static int push_port(int value, int **ports, size_t *count, size_t *capacity) {
    if (!is_valid_port(value)) {
        return -1;
    }
    for (size_t i = 0; i < *count; ++i) {
        if ((*ports)[i] == value) {
            return 0;
        }
    }
    if (*count == *capacity) {
        size_t new_capacity = (*capacity == 0) ? 8 : (*capacity * 2);
        int *tmp = realloc(*ports, new_capacity * sizeof(int));
        if (!tmp) {
            return -1;
        }
        *ports = tmp;
        *capacity = new_capacity;
    }
    (*ports)[*count] = value;
    (*count)++;
    return 0;
}

int parse_range_or_list(const char *spec, int **ports_out, size_t *count_out) {
    if (!spec || !ports_out || !count_out) {
        return -1;
    }
    *ports_out = NULL;
    *count_out = 0;
    size_t capacity = 0;

    char *copy = strdup(spec);
    if (!copy) {
        return -1;
    }

    char *token = strtok(copy, ",");
    while (token) {
        trim_spaces(token);
        if (strchr(token, '-')) {
            char *dash = strchr(token, '-');
            *dash = '\0';
            const char *start_str = token;
            const char *end_str = dash + 1;

            int start_port = 0;
            int end_port = 0;
            if (str_to_int(start_str, &start_port) != 0 ||
                str_to_int(end_str, &end_port) != 0 ||
                start_port > end_port) {
                free(copy);
                free(*ports_out);
                *ports_out = NULL;
                *count_out = 0;
                return -1;
            }
            for (int port = start_port; port <= end_port; ++port) {
                if (push_port(port, ports_out, count_out, &capacity) != 0) {
                    free(copy);
                    free(*ports_out);
                    *ports_out = NULL;
                    *count_out = 0;
                    return -1;
                }
            }
        } else {
            int single_port = 0;
            if (str_to_int(token, &single_port) != 0 ||
                push_port(single_port, ports_out, count_out, &capacity) != 0) {
                free(copy);
                free(*ports_out);
                *ports_out = NULL;
                *count_out = 0;
                return -1;
            }
        }
        token = strtok(NULL, ",");
    }

    free(copy);
    if (*count_out == 0) {
        free(*ports_out);
        *ports_out = NULL;
        return -1;
    }
    return 0;
}
