#ifndef SPRINKLER_ZONE_H
#define SPRINKLER_ZONE_H

#include <cstdint>
#include <McuHardwareInterfaces.h>

/**
 * @brief Represents a Sprinkler Zone.
 * 
 */
class Zone : public DigitalOutputPin
{
public:
    Zone(uint8_t zoneNumber, uint8_t pinNumber, bool invertLogic=false)
        : m_zoneNumber(zoneNumber), DigitalOutputPin(pinNumber, invertLogic) {}

    const uint8_t getZoneNumber() const { return m_zoneNumber; }

private:
    uint8_t m_zoneNumber;
};

#endif // SPRINKLER_ZONE_H