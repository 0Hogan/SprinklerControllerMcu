#ifndef STATUS_INDICATOR_H
#define STATUS_INDICATOR_H
#include <cstdint>

#include "McuHardwareInterfaces.h"

/**
 * @brief A Status Indicator to communicate the internal state of the software via some form of hardware.
 *
 * @todo Make this a base class and build off of it for dual-LED status indicators.
 */
class StatusIndicator
{
public:
    StatusIndicator(uint8_t led1Pin, uint8_t led2Pin) : m_led1(DigitalOutputPin(led1Pin)), m_led2(DigitalOutputPin(led2Pin))
    {
        m_led1.on();
        m_led2.off();
        m_timeLastToggled_ms = millis();
    }

    void refresh()
    {
        if (millis() - m_timeLastToggled_ms > TIME_BETWEEN_TOGGLES_MS)
        {
            m_led1.toggle();
            m_led2.toggle();
            m_timeLastToggled_ms = millis();
        }
    }

private:
    uint64_t m_timeLastToggled_ms = 0;
    const uint64_t TIME_BETWEEN_TOGGLES_MS = 1000;

    DigitalOutputPin m_led1;
    DigitalOutputPin m_led2;
};

#endif STATUS_INDICATOR_H