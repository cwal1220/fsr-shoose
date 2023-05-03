#include <WiFi.h>
#include <DNSServer.h>
#include <ESP32WebServer.h>
#include <EEPROM.h>
#include <ESPmDNS.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>

#define EEPROM_LENGTH 1024

char eRead[30];
byte len;
char ssid[30];
char password[30];

bool captive = true;

WiFiClient espClient;
const byte DNS_PORT = 53;
IPAddress apIP(192, 168, 1, 1);
DNSServer dnsServer;
ESP32WebServer webServer(80);
int count = 0;

String responseHTML = ""
    "<!DOCTYPE html><html><head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\"><title>Wi-Fi Setting Page</title></head><body><center>"
    "<p>Wi-Fi Setting Page</p>"
    "<form action='/button'>"
    "<p><input type='text' name='ssid' placeholder='SSID' onblur='this.value=removeSpaces(this.value);'></p>"
    "<p><input type='text' name='password' placeholder='WLAN Password'></p>"
    "<p><input type='submit' value='Submit'></p></form>"
    "<p>Wi-Fi Setting Page</p></center></body>"
    "<script>function removeSpaces(string) {"
    "   return string.split(' ').join('');"
    "}</script></html>";


void setup() 
{
  Serial.begin(115200);
  EEPROM.begin(EEPROM_LENGTH);
  pinMode(0, INPUT_PULLUP);
  attachInterrupt(0, initDevice, FALLING);

  ReadString(0, 30);
  if (!strcmp(eRead, ""))
  {
    setup_captive();    
  }
  else 
  {
    captive = false;
    strcpy(ssid, eRead);
    ReadString(30, 30);
    strcpy(password, eRead);
    setup_runtime();  
  }

}

void loop() {
  if (captive)
  {
    Serial.println("Captive Mode");
    dnsServer.processNextRequest();
    webServer.handleClient();
  }
  else
  {
    // 센서에서 데이터를 읽어옴
    float sensorValue = analogRead(A0);
    String jsonString;
    StaticJsonDocument<200> sensorDataJson;

    sensorDataJson["left_0"] = count;
    sensorDataJson["left_1"] = count;
    sensorDataJson["left_2"] = count;
    sensorDataJson["left_3"] = count;
    sensorDataJson["left_4"] = count++; 
    serializeJson(sensorDataJson, jsonString);
    
    
    // HTTP POST 요청을 생성
    HTTPClient http;
    http.begin("http://192.168.219.197:3004/update_left"); // 웹 서버 주소 입력
    http.addHeader("Content-Type", "application/json"); // 요청 헤더 설정
    int httpResponseCode = http.POST(jsonString); // 데이터 전송
    http.end();
  
    // 전송 결과를 시리얼 모니터에 출력
    Serial.println(httpResponseCode);
    yield;
  }
}


void setup_runtime() {
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.println("");

  // Wait for connection
  int i = 0;
  while (WiFi.status() != WL_CONNECTED) 
  {
    delay(1000);
    Serial.print(".");
    yield;
    if(i++ > 20) 
    {
      ESP.restart();
      return; 
    }
  }
  Serial.println("");
  Serial.print("Connected to "); Serial.println(ssid);
  Serial.print("IP address: "); Serial.println(WiFi.localIP());

  if (MDNS.begin("FOOT_LEFT")) {
   Serial.println("MDNS responder started");
  }
  
  webServer.onNotFound(handleNotFound);
  webServer.begin();
  Serial.println("HTTP server started");  
}

void setup_captive() {    
  WiFi.mode(WIFI_AP);
  WiFi.softAPConfig(apIP, apIP, IPAddress(255, 255, 255, 0));
  WiFi.softAP("FOOT_LEFT");
  
  dnsServer.start(DNS_PORT, "*", apIP);

  webServer.on("/button", button);

  webServer.onNotFound([]() {
    webServer.send(200, "text/html", responseHTML);
  });
  webServer.begin();
  Serial.println("Captive Portal Started");
}

void button()
{
  Serial.println("button pressed");
  Serial.println(webServer.arg("ssid"));
  SaveString(0, (webServer.arg("ssid")).c_str());
  SaveString(30, (webServer.arg("password")).c_str());
  webServer.send(200, "text/plain", "OK");
  ESP.restart();
}

void SaveString(int startAt, const char* id) 
{ 
  for (byte i = 0; i <= strlen(id); i++)
  {
    EEPROM.write(i + startAt, (uint8_t) id[i]);
  }
  EEPROM.commit();
}

void ReadString(byte startAt, byte bufor) 
{
  for (byte i = 0; i <= bufor; i++) {
    eRead[i] = (char)EEPROM.read(i + startAt);
  }
  len = bufor;
}

void handleNotFound()
{
  String message = "File Not Found\n";
  webServer.send(404, "text/plain", message);
}

ICACHE_RAM_ATTR void initDevice() {
    Serial.println("Flushing EEPROM....");
    SaveString(0, "");
    ESP.restart();
}

