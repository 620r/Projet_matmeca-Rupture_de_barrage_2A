#pragma once
#include <Eigen/Dense>
#include <string>


class Parametres {
public:
    // --- Constructeur
    Parametres() = default;

    // --- Données publiques accessibles
    double hG, hD;            //hauteurs d'eau à t0
    double tmax, t_pas, CFL;         //paramètres temporels
    double x_lim;             //parametre spatial 
    std::string fichier_maillage;  //fichier.mesh contenant le maillage
    
    // --- Fonctions publiques accessibles
    void lecture_fichier(const std::string& filename);
    void print_donnees_initiales();
};
