#include "solveur_vf.h"
#include "solution_exacte.h"
#include "erreur.h"

#include <string>
#include <iostream>
#include <fstream>
#include <vector>
#include <ctime>


using namespace std;
using namespace Eigen;

int main(int argc, char ** argv) // les arguments sont le fichier d'entrée des conditions initiales et le fichier des paramètres du solveur
{

    // on va faire une lecture de deux fichiers: un pour les conditions initiales et un pour les paramètres du solveur

    // ----- vérification du nombre d'arguments -----
    if (argc != 3)
    {
        cerr << "fichier d'entrée: " << argv[0] << " <input_file> <solver_params_file>" << endl;
        return 1;
    }


    // ----- lecture du fichier d'entrée de condition initiale -----


    string string_cond_init = argv[1];
    ifstream input_cond_init(string_cond_init);
    if (!input_cond_init.is_open()) {  // si le fichier n'est pas ouvert, on l'affiche et on quitte
        cerr << "Error opening file: " << string_cond_init << endl;
        return 1; 
    }


    double longueur_gauche, longueur_droite, hgauche, hdroite;
    int nb_subdivisions;


    input_cond_init >> longueur_gauche >> longueur_droite >> nb_subdivisions >> hgauche >> hdroite;
    input_cond_init.close();


    cout <<"----------------------------"<< endl;
    cout << "Conditions initiales lues: longueur_gauche=" << longueur_gauche << ", longueur_droite=" << longueur_droite << ", nb_subdivisions=" << nb_subdivisions << ", hgauche=" << hgauche << ", hdroite=" << hdroite << endl;
    cout <<"----------------------------"<< endl;


    Maillage maillage(longueur_gauche, longueur_droite, nb_subdivisions, hgauche, hdroite);
    Maillage maillage_exacte(longueur_gauche, longueur_droite, nb_subdivisions, hgauche, hdroite);
    cout<<"Maillage créé." << endl;





//     cout << "Test d'accès aux mailles..." << endl;
// try {
//     if (maillage.getMailles().size() != nb_subdivisions+2) {
//         cerr << "Erreur: taille incorrecte du vecteur de mailles" << endl;
//         return 1;
//     }
//     cout << "Taille du vecteur de mailles: " << maillage.getMailles().size() << endl;
// } catch (const exception& e) {
//     cerr << "Exception lors de l'accès aux mailles: " << e.what() << endl;
//     return 1;
// }

// cout << "Test d'accès réussi, lecture du fichier solveur..." << endl;





//-----lecture du fichier d'entrée des paramètres du solveur-----


    string string_solver_params = argv[2];
    cout <<"Lecture des paramètres du solveur depuis le fichier: " << string_solver_params << endl;


    ifstream input_solver_params(string_solver_params);
    if (!input_solver_params.is_open()) {// si le fichier n'est pas ouvert, on l'affiche et on quitte
        cerr << "Error opening file: " << string_solver_params << endl;
        return 1; 
    }

    cout <<"----------------------------"<< endl;


    double t_init,t_final, coef_cfl;
    string output_filename;
    int int_sortie;
    int use_muscl_int = 1; // 1: ordre 1, 2: ordre 2
    

    input_solver_params >> t_init >> t_final  >> coef_cfl >> output_filename >> int_sortie >> use_muscl_int;
    //cout<<"Paramètres du solveur lus." << endl;
    input_solver_params.close();

    bool use_muscl = (use_muscl_int != 1); // 1: ordre 1, 2: ordre 2

    if (use_muscl) {
        cout << "MODE MUSCL ORDRE 2 ACTIVÉ" << endl;
    } else {
        cout << "MODE ORDRE 1" << endl;
    }
    
    
    cout <<"----------------------------"<< endl;   
    cout << "Paramètres du solveur lus: t_init=" << t_init << ", t_final=" << t_final << ", coef_cfl=" << coef_cfl << ", output_filename=" << output_filename << endl;
    cout <<"----------------------------"<< endl;

    // -----------------------------------------------------------------------------------
    // ------------------ fin de lecture des paramètres du solveur------------------------
    // -----------------------------------------------------------------------------------



    // ---- création du maillage et de la solution exacte ----
    Solveur_VF solveur(maillage, output_filename, t_init, coef_cfl, t_final, use_muscl);

    Solution_Exacte solution_exacte(maillage_exacte, hgauche, hdroite, output_filename+"_exacte");

    
    double pas_sortie = (t_final-t_init)/int_sortie;


    //test du maillage
    //maillage.afficherMaillage();


// for (int i = 0; i < nb_subdivisions; i++)
// {
//     cout << maillage.getMailles()[i].getPosition() << " " << maillage.getMailles()[i].getHauteur() << " " << maillage.getMailles()[i].getDebit() << endl;
// }

// on obtient bien le maillage avec x=0 entre deux mailles

    //test du flux
    //Solveur_Flux solveur_flux(maillage);
    //solveur_flux.afficherFlux();

    // -----------------------------------------------------------------------------------
    // ------------------ boucle de temps pour le calcul de la solution ------------------
    // -----------------------------------------------------------------------------------
    int iteration = 0;
    double t = t_init;
    double t_sortie=t_init; //temps pour sortie de fichier

    // ---- sauvegarde de l'état initial -----

    solveur.sauvegarde_etat(iteration); // sauvegarde état initial
    solution_exacte.calculerSolutionExacte(t);  // on calcul actuellement la solution exacte à chaque temps
    solution_exacte.save_sol_exacte(iteration,t);
    iteration+=1;

    // ---- boucle de temps -----
    while (t < t_final) {
        solveur.avancerTemps();
        t = solveur.Get_T();

        if (t - t_sortie > pas_sortie) {
            t_sortie = t;
            solution_exacte.calculerSolutionExacte(t);
            solution_exacte.save_sol_exacte(iteration, t);
            solveur.sauvegarde_etat(iteration);
        }

        iteration += 1;
        cout << "Itération " << iteration << " à t = " << t << endl;
    }

    // ---- sauvegarde de l'état final -----
    solution_exacte.calculerSolutionExacte(t);
    solution_exacte.save_sol_exacte(iteration, t);
    solveur.sauvegarde_etat(iteration);

    // ---- erreur au temps final -----
    auto err_final = erreur(solveur.Get_Maillage(), solution_exacte.Get_Maillage());
    auto err_final_L1 = erreur_L1(solveur.Get_Maillage(), solution_exacte.Get_Maillage());

    // ----  test avec des intervalles autour des discontinuités x2 et x3 -----
    const double dx_mask = maillage.getDeltaX();
    const double demi_largeur = 30.0 * dx_mask; // exclure +-30 dx autour des centres
    std::vector<std::pair<double,double>> intervalles;
    intervalles.emplace_back(solution_exacte.getX2() - demi_largeur, solution_exacte.getX2() + demi_largeur);
    intervalles.emplace_back(solution_exacte.getX3() - demi_largeur, solution_exacte.getX3() + demi_largeur);

    auto err_mask_L2 = erreur_L2_masquee(solveur.Get_Maillage(), solution_exacte.Get_Maillage(), intervalles);
    auto err_mask_L1 = erreur_L1_masquee(solveur.Get_Maillage(), solution_exacte.Get_Maillage(), intervalles);

    cout << "Simulation terminéegit à t = " << t <<" avec " << iteration << " itérations." << endl;
    cout << "Erreur finale sur la hauteur: " << err_final[0] << endl;
    cout << "Erreur finale sur le débit: " << err_final[1] << endl;
    cout << "Erreur finale l1(hauteur): " << err_final_L1[0] << endl;
    cout << "Erreur finale l1(débit): " << err_final_L1[1] << endl;
    cout << "Erreur finale l2 masquée(hauteur): " << err_mask_L2[0] << endl;
    cout << "Erreur finale l2 masquée(débit): " << err_mask_L2[1] << endl;
    cout << "Erreur finale l1 masquée(hauteur): " << err_mask_L1[0] << endl;
    cout << "Erreur finale l1 masquée(débit): " << err_mask_L1[1] << endl;
    cout << "Delta x: " << maillage.getDeltaX() << endl;

    // coder le calcul de l'erreur entre la solution numérique et la solution exacte ici
       


    //---------------------------------------------------------------------------------
    //------------------ fin du programme ---------------------------------------------
    //---------------------------------------------------------------------------------
    
    if (use_muscl) {
        cout << "MODE MUSCL ORDRE 2 ACTIVÉ" << endl;
    } else {
        cout << "MODE ORDRE 1" << endl;
    }
    

    return 0;
    

}