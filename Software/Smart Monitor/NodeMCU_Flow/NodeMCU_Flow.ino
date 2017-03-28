/*
Credit: 14Corge

Project: Smart Water Meter (Wifi)
Objective: Upload the flow of water regularly

*/
#include <ESP8266WiFi.h>
#include <ArduinoJson.h>

//FLOW Section setup
int flowPin1 = D5;
unsigned long flow1count = 0;
#define countof(a) (sizeof(a) / sizeof(a[0]))

String apiKey = "LGSAE13W82VM4RTE"; //Change
const char* ssid = "CEPT_STUDENT"; // Change
const char* password = ""; //Change
const char* server = "139.59.43.105";
String url = "http://139.59.43.105:3000/channels/1/fields/1/last.json?";
WiFiClient client;

//Json Experiment
const size_t bufferSize = JSON_OBJECT_SIZE(3) + 100;
DynamicJsonBuffer jsonBuffer(bufferSize);
int field1, lits, use = 0;

void setup() {
  Serial.begin (115200);
  pinMode(BUILTIN_LED, OUTPUT);
  pinMode(flowPin1, INPUT_PULLUP);
  attachInterrupt(flowPin1, flow1, CHANGE);
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  
  
}

void loop() {
    //Section to refresh the previous readings
    if(lits == 0){
    refreshmemory();
    use = field1;
    
    if(use!=0){
    Serial.print("The data retrieved shows use of litres ");
    Serial.println(use);
    }
    else{
      use = lits + use;
      collectdata();
    }
    
}
}

void flow1()
{
flow1count +=1;
}

void refreshmemory(){
  
  if (client.connect(server,3000)) { // "184.106.153.149" or api.thingspeak.com
      digitalWrite (BUILTIN_LED, HIGH);
      client.print(String("GET ") + url + " HTTP/1.1\r\n" +
               "Host: " + server + "\r\n" + 
               "Connection: close\r\n\r\n");

      unsigned long timeout = millis();
      while (client.available() == 0) {
        if (millis() - timeout > 10000) {
          Serial.println(">>> Client Timeout !");
          client.stop();
          return;
            }
        }
    while(client.available()){
        String line = client.readStringUntil('\r');
        JsonObject& root = jsonBuffer.parseObject(line);
        const char* created_at = root["created_at"]; // "2017-03-28T08:49:27+05:30"
        long entry_id = root["entry_id"]; // 158106
        field1 = root["field1"];
      }
    Serial.print ("The data is ");
    Serial.println (field1);
    digitalWrite (BUILTIN_LED, LOW);
    }
}

void collectdata(){
  Serial.print("Flow 1 in Liters: ");
  lits = (flow1count/450.0);
  use = field1+lits;
  Serial.print(use);
//Serial.print("\tFlow 2 in Liters: ");
//Serial.print(flow2count/450.0);
  Serial.print("\tFlow 1 in pulses: ");
  Serial.print(flow1count);
  Serial.println("");
//Serial.print("\tFlow 2 in pulse: ");
//Serial.println(flow2count);
  delay(00);

  if (client.connect(server,3000)) { // "184.106.153.149" or api.thingspeak.com
      digitalWrite (BUILTIN_LED, HIGH);
      String postStr = apiKey;
      //postStr +="&field1=";
      //postStr +="&field1=";
      //postStr += String("WeMos1");
      postStr +="&field1=";
      postStr += String(lits);
      /*postStr +="&field2=";
      postStr += String(h);*/
      postStr += "\r\n\r\n";
      
      client.print("POST /update HTTP/1.1\n");
      //client.print("Host: api.thingspeak.com\n");
      client.print("Host: http://139.59.43.105:3000/update\n");
      client.print("Connection: close\n");
      client.print("X-THINGSPEAKAPIKEY: "+apiKey+"\n");
      client.print("Content-Type: application/x-www-form-urlencoded\n");
      client.print("Content-Length: ");
      client.print(postStr.length());
      client.print("\n\n");
      client.print(postStr);
      //delay(5000);
      }
      digitalWrite (BUILTIN_LED, LOW);
      client.stop();
}

