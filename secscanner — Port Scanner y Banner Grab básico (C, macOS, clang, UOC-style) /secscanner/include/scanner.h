#ifndef SCANNER_H
#define SCANNER_H

#include <stddef.h>

typedef struct {
    const char *target;
    int *ports;
    size_t port_count;
    int timeout_ms;
    int concurrency;
    int verbose;
} scan_opts_t;

/**
 * Ejecuta el escaneo TCP sobre la lista de puertos indicada.
 * @param opts Opciones ya validadas e inicializadas.
 * @return 0 si todo fue bien, distinto de cero si hubo errores.
 */
int run_scan(const scan_opts_t *opts);

/**
 * Conecta con un host IPv4 usando un timeout en milisegundos.
 * @param host Dirección IPv4 en notación punto decimal.
 * @param port Puerto TCP al que conectarse.
 * @param timeout_ms Timeout global en milisegundos.
 * @return Descriptor de socket si conecta, -1 en error.
 */
int tcp_connect_with_timeout(const char *host, int port, int timeout_ms);

#endif /* SCANNER_H */
