#include "SprinklerSystem.h"

/**
 * @brief Turns off all other zones, and then turns on the given zone.
 * 
 * @param zoneNumber The zone number to turn on.
 */
void SprinklerSystem::turnZoneOn(const uint8_t zoneNumber)
{
    for (uint8_t i = 0; i < m_zones.size(); i++)
    {
        if (i == zoneNumber)
        {
            continue;
        }
        m_zones[i].turnOff();
    }
    m_zones[zoneNumber].turnOn();
}

/**
 * @brief Turns off the given zone.
 * 
 * @param zoneNumber The zone number to turn off.
 */
void SprinklerSystem::turnZoneOff(const uint8_t zoneNumber)
{
    m_zones[zoneNumber].turnOff();
}

/**
 * @brief Turns off all zones.
 * 
 */
void SprinklerSystem::turnAllZonesOff()
{
    for (int i = 0; i < m_zones.size(); i++)
    {
        m_zones[i].turnOff();
    }
}

/**
 * @brief Returns which zone is currently turned on.
 * 
 * @return int The number for the zone currently turned on or -1 if none are on.
 */
int SprinklerSystem::whichZoneIsOn() const
{
    for (uint8_t i = 0; i < m_zones.size(); i++)
    {
        if (m_zones[i].getState())
        {
            return i;
        }
    }
    return -1;
}