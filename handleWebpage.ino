
#include "handleWebpage.h"
#include "Config.h"

ESP8266WebServer* HandleWebpage::_webServer = nullptr;

HandleWebpage::HandleWebpage(String filelist)
{
  _webServer = new ESP8266WebServer(80);
  _filelist = filelist;
};

void HandleWebpage::handleRoot() {                         // When URI / is requested, send a web page with a button to toggle the LED
  handleWebRequests();
}

void HandleWebpage::setCallBackStopSound(CallBackStopSound callBackStopSound)
{
  _callBackStopSound = callBackStopSound;
}

void HandleWebpage::setCallBackSetMaxGain(CallBackSetMaxGain callBackSetMaxGain)
{
  _callBackSetMaxGain = callBackSetMaxGain;
}

void HandleWebpage::setCallBackPlaySound(CallBackPlaySound callBackPlaySound)
{
  _callBackPlaySound = callBackPlaySound;
}

void HandleWebpage::handleStopSound() {
  Serial.println("handleStopSound" + _webServer->arg("plain"));
  if(_callBackStopSound !=nullptr)
  {
    _callBackStopSound();
    _webServer->send(200, "text/plane", "{\"success\": true}");
  }
  else
  {
    _webServer->send(200, "text/plane", "{\"success\": false}");
  }
}

void HandleWebpage::handleSaveData()
{
  String jsonSaveData = _webServer->arg("plain");
  Serial.printf("handleSaveData: %s\n", jsonSaveData.c_str());
  File file = SD.open(CONFIG_FILE_NAME,  (O_WRONLY | O_CREAT));
  file.write(jsonSaveData.c_str());
  file.close();
}

void HandleWebpage::handleSetMaxGain()
{
  Serial.println("handleSetMaxGain" + _webServer->arg("plain"));
  if(_webServer->hasArg("maxGain"))
  {
    float maxGain = _webServer->arg("maxGain").toFloat();
    if (_callBackSetMaxGain != nullptr)
    {
      _callBackSetMaxGain(maxGain);
      _webServer->send(200, "text/plane", "{\"success\": true}");
    }
    else
    {
      Serial.println("Error: _callBackSetMaxGain ==nullptr");
      _webServer->send(200, "text/plane", "{\"success\": false}");
    }
  }
  else
  {
    Serial.println("Error handlePlaySound: missing argument maxGain!");
    _webServer->send(200, "text/plane", "{\"success\": false}");
  }
}

void HandleWebpage::handlePlaySound() {
  Serial.println("handlePlaySound" + _webServer->arg("plain"));
  
  int volume = -1;
  String filename = "";
  bool noError = true;

  if(_webServer->hasArg("volume"))
  {
    volume = _webServer->arg("volume").toInt();
  }
  else
  {
    if (noError)
    {
      Serial.println("Error handlePlaySound: missing argument volume!");
      noError = false;
    }
  }

  if(noError && _webServer->hasArg("soundFile"))
  {
    filename = _webServer->arg("soundFile");
  }
  else
  {
    if (noError)
    {
      Serial.println("Error handlePlaySound: missing argument soundFile!");
      noError = false;
    }
  }

  if(noError && (_callBackPlaySound !=nullptr))
  {
    _callBackPlaySound(filename, volume);
    _webServer->send(200, "text/plane", "{\"success\": true}");
  }
  else
  {
    if (noError)
    {
      Serial.println("Error: _callBackPlaySound ==nullptr");
      noError = false;
    }
  }

  if (!noError)
  {
    _webServer->send(200, "text/plane", "{\"success\": false}");
  }
}

void HandleWebpage::handleGetData() {  
  //String jsonAnswer = "[\"test.mp3\",	\"ff-16b-2c-44100hz.mp3\"]";
  Serial.println("handleGetData send ->  value: " + _filelist);
  
  String configJson = "{\"gainFactor\": 0.1, \"playList\": []}";

  if (SD.exists(CONFIG_FILE_NAME))
  {
    File configFile = SD.open(CONFIG_FILE_NAME, FILE_READ);
    configJson = configFile.readString();
    configFile.close();
  }
  
  String dataJson = "{\"config\":" + configJson + ", \"files\":" + _filelist + "}";

  _webServer->send(200, "text/plane", dataJson);


}

void HandleWebpage::handleWebRequests(){
  if(!loadFromLittleFS(_webServer->uri()))
  {
    Serial.println("Error: handleWebRequests");
    String message = "File Not Detected\n\n";
    message += "URI: ";
    message += _webServer->uri();
    message += "\nMethod: ";
    message += (_webServer->method() == HTTP_GET)?"GET":"POST";
    message += "\nArguments: ";
    message += _webServer->args();
    message += "\n";
    for (uint8_t i=0; i < _webServer->args(); i++){
      message += " NAME:" + _webServer->argName(i) + "\n VALUE:" + _webServer->arg(i) + "\n";
    }
    _webServer->send(404, "text/plain", message);
    //Serial.println(message);
  }
}

bool HandleWebpage::loadFromLittleFS(String path){
  bool returnValue = true;

  Serial.println("Load path: " + path);
  String dataType = "text/plain";

  if(path.endsWith("/")) path += "index.html";
 
  if(path.endsWith(".src")) path = path.substring(0, path.lastIndexOf("."));
  else if(path.endsWith(".html")) dataType = "text/html";
  else if(path.endsWith(".htm")) dataType = "text/html";
  else if(path.endsWith(".css")) dataType = "text/css";
  else if(path.endsWith(".js")) dataType = "application/javascript";
  else if(path.endsWith(".png")) dataType = "image/png";
  else if(path.endsWith(".gif")) dataType = "image/gif";
  else if(path.endsWith(".jpg")) dataType = "image/jpeg";
  else if(path.endsWith(".ico")) dataType = "image/x-icon";
  else if(path.endsWith(".xml")) dataType = "text/xml";
  else if(path.endsWith(".pdf")) dataType = "application/pdf";
  else if(path.endsWith(".zip")) dataType = "application/zip";
  if (LittleFS.exists(path))
  {
    File dataFile = LittleFS.open(path.c_str(), "r");
    if (_webServer->hasArg("download")) dataType = "application/octet-stream";

    if (_webServer->streamFile(dataFile, dataType) != dataFile.size()) {
      Serial.println("Error: streamed file has different size!");
    }
    dataFile.close();
  }
  else
  {
    //Serial.println("Error: Path does not exist and will be redirect to root:");
    Serial.printf("Ignore path %s\n", path.c_str());
    returnValue = loadFromLittleFS("/");
  }
  return returnValue;
}

void HandleWebpage::setupHandleWebpage()
{
  // replay to all requests with same HTML
  _webServer->onNotFound(std::bind(&HandleWebpage::handleWebRequests, this));

  // replay to all requests with same HTML
  //Information about using std::bind -> https://github.com/esp8266/Arduino/issues/1711

  _webServer->on("/", HTTP_GET, std::bind(&HandleWebpage::handleRoot, this));
  _webServer->on("/getData", HTTP_GET, std::bind(&HandleWebpage::handleGetData, this));
  _webServer->on("/playSound", HTTP_POST, std::bind(&HandleWebpage::handlePlaySound, this));
  _webServer->on("/stopSound", HTTP_POST, std::bind(&HandleWebpage::handleStopSound, this));
  _webServer->on("/setMaxGain", HTTP_POST, std::bind(&HandleWebpage::handleSetMaxGain, this));
  _webServer->on("/saveData", HTTP_POST, std::bind(&HandleWebpage::handleSaveData, this));
  _webServer->begin();
}

void HandleWebpage::handleClient()
{
  _webServer->handleClient();
}