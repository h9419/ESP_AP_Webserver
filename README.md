# ESP AP Webserver


https://user-images.githubusercontent.com/58384315/146852841-89e57094-9a5c-42eb-8f55-0560b2ba8388.mp4


This is my experiment with "mobile app development" for the ESP32. The project consists of two parts, the ESP32 code and the React Web Application. The ESP32 acts as a WiFi hotspot with captive portal, similar to the free WiFi without password but a website will pop up prompting you to log in or agree their terms of services. The ESP32 will serve a website in its captive portal as well as opening a websocket for real-time interaction with the mobile interface.

## ESP32 Sketch

By default, the ESP32 sketch will create a SoftAP WiFi hotspot named "ESP32 SoftAP" without password. You can change the definition of the SSID and password inside main.cpp. Modify `wsEventHandler.cpp` for your custom implementation of the websocket server that handles recieving and sending messages to all clients.

In Platform IO -> Project Tasks -> esp32dev -> Platform, there are options to build and upload the SPIFFS filesystem image. Scripts placed in the scripts folder will be called to build the react project into a web app, rename the files to shorten the filenames and compress them using gzip to save on storage and delivery time. You may rename your react project directory and modify the `react_proj_dir` variable in scripts/prepare_React.py

In the included example, websocket clients are saved and all will be alerted of any state changes of the ESP32. Each and every websocket messages are expected to update all the state variables of the ESP32 and will be pushed to all clients. The ESP32 will only relay the string to other connected clients and new clients.

## React Web App

The React Web App acts as a websocket client and generates the interface based on what it recieves from the ESP32 server. Feel free to rip apart everything and make it your own.

In the included application, setState is only called on websocket message so that disconnecting from the server will not make the interface update anymore.

## Installation and setup

1. Install [VSCode](https://code.visualstudio.com/) or [VSCodium](https://vscodium.com/)
    * Install the [Platform IO extension](https://platformio.org/install/ide?install=vscode)
2. Install [Node.js](https://nodejs.org/en/) for npm and your extension of choice for JavaScript

## Uploading to ESP32

There are two actions required to upload this to your ESP32.

1. Upload the code (Platform IO -> Project Tasks -> esp32dev -> General -> Upload)
2. Upload the React Web App (Platform IO -> Project Tasks -> esp32dev -> Platform -> Upload Filesystem Image)
