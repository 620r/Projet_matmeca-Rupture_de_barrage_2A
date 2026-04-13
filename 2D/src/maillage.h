#pragma once
#include <vector>
#include <string>


class Maillage {
public:
    // Constructeur vide
    Maillage() = default;

    // --- Données publiques accessibles
    std::vector<std::vector<double>> coord_noeud;   // coordonnées des noeuds [nb_noeuds][2] pour x et y
    std::vector<std::vector<int>> noeud_maille;     // noeuds par maille [nb_mailles][3] pour les 3 noeuds de chaque maille
    std::vector<double> aire_maille;                // aire de chaque maille [nb_mailles]
    std::vector<int> cl_arete;                      // code CL par arête [nb_aretes]
    std::vector<std::vector<int>> maille_arete;     // maille connectée à chaque arête [nb_aretes][2] pour les maille de droite et gauche
    std::vector<std::vector<int>> arete_maille;     // arêtes de chaque maille [nb_mailles][3] pour les 3 arrêtes de chaque maille
    std::vector<std::vector<double>> milieu_arete;  // milieu de chaque arête [nb_aretes][2] pour x et y
    std::vector<double> l_arete;                    // longueur des arêtes [nb_aretes]
    std::vector<double> d_arete;                    // distance entre les centres des mailles de part et d'autre de chaque arrête [nb_aretes]
    std::vector<std::vector<int>> noeud_arete;      // noeuds de chaque arête [nb_aretes][2]
    std::vector<std::vector<double>> centre_maille; // coordonnées des centre des mailles [nb_mailles][2] pour x et y
    std::vector<std::vector<int>> noeud_arete_bord; // noeuds des arêtes de bord [nb_aretes_bord ?][2]
    std::vector<int> cl_arete_bord;                 // code CL arêtes de bord [nb_aretes_bord ?]
    double d_carac;                                 // distance caractéristique du maillage

    // --- Fonctions publiques accessibles
    void lire_mesh_medit(const std::string &fichier);
    void calcul_connectivite();
    void calcul_aires();
    void calcul_centres_et_aretes();
    void calcul_d_carac();
    void sortie_vtk(int iter, const std::vector<std::pair<std::string,std::vector<double>>> &scalars) const;
    	// Chaque paire = (nom_champ, valeurs_par_maille)
};
