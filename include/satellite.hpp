#ifndef SATELLITE_HPP
#define SATELLITE_HPP

#include "planet.hpp"
#include <string>
#include <array>

class Planet;

class Satellite {
public:
    std::string name;
    std::array<double, 3> position;
    std::array<double, 3> velocity;
    std::array<double, 3> acceleration;
    double mass;
    std::array<double, 6> preState;
    // Rotation characteristics
    std::array<double, 3> rotation;    // [roll, pitch, yaw] in radians or degrees
    std::array<double, 3> rotationRates;    // [rollSpeed, pitchSpeed, yawSpeed] in radians or degrees per second

    Satellite(const std::string& name, const std::array<double, 3>& pos,
              const std::array<double, 3>& vel, const std::array<double, 3>& acc,
              double mass, std::array<double, 6> preState, std::array<double, 3> rot,
              std::array<double, 3> rotR);

    void updatePosition(const std::vector<Planet>& planets, const std::vector<Satellite>& satellites, double timeStep, const std::string& pointOfRef, double time);
    
    //void applyManeuver(const std::array<double, 3>& deltaV);
};

#endif // SATELLITE_HPP
