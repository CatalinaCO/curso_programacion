#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct comparte{
    char nombre[50];
    char direccion[50];
    int edad;
    struct comparte *next;
    struct comparte *prev;
}Registro;

typedef struct l {
    Registro *inicial; //centinela de cabeza, no guarda info real
    Registro *final;   //centinela de cola, no guarda info real
}Lista;

Registro *crear(char *nombre, char *direccion, int edad){
    Registro *nuevo = (Registro *)malloc(sizeof(Registro));
    if(nuevo == NULL){
        return NULL;
    }

    strcpy(nuevo->nombre, nombre);
    strcpy(nuevo->direccion, direccion);
    nuevo->edad = edad;
    nuevo->next = NULL;
    nuevo->prev = NULL;

    return nuevo;
}

int crear_lista(Lista *lista){
    Registro *N_inicial = (Registro *)malloc(sizeof(Registro));
    Registro *N_final = (Registro *)malloc(sizeof(Registro));

    if(N_inicial == NULL || N_final == NULL){
        return 0;
    }

    N_inicial->next = N_final;
    N_inicial->prev = NULL;
    N_final->prev = N_inicial;
    N_final->next = NULL;

    lista->inicial = N_inicial;
    lista->final = N_final;

    return 1;
}

int esta_vacia(Lista *lista){
    return lista->inicial->next == lista->final;
}

// esta funcion la uso adentro de agregar/quitar para no repetir
// el mismo while en cada una
void mostrar(Lista *lista){
    Registro *temp = lista->inicial->next;

    if(temp == lista->final){
        printf("  (vacia)\n");
        return;
    }

    printf("  ");
    while(temp != lista->final){
        printf("[%s] ", temp->nombre);
        temp = temp->next;
    }
    printf("\n");
}

// agrega al inicio e imprime como quedo la lista
void agregar_inicio(Lista *lista, char *nombre, char *direccion, int edad){
    Registro *nuevo = crear(nombre, direccion, edad);
    if(nuevo == NULL) return;

    Registro *temp = lista->inicial->next;

    nuevo->next = temp;
    nuevo->prev = lista->inicial;
    lista->inicial->next = nuevo;
    temp->prev = nuevo;

    printf("agregue %s al inicio ->\n", nombre);
    mostrar(lista);
}

// agrega al final e imprime como quedo la lista
void agregar_final(Lista *lista, char *nombre, char *direccion, int edad){
    Registro *nuevo = crear(nombre, direccion, edad);
    if(nuevo == NULL) return;

    Registro *temp = lista->final->prev;

    nuevo->prev = temp;
    nuevo->next = lista->final;
    lista->final->prev = nuevo;
    temp->next = nuevo;

    printf("agregue %s al final ->\n", nombre);
    mostrar(lista);
}

// quita el primero, imprime quien salio y como quedo la lista
int quitar_inicio(Lista *lista, Registro *dato){
    if(esta_vacia(lista)){
        printf("la lista ya esta vacia\n");
        return 0;
    }

    Registro *temp = lista->inicial->next;

    if(dato != NULL){
        strcpy(dato->nombre, temp->nombre);
        strcpy(dato->direccion, temp->direccion);
        dato->edad = temp->edad;
    }

    lista->inicial->next = temp->next;
    temp->next->prev = lista->inicial;

    printf("saco %s del inicio ->\n", temp->nombre);
    free(temp);
    mostrar(lista);
    return 1;
}

// quita el ultimo, imprime quien salio y como quedo la lista
int quitar_final(Lista *lista, Registro *dato){
    if(esta_vacia(lista)){
        printf("la lista ya esta vacia\n");
        return 0;
    }

    Registro *temp = lista->final->prev;

    if(dato != NULL){
        strcpy(dato->nombre, temp->nombre);
        strcpy(dato->direccion, temp->direccion);
        dato->edad = temp->edad;
    }

    lista->final->prev = temp->prev;
    temp->prev->next = lista->final;

    printf("saco %s del final ->\n", temp->nombre);
    free(temp);
    mostrar(lista);
    return 1;
}

//pila: entra y sale por el mismo lado (inicio)
void push(Lista *lista, char *nombre, char *direccion, int edad){
    agregar_inicio(lista, nombre, direccion, edad);
}
int pop(Lista *lista, Registro *dato){
    return quitar_inicio(lista, dato);
}

//cola: entra por el final, sale por el inicio
void encolar(Lista *lista, char *nombre, char *direccion, int edad){
    agregar_final(lista, nombre, direccion, edad);
}
int desencolar(Lista *lista, Registro *dato){
    return quitar_inicio(lista, dato);
}

void liberar(Lista *lista){
    Registro *temp = lista->inicial->next;
    Registro *sig;

    while(temp != lista->final){
        sig = temp->next;
        free(temp);
        temp = sig;
    }

    free(lista->inicial);
    free(lista->final);
}

int main(){
    Lista pila, cola;
    crear_lista(&pila);
    crear_lista(&cola);
    Registro dato;

    printf("PILA (LIFO)\n");
    push(&pila, "Juan", "Cra 5 #10-20", 22);
    push(&pila, "Pedro", "Cll 8 #4-15", 19);
    push(&pila, "Maria", "Av Siempre Viva 45", 27);

    while(pop(&pila, &dato));

    printf("\nCOLA (FIFO)\n");
    encolar(&cola, "Juan", "Cra 5 #10-20", 22);
    encolar(&cola, "Pedro", "Cll 8 #4-15", 19);
    encolar(&cola, "Maria", "Av Siempre Viva 45", 27);

    while(desencolar(&cola, &dato));

    liberar(&pila);
    liberar(&cola);

    return 0;
}