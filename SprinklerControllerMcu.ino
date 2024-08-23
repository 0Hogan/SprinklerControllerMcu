#include <cstdint>
#include <vector>

#include <AdafruitPublisher.h>
#include <AdafruitSubscriber.h>
#include <Arduino.h>
#include <Log.h>
#include <McuHardwareInterfaces.h>
#include <WiFi.h>

#include "Secrets.h"
#include "SprinklerSystemControl.h"
#include "SprinklersCmdMsg.h"
#include "StatusIndicator.h"
#include "Zone.h"

/**
 * Zone 1: Irrigation Line
 * Zone 2: Street-side of Front Sidewalk
 * Zone 3: Front Main
 * Zone 4: House-Side of Street-side of Side Sidewalk
 * Zone 5: House-side of Side Sidewalk
 * Zone 6: Street-Side of Street-side of Side Sidewalk
 * Zone 7:
 * Zone 8:
 * Zone 9:
*/
static const uint8_t ZONE_PINS[] = { 1, 2, 3, 4, 5, 6, 7, 8, 9 };
static const uint8_t LED1_PIN = 12;
static const uint8_t LED2_PIN = 13;

static const char SPRINKLERS_CMD_TOPIC_NAME[] = "/iot/sprinklers/cmd";
static const char SPRINKLERS_STATUS_TOPIC_NAME[] = "/iot/sprinklers/status";
static const uint64_t SUBSCRIPTION_CHECKER_TIMEOUT_MS = 500;

StatusIndicator statusIndicator(LED1_PIN, LED2_PIN);
WiFiClient wiFiClient;
Adafruit_MQTT_Client adafruitClient(&wiFiClient, serverAddress, serverPort, serverUsername, serverPassword);
std::shared_ptr<Mqtt::AdafruitSubscriber<Mqtt::SprinklersCmdMsg>> sprinklersCmdSub;
std::shared_ptr<Mqtt::AdafruitPublisher<Mqtt::SprinklersStatusMsg>> sprinklersStatusPub;
std::unique_ptr<SprinklerSystemControl> sprinklerSystemControl;

void sprinklerCmdCallbackWrapper(char* payload, uint16_t payloadSize)
{
    if (sprinklerSystemControl)
    {
        sprinklerSystemControl->sprinklerCmdCallback(payload, payloadSize);
    }
    else
    {
        LOG_ERROR("Received a Sprinkler Command Message, but no sprinklerSystemControl object exists yet!\n"
                  "Payload=\"%s\"", payload);
    }
}

void checkSubscriptions()
{
    adafruitClient.processPackets(SUBSCRIPTION_CHECKER_TIMEOUT_MS);
}

void setup()
{
    Serial.begin(115200);
    Logger::addOutputStream(&Serial);
    LOG_INFO("***** START OF DAY *****");

    WiFi.begin(wifiSsid, wifiPassword);

    std::vector<Zone> zones;
    for (const auto &pin : ZONE_PINS)
    {
        zones.push_back(Zone((zones.size() + 1), pin));
    }

    sprinklersCmdSub = std::make_shared<Mqtt::AdafruitSubscriber<Mqtt::SprinklersCmdMsg>>(&adafruitClient, SPRINKLERS_CMD_TOPIC_NAME);
    sprinklersStatusPub = std::make_shared<Mqtt::AdafruitPublisher<Mqtt::SprinklersStatusMsg>>(&adafruitClient, SPRINKLERS_STATUS_TOPIC_NAME);
    sprinklerSystemControl = std::make_unique<SprinklerSystemControl>(zones, sprinklersStatusPub, sprinklersCmdSub);
    sprinklersCmdSub->subscribe(sprinklerCmdCallbackWrapper);
    adafruitClient.connect();
}

void loop()
{
    checkSubscriptions();
    sprinklerSystemControl->processJobs();
    statusIndicator.refresh();
}