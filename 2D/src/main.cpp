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

    double sum;
    double am;
    VectorXd Flux_num(2);

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


  //4 Classe Solution .s

    int nb_mailles = m.aire_maille.size();

    Solution s(nb_mailles);
    s.initialisation_Un(m, p); //Un, Unp1, F, b

  //5 Paramètres temporels

    double t = 0 ;
    double delta_t;


// ### BOUCLE EN TEMPS ###
cout << "### BOUCLE EN TEMPS ###" << endl;

  while (t < p.tmax)
  {

  //1 Boucle sur les mailles pour F, Unp1 et b

    s.maj_maille(m);

  //2 Mise à jour paramètres temps

    t = p.tmax ; //oui non on est d'accord

  //3 Boucle sur les arrêtes pour FF et Unp1




  }



//   // Boucle en temps
//   while (t<t_final)
//   {
//     // 1) Boucle sur les mailles : Pour calculer F(Un)

//     for (int k=0 ; k<nb_mailles ; k++)
//     {
//       Unp1(k)=Un(k) ;



//       flux=0.5*(F(Un(m.maille_arete(am,2)))+F(Un(j))) +
//       (be/2)*(Un(m.maille_arete(am,2))-Un(j)); }  // Doute : (UL-UK) ???   : 0,5(F(Uk)+F(UL)).ne-(be/2)(UL-UK)
//flux=0.5*(F(Un(m.maille_arete(am,2)))+F(Un(j))) +
  //(be/2)*(Un(m.maille_arete(am,2))-Un(j)); }  // Doute : (UL-UK) ???   : 0,5(F(Uk)+F(UL)).ne-(be/2)(UL-UK)
//     }

//     // 2) Boucle sur les arrêtes pour le calcul de be (mêmes vp que lors du S7 ???)

//     for (int e=0 ; e<nb_arretes ; e++)
//     {

//     }

//     // 3) Calcul de la CFL -> besoin de be

//     delta_t = delta_k / max(be) ; // Valable sur une maille k ???

//     // 4) Boucle sur les arrêtes
//   }





//   ////////////////////////////::  AIDE ////////////:::::::::
//   // Boucle en temps
//   while (t<t_final)
//   {
//       // Boucle sur les mailles
//       for (int k=0 ; k<nb_mailles ; k++) // Boucle sur les mailles
//       {
//           Unp1(k)=Un(k) ;
//           sum = 0 ;
//       }


//       // BOUCLE A REVOIR AVEC VRAIES CL
//       for (int j=0 ; j<4 ; j++) // Boucle sur les arretes (3 arretes car triangle)
//       {
//           am = m.arete_maille[j][k]; ;

//           if (m.maille_arete[am][2]==0) // if arrete de bord : maille_arete(arete_maille(j,k),1)==0
//           {

//           if (m.cl_arete[am]==20) {

//             flux = phi_b ; } // A VOIR

//           else if (m.cl_arete[am]==10) {

//             flux = -D*(UG-Un(j))/m.d_arete[am] ; } // A VOIR

//           else if (m.cl_arete(am)==11) {

//             flux = -D*(UD-Un(j))/m.d_arete[am] ; } // A VOIR


//           else

//           double be = std::max(abs(Un(m.maille_arete(am,2))), abs(Un(maille_arete(am,1)))) ;  // Doute : max des vp

//           if (m.maille_arete(am,1)==j) {       // if pour savoir si 1 ou 2 à mettre dans maille arete

//               flux=-0.5*(F(Un(m.maille_arete(am,2)))+F(Un(j))) +
//               (be/2)*(Un(m.maille_arete(am,2))-Un(j)); }  // Doute : (UL-UK)

//           else {

//               flux=-0.5*(F(Un(m.maille_arete(am,1)))+F(Un(j))) +
//               (be/2)*(Un(m.maille_arete(am,1))-Un(j)); }  // Doute : (UL-UK)



//           sum = sum+flux*m.l_arete(am) ;

//           }

//       Unp1(j)= Un(j) - (delta_t / m.aire_maille(j))*sum ; // OK
//       Un(j)=Unp1(j) ;

//       }

//   }



//   return 0;
// }


}