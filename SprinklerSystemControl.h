#ifndef SPRINKLER_SYSTEM_H
#define SPRINKLER_SYSTEM_H

#include <cstdint>
#include <deque>
#include <vector>

#include <Client.h>
#include <McuLogger.h>
#include <Publisher.h>
#include <Subscriber.h>

#include "Job.h"
#include "SprinklersStatusMsg.h"
#include "SprinklersCmdMsg.h"
#include "Zone.h"

/**
 * @brief A class to control which Sprinkler Zone is on (if any).
 *
 */
class SprinklerSystemControl
{
public:
    SprinklerSystemControl(std::vector<Zone> zones) : m_zones(zones) {}
    SprinklerSystemControl(std::vector<Zone> zones,
                           std::shared_ptr<Mqtt::Publisher<Mqtt::SprinklersStatusMsg>> sprinklersStatusPub,
                           std::shared_ptr<Mqtt::Subscriber<Mqtt::SprinklersCmdMsg>> sprinklersCmdSub)
                           : m_zones(zones), m_sprinklerStatusPub(sprinklersStatusPub), m_sprinklerCmdSub(sprinklersCmdSub)
    {}

    void processJobs();
    /**
     * @brief Callback for to be executed whenever a Sprinkler Command message is received.
     *
     * @param payload
     * @param payloadSize
     */
    void sprinklerCmdCallback(const char* payload, const uint16_t payloadSize);

    void enqueueZone(const uint8_t zoneNumber, const uint64_t duration_s);
    void dequeueZone(const uint8_t zoneNumber);
    void dequeueJobByIndex(const uint8_t index);

    int whichZoneIsOn() const;

private:
    void turnZoneOn(const uint8_t zoneNumber);
    void turnZoneOff(const uint8_t zoneNumber);
    void turnAllZonesOff();

    std::vector<Zone> m_zones;
    std::deque<Job> m_jobs;

    std::shared_ptr<Mqtt::Publisher<Mqtt::SprinklersStatusMsg>> m_sprinklerStatusPub = nullptr;
    std::shared_ptr<Mqtt::Subscriber<Mqtt::SprinklersCmdMsg>> m_sprinklerCmdSub = nullptr;
};
#endif //SPRINKLER_SYSTEM_H