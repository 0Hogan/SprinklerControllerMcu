#ifndef SPRINKLER_JOB_H
#define SPRINKLER_JOB_H

#include <cstdint>
#include <McuLogger.h>

#include "Zone.h"

/**
 * @brief A job is a task that can be run on a specific zone for a specific amount of time.
 *
 * @todo Need to re-evaluate the distribution of responsibilities between SprinklerSystemControl class and this class.
 */
class Job
{
  public:
    /**
     * @brief Construct a new Job object
     *
     * @param zone The zone that should be run.
     * @param duration_s How long the job should be run for (in seconds).
     */
    Job(Zone* zone, time_t duration_s) : m_zone(zone), m_duration_ms(duration_s * MS_PER_S) { LOG_DEBUG("Created job (Zone #%u; Duration (s): %llu", zone->getZoneNumber(), duration_s); }

    ~Job();
    /**
     * @brief Sets the job duration.
     *
     * @param duration_s The job duration (in seconds).
     */
    void updateDuration(time_t duration_s) { m_duration_ms = duration_s * MS_PER_S; }

    const bool isComplete() const { return getTimeRemaining_s() == 0; }

    /**
     * @brief Returns the job duration (in seconds).
     *
     * @return const time_t The job duration (in seconds).
     */
    const time_t getDuration_s() const { return m_duration_ms / MS_PER_S; }

    /**
     * @brief Get the StartTime ms object
     *
     * @return const time_t
     *
     * @todo @mhogan, you should be able to get rid of this when done debugging...
     */
    const time_t getStartTime_ms() const { return m_startTime_ms; }

    const time_t getTimeRemaining_s() const
    {
        if (m_startTime_ms == 0)
        {
            LOG_DEBUG("Start time is 0! Haven't started this job yet!");
            return getDuration_s();
        }
        time_t endTime_ms = m_startTime_ms + m_duration_ms;
        if (millis() > endTime_ms)
        {
            LOG_DEBUG("Time's up!");
            return 0;
        }
        return (endTime_ms - millis()) / MS_PER_S;
    }

    const uint8_t getZoneNumber() const { return m_zone->getZoneNumber(); }

    void startJob();
    void pauseJob();
    // void resumeJob(); ///< @todo Need to implement this fn.
    void stopJob();
    void completeJob();

  private:
    Zone* m_zone; ///< The zone that should be run
    time_t m_duration_ms; ///< The time (in milliseconds) for which the job (i.e. the sprinkler) should run.
    time_t m_startTime_ms = 0; ///< The time (in milliseconds) the job started execution.

    static const time_t MS_PER_S = 1000; ///< The number of milliseconds in one second.
};

#endif // SPRINKLER_JOB_H