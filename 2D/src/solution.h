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
    Eigen::MatrixXd Un, Unp1;   //Matrice solution (nb_mailles, 2) pour q et h
    Eigen::MatrixXd F;          //Matrice flux (nb_mailles, 2) 
    Eigen::VectorXd b;          //Vecteur valeurs propres
    
    // --- Fonctions publiques accessibles
    void initialisation_Un(const Maillage& m, const Parametres& p);
    void maj_maille(const Maillage& m);

};
