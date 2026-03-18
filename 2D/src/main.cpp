#include "maillage.h"

#include <string>
#include <iostream>
#include <fstream>
#include <vector>
#include <ctime>
#include <Eigen/Dense>

using namespace std;
using namespace Eigen;


int main(int argc, char * argv[])
{
    Maillage m;
    m.lire_mesh_medit("fichier.mesh");
    m.calcul_connectivite();
    m.calcul_aires();
    m.calcul_centres_et_aretes();



    // Déclarations :


    // Données lues dans fichier param.dat :::::::::

    
    //::::::::::::::::::::::::::::::::::::::::::::::


    // Données de maillage.h ::::::::::::::::::::::::
    //VectorXd arete_maille(), aire_maille(), maille_arete() ;  
    //VectorXd l_arete(), cl_arete(), d_arete() ;  
    //:::::::::::::::::::::::::::::::::::::::::::::::

    // Constantes CL ::::::::
    double UD, UG ;
    double Uinit ;
    int cl ;
    double phi_b;
    //:::::::::::::::::::::::

    

    // Autre ::::::::::::::
    double flux ;
    VectorXd Un , Unp1 ;
    //-::::::::::::::::::::

    // Temps, itération.. :::::::
    double t, t_final ;
    double delta_t; 
    int nb_mailles ;
    //:::::::::::::::::::::::::::

    //Boucle
    double sum;
    double am; 
    int j, k;


    // Lecture de fichiers
    std::ifstream inputFile_Param(argv[1]); 
    string line;
    if (inputFile_Param.fail())
    {
      std::cout<<"N'ouvre pas"<<std::endl;
      return 1;
    }
    while (getline(inputFile_Param,line))
    {
      istringstream iss(line);
      string key, eq;
      if(iss>>key>>eq)
      {
        if (key == "N") 
        {
        iss >> N; 
        
        sol0.resize(2*N);
        w.resize(N);
        }

        else if (key == "y2") {iss >> sol0(3)  ; coords_presentes = true;}

        else if (key == "x1") { iss >> sol0(0) ; coords_presentes = true;}
        else if (key == "y1") { iss >> sol0(1)  ; coords_presentes = true;}

      }
    }

    inputFile_Param.close(); 


    // Affichage des données récupérées
    std::cout<<" "<<std::endl;
    std::cout<<"------ Données : ----------------------------------------------"<<std::endl;
    std::cout<<" "<<std::endl;



    // Initialisations :

    ///////////////////
    //   A CODER    //
    //////////////////



    // Boucle en temps :

    while (t<t_final)
    {
        for (int k=0 ; k<nb_mailles ; k++) // Boucle sur les mailles
        {
            Unp1(k)=Un(k) ;
            sum = 0 ;


            // BOUCLE A REVOIR AVEC VRAIES CL 
            for (int j=0 ; j<4 ; j++) // Boucle sur les arretes (3 arretes car triangle)
            {
                am = m.arete_maille(j)(k) ; 

                if (m.maille_arete(am,2)==0) // if arrete de bord : maille_arete(arete_maille(j,k),1)==0
                {

                if (m.cl_arete(am)==20) {

                  flux = phi_b ; } // A VOIR 

                else if (m.cl_arete(am)==10) {

                  flux = -D*(UG-Un(j))/m.d_arete(am) ; } // A VOIR 

                else if (cl_arete(am)==11) {

                  flux = -D*(UD-Un(j))/m.d_arete(am) ; } // A VOIR
                

                else 

                double be = std::max(abs(Un(m.maille_arete(am,2))), abs(Un(maille_arete(am,1)))) ;  // Doute : max des vp

                if (m.maille_arete(am,1)==j) {       // if pour savoir si 1 ou 2 à mettre dans maille arete 
                    
                    flux=-0.5*(F(Un(m.maille_arete(am,2)))+F(Un(j))) + 
                    (be/2)*(Un(m.maille_arete(am,2))-Un(j)); }  // Doute : (UL-UK)

                else {

                    flux=-0.5*(F(Un(m.maille_arete(am,1)))+F(Un(j))) + 
                    (be/2)*(Un(m.maille_arete(am,1))-Un(j)); }  // Doute : (UL-UK)



                sum = sum+flux*m.l_arete(am) ;

                }

            Unp1(j)= Un(j) - (delta_t / m.aire_maille(j))*sum ; // OK
            Un(j)=Unp1(j) ;
            }
            
        }

    }



    return 0;
}
