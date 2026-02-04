#include "solution_exacte.h"
#include <iostream>
#include <fstream>  

using namespace std;
using namespace Eigen;




double Solution_Exacte::F (double h){  // fonction f à anuller sur la partie plate de l'onde
    double A, resultat;
    double g , hd , hg;
    g = this->_g;
    hd = this->_hd;
    hg = this-> _hg;


    A = sqrt((g*(hd + h))/(2.*hd*h));
    resultat = 2.*(sqrt(g*hg)-sqrt(g*h)) - (h - hd) * A;
    return resultat;
}

double Solution_Exacte::F_prime(double h) // et la dérivée de la fonction f 
{
    double A, resultat;
    double g , hd ;
    g = this->_g;
    hd = this->_hd;



    A = sqrt((g*(hd + h))/(2.*hd*h));
    resultat = -sqrt(g/h) - A + ((sqrt(g)*sqrt(hd) * (h-hd))/ (2. * sqrt(2.) *(pow(h,3./2.)) * sqrt(h+hd)) );
    return resultat;
}






double Solution_Exacte::newton(double h0, double tol, int max_iter) //méthode de recherche de racine de la fonction f avec Newton
{
    double h = h0;
    double tolerance = tol;
    int imax = max_iter;
    int i = 0;
    
    //std::cout << "  Newton: h_init=" << h << std::endl;
    
    while ((i<imax) && (abs(F(h)) > tolerance) && (h > 0.) && (h < 10000*this->_hg)) 
    // condition d'arrêt :: nombre max d'itérations, valeur absolue de f(h) inférieure à la tolérance, h positive et pas trop grande
    {
        double f_val = F(h);
        double df_val = F_prime(h);
        
        // if (i < 3) {  // Afficher les 3 premières itérations
        //     //std::cout << "    iter " << i << ": h=" << h << ", F(h)=" << f_val 
        //               //<< ", F'(h)=" << df_val << std::endl;
        // }
        
        h = h - (f_val/df_val);
        i = i+1;
    }
    
    //std::cout << "  Newton convergé après " << i << " itérations: h=" << h << std::endl;
    
    return h;
}





double Solution_Exacte::U_pente(double x, double t)// calcule de la vitesse dans la pente
{
    double g = this->_g;
    double hg = this->_hg;
    
 
    double u_pente_sans_x = (2./3.) * sqrt(g*hg);
    double u_pente = u_pente_sans_x + (x/t) * (2./3.);
    
    return u_pente;
}

double Solution_Exacte::H_pente(double x, double t) // calcule de la hauteur dans la pente
{
    double g = this->_g;
    double hg = this->_hg;

    double h_pente = (1./(9.*g)) * pow((-x/t) + 2.*sqrt(g*hg), 2);
    
    return h_pente;
}




void Solution_Exacte::calculerSolutionExacte(double temps)
{
    //std::cout << "\n========================================" << std::endl;
    //std::cout << "CALCUL SOLUTION EXACTE à t = " << temps << std::endl;
    //std::cout << "========================================" << std::endl;
    
    double h_star, u_star;
    double hg = this->_hg;
    double hd = this->_hd;
    double g = this->_g;    

    //std::cout << "Paramètres: hg=" << hg << ", hd=" << hd << ", g=" << g << std::endl;

    // CAS SPÉCIAL : hg = hd
    if (abs(hg - hd) < 1e-10) {
        int N = this->_maillage.getNbSubdivisions();
        for (int i = 1; i <= N; ++i) {  // mettre à jour les mailles internes [1..N]
            maille m = this->_maillage.getMailles(i);
            m.setHauteur(hg);
            m.setDebit(0.0);
            this->_maillage.setMailles(i, m);
        }
        return;
    }


    // CAS GÉNÉRAL

    double h0 = hg;
    h_star = this->newton(h0, 1e-6, 100); // on cherche la racine de la fonction f avec Newton
    
    //std::cout << "Après Newton: h* = " << h_star << std::endl;
    //std::cout << "F(h*) = " << F(h_star) << std::endl;
    
    u_star = 2.*(sqrt(g*hg) - sqrt(g*h_star));
    
    //std::cout << "u* = " << u_star << std::endl;

    // calcul des positions des mailles
    
    double sigma1 = -sqrt(g*hg); 
    double sigma2 = (u_star - sqrt(g*h_star));
    double sigma3 = -h_star*u_star/(hd - h_star);
    
    //std::cout << "Vitesses: σ1=" << sigma1 << ", σ2=" << sigma2 << ", σ3=" << sigma3 << std::endl;
    
    double x1 = sigma1*temps;
    double x2 = sigma2*temps;
    double x3 = sigma3*temps;

    // enregistrer les positions caractéristiques
    this->_x1 = x1;
    this->_x2 = x2;
    this->_x3 = x3;



    
    //std::cout << "Positions: x1=" << x1 << ", x2=" << x2 << ", x3=" << x3 << std::endl;
    //std::cout << "Domaine du maillage: [" << this->_maillage.getMailles(0).getPosition() 
              //<< ", " << this->_maillage.getMailles(this->_maillage.getNbSubdivisions()-1).getPosition() << "]" << std::endl;

    int N = this->_maillage.getNbSubdivisions();
    int count[4] = {0, 0, 0, 0};

    for (int i = 1; i <= N; ++i) {  // couvre toutes les mailles internes [1..N]
        maille m = this->_maillage.getMailles(i);
        double x = m.getPosition();
        double h_exacte, u_exacte;
        
        if (x < x1) { // zone gauche constante
            h_exacte = hg;
            u_exacte = 0.;
            count[0]++;
        }
        else if (x >= x1 && x < x2) { // zone intermédiaire 
            h_exacte = this->H_pente(x, temps);
            u_exacte = this->U_pente(x, temps);
            count[1]++;
            
            // Afficher quelques valeurs pour débugger
            if (count[1] <= 3) {
                //std::cout << "  Zone détente [" << count[1] << "]: x=" << x 
                          //<< " → h=" << h_exacte << ", u=" << u_exacte << std::endl;
            }
        }
        else if (x >= x2 && x < x3) {  // zone plate de l'onde
            h_exacte = h_star;
            u_exacte = u_star;
            count[2]++;
            
           
        }
        else {  // zone droite constante
            h_exacte = hd;
            u_exacte = 0.;
            count[3]++;
            
            
        }
        
        m.setHauteur(h_exacte);    
        m.setDebit(h_exacte * u_exacte);
        this->_maillage.setMailles(i, m);
    }

    // mettre à jour les fantômes (transmissif)
    this->_maillage.setMailles(0,  this->_maillage.getMailles(1));
    this->_maillage.setMailles(N+1, this->_maillage.getMailles(N));

    // Test de cohérence à x2

    // std::cout << "Répartition: Zone1(gauche)=" << count[0] 
    //           << ", Zone2(détente)=" << count[1] 
    //           << ", Zone3(plateau)=" << count[2] 
    //           << ", Zone4(droite)=" << count[3] << std::endl;
    // std::cout << "========================================\n" << std::endl;
}



void Solution_Exacte::save_sol_exacte(int iteration, double temps) const
{
    system("mkdir -p output_exacte");
    ofstream file;
    string filename_iter = "output_exacte/" +  this->_filename + "_iter_" + to_string(iteration) +"_t_" + to_string(temps) + ".csv";
    file.open(filename_iter);
    if (!file.is_open()) {
        cerr << "Erreur lors de l'ouverture du fichier " << filename_iter << " pour l'écriture." << endl;
        return;
    }
    const vector<maille>& mailles = this->_maillage.getMailles();
    file << "Position,Hauteur,Debit,Vitesse\n";
    int nb_mailles = mailles.size();
    for (int i = 1; i < nb_mailles - 1; ++i) { // exclure les fantômes 0 et N+1
        const auto& m = mailles[i];
        file << m.getPosition() << "," << m.getHauteur() << "," << m.getDebit() << "," << m.calculerVitesse() << "\n";
    }
    file.close();
}
