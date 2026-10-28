#include <array>
#include <iomanip>
#include <iostream>
#include <string>
#include <vector>

#include "evolution_algo.hpp"

//funcion main de la implementacion del algoritmo evolutivo

int main(int argc, char* argv[])
{
    unsigned semilla = (argc > 1) ? std::stoul(argv[1]) : 2026u;
    rng.seed(semilla);
    std::cout << "Semilla: " << semilla << "\n\n";

    auto pob = inicializar_poblacion();
    ordenar(pob);
    reportar(0,pob);

    for (int g=1; g<= GENERACIONES; ++g) {
        nueva_generacion(pob);
        if (g % IMPRIME_CADA == 0) reportar(g, pob);
    }

    std::cout << "\nMejor individuo:\n";
    imprimir(pob.front());
    return 0;
}