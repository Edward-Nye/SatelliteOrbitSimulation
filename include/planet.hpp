#ifndef PLANET_HPP
#define PLANET_HPP

#include <string>
#include <array>

class Planet {
public:
    std::string name;
    std::array<double, 3> position;
    std::array<double, 3> velocity;
    std::array<double, 3> acceleration;
    double radius;
    double mass;
    std::array<double, 2> spin;

    Planet(const std::string& name, const std::array<double, 3>& pos,
           const std::array<double, 3>& vel, const std::array<double, 3>& acc,
           double radius, double mass, const std::array<double, 2>& spin);

    void updatePosition(double timeStep);
};

#endif // PLANET_HPP
