#include "solution.h"

#include <string>
#include <iostream>
#include <fstream>
#include <vector>
#include <ctime>
#include <Eigen/Dense>

using namespace std;
using namespace Eigen;

int main(int argc, char * argv[]) //argv[1] = le fichier données initiales
{

// ### DECLARATIONS et INITIALISATIONS ###
cout << "### DECLARATIONS et INITIALISATIONS ###" << endl;

  //1 Outils boucles

    int k, l;
    int iter = 0;

  //2 Données initiales .p

    Parametres p;
    p.lecture_fichier(argv[1]); //hG, hD, tmax, CFL, x_lim, fichier_maillage
    p.print_donnees_initiales();

  //3 Classe Maillage .m

    Maillage m;
    m.lire_mesh_medit(p.fichier_maillage);
    m.calcul_connectivite();
    m.calcul_aires();
    m.calcul_centres_et_aretes();
    m.calcul_d_carac();

    int nb_mailles = m.aire_maille.size();
    int nb_aretes = m.noeud_arete.size();
    double d = m.d_carac ;

    cout << "nb_mailles = " << nb_mailles << " et nb_aretes = " << nb_aretes << " d = " << d << endl; 
    cout << ""<< endl; 

  //4 Classe Solution .s

    Solution s(nb_mailles);
    s.initialisation_Un(m, p); //Un, Unp1, F, b
    m.sortie_vtk(iter, s.Un);

  //5 Paramètres temporels

    double t = 0 ;
    double dt;
    double t_print = p.t_pas;

// ### BOUCLE EN TEMPS ###
cout << "### BOUCLE EN TEMPS ###" << endl;

  while (t < p.tmax)
  {

  //1 Boucle sur les mailles pour F, Unp1 et b

    s.calcul_maille(m);

  //2 Mise à jour paramètres temps

    dt = p.CFL *d /s.b.maxCoeff(); // CFL *distance carcatériqtique minimale /max des valeurs propres
    t = t + dt;
    iter += 1;
    cout << " b =" << s.b.maxCoeff() << " dt=" << dt << endl;

  //3 Boucle sur les arrêtes pour FF et Unp1

    for(int e=0;e<nb_aretes;e++){

      //numéros des mailles voisines
      k = m.maille_arete[e][0];
      l = m.maille_arete[e][1];
       
      if (l == -1) //maille de bord je crois
      {
        // --- ATTENTION ---
        // A changer pour un calcul de flux qui dépende de cl_arete_bord
        // --- ATTENTION ---
      }            
      else //aute maille
      {
        // --- ATTENTION ---
        // Je ne calcul pas encore le vecteur ne dans la fonction calcul_flux
        // --- ATTENTION ---

        s.calcul_flux(k, l);

        // --- ATTENTION ---
        // Je ne suis pas sûre de pour quelles mailles je fais + ou - 
        // peut-etre metre ces 4 lignes dans la classe solution ou dans une boucle
        // --- ATTENTION ---

        //ajout du flux dans l'une des mailles voisines
        s.Unp1(k,0) += + dt / m.aire_maille[k] * m.l_arete[e] * s.Flux_num(0);
        s.Unp1(k,1) += + dt / m.aire_maille[k] * m.l_arete[e] * s.Flux_num(1);

        //soustraction du flux dans l'autre
        s.Unp1(l,0) += - dt / m.aire_maille[l] * m.l_arete[e] * s.Flux_num(0);
        s.Unp1(l,1) += - dt / m.aire_maille[l] * m.l_arete[e] * s.Flux_num(1);
      }
    }

  //4 Mise à jour Un

    s.maj_Un();

  //4 Sortie VTK

    if(t>t_print)
    {
      cout << t << " s, Sortie " << iter << endl; 
      m.sortie_vtk(iter, s.Un);
      t_print += p.t_pas;
    }


  }


}