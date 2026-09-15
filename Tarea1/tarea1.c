#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <immintrin.h>

union Uint128 {
    unsigned long long int Num2[2];
    __m128i Num;
};

//Suma de 64 bits con acarreo
unsigned long long add64_carry (unsigned long long int a, unsigned long long int b, unsigned long long int *carry) {
    unsigned long long int sum = a + b;
    *carry = (sum < a) ? 1 : 0; // Check for overflow
    return sum;
};

//Se extraen las 4 mitades de 32 bits de los 2 enteros de 64 bits 
void mul128 (union Uint128 A, union Uint128 B, unsigned long long result[4]) {
    
    unsigned long long A_low = A.Num2[0], A_high = A.Num2[1];
    unsigned long long B_low = B.Num2[0], B_high = B.Num2[1];


    //La idea principal de lo siguiente es multiplicar los 2 enteros de 128 bits sabiendo que 
    // A=A_high * 2^64 + A_low y B=B_high * 2^64 + B_low, entonces 
    //A*B = (A_high * 2^64 + A_low) * (B_high * 2^64 + B_low) = A_high*B_high*2^128 + (A_high*B_low + A_low*B_high)*2^64 + A_low*B_low

    unsigned long long high1, high2, high3, high4;
    unsigned long long low1= _mulx_u64(A_low, B_low, &high1); // A_low * B_low
    unsigned long long low2= _mulx_u64(A_low, B_high, &high2); // A_low * B_high
    unsigned long long low3= _mulx_u64(A_high, B_low, &high3); // A_high * B_low
    unsigned long long low4= _mulx_u64(A_high, B_high, &high4); // A_high * B_high

    //las siguientes columnas corresponden al resultado final de la multiplicación de 128 bits
    // Dando como resultado 4 enteros de 64 bits, donde el entero de menor peso es low1 y el de mayor peso es high4

    //Columna 0
    result[0]=low1;

    //Columna 1
    unsigned long long c1, c2;
    unsigned long long col1 = add64_carry(low2, low3, &c1); // Suma de las columnas 2 y 3
    col1 = add64_carry(col1, high1, &c2); // Suma de la columna 1 con el acarreo de la suma anterior
    result[1] = col1; // Resultado de la columna 2
    unsigned long long carry_to_col2= c1 + c2; // Acarreo a la columna 2

    //Columna 2
    unsigned long long c3,c4, c5;
    unsigned long long col2 = add64_carry(high2, high3, &c3); // Suma de las columnas 4 y 2
    col2 = add64_carry(col2,low4, &c4); // Suma de la columna 2 con el acarreo de la suma anterior
    col2 = add64_carry(col2, carry_to_col2, &c5);
    result[2] = col2; // Resultado de la columna 3
    unsigned long long carry_to_col3= c3 + c4 + c5; // Acarreo a la columna 3

    //Columna 3
    result[3] = high4 + carry_to_col3; // Resultado de la columna
}

int main () {

    // Generamos 2 enteros de 128 bits aleatorios
    srand(time(NULL));

    union Uint128 A, B;
    unsigned long long result[4];

    // Generamos A 
    unsigned long long a_parte_alta1 = rand();
    unsigned long long a_parte_baja1 = rand();
    unsigned long long a_mitad_baja = a_parte_alta1<<32 | a_parte_baja1;

    unsigned long long a_parte_alta2 = rand();
    unsigned long long a_parte_baja2 = rand();
    unsigned long long a_mitad_alta = (a_parte_alta2<<32 | a_parte_baja2) | (1ULL << 63);

    A.Num2[0] = a_mitad_baja;
    A.Num2[1] = a_mitad_alta;

    // Generamos B
    unsigned long long b_parte_alta1 = rand();
    unsigned long long b_parte_baja1 = rand();
    unsigned long long b_mitad_baja = b_parte_alta1<<32 | b_parte_baja1;

    unsigned long long b_parte_alta2 = rand();
    unsigned long long b_parte_baja2 = rand();
    unsigned long long b_mitad_alta = (b_parte_alta2<<32 | b_parte_baja2) | (1ULL << 63);

    B.Num2[0] = b_mitad_baja;
    B.Num2[1] = b_mitad_alta;

    //imprimimos los 2 enteros de 128 bits en su forma low y high, y el resultado de la multiplicación de 256 bits por columnas de 64 bits
    printf("A (high: %llu, low: %llu)\n", A.Num2[1], A.Num2[0]);
    printf("B (high: %llu, low: %llu)\n", B.Num2[1], B.Num2[0]);

    mul128(A, B, result);
    printf("Resultado (col3 col2 col1 col0): %llu %llu %llu %llu\n", result[3], result[2], result[1], result[0]);

    return 0;
};