#include <Arduino.h>
#include <WiFiManager.h>
#include <WiFi.h>
#include <DNSServer.h>
#include <LittleFS.h>
#include <ESPAsyncWebServer.h>
#include "wsEventHandler.h"
#include <ArduinoJson.h>

#define SSID "ESP32 SoftAP" // This is the SSID that ESP32 will broadcast
// #define CAPTIVE_DOMAIN "http://domain-name-to-show" // This is the SSID that ESP32 will broadcast
// Uncomment the following line to enable password in the wifi acces point
// #define PASSWORD "12345678" // password should be atleast 8 characters to make it work
#define DNS_PORT 53
// Options to enable serial printing
#define VERBOSE

DNSServer dnsServer;
AsyncWebServer server(80);
AsyncWebSocket websocket("/ws");

IPAddress localIp(192, 168, 1, 1);

void redirectToIndex(AsyncWebServerRequest *request)
{
#ifdef CAPTIVE_DOMAIN
  request->redirect(CAPTIVE_DOMAIN);
#else
  request->redirect("http://" + localIp.toString());
#endif
}

void setup()
{
  pinMode(2, OUTPUT);
  pinMode(16, OUTPUT);
  pinMode(17, OUTPUT);
  pinMode(18, OUTPUT);

  digitalWrite(2, LOW);
  digitalWrite(16, HIGH);
  digitalWrite(17, HIGH);
  digitalWrite(18, HIGH);
  Serial.begin(115200);

  WiFiManager wm;

  // reset settings - wipe stored credentials for testing
  // these are stored by the esp library
  // wm.resetSettings();

  // Automatically connect using saved credentials,
  // if connection fails, it starts an access point with the specified name ( "AutoConnectAP"),
  // if empty will auto generate SSID, if password is blank it will be anonymous AP (wm.autoConnect())
  // then goes into a blocking loop awaiting configuration and will return success result

  bool res;
  // res = wm.autoConnect(); // auto generated AP name from chipid
  // res = wm.autoConnect("AutoConnectAP"); // anonymous ap


  res = wm.autoConnect("AutoConnectAP"); // password protected ap

  if (!res)
  {
    Serial.println("Failed to connect");
    // ESP.restart();
  }
  else
  {
    
    if (!LittleFS.begin())
    {
      Serial.println("An Error has occurred while mounting LittleFS");
      return;
    }
    localIp = WiFi.localIP();
    dnsServer.start(DNS_PORT, "midominio.local", WiFi.localIP());
    Serial.println("Connected to wifi  "+ WiFi.localIP().toString());
    // bind websocket to async web server
    websocket.onEvent(wsEventHandler);
    server.addHandler(&websocket);
    // setup statuc web server
    server.serveStatic("/", LittleFS, "/www/")
        .setDefaultFile("index.html");
    // Captive portal to keep the client
    server.onNotFound(redirectToIndex);
    server.begin();

    for (int i = 0; i < 10; i++)
    {
      digitalWrite(2, HIGH);
      vTaskDelay(100 / portTICK_PERIOD_MS);
      digitalWrite(2, LOW);
      vTaskDelay(100 / portTICK_PERIOD_MS);
    }
    

#ifdef VERBOSE
    Serial.println("Server Started");
#endif
  }
}

void loop()
{
  // serve DNS request for captive portal
  dnsServer.processNextRequest();
  vTaskDelay(1 / portTICK_PERIOD_MS);
}