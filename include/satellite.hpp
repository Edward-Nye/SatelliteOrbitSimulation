#ifndef SATELLITE_HPP
#define SATELLITE_HPP

#include <string>
#include <array>

class Satellite {
public:
    std::string name;
    std::array<double, 3> position;
    std::array<double, 3> velocity;
    std::array<double, 3> acceleration;
    double mass;

    Satellite(const std::string& name, const std::array<double, 3>& pos,
              const std::array<double, 3>& vel, const std::array<double, 3>& acc,
              double mass);

    void updatePosition(const Planet& planet, double timeStep);
    void applyManeuver(const std::array<double, 3>& deltaV);
};

#endif // SATELLITE_HPP