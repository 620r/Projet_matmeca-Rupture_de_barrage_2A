#include "solution.h"
#include <iostream>


// --- Constructeur
Solution::Solution(int nb_mailles)
    : nb_mailles(nb_mailles),
      Un(nb_mailles, 3),
      Unp1(nb_mailles, 3),
      F(nb_mailles, 3), 
      G(nb_mailles, 3),
      b(nb_mailles),
      Flux_num(3)
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
        Un(k,1) = 0; //qx
        Un(k,2) = 0; //qy
    }
}

// --- Mise à jour maille
void Solution::calcul_maille(const Maillage& m)
{
    for (int k=0 ; k<nb_mailles ; k++)
    {
        //Unp1 (pour être sûr)
        Unp1(k,0) = Un(k,0);
        Unp1(k,1) = Un(k,1);
        Unp1(k,2) = Un(k,2);

        //Grandeurs explicites
        double h = Un(k,0);
        double qx = Un(k,1);
        double qy = Un(k,2);

        //Calcul flux, projection sur x
        F(k,0) = qx; 
        F(k,1) = qx*qx/h + g*h*h/2; 
        F(k,2) = qx*qy/h;

        //Calcul flux, projection sur y
        G(k,0) = qy; 
        G(k,1) = qx*qy/h; 
        G(k,2) = qy*qy/h + g*h*h/2;

        //Calcul valeurs propres
        b(k) = std::abs(qx/h) + std::sqrt(g*h); // c + sqrt(gh)
    }
}

// --- Calcul du flux
void Solution::calcul_FG(double h, double qx, double qy, double Ff[3], double Gg[3])
{
    Ff[0]  = qx; 
    Ff[1]  = qx*qx/h + g*h*h/2; 
    Ff[2]  = qx*qy/h;
    Gg[0] = qy;
    Gg[1] = qx*qy/h; 
    Gg[2] = qy*qy/h + g*h*h/2;
}

// --- Calcul flux numérique de bord
void Solution::calcul_flux_bord(const int k, double nx, double ny, int cl, double h_reservoir)
{
    // --- Construction de la ghost cell ---
    double hg, qxg, qyg;
    double qx_n = Un(k,1)*nx + Un(k,2)*ny;

    if (cl == 1 || cl == 3) // MUR
    {
        hg  = Un(k,0);
        qxg = Un(k,1) - 2*qx_n*nx;
        qyg = Un(k,2) - 2*qx_n*ny;
    }
    else if (cl == 4) // RESERVOIR
    {
        hg  = h_reservoir;
        qxg = Un(k,1);
        qyg = Un(k,2);
    }
    else if (cl == 2) // SORTIE
    {
        hg  = Un(k,0);
        qxg = Un(k,1);
        qyg = Un(k,2);
    }
    else 
    {
        std::cout<<"errreur, cl non reconnue";
    }

    //F et G de la ghost cell 
    double Fg[3], Gg[3];
    calcul_FG(hg, qxg, qyg, Fg, Gg);

    // max des valeurs propres
    double ug_n = (qxg*nx + qyg*ny) / hg;
    double be = std::max(b(k), std::abs(ug_n) + std::sqrt(g*hg));

    // Rusanov 
    double Ug[3] = {hg, qxg, qyg};
    for(int i=0; i<3; i++)
        Flux_num(i) = 0.5 *((F(k,i)+Fg[i])*nx + (G(k,i)+Gg[i])*nx) - be/2*(Ug[i]-Un(k,i));
}

// --- Calcul flux numérique
void Solution::calcul_flux(const int k, const int l, double nx, double ny)
{
    double be = std::max(b(k),b(l));

    for (int i=0 ; i<3 ; i++)
        Flux_num(i) = 0.5 *((F(k,i)+F(l,i))*nx + (G(k,i)+G(l,i))*nx) - be/2*(Un(l,i)-Un(k,i));
}

// --- Mise à jour maille
void Solution::maj_Un()
{
    for (int k=0 ; k<nb_mailles ; k++)
    {
      Un(k,0) = Unp1(k,0);
      Un(k,1) = Unp1(k,1);
      Un(k,2) = Unp1(k,2);
    }
}