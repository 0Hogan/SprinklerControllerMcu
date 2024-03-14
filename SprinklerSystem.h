#include <vector>

#include "../CommonHardwareInterfaces/Relay.h"

#ifndef SPRINKLER_SYSTEM_H
#define SPRINKLER_SYSTEM_H

class SprinklerSystem
{
  public:
    SprinklerSystem(const std::vector<Relay> zones) : m_zones(zones) {};

    void turnZoneOn(const uint8_t zoneNumber);
    void turnZoneOff(const uint8_t zoneNumber);
    void turnAllZonesOff();
    int whichZoneIsOn() const;

  private:
    std::vector<Relay> m_zones;
};

#endif //SPRINKLER_SYSTEM_H