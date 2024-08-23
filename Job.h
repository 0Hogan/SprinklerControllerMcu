#ifndef SPRINKLER_JOB_H
#define SPRINKLER_JOB_H

#include "Zone.h"

class Job
{
  public:
    /**
     * @brief Construct a new Job object
     *
     * @param zone The zone that should be run.
     * @param duration_s How long the job should be run for (in seconds).
     */
    Job(Zone zone, time_t duration_s) : m_zone(zone), m_duration_ms(duration_s * MS_PER_S) {};

    /**
     * @brief Sets the job duration.
     *
     * @param duration_s The job duration (in seconds).
     */
    void updateDuration(time_t duration_s) { m_duration_ms = duration_s * MS_PER_S; }

    const bool isComplete() const { return getTimeRemaining_s() <= 0; }

    /**
     * @brief Returns the job duration (in seconds).
     *
     * @return const time_t The job duration (in seconds).
     */
    const time_t getDuration_s() const { return m_duration_ms / MS_PER_S; }

    const time_t getTimeRemaining_s() const { return m_startTime_ms == 0 ? getDuration_s() : (m_duration_ms - (millis() - m_startTime_ms)) / MS_PER_S; }

    const auto getZoneNumber() const { return m_zone.getZoneNumber(); }

    void startJob();
    void pauseJob();
    void stopJob();
    void completeJob();

  private:
    Zone m_zone; ///< The zone that should be run
    time_t m_duration_ms; ///< The time (in seconds) for which the job (i.e. the sprinkler) should run.
    /// @todo Determine if m_startTime_s makes more sense in SprinklerSystemControl
    time_t m_startTime_ms = 0; ///< The time (in milliseconds) the job started execution.

    static const time_t MS_PER_S = 1000; ///< The number of milliseconds in one second.
};

#endif // SPRINKLER_JOB_H