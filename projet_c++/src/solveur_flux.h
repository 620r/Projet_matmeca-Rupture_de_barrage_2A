#ifndef SOLVEUR_FLUX_H
#define SOLVEUR_FLUX_H


#include <Eigen/Dense>
#include "maillage.h"
#include <vector>   
#include "limiteur.h"


class Solveur_Flux { // on va faire un flux Rusanov pour calculer les flux dans le maillage
private:
    Maillage& _maillage; // on va reprendre la valeur du maillage à chaque pas de temps, afin de calculer les nouveaux flux
public:
    Solveur_Flux(Maillage& maillage) : _maillage(maillage) {} //constucteur
    Eigen::Vector2d calculerFlux(const maille& m1, const maille& m2); // calcule le flux entre les mailles m1 et m2 ordre 1
    Eigen::Vector2d calculerFluxOrdre2(int i); // calcule le flux entre les mailles m1 et m2 ordre 2
    Eigen::Vector2d calculerFluxLimite(int i, bool use_muscl = false); // calcule le flux entre les mailles m1 et m2 limité van Leer
    void afficherFlux(); // affiche les flux pour le contrôle
};

#endif // SOLVEUR_FLUX_H