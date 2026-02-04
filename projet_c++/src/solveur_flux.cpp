#include "solveur_flux.h"
#include <iostream>
using namespace std;
using namespace Eigen;  



Eigen::Vector2d Solveur_Flux::calculerFlux(const maille& m1, const maille& m2)
{
    Eigen::Vector2d flux;
    double hg = m1.getHauteur();
    double hd = m2.getHauteur();
    double qg = m1.getDebit();
    double qd = m2.getDebit();
    double ug = m1.calculerVitesse();
    double ud = m2.calculerVitesse();
    double g = 9.81;

    // Rusanov flux
    double de = max(abs(ug) + sqrt(g*hg), abs(ud) + sqrt(g*hd)); // calcul du coefficient de diffusion comme dans les notes
    
    flux(0) = 0.5 * (qg + qd) - 0.5 * de * (hd - hg); // flux de hauteur
    flux(1) = 0.5 * (qg*ug + 0.5*g*hg*hg + qd*ud + 0.5*g*hd*hd) - 0.5 * de * (qd - qg); // flux de debit
    
    return flux;
}

void Solveur_Flux::afficherFlux() 
{
    const std::vector<maille>& mailles = _maillage.getMailles();
    for (size_t i = 0; i < mailles.size() - 1; ++i) {
        Eigen::Vector2d flux = calculerFlux(mailles[i], mailles[i + 1]);
        cout << "Flux entre la maille " << i << " et la maille " << i + 1 << ": ";
        cout << "Hauteur: " << flux(0) << ", Debit: " << flux(1) << endl;
    }
}

