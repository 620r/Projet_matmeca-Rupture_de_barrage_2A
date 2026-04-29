#include "maillage.h"
#include "parametres.h"

#pragma once
#include <Eigen/Dense>
#include <string>


class Solution {
private:
    int nb_mailles;
    double g = 9.81;

public:
    // --- Constructeur
    Solution(int nb_mailles);

    // --- Données publiques accessibles
    Eigen::MatrixXd Un, Unp1;   //Matrice solution (nb_mailles, 3) pour h et qx et qy
    Eigen::MatrixXd F;          //Matrice flux (nb_mailles, 3), projection sur x
    Eigen::MatrixXd G;          //Matrice flux (nb_mailles, 3), projection sur y
    Eigen::VectorXd b;          //Vecteur valeurs propres
    Eigen::VectorXd Flux_num;
    
    // --- Fonctions publiques accessibles
    void initialisation_Un(const Maillage& m, const Parametres& p);
    void calcul_maille(const Maillage& m);
    void calcul_FG(double h, double qx, double qy, double Ff[3], double Gg[3]);
    void calcul_flux_bord(const int k, double nx, double ny, int cl, double h_reservoir);
    void calcul_flux(const int k, const int l, double nx, double ny);
    void maj_Un();
};
