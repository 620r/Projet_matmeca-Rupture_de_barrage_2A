
#ifndef LIMITEUR_H
#define LIMITEUR_H

#include <cmath>
// codage du limiteur de Van der Leer
class Limiteur {
public:
    static double phiVanLeer(double theta) {
        if (!std::isfinite(theta) || theta <= 0.0) {
            return 0.0;
        }
        return (theta + std::abs(theta)) / (1.0 + std::abs(theta));
    }
};

#endif