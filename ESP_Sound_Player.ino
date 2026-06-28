

#include <ESP8266WiFi.h>    //https://github.com/esp8266/Arduino
#include "handleAudio.h"
#include "handleLedRing.h"
#include "handleWebpage.h"
#include "LittleFS.h"
#include <SD.h>
#include "Config.h"

// You may need a fast SD card. Set this as high as it will work (40MHz max).
#define SPI_SPEED SD_SCK_MHZ(35)

//declaration of needed instances
HandleAudio *handleAudio;
HandleLedRing *handleLedRing;
HandleWebpage *handleWebpage;

File dir;
unsigned long milliTime;
unsigned long lastMilliTime = 0;
String filelist;

void setup()
{
    Serial.begin(115200);
    Serial.print("\n");
    
    //init little filesystem
    if (LittleFS.begin())
    {
        Serial.print("LittleFS started successfully\n");
        FSInfo fs_info;
        LittleFS.info(fs_info);
        Serial.printf("FS total Bytes %d\n", fs_info.totalBytes);
        Serial.printf("FS used Bytes %d\n", fs_info.usedBytes);
    }
    else
    {
        Serial.print("Error: Could not start LittleFS!\n");
    }

    Serial.print("Initializing SD card...");
    if (!SD.begin(16, SPI_SPEED))
    {
        Serial.println("initialization failed!");
        filelist = "[]";
    }
    else
    {
        Serial.println("initialization done.");
        dir = SD.open("/");
        Serial.println("Start searching of mp3 files...");
        bool isFile = true;
        filelist = "[";
        int filecount = 0;
        while(isFile)
        {
            File file = dir.openNextFile();
            if (file)
            {
                if (String(file.name()).endsWith(".mp3"))
                {
                    Serial.printf("\t%s\n", file.name());
                    if (filecount == 0)
                    {
                        filelist = filelist +  "\"" + file.name() + "\"";
                    }
                    else
                    {
                        filelist = filelist + ", \"" +  file.name() + "\"";
                    }
                    filecount++;
                }
            }
            else
            {
                isFile = false;
                Serial.println("File search done");
            }
        }
        filelist = filelist + "]";
    }
    

    handleWebpage = new HandleWebpage(filelist);
    handleAudio = new HandleAudio();
    handleLedRing = nullptr;

    if (LED_RING_ENABLED)
    {
        handleLedRing = new HandleLedRing(LED_RING_LED_COUNT, LED_RING_PIN, LED_RING_BRIGHTNESS);
        handleLedRing->begin();
    }

    handleWebpage->setCallBackPlaySound(handleAudio->playSound);
    handleWebpage->setCallBackStopSound(handleAudio->stopSound);
    handleWebpage->setCallBackSetMaxGain(handleAudio->setMaxGain);
    
    handleWebpage->setupHandleWebpage();

    WiFi.mode(WIFI_AP);
    WiFi.softAPConfig(apIP, apIP, IPAddress(255, 255, 255, 0));
    WiFi.softAP("ESPSoundPlayer");

    // if DNSServer is started with "*" for domain name, it will reply with
    // provided IP to all DNS request
    dnsServer.start(DNS_PORT, "*", apIP);
}

void loop()
{
    bool soundPlaying = handleAudio->isSoundPlaying();
    if (handleLedRing != nullptr)
    {
        handleLedRing->updateFromAudioLevel(handleAudio->getCurrentLevel(), soundPlaying);
    }

    if (!soundPlaying)
    {
        dnsServer.processNextRequest();
        handleWebpage->handleClient();
    }
    else
    {
        milliTime = millis();
        if((milliTime - lastMilliTime) > 10)
        {
            lastMilliTime = milliTime;
            dnsServer.processNextRequest();
            handleWebpage->handleClient();
        }
        
    }
}
