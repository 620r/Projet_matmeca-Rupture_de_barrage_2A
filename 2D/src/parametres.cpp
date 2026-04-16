#include "parametres.h"

#include <string>
#include <iostream>
#include <fstream>

using namespace std;

// --- Lecture du fichier d'inialisation
void Parametres::lecture_fichier(const std::string& filename)
{
    ifstream fichier_initialisation(filename);
    string line;

    if (fichier_initialisation.fail())
    {
      cout << "Pas de fichier d'initialisation" << endl;
      exit(1);
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
        else if (key == "t_pas") { iss >> t_pas ;}
        else if (key == "CFL") { iss >> CFL ;}
        else if (key == "x_lim") { iss >> x_lim ;}
        else if (key == "fichier_maillage") { iss >> fichier_maillage ;}
        //sûrement aussi des CL à lire 
      }
    }

    fichier_initialisation.close();
}

// --- Affichage des données initiales
void Parametres::print_donnees_initiales()
{
  cout << "hG :" << hG << ",  hD :" << hD << endl;
  cout << "tmax :" << tmax << ",  CFL :" << CFL << endl;
  cout << "x_lim :" << x_lim << ",  fichier_maillage :" << fichier_maillage << endl;
  cout << " " << endl;
}