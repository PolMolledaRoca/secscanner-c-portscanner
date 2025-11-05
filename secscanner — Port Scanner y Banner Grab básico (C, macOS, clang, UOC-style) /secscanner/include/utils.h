#ifndef UTILS_H
#define UTILS_H

#include <stddef.h>

/**
 * Convierte una cadena a entero validando límites.
 * @param str Cadena a convertir.
 * @param out Valor convertido si tiene éxito.
 * @return 0 si es válido, -1 en caso de error.
 */
int str_to_int(const char *str, int *out);

/**
 * Elimina el salto de línea final de una cadena si existe.
 * @param str Cadena mutable terminada en '\0'.
 */
void trim_newline(char *str);

/**
 * Duerme el hilo actual en milisegundos.
 * @param ms Milisegundos a esperar.
 */
void ms_sleep(int ms);

/**
 * Parsea una lista/rango de puertos (ej. "22,80,8000-8010").
 * @param spec Cadena de entrada.
 * @param ports_out Array de puertos (se reserva con malloc).
 * @param count_out Número de puertos.
 * @return 0 si todo bien, -1 en caso de error.
 */
int parse_range_or_list(const char *spec, int **ports_out, size_t *count_out);

#endif /* UTILS_H */
