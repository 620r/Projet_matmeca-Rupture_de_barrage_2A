#ifndef ERREUR_H
#define ERREUR_H
#include "solveur_vf.h"
#include "solution_exacte.h"

std::vector<double> erreur(Maillage& maillage_approx, Maillage& maillage_exacte);

// Ajouts pour le diagnostic
std::vector<double> erreur_L1(const Maillage& maillage_approx, const Maillage& maillage_exacte);

// Erreurs masquées (exclusion d'intervalles autour des discontinuités)
// Chaque intervalle est fourni par une paire [xmin, xmax]
std::vector<double> erreur_L1_masquee(const Maillage& maillage_approx,
									  const Maillage& maillage_exacte,
									  const std::vector<std::pair<double,double>>& intervalles_exclus);

std::vector<double> erreur_L2_masquee(const Maillage& maillage_approx,
									  const Maillage& maillage_exacte,
									  const std::vector<std::pair<double,double>>& intervalles_exclus);






#endif // ERREUR_H