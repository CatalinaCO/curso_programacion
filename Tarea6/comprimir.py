import heapq
import os
import struct
import sys
import zlib
from collections import Counter

FIRMA = b"CATA"
VERSION = 1
# firma, version, relleno, k, tamaño original, crc32, largo del nombre
FORMATO_HEADER = ">4sBBHQIH"


class NodoHuffman:

  def __init__(self, char, freq, orden):
    self.char = char
    self.freq = freq
    self.orden = orden
    self.left = None
    self.right = None

  def __lt__(self, other):
    # si empatan en frecuencia se desempata por orden de creacion
    if self.freq == other.freq:
      return self.orden < other.orden
    return self.freq < other.freq

  def es_hoja(self):
    return self.left is None and self.right is None


class CompresorHuffman:

  def __init__(self, ruta_archivo):
    self.ruta_archivo = ruta_archivo
    self.heap = []
    self.codigos = {}
    self.contador = 0

  def _nuevo_nodo(self, char, freq):
    nodo = NodoHuffman(char, freq, self.contador)
    self.contador += 1
    return nodo

  def _construir_arbol(self, frecuencias):
    for char in sorted(frecuencias):
      heapq.heappush(self.heap, self._nuevo_nodo(char, frecuencias[char]))

    while len(self.heap) > 1:
      nodo1 = heapq.heappop(self.heap)
      nodo2 = heapq.heappop(self.heap)

      combinado = self._nuevo_nodo(None, nodo1.freq + nodo2.freq)
      combinado.left = nodo1
      combinado.right = nodo2

      heapq.heappush(self.heap, combinado)

    return self.heap[0] if self.heap else None

  def _generar_codigos_rec(self, nodo, codigo_actual):
    if nodo is None:
      return

    if nodo.es_hoja():
      # si el archivo tiene un solo simbolo el codigo quedaria vacio
      self.codigos[nodo.char] = codigo_actual if codigo_actual else "0"
      return

    self._generar_codigos_rec(nodo.left, codigo_actual + "0")
    self._generar_codigos_rec(nodo.right, codigo_actual + "1")

  def _arbol_a_bits(self, nodo):
    # preorden: 0 = nodo interno, 1 + 8 bits del byte = hoja
    if nodo.es_hoja():
      return "1" + "{0:08b}".format(nodo.char)
    return "0" + self._arbol_a_bits(nodo.left) + self._arbol_a_bits(nodo.right)

  def _bits_a_bytes(self, bits):
    relleno = (8 - len(bits) % 8) % 8
    bits += "0" * relleno
    b = bytearray()
    for i in range(0, len(bits), 8):
      b.append(int(bits[i : i + 8], 2))
    return b, relleno

  def comprimir(self, ruta_salida):
    with open(self.ruta_archivo, "rb") as f:
      datos = f.read()

    frecuencias = Counter(datos)
    raiz = self._construir_arbol(frecuencias)
    self._generar_codigos_rec(raiz, "")

    bits_arbol = self._arbol_a_bits(raiz) if raiz else ""
    bytes_arbol, _ = self._bits_a_bytes(bits_arbol)

    texto_codificado = "".join([self.codigos[b] for b in datos])
    bytes_datos, relleno = self._bits_a_bytes(texto_codificado)

    nombre = os.path.basename(self.ruta_archivo).encode("utf-8")
    header = struct.pack(
        FORMATO_HEADER,
        FIRMA,
        VERSION,
        relleno,
        len(frecuencias),
        len(datos),
        zlib.crc32(datos),
        len(nombre),
    )

    with open(ruta_salida, "wb") as salida:
      salida.write(header)
      salida.write(nombre)
      salida.write(bytes_arbol)
      salida.write(bytes_datos)

    total = len(header) + len(nombre) + len(bytes_arbol) + len(bytes_datos)
    print(f"Archivo comprimido exitosamente en: {ruta_salida}")
    print(f"Tamaño: {len(datos)} bytes -> {total} bytes")


if __name__ == "__main__":
  if len(sys.argv) < 2:
    print("Uso: python comprimir.py archivo [salida.huf]")
    sys.exit(1)

  entrada = sys.argv[1]
  salida = sys.argv[2] if len(sys.argv) > 2 else entrada + ".huf"

  compresor = CompresorHuffman(entrada)
  compresor.comprimir(salida)
