# cython: language_level=3
import heapq
from operator import __lt__
import os
from collections import Counter


cdef class NodoHuffman:
  
  cdef public str caracter
  cdef public long freq
  cdef public NodoHuffman left, right

  def __init__(self, str caracter, long freq):
    self.caracter = caracter
    self.freq = freq
    self.left = None
    self.right = None

  def __lt__(self, NodoHuffman other):
    return self.freq < other.freq


cdef class CompresorHuffman:
  
  cdef public str ruta_archivo
  cdef public list heap
  cdef public dict codigos
  cdef public dict inverso_codigos

  def __init__(self, str ruta_archivo):
    self.ruta_archivo = ruta_archivo
    self.heap = []
    self.codigos = {}
    self.inverso_codigos = {}

  cdef _calcular_frecuencias(self, str texto):
    return Counter(texto)

  cdef NodoHuffman _construir_arbol(self, frecuencias):
    cdef NodoHuffman nodo, nodo1, nodo2, combinado
    cdef str caracter
    cdef long freq

    for caracter, freq in frecuencias.items():
      nodo = NodoHuffman(caracter, freq)
      heapq.heappush(self.heap, nodo)

    while len(self.heap) > 1:
      nodo1 = heapq.heappop(self.heap)
      nodo2 = heapq.heappop(self.heap)

      combinado = NodoHuffman(None, nodo1.freq + nodo2.freq)
      combinado.left = nodo1
      combinado.right = nodo2

      heapq.heappush(self.heap, combinado)

    return self.heap[0] if self.heap else None

  cdef _generar_codigos_rec(self, NodoHuffman nodo, str codigo_actual):
    if nodo is None:
      return

    if nodo.caracter is not None:
      self.codigos[nodo.caracter] = codigo_actual
      self.inverso_codigos[codigo_actual] = nodo.caracter
      return

    self._generar_codigos_rec(nodo.left, codigo_actual + "0")
    self._generar_codigos_rec(nodo.right, codigo_actual + "1")

  cdef str _pad_encoded_text(self, str encoded_text):
    cdef int extra_padding
    cdef str padded_info

    extra_padding = 8 - len(encoded_text) % 8
    encoded_text += "0" * extra_padding
    padded_info = "{0:08b}".format(extra_padding)
    return padded_info + encoded_text

  cdef bytearray _get_byte_array(self, str padded_encoded_text):
    cdef bytearray b
    cdef Py_ssize_t i
    cdef str byte

    if len(padded_encoded_text) % 8 != 0:
      raise ValueError("El texto codificado no está alineado a bytes.")
    b = bytearray()
    for i in range(0, len(padded_encoded_text), 8):
      byte = padded_encoded_text[i : i + 8]
      b.append(int(byte, 2))
    return b

  def comprimir(self, ruta_salida):

    cdef str texto, texto_codificado, texto_relleno
    cdef NodoHuffman raiz
    cdef bytearray bytes_salida

    with open(self.ruta_archivo, "r", encoding="utf-8") as f:
      texto = f.read()

    frecuencias = self._calcular_frecuencias(texto)
    raiz = self._construir_arbol(frecuencias)
    self._generar_codigos_rec(raiz, "")

    texto_codificado = "".join([self.codigos[caracter] for caracter in texto])
    texto_relleno = self._pad_encoded_text(texto_codificado)
    bytes_salida = self._get_byte_array(texto_relleno)

    # Guardar tabla de frecuencias y bytes comprimidos (implementación básica)
    with open(ruta_salida, "wb") as salida:
      # Guardar longitud de la tabla y los datos codificados
      salida.write(bytes_salida)

    print(f"Archivo comprimido exitosamente en: {ruta_salida}")
    return self.codigos, raiz

  cdef str _remover_padding(self, str padded_encoded_text):
    cdef str padded_info, texto_codificado
    cdef int extra_padding
  
    padded_info = padded_encoded_text[:8]
    extra_padding = int(padded_info, 2)

    texto_codificado = padded_encoded_text[8:]
    texto_codificado = texto_codificado[
        :-extra_padding if extra_padding > 0 else None
    ]
    return texto_codificado

  def descomprimir(self, str ruta_comprimido, NodoHuffman raiz_arbol, str ruta_salida):

    cdef list bits, texto_decodificado
    cdef bytes byte
    cdef int byte_val
    cdef str texto_bits, texto_codificado, texto_actual, resultado
    cdef Py_UCS4 bit
    cdef NodoHuffman nodo_actual
    
    with open(ruta_comprimido, "rb") as f:
      bits = []
      byte = f.read(1)
      while byte:
        byte_val = ord(byte)
        bits.append("{0:08b}".format(byte_val))
        byte = f.read(1)

    texto_bits = "".join(bits)
    texto_codificado = self._remover_padding(texto_bits)

    texto_actual = ""
    texto_decodificado = []
    nodo_actual = raiz_arbol

    for bit in texto_codificado:
      texto_actual += bit
      if bit == "0":
        nodo_actual = nodo_actual.left
      else:
        nodo_actual = nodo_actual.right

      if nodo_actual.caracter is not None:
        texto_decodificado.append(nodo_actual.caracter)
        nodo_actual = raiz_arbol

    resultado = "".join(texto_decodificado)
    with open(ruta_salida, "w", encoding="utf-8") as salida:
      salida.write(resultado)

    print(f"Archivo descomprimido exitosamente en: {ruta_salida}")
    return resultado



