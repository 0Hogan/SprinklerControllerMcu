#include "Job.h"

Job::~Job()
{
    m_zone->off();
    LOG_DEBUG("Job for zone #%u destroyed.", getZoneNumber());
}

/**
 * @brief Starts execution of the job.
 *
 */
void Job::startJob()
{
    m_zone->on();
    m_startTime_ms = millis();
    LOG_INFO("Started job (zoneNumber=%u; duration_s=%lu seconds; m_startTime_ms=%llu)", getZoneNumber(), getDuration_s(), m_startTime_ms);
}

/**
 * @brief Pauses execution of the job (so that it can be resumed where it was at)
 *
 */
void Job::pauseJob()
{
    m_zone->off();
    m_duration_ms = millis() - m_startTime_ms;
    m_startTime_ms = 0;
    LOG_INFO("Paused job execution for zone #%u with %lu seconds remaining.", getZoneNumber(), getDuration_s());
}

/**
 * @brief Stops execution of the job.
 *
 */
void Job::stopJob()
{
    m_zone->off();
    m_duration_ms = 0;
    LOG_INFO("Stopped running zone #%u without completing.", getZoneNumber());
}

/**
 * @brief Completes the job.
 *
 */
void Job::completeJob()
{
    m_zone->off();
    LOG_INFO("Finished running zone #%u for %lu seconds.", getZoneNumber(), getDuration_s());
}
