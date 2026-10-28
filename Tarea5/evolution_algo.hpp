// Algoritmo gen+etico binario para minimizar la función de Rastrigin en 2 variables
// cada una codificada en un entero sin signo de 32 bits
#pragma once

#include <iostream>
#include <vector>
#include <random>
#include <algorithm>
#include <array>
#include <bitset>
#include <cmath>
#include <limits>
#include <cstdint>
#include <iomanip>
#include <utility>
#include <numbers>

// Definicion de los parámetros del algoritmo

using Gen = std::uint32_t; // Tipo de dato para representar un gen (entero sin signo de 32 bits)

constexpr int N_VAR= 2; // x,y
constexpr int L = 32;   // bits por variable
constexpr Gen GEN_MAX = std::numeric_limits<Gen>::max();
constexpr double A_INF = -5.12; // limite inferior del dominio de la función
constexpr double B_SUP = 5.12;  // limite superior del dominio de la función

constexpr int MU = 50; // tamaño de la población
constexpr int LAMBDA = MU  ; // número de hijos por generación
constexpr int GENERACIONES = 50; // número de generaciones
constexpr double P_CRUCE = 0.9; // probabilidad de cruce
constexpr double P_MUTACION = 0.1; // probabilidad de mutación por bit
constexpr int IMPRIME_CADA = 10; // imprimir cada n generaciones
constexpr bool DETALLE = false; // imprimir detalle de cada generación  

// 1) Deifinimos los individuos
struct Individuo {
    std::array<Gen, N_VAR> gen{}; // genotipo (lo que se hereda)
    std::array<double, N_VAR> var{}; // fenotipo (x,y)
    double f=0.0; // aptitud, se minimiza
};

// 2) Generamos una población aleatoria

std::mt19937 rng; // Generador de numeros aleatorios llamado rng

double aleatorio01()
{
    static std::uniform_real_distribution<double> U(0.0,1.0);
    return U(rng);
}

int aleatorio_entero(int a, int b)
{
    std::uniform_int_distribution<int> D(a,b);
    return D(rng);
}

Gen gen_aleatorio()
{
    static std::uniform_int_distribution<Gen> D(0, GEN_MAX);
    return D(rng);
}

// Formula de decodificacion de binarios a reales
double decodificar(Gen g)
{
    return A_INF + (static_cast<double>(g)/GEN_MAX)*(B_SUP - A_INF);
}

// Funcion Rastrigin
double rastrigin (const std::array<double,N_VAR>& v)
{
    double s= 10.0 * N_VAR;
    for (double xi : v){
        s += xi*xi -10.0*std::cos(2.0*std::numbers::pi*xi);
    }
    return s;
}

// Funcion evaluar: traduce sus genes y aplica Rastrigin
void evaluar(Individuo& ind)
{
    for (int k=0; k < N_VAR; ++k) {
        ind.var[k] = decodificar(ind.gen[k]);
    }
    ind.f = rastrigin(ind.var);
}

// Población incial 
std::vector<Individuo> inicializar_poblacion()
{
    std::vector<Individuo> pob (MU);
    for (auto& ind : pob) {
        for (auto& gg : ind.gen) gg= gen_aleatorio();
        evaluar(ind);
    }
    return pob;
}

// 3) Cruza (seleccion de dos individuos de forma aleatoria)
std::pair<int,int> seleccionar_padres()
{
    int i = aleatorio_entero(0,MU-1);
    int j;
    do { j= aleatorio_entero(0,MU-1);} while (j==i);
    return {i,j};
}

// Corte aleatorio dentro de cada variable, almacena el punto de corte
//usado en la variable
std::pair<Individuo,Individuo> cruzar (const Individuo& p1, const Individuo& p2, std::array<int,N_VAR>& cortes)
{
    Individuo h1= p1, h2= p2;
    cortes.fill(0);
    if (aleatorio01()< P_CRUCE){
        for (int k=0; k< N_VAR; ++k) {
            int c = aleatorio_entero(1,L-1);    //punto de corte "c"
            cortes[k]=c;
            Gen m = (Gen{1} << (L-c)) - 1;
            h1.gen[k] = (p1.gen[k] & ~m ) | (p2.gen[k] & m);
            h2.gen[k] = (p2.gen[k]& ~m) | (p1.gen[k] & m);
        }
    }
    return {h1,h2};
}

// 4) Mutacion a hijos
int mutar (Individuo& h)
{
    int volteados = 0;
    for (int k=0; k< N_VAR; ++k)
        for (int i=0; i<L; ++i){
            if (aleatorio01()<P_MUTACION) {
                h.gen[k] ^= (Gen{1} << i);
                ++volteados;
            
        }
    }
    return volteados;
}

// 6) Ordenar población (se eliminan los menos aptos)
// Los mas aptos son los que minimizan la función 
void ordenar(std::vector<Individuo>& pob)
{
    std::sort(pob.begin(), pob.end(),
            [](const Individuo& a, const Individuo& b){ return a.f<b.f;});
}

void reemplazar(std::vector<Individuo>& pob, std::vector<Individuo>& hijos)
{
    pob.insert(pob.end(), hijos.begin(),hijos.end()); //2.MU Individuos
    ordenar(pob);
    pob.resize(MU);
}

// Nueva generacion ( con la funciones anteriormente creadas )
void nueva_generacion(std::vector<Individuo>& pob)
{
    std::vector<Individuo> hijos;
    hijos.reserve(LAMBDA);

    while (static_cast<int>(hijos.size())<LAMBDA){
        auto [i,j] = seleccionar_padres();
        std::array<int, N_VAR> cortes{};
        auto [h1,h2] = cruzar(pob[i], pob[j], cortes);
        mutar(h1);
        mutar(h2);
        evaluar(h1);
        evaluar(h2);
        hijos.push_back(h1);
        hijos.push_back(h2);
        }
    reemplazar(pob, hijos);
}


// Impresión y reporte del resultado final
void imprimir(const Individuo& ind)
{
    std::cout << std::bitset<L> (ind.gen[0])<< '|' << std::bitset<L>(ind.gen[1])
    << std::scientific << std::setprecision(4)
    << " x=" << std::setw(11) << ind.var[0]
    << " y=" << std::setw(11) << ind.var[1]
    << " f=" << ind.f << '\n';
}

void reportar(int gen, const std::vector<Individuo>& pob)
{
    double suma=0.0;
    for (const auto& ind : pob) suma += ind.f;
    std::cout << "gen " << std::setw(4) << gen
    << std::scientific << std::setprecision(4)
    << " mejor f= " << pob.front().f
    << " promedio f= " << suma/pob.size() << '\n';
}
