#ifndef SOLVEUR_VF_H
#define SOLVEUR_VF_H

#include <Eigen/Dense>
#include "solveur_flux.h"
#include <vector>
#include "limiteur.h"

class Solveur_VF { // classe qui va effectuer la mise à jour du maillage à chaque pas de temps
private:  
    Maillage _maillage; 
    Solveur_Flux _solveur_flux;
    double _dx;
    double _t;
    double _t_final;
    double _coef_cfl ; // pour la condition de stabilité CFL, on a dt = coef_cfl * dtmax
    std::string _filename;  // on va faire des sorties en csv
    bool _use_muscl = false; // pour activer ou non le schéma MUSCL (limiteur de Van der Leer)
public:
    Solveur_VF(const Maillage& maillage, const std::string& filename, const double t_init, double coef_cfl, double t_final, bool use_muscl = false);
    void avancerTemps(); //avancer le maillage d'un pas de temps dt recalculer afin de respecter la cfl ( on programme en EE pour l'instant, si possible faire en RK2 ou RK4)
    void sauvegarde_etat(int iteration); //sauvegarder l'état du maillage dans un fichier, faire en sorte d'avoir un fichier par pas de temps
    double Get_T() const;
    Maillage& Get_Maillage() { return _maillage; }
};  
#endif // SOLVEUR_VF_H