

#include <ArduinoJson.h> // ** ใช้กับ version 5.13 เท่านั้น  

#include <ESP8266WiFi.h>
#include <MicroGear.h>
        

//-------------------------------------------------------
const char* ssid     = "GT TECH_2.4G";     
const char* password = "48044993255";     

#define APPID   "ESPBarebone"
#define KEY     "nMlUTzXoV8hGBDt"         // key
#define SECRET  "okbLjurpmdFMTRm6WJiYd8Zz7"
#define ALIAS   "esp8266BareboneNetpie"

#define FEEDID   "BareboneESP"           // ***** FeedID
#define FEEDAPI  "1LNVKNrQePUj47vKbiL8EslEE6kqOKep"        //  FeedAPI 


float humid = 10;     // ค่าความชื้น
float temp  = 30;     // ค่าอุณหภูมิ

long lastDHTRead = 0;
long lastTimeWriteFeed = 0;

WiFiClient client;
MicroGear microgear(client);

void onConnected(char *attribute, uint8_t* msg, unsigned int msglen) {
    Serial.println("Connected to NETPIE...");
    microgear.setAlias(ALIAS);
}

void setup() {
    Serial.begin(115200);

    if (WiFi.begin(ssid, password)) {
        while (WiFi.status() != WL_CONNECTED) {
            delay(1000);
            Serial.print(".");
        }
    }
    Serial.println("WiFi connected");
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());
    
    microgear.on(CONNECTED, onConnected);
    microgear.init(KEY,SECRET,ALIAS);   // กำหนดค่าตันแปรเริ่มต้นให้กับ microgear
    microgear.connect(APPID);           // ฟังก์ชั่นสำหรับเชื่อมต่อ NETPIE
}

void loop() {
    if (microgear.connected()) {
        microgear.loop();

        if(millis() - lastDHTRead > 2000){
          humid = 40 -4;     // อ่านค่าความชื้น
          temp  = 42 -4;  // อ่านค่าอุณหภูมิ
          lastDHTRead = millis();
          
          Serial.print("Humid: "); Serial.print(humid); Serial.print(" %, ");
          Serial.print("Temp: "); Serial.print(temp); Serial.println(" C ");
    
          // ตรวจสอบค่า humid และ temp เป็นตัวเลขหรือไม่
          if (isnan(humid) || isnan(temp)) {
            Serial.println("Failed to read from DHT sensor!");
          }
          else{
            // เตรียมสตริงในรูปแบบ "{humid},{temp}"
           // String datastring = (String)humid+","+(String)temp;
           // Serial.print("Sending --> ");
           // Serial.println(datastring);
//            microgear.publish(DHTDATATOPIC,datastring);  
          }
        }
        
        if(millis()-lastTimeWriteFeed > 15000){
          lastTimeWriteFeed = millis();
          if(humid!=0 && temp!=0){

            DynamicJsonBuffer jsonBuffer;
            JsonObject& root = jsonBuffer.createObject();
            root["bat"] = 0.1;
            //root["humid"] = humid;

            String jsonData;
            root.printTo(jsonData);
  
            Serial.print("Write Feed --> ");
            Serial.println(jsonData);
//            microgear.writeFeed(FEEDID,data); //**
//            microgear.writeFeed(FEEDID,data,FEEDAPI); 
            microgear.writeFeed(FEEDID,jsonData,FEEDAPI); 
          }
        }
    }
    else {
        Serial.println("connection lost, reconnect...");
        microgear.connect(APPID); 
    }
}
