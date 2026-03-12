#include "solveur_vf.h"
#include <iostream>
#include <fstream>
using namespace std;
using namespace Eigen;

// ---------------------------------------Constructeur-----------------------------------

Solveur_VF::Solveur_VF(const Maillage& maillage, const std::string& filename, double t_init, double coef_cfl, double t_final, bool use_muscl)
        : _maillage(maillage), _solveur_flux(Solveur_Flux(_maillage)), _filename(filename), _t(t_init), 
            _dx(maillage.getDeltaX()), _t_final(t_final), _coef_cfl(coef_cfl), _use_muscl(use_muscl)
{
}

// ---------------------------------------un pas de temps-----------------------------------

void Solveur_VF::avancerTemps()
{
    // accès direct au maillage
    vector<maille> & mailles = _maillage.getMailles();
    size_t n = mailles.size();

    // copie pour stocker l'état à n+1
    vector<maille> nouvelles_mailles = mailles;

    // ================== 1) calcul de dt (CFL) ==================
    double g = 9.81;
    double max_de = 0.0;
    const double eps_h = 1e-10;

    for (size_t i = 0; i < n - 1; ++i) {
        double hg = max(mailles[i].getHauteur(), eps_h);
        double hd = max(mailles[i+1].getHauteur(), eps_h);

        double ug = mailles[i].getDebit()   / hg;
        double ud = mailles[i+1].getDebit() / hd;

        double de = max(abs(ug) + sqrt(g * hg),
                        abs(ud) + sqrt(g * hd));

        if (isfinite(de) && de > max_de) {
            max_de = de;
        }
    }

    double dt;
    if (!(max_de > 0.0) || !isfinite(max_de)) {
        cerr << "max_de invalide -> dt = 1e-6" << endl;
        dt = 1e-6;
    } else {
        dt = _coef_cfl * (_dx / max_de);
    }

    // ajustement pour t_final
    if (_t + dt > _t_final) {
        dt = _t_final - _t;
    }

    // si dt trop petit, on sort pour éviter de boucler à l'infini
    if (dt <= 1e-14) {
        cerr << "dt trop petit (" << dt << "), on force t = t_final" << endl;
        _t = _t_final;
        return;
    }

    // ================== 2) conditions aux bords (fantômes) ==================
    // bords transmissifs simples
    mailles[0].setHauteur( mailles[1].getHauteur() );
    mailles[0].setDebit(   mailles[1].getDebit()   );
    mailles[n-1].setHauteur( mailles[n-2].getHauteur() );
    mailles[n-1].setDebit(   mailles[n-2].getDebit()   );

    // ================== 3) calcul des flux et mise à jour ==================
    Eigen::Vector2d flux_gauche;
    Eigen::Vector2d flux_droite;

    // flux à l'interface 0|1 (toujours ordre 1)
    flux_gauche = _solveur_flux.calculerFlux(mailles[0], mailles[1]);

    // cellules internes : i = 1 .. n-2
    for (size_t i = 1; i < n - 1; ++i) {

        // flux à droite : interface i|i+1
        if (_use_muscl && i >= 1 && i + 2 < n) {
            // flux MUSCL limité (ordre 2)
            flux_droite = _solveur_flux.calculerFluxLimite((int)i, true);
        } else {
            // flux ordre 1
            flux_droite = _solveur_flux.calculerFlux(mailles[i], mailles[i+1]);
        }

        // schéma volumes finis : U_i^{n+1} = U_i^n - dt/dx (F_{i+1/2} - F_{i-1/2})
        double hnew = mailles[i].getHauteur()
                      - (dt / _dx) * (flux_droite(0) - flux_gauche(0));
        double qnew = mailles[i].getDebit()
                      - (dt / _dx) * (flux_droite(1) - flux_gauche(1));

        // sécurité : positivité et NaN
        if (!isfinite(hnew) || hnew < eps_h) {
            hnew = eps_h;
            qnew = 0.0;
        }
        if (!isfinite(qnew)) {
            qnew = 0.0;
        }

        nouvelles_mailles[i].setHauteur(hnew);
        nouvelles_mailles[i].setDebit(qnew);

        // pour la maille suivante, le flux de droite devient le flux de gauche
        flux_gauche = flux_droite;
    }

    // ================== 4) conditions aux bords sur le nouvel état ==================
    nouvelles_mailles[0].setHauteur( nouvelles_mailles[1].getHauteur() );
    nouvelles_mailles[0].setDebit(   nouvelles_mailles[1].getDebit()   );
    nouvelles_mailles[n-1].setHauteur( nouvelles_mailles[n-2].getHauteur() );
    nouvelles_mailles[n-1].setDebit(   nouvelles_mailles[n-2].getDebit()   );

    // ================== 5) remplacement de l'ancien état ==================
    _maillage.setMailles(nouvelles_mailles);
    _t += dt;
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