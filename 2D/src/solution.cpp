#include "solution.h"


// --- Constructeur
Solution::Solution(int nb_mailles)
    : nb_mailles(nb_mailles),
      Un(nb_mailles, 2),
      Unp1(nb_mailles, 2),
      F(nb_mailles, 2),
      b(nb_mailles),
      Flux_num(2)
{
}

// --- Initialisation
void Solution::initialisation_Un(const Maillage& m, const Parametres& p)
{
    for (int k=0 ; k<nb_mailles ; k++)
    {
        // Hauteur initiale 
        if (m.centre_maille[k][0] < p.x_lim)
        { Un(k,0) = p.hG; } 
        else 
        { Un(k,0) = p.hD; }

        // Débit initial
        Un(k,1) = 0;  
    }
}

// --- Mise à jour maille
void Solution::maj_maille(const Maillage& m)
{
    for (int k=0 ; k<nb_mailles ; k++)
    {
      //Calcul flux
      F(k,0) = Un(k,1); //q
      F(k,1) = pow(Un(k,1),2)/Un(k,0) + g*pow(Un(k,0),2)/2; //q2/h + gh2/2

      //Calcul valeurs propres
      b(k) = Un(k,1)/Un(k,0) + pow(g*Un(k,0),1/2); //q/h + sqrt(gh)

      //Mise à jour Unp1
      Unp1(k,0) = Un(k,0);
      Unp1(k,1) = Un(k,1);
    }
}

// --- Calcul flux numérique
void Solution::calcul_flux(const int k, const int l)
{
    double be = std::max(b(k),b(l));
    Flux_num(0) = 0.5 *(F(k,0)+F(l,0)) + be /2 *(Un(k,0)-Un(l,0)); //0,5(F(Uk)+F(UL)).ne-(be/2)(UL-UK)
    Flux_num(1) = 0.5 *(F(k,1)+F(l,1)) + be /2 *(Un(k,1)-Un(l,1));
    // Je ne calcul pas encore le vecteur ne
}
