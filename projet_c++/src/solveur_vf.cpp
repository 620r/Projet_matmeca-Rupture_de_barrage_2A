#include "solveur_vf.h"
#include <iostream>
#include <fstream>
using namespace std;
using namespace Eigen;

// ---------------------------------------Constructeur-----------------------------------

Solveur_VF::Solveur_VF(const Maillage& maillage, const std::string& filename, double t_init, double coef_cfl, double t_final)
        : _maillage(maillage), _solveur_flux(Solveur_Flux(_maillage)), _filename(filename), _t(t_init), 
            _dx(maillage.getDeltaX()), _t_final(t_final), _coef_cfl(coef_cfl), _use_muscl(use_muscl)
{
}

// ---------------------------------------un pas de temps-----------------------------------

void Solveur_VF::avancerTemps()
{
    vector<maille> mailles = this->_maillage.getMailles();
    vector<maille> nouvelles_mailles = mailles;
    size_t n = mailles.size();

    //on doit recalculer dt à chaque iteration en temps en fonction de be car la condition de stabilité CFL varie
    
    double dt;
    double max_de = 0.0;
    double g = 9.81;
    for (size_t i = 0; i < n - 1; ++i) {
        double hg = mailles[i].getHauteur();//hauteur dans la maille i
        double hd = mailles[i+1].getHauteur();//hauteur dans la maille i+1
        double ud = mailles[i+1].calculerVitesse(); // vitesse dans la maille i+1
        double ug = mailles[i].calculerVitesse(); // vitesse dans la maille i
        double de = max(abs(ug) + sqrt(hg*g), abs(ud) + sqrt(hd*g));
        if (de > max_de) {
            max_de = de;  // on cherche le max de de pour le calcul de dt
        }

    
    }
    // calcul de dt (CFL)
    if (max_de > 0.0) {
        dt = _coef_cfl * (_dx / max_de);
    } else {
        dt = 1;
        cout << "attention pb avec dt ou de"<< endl;
    }

    //  pour atteindre exactement t_final
    if (_t + dt > _t_final) {
        dt = _t_final - _t;
    }

    //cout << "Debug: dt calculé = " << dt << ", max_de = " << max_de << endl;



    // // on implémente ici un pb avec des conditions de flux au bord nulles

    


    // Mettre à jour les fantômes pour le calcul des flux 
    mailles[0].setHauteur(mailles[1].getHauteur());
    mailles[0].setDebit(mailles[1].getDebit());
    mailles[n-1].setHauteur(mailles[n-2].getHauteur());
    mailles[n-1].setDebit(mailles[n-2].getDebit());

    Eigen::Vector2d flux_droite(0.0, 0.0);
    Eigen::Vector2d flux_gauche(0.0, 0.0);

    // Flux à gauche (interface 0|1)
    flux_gauche = _solveur_flux.calculerFlux(mailles[0], mailles[1]);

    for (size_t i = 1 ; i < n - 1; ++i) {  // i = 1..n-2 (cellules internes)

        //excusion des bords
        if (_use_muscl .AND. i >= 1 .AND. i + 2 < n) {
            flux_droite = _solveur_flux.calculerFluxLimite(i, _use_muscl);
        } else {
            flux_droite = _solveur_flux.calculerFlux(mailles[i], mailles[i + 1]);
        }
    }

    // Fantômes transmis pour le nouvel état
    nouvelles_mailles[0].setHauteur(nouvelles_mailles[1].getHauteur());
    nouvelles_mailles[0].setDebit(nouvelles_mailles[1].getDebit());
    nouvelles_mailles[n-1].setHauteur(nouvelles_mailles[n-2].getHauteur());
    nouvelles_mailles[n-1].setDebit(nouvelles_mailles[n-2].getDebit());

    this->_maillage.setMailles(nouvelles_mailles);
    this->_t += dt;
}



void Solveur_VF::sauvegarde_etat(int iteration)
{
    // Créer le dossier output s'il n'existe pas
    system("mkdir -p output");

    ofstream file;
    // ecrire dans un fichier csv, dans un dossier output
    string filename_iter = "output/" +  this->_filename + "_iter_" + to_string(iteration) +"_t_" + to_string(this->_t ) + ".csv";
    file.open(filename_iter);
    if (!file.is_open()) {
        cerr << "Erreur lors de l'ouverture du fichier " << filename_iter << " pour l'écriture." << endl;
        return;
    }
    const vector<maille>& mailles = this->_maillage.getMailles();
    file << "Position,Hauteur,Debit,Vitesse\n";
    int nb_mailles = mailles.size();
    for (int i = 1; i < nb_mailles - 1; ++i) { // exclut 0 et n-1 (fantômes)
        const auto& m = mailles[i];
        file << m.getPosition() << "," << m.getHauteur() << "," << m.getDebit() << "," << m.calculerVitesse() << "\n";
    }
    file.close();
}

double Solveur_VF::Get_T() const
{
    return this->_t;
}