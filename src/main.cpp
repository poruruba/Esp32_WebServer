#define LGFX_AUTODETECT // 自動認識

#include <M5Core2.h>
#include <LovyanGFX.hpp>
#include <WiFi.h>
#include <FS.h>
#include <SPIFFS.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <AsyncJson.h>
#include <ArduinoJson.h>
#include "base64.hpp"

const char *wifi_ssid = "【WiFiアクセスポイントのSSID】";
const char *wifi_password = "【WiFiアクセスポイントのパスワード】";

#define HTTP_PORT 80
#define FORMAT_SPIFFS_IF_FAILED true
#define BUFFER_SIZE 30 * 1024
unsigned char buffer[BUFFER_SIZE];

AsyncWebServer server(HTTP_PORT);
static LGFX lcd;

void wifi_connect(const char *ssid, const char *password);

void notFound(AsyncWebServerRequest *request){
  if (request->method() == HTTP_OPTIONS){
    request->send(200);
  }else{
    request->send(404);
  }
}

void setup() {
  Serial.begin(9600);
  SPIFFS.begin(FORMAT_SPIFFS_IF_FAILED);

  wifi_connect(wifi_ssid, wifi_password);

  lcd.init();

  server.on("/get", HTTP_GET, [](AsyncWebServerRequest *request) {
    AsyncWebParameter *p = request->getParam("download");
    if( p != NULL )
      Serial.println(p->value());

    request->send(200, "text/plain", "Hello, world");
  });

  AsyncCallbackJsonWebHandler *handler = new AsyncCallbackJsonWebHandler("/post", [](AsyncWebServerRequest *request, JsonVariant &json) {
    JsonObject jsonObj = json.as<JsonObject>();
    int p1 = jsonObj["p1"] | -1;

    const char *p_param = jsonObj["param"];
    if( p_param != NULL ){
      if (decode_base64_length((unsigned char *)p_param) <= sizeof(buffer) ){
        int length = decode_base64((unsigned char *)p_param, buffer);
        lcd.drawJpg(buffer, length);
      }
    }

//    request->send(200, "application/json", "{}");
    AsyncJsonResponse *response = new AsyncJsonResponse();
    JsonObject root = response->getRoot();
    root["p1"] = p1;
    root["message"] = "Hello World";
    response->setLength();
    request->send(response);
  });
  server.addHandler(handler);

  server.on("/image", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send(SPIFFS, "/www/img/test.jpg", "image/jpeg");
  });

  server.serveStatic("/", SPIFFS, "/www/").setDefaultFile("index.html");
  server.onNotFound(notFound);

  DefaultHeaders::Instance().addHeader("Access-Control-Allow-Origin", "*");
  DefaultHeaders::Instance().addHeader("Access-Control-Allow-Headers", "*");
  server.begin();
}

void loop() {
  // put your main code here, to run repeatedly:
}

void wifi_connect(const char *ssid, const char *password){
  Serial.println("");
  Serial.print("WiFi Connenting");

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED)
  {
    Serial.print(".");
    delay(1000);
  }

  Serial.println("");
  Serial.print("Connected : ");
  Serial.println(WiFi.localIP());
}