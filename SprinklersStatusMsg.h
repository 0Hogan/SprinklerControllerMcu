#ifndef SPRINKLERS_STATUS_MSG_H
#define SPRINKLERS_STATUS_MSG_H

#include <list>

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

        SprinklersStatusMsg(const std::list<Job*> currentJobs) : Message(), jobs(currentJobs) {}

        const std::string getPayload() const override
        {
            return serialize();
        }

        const std::string serialize() const
        {
            JsonDocument doc;
            for (const auto& job : jobs)
            {
                JsonObject job_json = doc.createNestedObject();
                job_json["ZoneNumber"] = job->getZoneNumber();
                job_json["Duration_s"] = job->getTimeRemaining_s();
            }

            std::string payloadBuffer;
            serializeJson(doc, payloadBuffer);
            LOG_DEBUG("Serialized SprinklersStatusMsg: %s", payloadBuffer.c_str());
            return payloadBuffer;
        }

        /******** MESSAGE DATA ********/
        std::list<Job*> jobs;

    };
}

#endif // SPRINKLERS_STATUS_MSG_H