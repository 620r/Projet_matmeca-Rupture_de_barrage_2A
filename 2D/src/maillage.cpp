#include "maillage.h"
#include <fstream>
#include <iostream>
#include <sstream>
#include <iomanip>
#include <cmath>
#include <algorithm>

using namespace std;

// ----------------------------------------
// Lecture du maillage Medit
// ----------------------------------------
void Maillage::lire_mesh_medit(const std::string &fichier) {
    std::ifstream in(fichier);
    if(!in) { std::cerr << "Impossible d'ouvrir " << fichier << "\n"; return; }

    std::string line;
    int nb_noeuds, nb_aretes_bord, nb_mailles;

    // --- ignorer 3 lignes d'en-tête
    for(int i=0;i<3;i++) std::getline(in,line);

    // --- lecture nombre de noeuds
    std::getline(in,line); in >> nb_noeuds;
    coord_noeud.resize(nb_noeuds, std::vector<double>(2));
    for(int i=0;i<nb_noeuds;i++)
        in >> coord_noeud[i][0] >> coord_noeud[i][1];

    // --- lecture arêtes de bord
    std::getline(in,line); std::getline(in,line); in >> nb_aretes_bord;
    std::vector<std::vector<int>> noeud_arete_bord(nb_aretes_bord, std::vector<int>(2));
    std::vector<int> cl_arete_bord(nb_aretes_bord);
    for(int i=0;i<nb_aretes_bord;i++)
        in >> noeud_arete_bord[i][0] >> noeud_arete_bord[i][1] >> cl_arete_bord[i];

    // --- lecture mailles
    std::getline(in,line); std::getline(in,line); in >> nb_mailles;
    noeud_maille.resize(nb_mailles, std::vector<int>(3));
    for(int i=0;i<nb_mailles;i++)
        in >> noeud_maille[i][0] >> noeud_maille[i][1] >> noeud_maille[i][2];

    in.close();
}

// ----------------------------------------
// Calcul connectivités
// ----------------------------------------
void Maillage::calcul_connectivite() {
    int nb_noeuds = coord_noeud.size();
    int nb_trig = noeud_maille.size();
    std::vector<std::vector<int>> flag(nb_noeuds, std::vector<int>(nb_noeuds,0));

    int nb_cotes = 0;
    for(int i=0;i<nb_trig;i++){
        int n1=noeud_maille[i][0], n2=noeud_maille[i][1], n3=noeud_maille[i][2];
        if(flag[n1][n2]==0){ nb_cotes++; flag[n1][n2]=nb_cotes; flag[n2][n1]=nb_cotes; }
        if(flag[n2][n3]==0){ nb_cotes++; flag[n2][n3]=nb_cotes; flag[n3][n2]=nb_cotes; }
        if(flag[n3][n1]==0){ nb_cotes++; flag[n3][n1]=nb_cotes; flag[n1][n3]=nb_cotes; }
    }

    // e = noeud_arete
    noeud_arete.resize(nb_cotes, std::vector<int>(2));
    for(int i=0;i<nb_trig;i++){
        int n1=noeud_maille[i][0], n2=noeud_maille[i][1], n3=noeud_maille[i][2];
        noeud_arete[flag[n1][n2]-1] = {n1,n2};
        noeud_arete[flag[n2][n3]-1] = {n2,n3};
        noeud_arete[flag[n3][n1]-1] = {n3,n1};
    }

    // ar = arete_maille, trig = maille_arete
    arete_maille.resize(nb_trig, std::vector<int>(3));
    maille_arete.resize(nb_cotes, std::vector<int>(2,0));
    for(int i=0;i<nb_trig;i++){
        int n1=noeud_maille[i][0], n2=noeud_maille[i][1], n3=noeud_maille[i][2];
        int edges[3] = {flag[n1][n2]-1, flag[n2][n3]-1, flag[n3][n1]-1};
        arete_maille[i] = {edges[0]+1, edges[1]+1, edges[2]+1};
        for(int e=0;e<3;e++){
            if(maille_arete[edges[e]][0]==0) maille_arete[edges[e]][0] = i+1;
            else maille_arete[edges[e]][1] = i+1;
        }
    }
}

// ----------------------------------------
// Calcul des aires
// ----------------------------------------
void Maillage::calcul_aires() {
    int nb_mailles = noeud_maille.size();
    aire_maille.resize(nb_mailles);
    for(int i=0;i<nb_mailles;i++){
        auto &a = coord_noeud[noeud_maille[i][0]];
        auto &b = coord_noeud[noeud_maille[i][1]];
        auto &c = coord_noeud[noeud_maille[i][2]];
        aire_maille[i] = std::abs( (b[0]-a[0])*(c[1]-a[1]) - (b[1]-a[1])*(c[0]-a[0]) )/2.0;
    }
}

// ----------------------------------------
// Calcul centres et arêtes
// ----------------------------------------
void Maillage::calcul_centres_et_aretes() {
    int nb_mailles = noeud_maille.size();
    int nb_aretes = noeud_arete.size();

    // centres de maille
    std::vector<std::vector<double>> centre_maille(nb_mailles, std::vector<double>(2));
    for(int i=0;i<nb_mailles;i++){
        auto &A = coord_noeud[noeud_maille[i][0]];
        auto &B = coord_noeud[noeud_maille[i][1]];
        auto &C = coord_noeud[noeud_maille[i][2]];
        double delta = 2.0*((B[0]-A[0])*(C[1]-A[1])-(B[1]-A[1])*(C[0]-A[0]));
        double ra2=A[0]*A[0]+A[1]*A[1];
        double rb2=B[0]*B[0]+B[1]*B[1];
        double rc2=C[0]*C[0]+C[1]*C[1];
        centre_maille[i][0] = ((rb2-ra2)*(C[1]-A[1]) - (rc2-ra2)*(B[1]-A[1]))/delta;
        centre_maille[i][1] = -((rb2-ra2)*(C[0]-A[0]) - (rc2-ra2)*(B[0]-A[0]))/delta;
    }

    // arêtes
    d_arete.resize(nb_aretes);
    l_arete.resize(nb_aretes);
    milieu_arete.resize(nb_aretes, std::vector<double>(2));
    for(int i=0;i<nb_aretes;i++){
        int tg = maille_arete[i][0]-1;
        int td = maille_arete[i][1]-1;
        auto &Cg = centre_maille[tg];
        std::vector<double> Cd(2,0.0);
        if(td>=0) Cd = centre_maille[td];
        else {
            auto &A = coord_noeud[noeud_arete[i][0]];
            auto &B = coord_noeud[noeud_arete[i][1]];
            Cd[0] = (A[0]+B[0])/2.0; Cd[1] = (A[1]+B[1])/2.0;
        }
        d_arete[i] = std::sqrt((Cd[0]-Cg[0])*(Cd[0]-Cg[0]) + (Cd[1]-Cg[1])*(Cd[1]-Cg[1]));
        auto &A = coord_noeud[noeud_arete[i][0]];
        auto &B = coord_noeud[noeud_arete[i][1]];
        milieu_arete[i][0] = 0.5*(A[0]+B[0]);
        milieu_arete[i][1] = 0.5*(A[1]+B[1]);
        l_arete[i] = std::sqrt((B[0]-A[0])*(B[0]-A[0]) + (B[1]-A[1])*(B[1]-A[1]));
    }
}

// ----------------------------------------
// Sortie VTK multi-champs
// ----------------------------------------
void Maillage::sortie_vtk(int iter, 
    const std::vector<std::pair<std::string,std::vector<double>>> &scalars) const
{
    std::string filename = "sortie_" + std::to_string(iter) + ".vtk";
    std::ofstream out(filename);
    if(!out){ std::cerr << "Impossible d'ouvrir " << filename << "\n"; return; }

    int nb_noeuds_local = coord_noeud.size();
    int nb_mailles_local = noeud_maille.size();

    out << "# vtk DataFile Version 2.0\n";
    out << "simulation\n";
    out << "ASCII\n";
    out << "DATASET UNSTRUCTURED_GRID\n";

    // Points
    out << "POINTS " << nb_noeuds_local << " double\n";
    for(auto &p : coord_noeud)
        out << std::setprecision(12) << p[0] << " " << p[1] << " 0.0\n";

    // Cells
    out << "CELLS " << nb_mailles_local << " " << nb_mailles_local*4 << "\n";
    for(auto &tri : noeud_maille)
        out << "3 " << tri[0] << " " << tri[1] << " " << tri[2] << "\n";

    // Cell types
    out << "CELL_TYPES " << nb_mailles_local << "\n";
    for(int i=0;i<nb_mailles_local;i++) out << "5\n";

    // Cell data (multi-scalars)
    out << "CELL_DATA " << nb_mailles_local << "\n";
    for(auto &s : scalars){
        const auto &nom = s.first;
        const auto &valeurs = s.second;
        if(valeurs.size()!=nb_mailles_local){
            std::cerr << "Attention : vecteur " << nom << " n'a pas la bonne taille\n";
            continue;
        }
        out << "SCALARS " << nom << " double\n";
        out << "LOOKUP_TABLE default\n";
        for(double v : valeurs) out << v << "\n";
    }
    out.close();
}
