#ifndef PLANET_HPP
#define PLANET_HPP

#include "satellite.hpp"
#include <string>
#include <array>

class Satellite;

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

    void updatePosition(const Satellite& Satellite, double timeStep, const std::string& pointOfRef);
};

#endif // PLANET_HPP
