#!/usr/bin/env bash
# Prueba básica para validar que el binario responde y detecta banners simples.

set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_ROOT="$(cd "${SCRIPT_DIR}/.." && pwd)"
BIN="${PROJECT_ROOT}/secscanner"

if [[ ! -x "${BIN}" ]]; then
    echo "El binario ${BIN} no existe o no es ejecutable." >&2
    exit 1
fi

echo "[INFO] Ejecutando escaneo de prueba contra scanme.nmap.org..."
OUTPUT="$("${BIN}" -t scanme.nmap.org -p 80 -T 300 -v 2>&1 || true)"
echo "${OUTPUT}"

if echo "${OUTPUT}" | grep -E "OPEN|HTTP" >/dev/null; then
    echo "[OK] Se encontró evidencia de puerto abierto o banner HTTP."
    exit 0
fi

echo "[ERROR] No se detectó salida esperada ('OPEN' o 'HTTP')." >&2
exit 1
