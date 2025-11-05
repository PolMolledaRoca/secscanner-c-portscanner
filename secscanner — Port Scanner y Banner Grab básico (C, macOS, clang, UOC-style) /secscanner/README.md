# secscanner - Port Scanner y Banner Grab basico

Secscanner es una utilidad en C que realiza escaneos TCP secuenciales y, de forma opcional, intenta extraer banners de servicios conocidos. El proyecto sigue un estilo academico (UOC) y esta pensado para macOS usando `clang`, aunque puede compilarse en cualquier entorno POSIX con las librerias de sockets estandar.

## Caracteristicas
- Resolucion IPv4 y conexion con timeout por puerto.
- Deteccion de puertos abiertos mediante `connect` no bloqueante.
- Intento de banner grab con sondas especificas para servicios comunes (HTTP, FTP, SMTP, IMAP, etc.).
- Analisis de listas y rangos de puertos (`22,80,8000-8010`) evitando duplicados.
- Binario unico sin dependencias externas mas alla de la libc del sistema.

## Requisitos
- macOS (desarrollado y probado con `clang`).
- Herramientas de compilacion basicas: `make`, `clang`, `sed`, `grep`.
- Acceso a la red para probar contra hosts reales (por ejemplo `scanme.nmap.org`).

## Compilacion
```bash
make          # compila el binario ./secscanner dentro del directorio del proyecto
make clean    # elimina artefactos generados (build/ y secscanner)
```

La configuracion por defecto genera objetos en `build/` y deja el ejecutable `secscanner` en la raiz.

## Uso
```
Uso: ./secscanner -t <objetivo> -p <puertos> [opciones]
Opciones:
  -t <objetivo>      Host o IP a escanear.
  -p <puertos>       Lista/rango ej. "22,80,8000-8010".
  -T <timeout_ms>    Timeout en milisegundos (por defecto 1000).
  -c <concurrency>   Numero de conexiones simultaneas (sin usar en esta version).
  -v                 Modo verboso (muestra puertos cerrados y banners).
  -h                 Mostrar ayuda.
```

Parametros destacados:
- `-t`: host o direccion IPv4 (se resuelve automaticamente si es nombre).
- `-p`: lista separada por comas con valores individuales y/o rangos.
- `-T`: controla el timeout tanto del escaneo como del banner grab.
- `-v`: imprime tambien puertos cerrados y los banners capturados.

### Ejemplos
```bash
./secscanner -t scanme.nmap.org -p 22,80 -v
./secscanner -t 192.168.1.10 -p 20-25,80,443 -T 500
```

## Pruebas
```bash
make test
```

El script `tests/test_basic.sh` compila el binario (si es necesario) y ejecuta un escaneo corto contra `scanme.nmap.org`, validando que aparezca la palabra `OPEN` o `HTTP` en la salida.

## Estructura del proyecto
- `src/`: codigos fuente principales (`main.c`, `scanner.c`, `banner.c`, `utils.c`).
- `include/`: cabeceras correspondientes.
- `build/`: objetos generados durante la compilacion (se crea automaticamente).
- `tests/`: scripts de validacion manual/automatizada.
- `Makefile`: tareas de compilacion, ejecucion rapida y pruebas.

## Detalles de implementacion
- El escaneo actual es completamente secuencial; el flag `-c` se acepta pero solo muestra un aviso si se usa un valor mayor que uno.
- El banner grab solo lanza sondas personalizadas para algunos puertos frecuentes. Para otros puertos se limita a leer el primer payload disponible.
- Se aplican timeouts tanto a la fase de conexion como a la lectura del banner mediante `select`.

## Buenas practicas legales
Utiliza este escaner unicamente sobre equipos y redes donde tengas permiso expreso. El abuso de herramientas de reconocimiento sin autorizacion puede ser ilegal y conllevar consecuencias disciplinarias o penales.

## Ideas de mejora
- Implementar escaneo concurrente respetando el flag `-c`.
- Anadir soporte IPv6.
- Registrar salidas en formato JSON o CSV.
- Extender el catalogo de sondas de banner grab y mejorar heuristicas de deteccion.
