

#ifdef ESP32
    #include <WiFi.h>
#else
    #include <ESP8266WiFi.h>    //https://github.com/esp8266/Arduino
#endif
#include "handleAudio.h"
#include "handleWebpage.h"
#include "LittleFS.h"
#include <SD.h>

// You may need a fast SD card. Set this as high as it will work (40MHz max).
#ifdef ESP8266
    #define SPI_SPEED SD_SCK_MHZ(35)
#endif

//declaration of needed instances
HandleAudio *handleAudio;
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
        #ifdef ESP32
            Serial.printf("FS total Bytes %d\n", LittleFS.totalBytes());
            Serial.printf("FS used Bytes %d\n", LittleFS.usedBytes());
        #else
            FSInfo fs_info;
            LittleFS.info(fs_info);
            Serial.printf("FS total Bytes %d\n", fs_info.totalBytes);
            Serial.printf("FS used Bytes %d\n", fs_info.usedBytes);
        #endif
    }
    else
    {
        Serial.print("Error: Could not start LittleFS!\n");
    }

    Serial.print("Initializing SD card...");
    
    #ifdef ESP8266
        if (!SD.begin(16, SPI_SPEED))
    #else
        if (!SD.begin(16))
    #endif
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
    if (!handleAudio->isSoundPlaying())
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
