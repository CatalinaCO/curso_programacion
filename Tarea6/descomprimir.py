import os
import struct
import sys
import zlib

FIRMA = b"CATA"
VERSION = 1
FORMATO_HEADER = ">4sBBHQIH"


class NodoHuffman:

  def __init__(self, char):
    self.char = char
    self.left = None
    self.right = None

  def es_hoja(self):
    return self.left is None and self.right is None


class DescompresorHuffman:

  def __init__(self, ruta_archivo):
    self.ruta_archivo = ruta_archivo
    self.bits_arbol = ""
    self.pos = 0

  def _leer_arbol(self):
    bit = self.bits_arbol[self.pos]
    self.pos += 1

    if bit == "1":
      char = int(self.bits_arbol[self.pos : self.pos + 8], 2)
      self.pos += 8
      return NodoHuffman(char)

    nodo = NodoHuffman(None)
    nodo.left = self._leer_arbol()
    nodo.right = self._leer_arbol()
    return nodo

  def _bytes_a_bits(self, datos):
    return "".join(["{0:08b}".format(b) for b in datos])

  def descomprimir(self, ruta_salida=None):
    with open(self.ruta_archivo, "rb") as f:
      datos = f.read()

    # 1. leer el header
    tam_header = struct.calcsize(FORMATO_HEADER)
    firma, version, relleno, k, tam_original, crc, largo_nombre = struct.unpack(
        FORMATO_HEADER, datos[:tam_header]
    )

    if firma != FIRMA:
      raise ValueError("El archivo no tiene formato .huf")
    if version != VERSION:
      raise ValueError(f"Version {version} no soportada")

    inicio = tam_header
    nombre = datos[inicio : inicio + largo_nombre].decode("utf-8")
    inicio += largo_nombre

    # 2. el arbol ocupa 10k - 1 bits, redondeado a bytes
    tam_arbol = (10 * k - 1 + 7) // 8 if k > 0 else 0
    bytes_arbol = datos[inicio : inicio + tam_arbol]
    bytes_datos = datos[inicio + tam_arbol :]

    # 3. reconstruir el arbol y decodificar
    resultado = bytearray()
    if k > 0:
      self.bits_arbol = self._bytes_a_bits(bytes_arbol)
      raiz = self._leer_arbol()

      texto_codificado = self._bytes_a_bits(bytes_datos)
      if relleno > 0:
        texto_codificado = texto_codificado[:-relleno]

      if raiz.es_hoja():
        resultado = bytearray([raiz.char]) * tam_original
      else:
        nodo_actual = raiz
        for bit in texto_codificado:
          if bit == "0":
            nodo_actual = nodo_actual.left
          else:
            nodo_actual = nodo_actual.right

          if nodo_actual.es_hoja():
            resultado.append(nodo_actual.char)
            nodo_actual = raiz

    # 4. comprobar que salio igual al original
    if len(resultado) != tam_original or zlib.crc32(resultado) != crc:
      raise ValueError("El archivo esta corrupto")

    if ruta_salida is None:
      ruta_salida = "recuperado_" + os.path.basename(nombre)

    with open(ruta_salida, "wb") as salida:
      salida.write(resultado)

    print(f"Archivo descomprimido exitosamente en: {ruta_salida}")


if __name__ == "__main__":
  if len(sys.argv) < 2:
    print("Uso: python descomprimir.py archivo.huf [salida]")
    sys.exit(1)

  entrada = sys.argv[1]
  salida = sys.argv[2] if len(sys.argv) > 2 else None

  descompresor = DescompresorHuffman(entrada)
  descompresor.descomprimir(salida)
