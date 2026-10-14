#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define TAMANIO 10

//Definimos el valor de cada nodo
typedef struct ValorNode {
    int valor;
    struct ValorNode *siguiente;
} ValorNode;


// Definimos el nodo de cada llave (lista enlazada)
typedef struct HashNode {
    char *clave;
    ValorNode *valores;
    struct HashNode *siguiente;
} HashNode;


//Definimos la tabla hash
typedef struct {
    HashNode *tabla[TAMANIO];
} HashTable;


// Función hash simple
unsigned int funcion_hash(const char *clave) {
    unsigned int hash=0;
    while (*clave) {
        hash = (hash * 31) + (unsigned char)*clave;
        clave++;
    }
    return hash % TAMANIO;
}


// Función auxiliar para buscar un nodo por clave
static HashNode *buscar_nodo(HashTable *ht, const char *clave) {
    HashNode *n = ht -> tabla[funcion_hash(clave)];
    while (n !=NULL && strcmp(n -> clave, clave ) != 0){
        n = n -> siguiente;
    }
    return n;
}


// Insertar un elemento
int hash_insertar(HashTable *ht, const char *clave, int valor) {
    ValorNode *nuevo= malloc(sizeof(ValorNode));
    if (nuevo == NULL) return -1;
    nuevo -> valor = valor;
    nuevo -> siguiente = NULL;

    HashNode *nodo = buscar_nodo(ht, clave);
    if (nodo == NULL) {
        unsigned int i = funcion_hash(clave);
        nodo = malloc(sizeof(HashNode));
        if (nodo == NULL) {
            free(nuevo);
            return -1;
        }
        nodo -> clave = strdup(clave);
        if (nodo -> clave == NULL) {
            free(nuevo);
            free(nodo);
            return -1;
        }
        nodo -> valores = nuevo;
        nodo -> siguiente = ht -> tabla[i];
        ht -> tabla[i] = nodo;
        return 1;
    }

    ValorNode *v = nodo -> valores;
    ValorNode *ultimo = NULL;
    while (v != NULL) {
        if (v -> valor == valor) {
            free(nuevo);
            return 0;
        }
        ultimo = v;
        v = v -> siguiente;
    }
    ultimo -> siguiente = nuevo;
    return 1;
}


// Buscar un elemento
int hash_buscar(HashTable *ht, const char *clave, int *encontrado) {
    HashNode *nodo = buscar_nodo(ht, clave);
    if (nodo != NULL) {
        *encontrado = 1;
        return nodo->valores->valor;
    }
    *encontrado = 0;
    return -1;
}


// Borrar una clave específica de la tabla
int hash_borrar(HashTable *ht, const char *clave, int valor) {
    unsigned int indice = funcion_hash(clave);
    HashNode *nodo = ht -> tabla[indice];
    HashNode *anterior = NULL;

    while (nodo != NULL && strcmp(nodo -> clave, clave) != 0) {
        anterior = nodo;
        nodo = nodo -> siguiente;
    }

    if (nodo == NULL) return 0;

    ValorNode *v = nodo -> valores;
    ValorNode *ultimo = NULL;
    while (v != NULL && v -> valor != valor) {
        ultimo = v;
        v = v -> siguiente;
    }

    if (v == NULL) return 0;

    if (ultimo == NULL) {
        nodo -> valores = v -> siguiente;
    } else {
        ultimo -> siguiente = v -> siguiente;
    }
    free(v);

    if (nodo -> valores == NULL) {
        if (anterior == NULL) {
            ht -> tabla[indice] = nodo -> siguiente;
        } else {
            anterior -> siguiente = nodo -> siguiente;
        }
        free(nodo -> clave);
        free(nodo);
    }
    return 1;
}


// destruir la tabla hash y liberar memoria
void hash_destruir(HashTable *ht) {
    for (int i=0; i < TAMANIO; i++) {
        HashNode *actual = ht -> tabla[i];
        while (actual != NULL) {
            HashNode *temporal = actual;
            actual = actual -> siguiente;

            ValorNode *v = temporal -> valores;
            while (v != NULL) {
                ValorNode *temp_valor = v;
                v = v -> siguiente;
                free(temp_valor);
            }
            
            free(temporal -> clave);
            free(temporal);
        }
        ht -> tabla[i] = NULL;
    }
}


// Imprimir la tabla hash
void hash_imprimir(HashTable *ht) {
    for (int i = 0; i < TAMANIO; i++) {
        HashNode *actual = ht->tabla[i];
        while (actual != NULL) {
            printf("Índice %d: Clave: %s, Valores: ", i, actual->clave);
            ValorNode *v = actual->valores;
            while (v != NULL) {
                printf("%d ", v->valor);
                v = v->siguiente;
            }
            printf("\n");
            actual = actual->siguiente;
        }
    }
}


int main() {
    HashTable HT = {0};

    hash_insertar(&HT, "Ernesto", 10);
    hash_insertar(&HT, "Braulio", 15);
    hash_insertar(&HT, "Catalina", 56);
    hash_insertar(&HT, "Alondra", 18);
    hash_insertar(&HT, "Ernesto", 18);
    
    //mostrar solo la tabla
    printf("\nTabla hash:\n");
    hash_imprimir(&HT);                 
    
    // eliminar una clave
    hash_borrar(&HT, "Ernesto", 18);      
    printf("\nTabla hash tras borrar Ernesto (18):\n");
    hash_imprimir(&HT);

    // liberar toda la memoria
    hash_destruir(&HT);                 
    return 0;
}