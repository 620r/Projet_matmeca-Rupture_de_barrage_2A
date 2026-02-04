#ifndef solUTION_EXACTE_H
#define solUTION_EXACTE_H
#include "maillage.h"


class Solution_Exacte // classe qui va calculer la solution exacte à un pas de temps donné , basée sur le code f90
{
    private:

        Maillage& _maillage; // on va reprendre le même maillage
        double _hg; // hauteur gauche
        double _hd; // hauteur droite
        double _g= 9.81;
        // positions caractéristiques (têtes/queues de l'onde et choc)
        double _x1 = 0.0;
        double _x2 = 0.0;
        double _x3 = 0.0;
        
        std::string _filename; // nom du fichier de sortie



    public:

    Solution_Exacte(Maillage& maillage, double hg, double hd, const std::string& filename) : _maillage(maillage), _hg(hg), _hd(hd), _filename(filename) {}
    void calculerSolutionExacte(double temps);
    // calcule la solution exacte à un pas de temps
    Maillage& Get_Maillage() { return _maillage; }
    // Accesseurs des positions caractéristiques
    double getX1() const { return _x1; }
    double getX2() const { return _x2; }
    double getX3() const { return _x3; }
    double F(double hauteur); // fonction f pour la solution exacte tiré des notes
    double F_prime(double hauteur); // dérivée de la fonction f pour la solution exacte
    double U_pente(double hauteur, double temps); // vitesse de la pente pour la solution exacte 
    double H_pente(double hauteur, double temps); // hauteur pour x dans la pente
    double newton(double h0, double tol, int max_iter); // méthode de Newton

    void save_sol_exacte(int iteration, double temps) const;// sauvegarde la solution exacte dans un fichier
};
#endif // solUTION_EXACTE_H