#include "solveur_flux.h"
#include <iostream>
#include <algorithm>
#include <cmath>
using namespace std;
using namespace Eigen;  


// Flux ordre 1 (Rusanov)
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
//------------------------------------------------------------------------------

// Flux ordre 2 pour interface i+1/2 (entre maille m1 et m2)
Eigen::Vector2d Solveur_Flux::calculerFluxOrdre2(int i)
{
    const std::vector<maille>& m = _maillage.getMailles();
    
    // Reconstruction linéaire non limitée
    // U_L = U_i + 0.5*(U_i - U_{i-1})  on calcul les valeurs reconstruites ie avec des polynomes de degré 1
    double hL = m[i].getHauteur() + 0.5 * (m[i].getHauteur() - m[i-1].getHauteur());
    double qL = m[i].getDebit()   + 0.5 * (m[i].getDebit()   - m[i-1].getDebit());
    
    // U_R = U_{i+1} - 0.5*(U_{i+2} - U_{i+1}) on calcul les valeurs reconstruites ie avec des polynomes de degré 1
    double hR = m[i+1].getHauteur() - 0.5 * (m[i+2].getHauteur() - m[i+1].getHauteur());
    double qR = m[i+1].getDebit()   - 0.5 * (m[i+2].getDebit()   - m[i+1].getDebit());
    
    // Sécurité: hauteur positive
    hL = std::max(hL, 1e-12);
    hR = std::max(hR, 1e-12);
    
    // États reconstruits temporaires afin de calculer le flux de Rusanov
    maille UL(0.0, hL, qL);
    maille UR(0.0, hR, qR);
    
    return calculerFlux(UL, UR);  // Réutilise Rusanov
}   
//------------------------------------------------------------------------------

// Flux limité van Leer pour interface i+1/2
Eigen::Vector2d Solveur_Flux::calculerFluxLimite(int i, bool use_muscl)
{
    if (use_muscl == 1) {
        // Mode ordre 1
        const std::vector<maille>& mailles = _maillage.getMailles();
        return calculerFlux(mailles[i], mailles[i+1]);
    }
    
    const std::vector<maille>& mailles = _maillage.getMailles();
    const double eps = 1e-12;
    
    // F^(1)_{i+1/2}
    Eigen::Vector2d F1 = calculerFlux(mailles[i], mailles[i+1]);
    
    // F^(2)_{i+1/2}
    Eigen::Vector2d F2 = calculerFluxOrdre2(i);
    
    // Calcul de theta = (U_{i+1} - U_i) / (U_i - U_{i-1})
    // Pour h:
    double num_h = mailles[i+1].getHauteur() - mailles[i].getHauteur();
    double den_h = mailles[i].getHauteur()   - mailles[i-1].getHauteur();
    
    // Éviter division par zéro
    if (abs(den_h) < eps) {
        if (den_h >= 0) {
            den_h = eps;
        } else {
            den_h = -eps;
        }
    }
    
    double theta_h = num_h / den_h;

    // Pour q:
    double num_q = mailles[i+1].getDebit() - mailles[i].getDebit();
    double den_q = mailles[i].getDebit()   - mailles[i-1].getDebit();
    // Éviter division par zéro
    if (abs(den_q) < eps) {
        if (den_q >= 0) {
            den_q = eps;
        } else {
            den_q = -eps;
        }
    }
    
    double theta_q = num_q / den_q;


    // Limiteur van Leer
    double phi_h = Limiteur::phiVanLeer(theta_h);
    double phi_q = Limiteur::phiVanLeer(theta_q);
    
    // F = F^(1) + phi(theta) * (F^(2) - F^(1)) suivant formule
    Eigen::Vector2d F;
    F(0) = F1(0) + phi_h * (F2(0) - F1(0));
    F(1) = F1(1) + phi_q * (F2(1) - F1(1));
    
    return F; // retourne le flux final limité van Leer
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

