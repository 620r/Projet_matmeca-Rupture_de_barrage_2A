#include "maillage.h"

#include <string>
#include <iostream>
#include <fstream>
#include <vector>
#include <ctime>
#include <Eigen/Dense>

using namespace std;
using namespace Eigen;


int main(int argc, char * argv[]) //le fichier données initiales
{

// ### DECLARATIONS et INITIALISATIONS ###

  //1 Outils boucles

    double sum;
    double am;
    int j, k;

  //2 Données initiales

    double hG, hD;            //hauteurs d'eau à t0
    double tmax, CFL;         //paramètres temporels
    string fichier_maillage;  //fichier.mesh contenant le maillage
    //sûrement aussi des CL à lire

  //3 Lecture fichier des données initiales

    ifstream fichier_initialisation(argv[1]);
    string line;
    if (fichier_initialisation.fail())
    {
      cout << "N'ouvre pas" << endl;
      return 1;
    }
    while (getline(fichier_initialisation,line))
    {
      istringstream iss(line);
      string key, eq;
      if(iss>>key>>eq)
      {
        if (key == "hG") {iss >> hG ;}
        else if (key == "hD") {iss >> hD ;}
        else if (key == "tmax") { iss >> tmax ;}
        else if (key == "CFL") { iss >> CFL ;}
        else if (key == "fichier_maillage") { iss >> fichier_maillage ;}
        //sûrement aussi des CL à lire 
      }
    }

    fichier_initialisation.close();

    // Affichage des données récupérées
    cout << " " << endl;
    cout << "------ Données : ------" << endl;
    cout << "hG :" << hG << "hD :" << hD << endl;
    // écrire les autres
    cout << " " << endl;

  //4 Maillage .m

    Maillage m;
    m.lire_mesh_medit(fichier_maillage);
    m.calcul_connectivite();
    m.calcul_aires();
    m.calcul_centres_et_aretes();

  //5 Solutions Un, Unp1, F, b

    int nb_mailles = m.aire_maille.size();
    MatrixXd Un(2, nb_mailles), Unp1(2, nb_mailles), F(2, nb_mailles);
    VectorXd b(nb_mailles);

    // peut-être faire une classe avec Un[1], Un[2], F[1], F[2], et b qui sont que des données ratachées aux mailles

  //6 Paramètres temporels

    double t = 0 ;
    double delta_t;

  //7 Flux numérique FF

  //8 Initialisation Un

// ### BOUCLE EN TEMPS ###

  //1 Boucle sur les mailles pour F, Unp1 et b

  //2 Mise à jour paramètres temps

  //3 Boucle sur les arrêtes pour FF et Unp1






//   // Temps, itération.. :::::::
//   double t, t_final ;
//   double delta_t;
//   int nb_mailles ;
//   //:::::::::::::::::::::::::::

//   //Boucle







//   // Initialisations :

//   Un(0)=hG_0;  // Hauteur initiale mais de quoi ???
//   Un(1)=q0;  // Débit initial mais de quoi ???



//   // Boucle en temps
//   while (t<t_final)
//   {
//     // 1) Boucle sur les mailles : Pour calculer F(Un)

//     for (int k=0 ; k<nb_mailles ; k++)
//     {
//       Unp1(k)=Un(k) ;



//       flux=0.5*(F(Un(m.maille_arete(am,2)))+F(Un(j))) +
//       (be/2)*(Un(m.maille_arete(am,2))-Un(j)); }  // Doute : (UL-UK) ???   : 0,5(F(Uk)+F(UL)).ne-(be/2)(UL-UK)

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