#ifndef BANNER_H
#define BANNER_H

#include <stddef.h>

/**
 * Intenta obtener el banner de un servicio TCP conocido.
 * @param host Host objetivo (nombre o IPv4).
 * @param port Puerto TCP objetivo.
 * @param timeout_ms Timeout para operaciones de E/S.
 * @param outbuf Buffer de salida donde se guarda el banner.
 * @param outlen Tamaño del buffer de salida.
 * @return 0 si se ha obtenido algún banner, -1 en caso contrario.
 */
int grab_banner(const char *host, int port, int timeout_ms, char *outbuf, size_t outlen);

#endif /* BANNER_H */
