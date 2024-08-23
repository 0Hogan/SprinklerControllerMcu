#include <iterator>

#include "SprinklerSystemControl.h"

void SprinklerSystemControl::processJobs()
{
    auto currentlyRunningZone = whichZoneIsOn();

    if (currentlyRunningZone == -1)
    {
        if (m_jobs.empty())
        {
            // Nothing to do. The job queue is empty and nothings on.
            return;
        }
        else
        {        
            auto currentJob = m_jobs.front();
            currentJob.startJob();
            return;
        }
    }
    else
    {
        if (m_jobs.empty())
        {
            LOG_ERROR("No jobs were in the queue, but zone #%d was somehow still running!", currentlyRunningZone);
            turnAllZonesOff();
            return;
        }
        
        auto currentJob = m_jobs.front();
        if (currentJob.getZoneNumber() != currentlyRunningZone)
        {
            turnZoneOff(currentlyRunningZone);
            LOG_ERROR("Zone #%u should be running, but zone #%d was running instead!", currentJob.getZoneNumber(), currentlyRunningZone);
            currentJob.startJob();
            return;
        }
        else
        {
            if (currentJob.isComplete())
            {
                currentJob.completeJob();
                m_jobs.pop_front();
                currentJob = m_jobs.front();
                currentJob.startJob();
                return;
            }
            else
            {
                // The correct zone is running, but the job isn't finished yet, so nothing to do...
                return;
            }
        }
    }
}

/**
 * @brief Adds a job to the queue.
 * 
 * @param zoneNumber The number of the zone to run.
 * 
 * @param duration_s The length of time (in seconds) for which the zone should be run.
*/
void SprinklerSystemControl::enqueueZone(const uint8_t zoneNumber, const uint32_t duration_s)
{
    for (auto& zone : m_zones)
    {
        if (zone.getZoneNumber() == zoneNumber)
        {
            m_jobs.push_back(Job(zone, duration_s));
            return;
        }
    }

    LOG_ERROR("Couldn't add the given job to the queue! (ZoneNumber=%u;Duration_s=%lu)", zoneNumber, duration_s);
}

/**
 * @brief Removes all jobs queued for the given zone number.
 * 
 * @param zoneNumber The zone number to remove from the queue.
*/
void SprinklerSystemControl::dequeueZone(const uint8_t zoneNumber)
{
    for (auto it = m_jobs.begin(); it != m_jobs.end(); ++it)
    {
        if (it->getZoneNumber() == zoneNumber)
        {
            m_jobs.erase(it);
        }
    }
}

/**
 * @brief Removes a job from the queue by its index.
 * 
 * @param index The index of the job to be removed from the queue.
*/
void SprinklerSystemControl::dequeueElementByIndex(const uint8_t index)
{
    auto it = std::next(m_jobs.cbegin(), index);
    m_jobs.erase(it);
}

/**
 * @brief Turns off all other zones, and then turns on the given zone.
 * 
 * @param zoneNumber The zone number to turn on.
 */
void SprinklerSystemControl::turnZoneOn(const uint8_t zoneNumber)
{
    for (uint8_t i = 0; i < m_zones.size(); i++)
    {
        if (i == zoneNumber)
        {
            continue;
        }
        turnZoneOff(i);
    }
    m_zones[zoneNumber].on();
}

/**
 * @brief Turns off the given zone.
 * 
 * @param zoneNumber The zone number to turn off.
 * 
 * @todo Make this function inline.
 */
void SprinklerSystemControl::turnZoneOff(const uint8_t zoneNumber)
{
    m_zones[zoneNumber].off();
}

/**
 * @brief Turns off all zones.
 * 
 */
void SprinklerSystemControl::turnAllZonesOff()
{
    for (int i = 0; i < m_zones.size(); i++)
    {
        m_zones[i].off();
    }
}

/**
 * @brief Returns which zone is currently turned on.
 * 
 * @return int The number for the zone currently turned on or -1 if none are on.
 */
int SprinklerSystemControl::whichZoneIsOn() const
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

void SprinklerSystemControl::sprinklerCmdCallback(const char* payload, const uint16_t payloadSize)
{
    Mqtt::SprinklersCmdMsg msg(payload, payloadSize);

    switch (msg.command)
    {
        case Mqtt::SprinklersCmdMsg::Command::InvalidCommandError:
            LOG_ERROR("Invalid command!\n\tPayload=\"%s\"");
            return;
        case Mqtt::SprinklersCmdMsg::Command::MissingCommandError:
            LOG_ERROR("Missing a command field!\n\tPayload=\"%s\"", payload);
            return;
        case Mqtt::SprinklersCmdMsg::Command::DeserializeError:
            LOG_ERROR("Encountered a deserialization error!\n\tPayload=\"%s\"", payload);
            return;
        case Mqtt::SprinklersCmdMsg::Command::EnqueueJob:
            enqueueZone(msg.zoneNumber, msg.duration_s);
            break;
        case Mqtt::SprinklersCmdMsg::Command::DequeueJobByZone:
            dequeueZone(msg.zoneNumber);
            break;
        case Mqtt::SprinklersCmdMsg::Command::PauseQueueExecution:
            /// @todo Need to implement pauseQueueExecution command.
            break;
        case Mqtt::SprinklersCmdMsg::Command::ResumeQueueExecution:
            /// @todo Need to implement resumeQueueExecution command.
            break;
        case Mqtt::SprinklersCmdMsg::Command::RequestQueueStatus:
            Mqtt::SprinklersStatusMsg statusMsg(m_jobs);
            m_sprinklerStatusPub->publish(statusMsg);
            break;
    }
}