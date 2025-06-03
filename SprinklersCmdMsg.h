#ifndef SPRINKLERS_CMD_MSG_H
#define SPRINKLERS_CMD_MSG_H

#include <ArduinoJson.h>
#include <Message.h>

#include "Job.h"

namespace Mqtt
{

    class SprinklersCmdMsg : Message
    {
    public:
        SprinklersCmdMsg(const char* payload, const size_t size) : Message()
        {
            deserialize(payload, size);
        }

        /**
         * @brief Get the Payload object
         *
         * @return const char*
         *
         * @todo Need to implement this. It isn't needed yet, so laziness prevails.
         */
        const char* getPayload() const override {}


        const char* serialize()
        {
            JsonDocument msg;
            msg["Command"] = command;
            msg["ZoneNumber"] = zoneNumber;
            msg["Duration_s"] = duration_s;

            std::string msgBuffer;
            serializeJson(msg, msgBuffer);
            return msgBuffer.c_str();
        }

        void deserialize(const char* payload, const size_t size)
        {
            JsonDocument msg;
            auto errorCode = deserializeJson(msg, payload, size);

            if (errorCode == DeserializationError::Ok)
            {
                command = msg["Cmd"];
                zoneNumber = msg["ZoneNumber"];
                duration_s = msg["Duration_s"];
            }
            else
            {
                command = Command::DeserializeError;
            }
        }

        enum Command : uint8_t
        {
            MissingCommandError, ///< Indicates that a command was missing
            EnqueueJob, ///< Adds a job to the queue
            DequeueJobByIndex, ///< Removes a job from the queue (using its index)
            PauseQueueExecution, ///< Pauses execution of the job queue
            ResumeQueueExecution, ///< Resumes execution of the job queue
            StopQueueExecution, ///< Cancels all jobs currently in the queue
            RequestQueueStatus, ///< Requests the current status of the queue
            DeserializeError, ///< Indicates an error while deserializing the message
            InvalidCommandError, ///< Indicates that the specified command is invalid
        };

        /******** MESSAGE DATA ********/
        Command command = Command::MissingCommandError;
        uint8_t zoneNumber = 0xff; ///< What zone number should be run
        uint64_t duration_s = 0; ///< How long the zone should be run (in seconds)
    };
}

#endif // SPRINKLERS_CMD_MSG_H