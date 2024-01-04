#include <Arduino.h>
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

const IPAddress apIP(192, 168, 2, 112);
const IPAddress gateway(255, 255, 255, 0);

DNSServer dnsServer;
AsyncWebServer server(80);
AsyncWebSocket websocket("/ws");

IPAddress localIp = IPAddress(0, 0, 0, 0);

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

  WiFi.disconnect();   // added to start with the wifi off, avoid crashing
  WiFi.mode(WIFI_OFF); // added to start with the wifi off, avoid crashing
  if (!LittleFS.begin())
  {
    Serial.println("An Error has occurred while mounting LittleFS");
    return;
  }
  DynamicJsonDocument configFile(512);
  File file = LittleFS.open("/wifidata.json", "r");
  if (!file)
  {
    Serial.println("Failed to open file for reading");
    return;
  }
  else
  {
    deserializeJson(configFile, file);
    Serial.println(configFile["ssid"].as<String>());
  }
  file.close();

  if (configFile["ssid"].as<String>() != "")
  {
    WiFi.mode(WIFI_STA);
    WiFi.begin(configFile["ssid"].as<String>().c_str(), configFile["password"].as<String>().c_str());
    Serial.println("Connecting to WiFi");
    while (WiFi.status() != WL_CONNECTED)
    {
      digitalWrite(2, HIGH);
      delay(250);
      Serial.print(".");
      digitalWrite(2, LOW);
      delay(250);
    }
    Serial.println("\nConnected to the WiFi network");
    localIp = WiFi.localIP();
    WiFi.config(localIp, localIp, gateway, localIp, localIp);
  }

  else
  {

    WiFi.mode(WIFI_AP);
#ifndef PASSWORD
    WiFi.softAP(SSID);
#else
    WiFi.softAP(SSID, PASSWORD);
#endif
    WiFi.softAPConfig(apIP, apIP, gateway);
    localIp = WiFi.softAPIP();
    Serial.println("\nWiFi AP is now running\nIP address: ");
    dnsServer.start(DNS_PORT, "fan", localIp);
  }

  Serial.println(localIp.toString());

  // bind websocket to async web server
  websocket.onEvent(wsEventHandler);
  server.addHandler(&websocket);
  // setup statuc web server
  server.serveStatic("/", LittleFS, "/www/")
      .setDefaultFile("index.html");
  // Captive portal to keep the client
  server.onNotFound(redirectToIndex);
  server.begin();

#ifdef VERBOSE
  Serial.println("Server Started");
#endif
}

void loop()
{
  // serve DNS request for captive portal
  dnsServer.processNextRequest();
  vTaskDelay(1 / portTICK_PERIOD_MS);
}