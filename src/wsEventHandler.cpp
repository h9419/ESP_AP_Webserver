#include <Arduino.h>
#include <ArduinoJson.h>
#include <ESPAsyncWebServer.h>
#include "wsEventHandler.h"

// allocate memory for recieved json data
#define BUFFER_SIZE 512
StaticJsonDocument<BUFFER_SIZE> recievedJson;
// initial device state
char dataBuffer[BUFFER_SIZE] = "{\"type\":\"message\",\"LED\":false}";
AsyncWebSocketClient *clients[16];

void wsEventHandler(AsyncWebSocket *server, AsyncWebSocketClient *client, AwsEventType type, void *arg, uint8_t *data, size_t len)
{
  if (type == WS_EVT_DATA)
  {
    // save the response as newest device state
    for (int i = 0; i < len; ++i)
      dataBuffer[i] = data[i];
    dataBuffer[len] = '\0';
#ifdef VERBOSE
    Serial.println(dataBuffer);
#endif
    // parse the recieved json data
    DeserializationError error = deserializeJson(recievedJson, (char *)data, len);
    if (error)
    {
      Serial.print(F("deserializeJson() failed: "));
      Serial.println(error.f_str());
      return;
    }
    if (strcmp(recievedJson["type"], "message") != 0)
      return;
    // get the target LED state
    bool led = recievedJson["LED"];
    digitalWrite(2, led);
    // send ACK
    client->text(dataBuffer, len);
    // alert all other clients
    for (int i = 0; i < 16; ++i)
      if (clients[i] != NULL && clients[i] != client)
        clients[i]->text(dataBuffer, len);
  }
  else if (type == WS_EVT_CONNECT)
  {
    Serial.println("Websocket client connection received");
    // ACK with current state
    client->text(dataBuffer);
    // store connected client
    for (int i = 0; i < 16; ++i)
      if (clients[i] == NULL)
      {
        clients[i] = client;
        break;
      }
  }
  else if (type == WS_EVT_DISCONNECT)
  {
    Serial.println("Client disconnected");
    // remove client from storage
    for (int i = 0; i < 16; ++i)
      if (clients[i] == client)
        clients[i] = NULL;
  }
}