#include <cstdint>
#include <vector>

#include <AdafruitPublisher.h>
#include <AdafruitSubscriber.h>
#include <Arduino.h>
#include <McuLogger.h>
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

const char SPRINKLERS_CMD_TOPIC_NAME[] = "/iot/sprinklers/cmd";
const char SPRINKLERS_STATUS_TOPIC_NAME[] = "/iot/sprinklers/status";
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

void MQTT_connect();

void setup()
{
    delay(5000);
    Serial.begin(115200);
    Logger::addOutputStream(&Serial);
    LOG_INFO("***** START OF DAY *****");

    auto stat = WiFi.begin(wifiSsid, wifiPassword);
    // WiFi.connect(wifiSsid, wifiPassword);
    // auto stat = WiFi.status();
    while (stat != WL_CONNECTED)
    {
        LOG_DEBUG("Waiting for WiFi Connection to %s (Status=%u)...", WiFi.SSID(), static_cast<uint8_t>(stat));
        delay(500);
        stat = WiFi.status();
    }
    LOG_INFO("Connected to WiFi Network: %s", wifiSsid);

    std::vector<Zone> zones; ///< @todo This can probably be switched to a statically defined array rather than a vector - though it might be nice if adding/removing zones remotely...
    for (const auto &pin : ZONE_PINS)
    {
        zones.push_back(Zone((zones.size() + 1), pin));
    }

    sprinklersCmdSub = std::make_shared<Mqtt::AdafruitSubscriber<Mqtt::SprinklersCmdMsg>>(&adafruitClient, SPRINKLERS_CMD_TOPIC_NAME);
    sprinklersStatusPub = std::make_shared<Mqtt::AdafruitPublisher<Mqtt::SprinklersStatusMsg>>(&adafruitClient, SPRINKLERS_STATUS_TOPIC_NAME);
    sprinklerSystemControl = std::make_unique<SprinklerSystemControl>(zones, sprinklersStatusPub, sprinklersCmdSub);
    sprinklersCmdSub->subscribe(sprinklerCmdCallbackWrapper);
    MQTT_connect();
    LOG_INFO("Adafruit client connected!");
}

void loop()
{
    MQTT_connect();
    checkSubscriptions();
    sprinklerSystemControl->processJobs();
    statusIndicator.refresh();
}

void MQTT_connect()
{
    int8_t ret;

    // Stop if already connected.
    if (adafruitClient.connected()) {
        return;
    }

    LOG_INFO("Connecting to MQTT... ");

    uint8_t retries = 3;
    while ((ret = adafruitClient.connect()) != 0) { // connect will return 0 for connected
        LOG_ERROR(adafruitClient.connectErrorString(ret));
        LOG_INFO("Retrying MQTT connection in 5 seconds...");
        adafruitClient.disconnect();
        delay(5000);  // wait 5 seconds
        retries--;
        if (retries == 0)
        {
            LOG_ERROR("Out of retries! Going into infinite loop...");
            // basically die and wait for WDT to reset me
            while (1)
            {
                statusIndicator.refresh();
            }
        }
    }
    LOG_INFO("MQTT Connected!");
}