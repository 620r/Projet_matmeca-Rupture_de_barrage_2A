// ce fichier va contenir les fonctions de maillage et la classe Maillage, compossé de mailles
#include "maillage.h"
#include <iostream>
#include <fstream>
#include <Eigen/Dense>

using namespace std;
using namespace Eigen;

//------------------------------------maille----------------------------
maille::maille()
{
    this->_position = 0.0;
    this->_hauteur = 0.0;
    this->_debit = 0.0;
}

maille::maille(const double position, const double hauteur, const double debit)
{
    this->_position = position;
    this->_hauteur = hauteur;
    this->_debit = debit;
}       

maille::~maille()
{
}

double maille::getPosition() const {
    return _position;
}
double maille::getHauteur() const {
    return _hauteur;
}
double maille::getDebit() const {
    return _debit;
}

double maille::calculerVitesse() const
{
    if (_hauteur != 0.0) {
        return _debit / _hauteur;
    } else {
        return 0.0; // évite la division par zéro
    }
}

//------------------------------------maillage----------------------------
Maillage::Maillage(double longueur_gauche,double longueur_droite, int nb_subdivisions, double hgauche, double hdroite) 
    : _longueur_gauche(longueur_gauche), _longueur_droite(longueur_droite), _nb_subdivisions(nb_subdivisions) {

    _delta_x = (_longueur_gauche + _longueur_droite) / (_nb_subdivisions + 1);

    int nb_gauche = int(_longueur_gauche / _delta_x);
    int nb_droite = _nb_subdivisions - nb_gauche;

    _longueur_gauche = nb_gauche * _delta_x;
    _longueur_droite = nb_droite * _delta_x;

    _mailles.resize(_nb_subdivisions + 2); // indices 0..N+1, internes 1..N

    // Centre des positions tel que l’interface x=0 est entre nb_gauche et nb_gauche+1
    const double offset = (nb_gauche + 0.5) * _delta_x;

    for (int i = 0; i <= _nb_subdivisions + 1; ++i) {
        double x = i * _delta_x - offset;           // centres des cellules
        double h = (x < 0.0) ? hgauche : hdroite;   // discontinuité à x=0
        _mailles[i] = maille(x, h, 0.0);
    }

    cout << "Maillage créé avec " << _nb_subdivisions << " mailles." << endl;
    cout << "Longueur gauche ajustée: " << _longueur_gauche << ", Longueur droite ajustée: " 
         << _longueur_droite << ", Delta x: " << _delta_x << endl;
}

Maillage::Maillage(const Maillage& entre)
{
this->_mailles = entre.getMailles();
this->_longueur_gauche = entre.getLongueur_gauche();
this->_longueur_droite = entre.getLongueur_droite();
this->_nb_subdivisions = entre.getNbSubdivisions();
this->_delta_x = entre.getDeltaX();

}



double Maillage::getLongueur_gauche() const {
    return _longueur_gauche;
}
double Maillage::getLongueur_droite() const {
    return _longueur_droite;
}
int Maillage::getNbSubdivisions() const {
    return _nb_subdivisions;
}
double Maillage::getDeltaX() const {
    return _delta_x;
}   
void Maillage::afficherMaillage() const {
    cout << "Longueur gauche: " << _longueur_gauche << endl;
    cout << "Longueur droite: " << _longueur_droite << endl;
    cout << "Nombre de subdivisions: " << _nb_subdivisions << endl;
    cout << "Delta x: " << _delta_x << endl;
}



