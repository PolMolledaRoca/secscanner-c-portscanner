#include "scanner.h"
#include "utils.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define DEFAULT_TIMEOUT_MS 1000

static void print_usage(const char *prog) {
    fprintf(stderr,
            "Uso: %s -t <objetivo> -p <puertos> [opciones]\n"
            "Opciones:\n"
            "  -t <objetivo>      Host o IP a escanear.\n"
            "  -p <puertos>       Lista/rango ej. \"22,80,8000-8010\".\n"
            "  -T <timeout_ms>    Timeout en milisegundos (por defecto %d).\n"
            "  -c <concurrency>   Número de conexiones simultáneas (sin usar en esta versión).\n"
            "  -v                 Modo verboso (muestra puertos cerrados y banners).\n"
            "  -h                 Mostrar esta ayuda.\n",
            prog, DEFAULT_TIMEOUT_MS);
}

int main(int argc, char *argv[]) {
    scan_opts_t opts;
    memset(&opts, 0, sizeof(opts));
    opts.timeout_ms = DEFAULT_TIMEOUT_MS;
    opts.concurrency = 1;

    int opt = 0;
    char *ports_spec = NULL;

    while ((opt = getopt(argc, argv, "t:p:T:c:vh")) != -1) {
        switch (opt) {
            case 't':
                opts.target = optarg;
                break;
            case 'p':
                ports_spec = optarg;
                break;
            case 'T': {
                int value = 0;
                if (str_to_int(optarg, &value) != 0 || value <= 0) {
                    fprintf(stderr, "Timeout inválido: %s\n", optarg);
                    return EXIT_FAILURE;
                }
                opts.timeout_ms = value;
                break;
            }
            case 'c': {
                int value = 0;
                if (str_to_int(optarg, &value) != 0 || value <= 0) {
                    fprintf(stderr, "Concurrencia inválida: %s\n", optarg);
                    return EXIT_FAILURE;
                }
                opts.concurrency = value;
                break;
            }
            case 'v':
                opts.verbose = 1;
                break;
            case 'h':
                print_usage(argv[0]);
                return EXIT_SUCCESS;
            default:
                print_usage(argv[0]);
                return EXIT_FAILURE;
        }
    }

    if (!opts.target || !ports_spec) {
        fprintf(stderr, "Faltan parámetros obligatorios.\n");
        print_usage(argv[0]);
        return EXIT_FAILURE;
    }

    if (parse_range_or_list(ports_spec, &opts.ports, &opts.port_count) != 0) {
        fprintf(stderr, "Especificación de puertos inválida.\n");
        return EXIT_FAILURE;
    }

    if (optind < argc) {
        fprintf(stderr, "Argumentos no reconocidos.\n");
        free(opts.ports);
        return EXIT_FAILURE;
    }

    int result = run_scan(&opts);

    free(opts.ports);
    return (result == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}
