#include <iterator>

#include "SprinklerSystemControl.h"

void SprinklerSystemControl::processJobs()
{
    auto currentlyRunningZone = whichZoneIsOn();

    if (currentlyRunningZone == -1)
    {
        if (m_jobs.empty())
        {
            // Nothing to do. The job queue is empty and nothing is on.
            return;
        }
        else
        {
            auto& currentJob = m_jobs.front();
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

        auto& currentJob = m_jobs.front();
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

                if (m_jobs.empty())
                {
                  LOG_INFO("Finished all queued jobs!");
                  return;
                }
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
void SprinklerSystemControl::enqueueZone(const uint8_t zoneNumber, const uint64_t duration_s)
{
    if (duration_s == 0)
    {
        LOG_ERROR("Skipping job with duration_s=0!");
        return;
    }

    for (auto& zone : m_zones)
    {
        if (zone.getZoneNumber() == zoneNumber)
        {
            m_jobs.push_back(Job(&zone, duration_s));
            LOG_INFO("Enqueued a new job (Zone=%u; Duration = %llu)", zoneNumber, duration_s);
            return;
        }
    }

    LOG_ERROR("Couldn't find the specified zone number! (ZoneNumber=%u; Duration_s=%llu)", zoneNumber, duration_s);
}

/**
 * @brief Removes all jobs queued for the given zone number.
 *
 * @param zoneNumber The zone number to remove from the queue.
*/
void SprinklerSystemControl::dequeueZone(const uint8_t zoneNumber)
{
    uint8_t jobsDequeued = 0;
    for (auto it = m_jobs.begin(); it != m_jobs.end(); ++it)
    {
        if (it->getZoneNumber() == zoneNumber)
        {
            m_jobs.erase(it);
            jobsDequeued++;
        }
    }
    LOG_INFO("Dequeued %u job(s) for zone #%u", jobsDequeued, zoneNumber);
}

/**
 * @brief Removes a job from the queue by its index.
 *
 * @param index The index of the job to be removed from the queue.\
 *
 * @todo Provide details about the job in the log msg.
*/
void SprinklerSystemControl::dequeueElementByIndex(const uint8_t index)
{
    auto it = std::next(m_jobs.cbegin(), index);
    m_jobs.erase(it);
    LOG_INFO("Dequeued job #%u.", index);
}

/**
 * @brief Turns off all other zones, and then turns on the given zone.
 *
 * @param zoneNumber The zone number to turn on.
 */
void SprinklerSystemControl::turnZoneOn(const uint8_t zoneNumber)
{
    PAUSE_LOGGING();
    for (uint8_t i = 0; i < m_zones.size(); i++)
    {
        if (i == zoneNumber)
        {
            continue;
        }
        turnZoneOff(i);
    }
    RESUME_LOGGING();
    LOG_DEBUG("Turning Zone #%u on!", zoneNumber);
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
    LOG_DEBUG("Turning Zone #%u off!", zoneNumber);
    m_zones[zoneNumber].off();
}

/**
 * @brief Turns off all zones.
 *
 */
void SprinklerSystemControl::turnAllZonesOff()
{
    LOG_DEBUG("Turning all zones off!");
    PAUSE_LOGGING();
    for (int i = 0; i < m_zones.size(); i++)
    {
        m_zones[i].off();
    }
    RESUME_LOGGING();
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
            return m_zones[i].getZoneNumber();
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
            LOG_ERROR("Tried to pause queue execution, but the logic hasn't been implemented yet!");
            break;
        case Mqtt::SprinklersCmdMsg::Command::ResumeQueueExecution:
            /// @todo Need to implement resumeQueueExecution command.
            LOG_ERROR("Tried to resume queue execution, but the logic hasn't been implemented yet!");
            break;
        case Mqtt::SprinklersCmdMsg::Command::RequestQueueStatus:
            LOG_INFO("Queue status was requested. Publishing now...");
            Mqtt::SprinklersStatusMsg statusMsg(m_jobs);
            m_sprinklerStatusPub->publish(statusMsg);
            break;
    }
}