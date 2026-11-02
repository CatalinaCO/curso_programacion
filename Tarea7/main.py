# Ejemplo de uso:
from huffman_cy import CompresorHuffman


if __name__ == "__main__":
  # Crear archivo de prueba
  with open("origen.txt", "w", encoding="utf-8") as f:
    f.write("este es un texto de prueba para el algoritmo de huffman clase de programacion 20 de octubre del 2026 10 a 12 am")

  compresor = CompresorHuffman("origen.txt")
  codigos, raiz = compresor.comprimir("archivo.bin")
  compresor.descomprimir("archivo.bin", raiz, "recuperado.txt")