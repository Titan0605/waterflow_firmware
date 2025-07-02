#include <TemperatureMeterModule.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>

TemperatureMeterModule::TemperatureMeterModule(const char* serverUrl, const char* mac)
    :_serverUrl(serverUrl), _mac(mac), _interval(60000){}

void TemperatureMeterModule::begin(int temperaturePin, unsigned long intervalMs){
    _tempSensor.begin(temperaturePin);
    _sensor.begin(_tempSensor);
    _interval = intervalMs;
}

void TemperatureMeterModule::handle(){
    unsigned long now = millis();
    if (now - _lastLecture < _interval) return;
    _lastLecture = now;

    if (WiFi.status() != WL_CONNECTED) return;

}

bool TemperatureMeterModule::getState(){

}

bool TemperatureMeterModule::sendTemperature(){
    String url = String(_serverUrl) + "/send-temperature";
    HTTPClient http;
    http.begin(url);
    http.addHeader("Content-Type", "application/json");

    int temp = _sensor.getTempCByIndex(0);
    StaticJsonDocument<200> doc;
    doc["mac_address"] = _mac;
    doc["temp"] = temp;

    String jsonData;
    serializeJson(doc, jsonData);

    int httpResponseCode = http.POST(jsonData);

    if(httpResponseCode == HTTP_CODE_OK){
        _temp = temp;
        bool achieved = checkTempClose(temp);
        return true;
    } else{
        return false;
    }
}

bool TemperatureMeterModule::checkTempClose(){
    String url = String(_serverUrl) + "/get-temperature-waterflow?mac_address=" + _mac;
    HTTPClient http;
    http.begin(url);
    int code = http.GET();
    if(code == HTTP_CODE_OK){
        String payload = http.getString();
        StaticJsonDocument<200> doc;
        DeserializationError err = deserializeJson(doc, payload);
        if (!err){
            bool autoClose = doc["autoClose"];
            int autoCloseTemp = doc["autoCloseTemp"];
            if(autoClose && (_temp < autoCloseTemp)){
                
            }
        }
    }
}

bool TemperatureMeterModule::sendCommand(bool close){

}
