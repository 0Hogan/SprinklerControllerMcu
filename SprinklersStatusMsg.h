#ifndef SPRINKLERS_STATUS_MSG_H
#define SPRINKLERS_STATUS_MSG_H

#include <deque>

#include <ArduinoJson.h>
#include <Message.h>

#include "Job.h"

namespace Mqtt
{
    // Topic: "/iot/sprinklers/stat"
    class SprinklersStatusMsg : public Message
    {
    public:
        SprinklersStatusMsg() : Message()
        {
            // setQos(1);
        }

        SprinklersStatusMsg(const std::deque<Job> currentJobs) : Message(), jobs(currentJobs) {}

        const char* getPayload() const override
        {
            return serialize();
        }

        const char* serialize() const
        {
            JsonDocument doc;
            JsonArray jobs_json = doc["jobs"].to<JsonArray>();
            for (const auto& job : jobs)
            {
                JsonObject job_json;
                job_json["Zone"] = job.getZoneNumber();
                job_json["TimeRemaining_s"] = job.getTimeRemaining_s();
                jobs_json.add(job_json);
            }

            std::string payloadBuffer;
            serializeJson(doc, payloadBuffer);

            return payloadBuffer.c_str();
        }

        /******** MESSAGE DATA ********/
        std::deque<Job> jobs;

    };
}

#endif // SPRINKLERS_STATUS_MSG_H