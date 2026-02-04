#include "erreur.h"
#include <iostream>
#include <fstream>
#include <vector>
#include <cmath>

using namespace std;
using namespace Eigen;
 
// maillageA = maillage approché
// maillageE = maillage exacte

vector<double> erreur(Maillage& maillage_approx, Maillage& maillage_exacte)
{
    vector<double> err(2, 0.0);
    const int N = maillage_approx.getNbSubdivisions();

    const auto& maillageA = maillage_approx.getMailles();
    const auto& maillageE = maillage_exacte.getMailles();

    const double dx = maillage_approx.getDeltaX();

    double sum_sq_h = 0.0, sum_sq_q = 0.0;

    // mailles internes [1..N] si 2 ghost-cells
    for (int i = 1; i <= N; ++i) {
        const double hA = maillageA[i].getHauteur();
        const double qA = maillageA[i].getDebit();
        const double hE = maillageE[i].getHauteur();
        const double qE = maillageE[i].getDebit();

        const double eh = hA - hE;
        const double eq = qA - qE;

        sum_sq_h += eh * eh;
        sum_sq_q += eq * eq;
    }

    // Norme L2 intégrée: sqrt( dx * Σ e^2 )
    err[0] = std::sqrt(dx * sum_sq_h);
    err[1] = std::sqrt(dx * sum_sq_q);
    return err;
}

std::vector<double> erreur_L1(const Maillage& maillage_approx, const Maillage& maillage_exacte)
{
    std::vector<double> err(2, 0.0);
    const int N = maillage_approx.getNbSubdivisions();

    const auto& maillageA = maillage_approx.getMailles();
    const auto& maillageE = maillage_exacte.getMailles();

    const double dx = maillage_approx.getDeltaX();

    double sum_abs_h = 0.0, sum_abs_q = 0.0;

    for (int i = 1; i <= N; ++i) {
        const double hA = maillageA[i].getHauteur();
        const double qA = maillageA[i].getDebit();
        const double hE = maillageE[i].getHauteur();
        const double qE = maillageE[i].getDebit();

        sum_abs_h += std::abs(hA - hE);
        sum_abs_q += std::abs(qA - qE);
    }

    // Norme L1 intégrée: dx * Σ |e|
    err[0] = dx * sum_abs_h;
    err[1] = dx * sum_abs_q;
    return err;
}

std::vector<double> erreur_L1_masquee(const Maillage& maillage_approx,
                                      const Maillage& maillage_exacte,
                                      const std::vector<std::pair<double,double>>& intervalles_exclus)
{
    std::vector<double> err(2, 0.0);
    const int N = maillage_approx.getNbSubdivisions();
    const auto& maillageA = maillage_approx.getMailles();
    const auto& maillageE = maillage_exacte.getMailles();
    const double dx = maillage_approx.getDeltaX();

    auto est_exclu = [&](double x) {
        for (const auto& iv : intervalles_exclus) {
            if (x >= iv.first && x <= iv.second) return true;
        }
        return false;
    };

    double sum_abs_h = 0.0, sum_abs_q = 0.0;
    for (int i = 1; i <= N; ++i) {
        double x = maillageA[i].getPosition();
        if (est_exclu(x)) continue;
        const double hA = maillageA[i].getHauteur();
        const double qA = maillageA[i].getDebit();
        const double hE = maillageE[i].getHauteur();
        const double qE = maillageE[i].getDebit();
        sum_abs_h += std::abs(hA - hE);
        sum_abs_q += std::abs(qA - qE);
    }
    err[0] = dx * sum_abs_h;
    err[1] = dx * sum_abs_q;
    return err;
}

std::vector<double> erreur_L2_masquee(const Maillage& maillage_approx,
                                      const Maillage& maillage_exacte,
                                      const std::vector<std::pair<double,double>>& intervalles_exclus)
{
    std::vector<double> err(2, 0.0);
    const int N = maillage_approx.getNbSubdivisions();
    const auto& maillageA = maillage_approx.getMailles();
    const auto& maillageE = maillage_exacte.getMailles();
    const double dx = maillage_approx.getDeltaX();

    auto est_exclu = [&](double x) {
        for (const auto& iv : intervalles_exclus) {
            if (x >= iv.first && x <= iv.second) return true;
        }
        return false;
    };

    double sum_sq_h = 0.0, sum_sq_q = 0.0;
    for (int i = 1; i <= N; ++i) {
        double x = maillageA[i].getPosition();
        if (est_exclu(x)) continue;
        const double hA = maillageA[i].getHauteur();
        const double qA = maillageA[i].getDebit();
        const double hE = maillageE[i].getHauteur();
        const double qE = maillageE[i].getDebit();
        const double eh = hA - hE;
        const double eq = qA - qE;
        sum_sq_h += eh * eh;
        sum_sq_q += eq * eq;
    }
    err[0] = std::sqrt(dx * sum_sq_h);
    err[1] = std::sqrt(dx * sum_sq_q);
    return err;
}

