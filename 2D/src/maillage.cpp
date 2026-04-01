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
void Maillage::lire_mesh_medit(const string &fichier) {
    ifstream in(fichier);
    if(!in) { cerr << "Impossible d'ouvrir " << fichier << "\n"; return; }

    string line;
    int nb_noeuds, nb_aretes_bord, nb_mailles;
    int n1, n2, n3, cl, mat;

    // --- ignorer 2 lignes d'en-tête
    for(int i=0;i<2;i++) getline(in,line);
    
    // Vertices
    getline(in,line); in >> nb_noeuds;
    if (line != "Vertices") {
        cout << "Ligne différente de Vertices : `" << line << "`\n";
        exit(1);
    }

    // --- lecture des coordonnées des noeuds
    coord_noeud.resize(nb_noeuds, vector<double>(2));
    for(int i=0;i<nb_noeuds;i++) {
        if (!(in >> coord_noeud[i][0] >> coord_noeud[i][1])) {
            cerr << "Erreur lecture des coordonnées du noeud " << i << "\n";
            return; // ou break si tu veux continuer
        }
    }

    // Edges
    getline(in,line); getline(in,line); in >> nb_aretes_bord;
    if (line != "Edges") {
        cout << "Ligne différente de Edges : `" << line << "`\n";
        exit(1);
    }

    // --- lecture du numéro de chaque noeud de l'arrête et de la CL associée
    noeud_arete_bord.resize(nb_aretes_bord, vector<int>(2));
    cl_arete_bord.resize(nb_aretes_bord);
    for(int i=0;i<nb_aretes_bord;i++){
        if (!(in >> n1 >> n2 >> cl)) {
            cerr << "Erreur lecture bords " << i << "\n";
            return;
        }
        noeud_arete_bord[i][0] = n1-1;
        noeud_arete_bord[i][1] = n2-1;
        cl_arete_bord[i] = cl;
    }

    // Triangles
    getline(in,line); getline(in,line); in >> nb_mailles;
    if (line != "Triangles") {
        cout << "Ligne différente de Triangles : `" << line << "`\n";
        exit(1);
    }

    // --- lecture du numéro de chaque noeud de la maille et de la CL associée
    noeud_maille.resize(nb_mailles, vector<int>(3));
    for(int i=0;i<nb_mailles;i++){
        if (!(in >> n1 >> n2 >> n3 >> mat)) {
            cerr << "Erreur lecture triangle " << i << "\n";
            return;
        }
        noeud_maille[i][0] = n1-1;
        noeud_maille[i][1] = n2-1;
        noeud_maille[i][2] = n3-1;
    }
    in.close();
}

// ----------------------------------------
// Calcul connectivités
// ----------------------------------------
void Maillage::calcul_connectivite() {

    int nb_noeuds = coord_noeud.size();
    int nb_trig = noeud_maille.size();

    noeud_arete.clear();
    maille_arete.clear();
    arete_maille.resize(nb_trig, vector<int>(3));

    vector<vector<int>> aretes_du_noeud(nb_noeuds);

    for(int i=0;i<nb_trig;i++){

        int n[3] = {
            noeud_maille[i][0],
            noeud_maille[i][1],
            noeud_maille[i][2]
        };

        for(int e=0;e<3;e++){
            int a = n[e];
            int b = n[(e+1)%3];
            if(a>b) swap(a,b);
            int edge_id = -1;

            // chercher seulement dans les arêtes attachées à a
            for(int id : aretes_du_noeud[a]){
                if(noeud_arete[id][0]==a && noeud_arete[id][1]==b){
                    edge_id = id;
                    break;
                }
            }

            if(edge_id == -1){
                edge_id = noeud_arete.size();
                noeud_arete.push_back({a,b});
                maille_arete.push_back({i,-1});
                aretes_du_noeud[a].push_back(edge_id);
                aretes_du_noeud[b].push_back(edge_id);
            }
            else{
                maille_arete[edge_id][1] = i;
            }
            arete_maille[i][e] = edge_id;
        }
    }

    // --- allocation et initialisation des CL
    cl_arete.resize(noeud_arete.size(), -1); // toutes les arêtes par défaut -1 (interne)

    // --- assigner les codes CL des arêtes de bord
    for(size_t i=0;i<noeud_arete_bord.size();i++){
        int a = noeud_arete_bord[i][0];
        int b = noeud_arete_bord[i][1];
        // rechercher l'arête correspondante dans noeud_arete
        for(size_t e=0;e<noeud_arete.size();e++){
            int n0 = noeud_arete[e][0];
            int n1 = noeud_arete[e][1];
            if( (n0==a && n1==b) || (n0==b && n1==a) ){
                cl_arete[e] = cl_arete_bord[i];
                break;
            }
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
        aire_maille[i] = abs( (b[0]-a[0])*(c[1]-a[1]) - (b[1]-a[1])*(c[0]-a[0]) )/2.0;
    }
}

// ----------------------------------------
// Calcul centres et arêtes
// ----------------------------------------
void Maillage::calcul_centres_et_aretes() {
    int nb_mailles = noeud_maille.size();
    int nb_aretes = noeud_arete.size();

    // centres de maille
    centre_maille.resize(nb_mailles, vector<double>(2));
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
    milieu_arete.resize(nb_aretes, vector<double>(2));
    for(int i=0;i<nb_aretes;i++){
        int tg = maille_arete[i][0];
        int td = maille_arete[i][1];
        auto &Cg = centre_maille[tg];
        vector<double> Cd(2,0.0);
        if(td>=0) Cd = centre_maille[td];
        else {
            auto &A = coord_noeud[noeud_arete[i][0]];
            auto &B = coord_noeud[noeud_arete[i][1]];
            Cd[0] = (A[0]+B[0])/2.0; Cd[1] = (A[1]+B[1])/2.0;
        }
        d_arete[i] = sqrt((Cd[0]-Cg[0])*(Cd[0]-Cg[0]) + (Cd[1]-Cg[1])*(Cd[1]-Cg[1]));
        auto &A = coord_noeud[noeud_arete[i][0]];
        auto &B = coord_noeud[noeud_arete[i][1]];
        milieu_arete[i][0] = 0.5*(A[0]+B[0]);
        milieu_arete[i][1] = 0.5*(A[1]+B[1]);
        l_arete[i] = sqrt((B[0]-A[0])*(B[0]-A[0]) + (B[1]-A[1])*(B[1]-A[1]));
    }
}

// ----------------------------------------
// Sortie VTK multi-champs
// ----------------------------------------
void Maillage::sortie_vtk(int iter, 
    const vector<pair<string,vector<double>>> &scalars) const
{
    string filename = "sortie_" + to_string(iter) + ".vtk";
    ofstream out(filename);
    if(!out){ cerr << "Impossible d'ouvrir " << filename << "\n"; return; }

    int nb_noeuds_local = coord_noeud.size();
    int nb_mailles_local = noeud_maille.size();

    out << "# vtk DataFile Version 2.0\n";
    out << "simulation\n";
    out << "ASCII\n";
    out << "DATASET UNSTRUCTURED_GRID\n";

    // Points
    out << "POINTS " << nb_noeuds_local << " double\n";
    for(auto &p : coord_noeud)
        out << setprecision(12) << p[0] << " " << p[1] << " 0.0\n";

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
        if(valeurs.size()!= static_cast<size_t>(nb_mailles_local)){
            cerr << "Attention : vecteur " << nom << " n'a pas la bonne taille\n";
            continue;
        }
        out << "SCALARS " << nom << " double\n";
        out << "LOOKUP_TABLE default\n";
        for(double v : valeurs) out << v << "\n";
    }
    out.close();
}
