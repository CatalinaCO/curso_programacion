import time
import lorem
import huffman as py   
import huffman_cy as cy    

ARCHIVO = "origen_grande.txt"

# Generar el texto de prueba
with open(ARCHIVO, "w", encoding="utf-8") as f:
    for _ in range(5000):
        f.write(lorem.paragraph() + "\n")

def medir(modulo):

    t0 = time.perf_counter()

    comp = modulo.CompresorHuffman(ARCHIVO)
    codigos, raiz = comp.comprimir("salida.bin")
    comp.descomprimir("salida.bin", raiz, "recuperado.txt")

    return time.perf_counter() - t0


tiempo_py = medir(py)
tiempo_cy = medir(cy)


print(f"\nMejor tiempo Python: {tiempo_py:.4f} s")
print(f"Mejor tiempo Cython: {tiempo_cy:.4f} s")
print(f"Aceleración: {tiempo_py / tiempo_cy:.2f}x")