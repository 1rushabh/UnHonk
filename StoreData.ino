#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <EEPROM.h>

const char *ssid = "Unhonk Auto1";
const char *password = "";

ESP8266WebServer server(80);

String webString="";

int switchPin = D8;                     // switch is connected to pin 8
volatile unsigned int switchPresses = 0;       
int flag = 0;
int switchState = 0;
int address = 0;                          // persistent memory address
char arrayToStore[10];                    // Must be greater than the length of string.

void handleRoot()                //Handler for the rooth path
{           
  server.send(200, "text/plain", "Hello world");
}

void startServer() 
{
  server.send(200, "text/plain", String(switchPresses-1));
}

void setup()
{
  pinMode(switchPin, INPUT_PULLUP);           //HORN
  Serial.begin(115200);
  EEPROM.begin(512);
  Serial.print("Configuring access point...");
  WiFi.softAP(ssid, password);
  IPAddress myIP = WiFi.softAPIP();
  Serial.print("AP IP address: ");
  Serial.println(myIP);
 
  int eepromValue_read = EEPROM.read(0);
  eepromValue_read = eepromValue_read << 8;
  eepromValue_read = eepromValue_read | EEPROM.read(1);
  if(eepromValue_read==-1)
  {
      switchPresses= 0;
  }
  else
  {
      switchPresses=(eepromValue_read-1);
  }
  server.on("/", handleRoot);
  server.on("/Honks", startServer);  // executes function startServer()
   
  server.begin();
  Serial.println("HTTP server started");
}

void loop() 
{
  switchState = digitalRead(switchPin);

    while (switchState == LOW && flag == 0)
    {
        Serial.println("Pressed");
        switchPresses++;
        flag = 1;

        EEPROM.put(0,switchPresses >> 8);
        EEPROM.put(1,switchPresses);   
        EEPROM.commit();             
    }
    while (switchState == HIGH && flag == 1)
    {
      flag = 0;
      Serial.println("Released");
    }
   
    server.handleClient();
}
