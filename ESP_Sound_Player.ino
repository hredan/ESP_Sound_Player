#ifdef ESP32
#include <WiFi.h>
#else
#include <ESP8266WiFi.h>  // https://github.com/esp8266/Arduino
#endif
#include "./handle_audio.h"
#include "./handle_esp_config.h"
#include "./handle_led_ring.h"
#include "./handle_webpage.h"
#include "LittleFS.h"
#include <SD.h>
#include "Config.h"

// You may need a fast SD card. Set this as high as it will work (40MHz max).
#ifdef ESP8266
#define SPI_SPEED SD_SCK_MHZ(35)
#endif

// declaration of needed instances
HandleAudio *handleAudio;
HandleLedRing *handleLedRing;
HandleWebpage *handleWebpage;

File dir;
uint32_t milliTime;
uint32_t lastMilliTime = 0;
String filelist;

void setup() {
    Serial.begin(115200);
    Serial.print("\n");

    // init little filesystem
    if (LittleFS.begin()) {
        Serial.print("LittleFS started successfully\n");
#ifdef ESP32
        Serial.printf("FS total Bytes %d\n", LittleFS.totalBytes());
        Serial.printf("FS used Bytes %d\n", LittleFS.usedBytes());
#else
        FSInfo fs_info;
        LittleFS.info(fs_info);
        Serial.printf("FS total Bytes %d\n", fs_info.totalBytes);
        Serial.printf("FS used Bytes %d\n", fs_info.usedBytes);
#endif
    } else {
        Serial.print("Error: Could not start LittleFS!\n");
    }

    Serial.print("Initializing SD card...");

#ifdef ESP8266
    if (!SD.begin(16, SPI_SPEED)) {
#else
    if (!SD.begin()) {
#endif
        Serial.println("initialization failed!");
        filelist = "[]";
    } else {
        Serial.println("initialization done.");
        dir = SD.open("/");
        Serial.println("Start searching of mp3 files...");
        bool isFile = true;
        filelist = "[";
        int filecount = 0;
        while (isFile) {
            File file = dir.openNextFile();
            if (file) {
                if (String(file.name()).endsWith(".mp3")) {
                    Serial.printf("\t%s\n", file.name());
                    if (filecount == 0) {
                        filelist = filelist + "\"" + file.name() + "\"";
                    } else {
                        filelist = filelist + ", \"" + file.name() + "\"";
                    }
                    filecount++;
                }
            } else {
                isFile = false;
                Serial.println("File search done");
            }
        }
        filelist = filelist + "]";
    }

    handleWebpage = new HandleWebpage(filelist);
    handleAudio = new HandleAudio();
    handleLedRing = nullptr;

    EspLedRingConfig ledRingConfig = getLedRingConfig();
    if (ledRingConfig.enabled) {
        handleLedRing = new HandleLedRing(ledRingConfig.ledCount, ledRingConfig.pin, ledRingConfig.brightness);
        handleLedRing->begin();
    }

    handleWebpage->setCallBackPlaySound(handleAudio->playSound);
    handleWebpage->setCallBackStopSound(handleAudio->stopSound);
    handleWebpage->setCallBackSetMaxGain(handleAudio->setMaxGain);

    WiFi.mode(WIFI_AP);

    EspApConfig apConfig = getApConfig();

    bool apConfigOk = WiFi.softAPConfig(apIP, apIP, IPAddress(255, 255, 255, 0));
    bool apStartOk = apConfig.password.length() > 0
                         ? WiFi.softAP(apConfig.ssid.c_str(), apConfig.password.c_str())
                         : WiFi.softAP(apConfig.ssid.c_str());

    Serial.printf("softAPConfig: %s\n", apConfigOk ? "OK" : "FAILED");
    Serial.printf("softAP start: %s\n", apStartOk ? "OK" : "FAILED");
    Serial.printf("softAP SSID: %s\n", apConfig.ssid.c_str());
    if (apStartOk) {
        Serial.print("AP IP: ");
        Serial.println(WiFi.softAPIP());
    }

    // if DNSServer is started with "*" for domain name, it will reply with
    // provided IP to all DNS request
    if (!dnsServer.start(DNS_PORT, "*", apIP)) {
        Serial.println("Warning: DNS server failed to start");
    }
    handleWebpage->setupHandleWebpage();
}

void loop() {
    bool soundPlaying = handleAudio->isSoundPlaying();
    if (handleLedRing != nullptr) {
        handleLedRing->updateFromAudioLevel(handleAudio->getCurrentLevel(), soundPlaying);
    }

    if (!soundPlaying) {
        dnsServer.processNextRequest();
        handleWebpage->handleClient();
    } else {
        milliTime = millis();
        if ((milliTime - lastMilliTime) > 10) {
            lastMilliTime = milliTime;
            dnsServer.processNextRequest();
            handleWebpage->handleClient();
        }
    }
}
