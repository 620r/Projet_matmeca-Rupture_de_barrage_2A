#ifndef MAILLAGE_H

#include <Eigen/Dense>
#include <fstream>
#include <vector>

class maille  // maille est un point de la maillage, avec position, hauteur, debit et vitesse calculable
{
private:
    double _position;
    double _hauteur;
    double _debit;
public:
    maille();
    maille(const double position, const double hauteur, const double debit);
    ~maille();
    double getPosition() const;
    double getHauteur() const;
    double getDebit() const;
    double calculerVitesse() const ;
    void setHauteur(double hauteur) { _hauteur = hauteur; };
    void setDebit(double debit) { _debit = debit; };

};




class Maillage {  // Maillage contient une liste de mailles, avec les operations associées
private:
    std::vector<maille> _mailles; 
    double _longueur_gauche;
    double _longueur_droite;
    int _nb_subdivisions;
    double _delta_x;
public:
    Maillage(double longueur_gauche,double longueur_droite, int nb_subdivisions, double hgauche, double hdroite); 
    // on prend discontinuité en 0, et on initialise les mailles en partant de 0 vers la gauche puis vers la droite à la bonne hauteur
    // on a aussi deux mailles fantomes aux extrémités pour les conditions aux limites ( des mailles en plus codé ici pour faire une sortie infinie)
    Maillage(const Maillage& entre);
    double getLongueur_gauche() const;
    double getLongueur_droite() const;
    int getNbSubdivisions() const;  
    double getDeltaX() const;
    void afficherMaillage() const; // affiche les mailles pour le contrôle
    std::vector<maille> getMailles() const { return _mailles; }; // retourne la liste des mailles
    maille getMailles(int i) const { return _mailles[i]; }; // retourne la maille i
    void setMailles(const std::vector<maille>& mailles) { _mailles = mailles; }; // met à jour la liste des mailles
    void setMailles(int i, const maille& maille) { _mailles[i] = maille; }; // met à jour la maille i


};
#define MAILLAGE_H

#endif // MAILLAGE_H