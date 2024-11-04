#include <Arduino.h>
#include <MFRC522v2.h>
#include <MFRC522DriverSPI.h>
//#include <MFRC522DriverI2C.h>
#include <MFRC522DriverPinSimple.h>
#include <MFRC522Debug.h>
#include <WiFi.h>
#include <AsyncTCP.h>
#include <HTTPClient.h>
#include "AsyncJson.h"
#include "ArduinoJson.h"
#include <WiFiClientSecure.h>
#include <ESPAsyncWebServer.h>
#include <SPI.h>
#include "index.h"

byte nuidPICC[4];
WiFiClientSecure client;

const char* ssid_ap = "NodeMcu2";
const char* pass_ap = "12345678";
const char* jsonText = "";
const char* jsonText2 = "";
const char* jsonText3 = "";
const char* jsonText4 = "";
String get_ssid = "";
String get_pass = "";
String message = "";
unsigned long previousMillis = 0;
unsigned long interval = 30000;
bool isLoginWifi = false;
bool isConnectWifi = false;
const char IRG_Root_X1 [] PROGMEM = R"CERT(
-----BEGIN CERTIFICATE-----
MIIFazCCA1OgAwIBAgIRAIIQz7DSQONZRGPgu2OCiwAwDQYJKoZIhvcNAQELBQAw
TzELMAkGA1UEBhMCVVMxKTAnBgNVBAoTIEludGVybmV0IFNlY3VyaXR5IFJlc2Vh
cmNoIEdyb3VwMRUwEwYDVQQDEwxJU1JHIFJvb3QgWDEwHhcNMTUwNjA0MTEwNDM4
WhcNMzUwNjA0MTEwNDM4WjBPMQswCQYDVQQGEwJVUzEpMCcGA1UEChMgSW50ZXJu
ZXQgU2VjdXJpdHkgUmVzZWFyY2ggR3JvdXAxFTATBgNVBAMTDElTUkcgUm9vdCBY
MTCCAiIwDQYJKoZIhvcNAQEBBQADggIPADCCAgoCggIBAK3oJHP0FDfzm54rVygc
h77ct984kIxuPOZXoHj3dcKi/vVqbvYATyjb3miGbESTtrFj/RQSa78f0uoxmyF+
0TM8ukj13Xnfs7j/EvEhmkvBioZxaUpmZmyPfjxwv60pIgbz5MDmgK7iS4+3mX6U
A5/TR5d8mUgjU+g4rk8Kb4Mu0UlXjIB0ttov0DiNewNwIRt18jA8+o+u3dpjq+sW
T8KOEUt+zwvo/7V3LvSye0rgTBIlDHCNAymg4VMk7BPZ7hm/ELNKjD+Jo2FR3qyH
B5T0Y3HsLuJvW5iB4YlcNHlsdu87kGJ55tukmi8mxdAQ4Q7e2RCOFvu396j3x+UC
B5iPNgiV5+I3lg02dZ77DnKxHZu8A/lJBdiB3QW0KtZB6awBdpUKD9jf1b0SHzUv
KBds0pjBqAlkd25HN7rOrFleaJ1/ctaJxQZBKT5ZPt0m9STJEadao0xAH0ahmbWn
OlFuhjuefXKnEgV4We0+UXgVCwOPjdAvBbI+e0ocS3MFEvzG6uBQE3xDk3SzynTn
jh8BCNAw1FtxNrQHusEwMFxIt4I7mKZ9YIqioymCzLq9gwQbooMDQaHWBfEbwrbw
qHyGO0aoSCqI3Haadr8faqU9GY/rOPNk3sgrDQoo//fb4hVC1CLQJ13hef4Y53CI
rU7m2Ys6xt0nUW7/vGT1M0NPAgMBAAGjQjBAMA4GA1UdDwEB/wQEAwIBBjAPBgNV
HRMBAf8EBTADAQH/MB0GA1UdDgQWBBR5tFnme7bl5AFzgAiIyBpY9umbbjANBgkq
hkiG9w0BAQsFAAOCAgEAVR9YqbyyqFDQDLHYGmkgJykIrGF1XIpu+ILlaS/V9lZL
ubhzEFnTIZd+50xx+7LSYK05qAvqFyFWhfFQDlnrzuBZ6brJFe+GnY+EgPbk6ZGQ
3BebYhtF8GaV0nxvwuo77x/Py9auJ/GpsMiu/X1+mvoiBOv/2X/qkSsisRcOj/KK
NFtY2PwByVS5uCbMiogziUwthDyC3+6WVwW6LLv3xLfHTjuCvjHIInNzktHCgKQ5
ORAzI4JMPJ+GslWYHb4phowim57iaztXOoJwTdwJx4nLCgdNbOhdjsnvzqvHu7Ur
TkXWStAmzOVyyghqpZXjFaH3pO3JLF+l+/+sKAIuvtd7u+Nxe5AW0wdeRlN8NwdC
jNPElpzVmbUq4JUagEiuTDkHzsxHpFKVK7q4+63SM1N95R1NbdWhscdCb+ZAJzVc
oyi3B43njTOQ5yOf+1CceWxG1bQVs5ZufpsMljq4Ui0/1lvh+wjChP4kqKOJ2qxq
4RgqsahDYVvTH9w7jXbyLeiNdd8XM2w9U/t7y0Ff/9yi0GE44Za4rF2LN9d11TPA
mRGunUHBcnWEvgJBQl9nJEiU0Zsnvgc/ubhPgXRR4Xq37Z0j4r7g1SgEEzwxA57d
emyPxgcYxn/eR44/KJ4EBs+lVDR3veyJm+kXQ99b21/+jh5Xos1AnX5iItreGCc=
-----END CERTIFICATE-----
)CERT";

MFRC522DriverPinSimple ss_pin(5); // Configurable, see typical pin layout above.
AsyncWebServer server(80);

MFRC522DriverSPI driver{ss_pin}; // Create SPI driver.
//MFRC522DriverI2C driver{}; // Create I2C driver.
MFRC522 rfid{driver};  // Create MFRC522 instance.

void setup() {
  Serial.begin(115200);  // Initialize serial communications with the PC for debugging.
  // while (!Serial);     // Do nothing if no serial port is opened (added for Arduinos based on ATMEGA32U4).
  rfid.PCD_Init();  // Init MFRC522 board.
  // MFRC522Debug::PCD_DumpVersionToSerial(rfid, Serial);	// Show details of PCD - MFRC522 Card Reader details.
	// Serial.println(F("Scan PICC to see UID, SAK, type, and data blocks..."));

  WiFi.mode(WIFI_AP_STA);
  WiFi.softAP(ssid_ap, pass_ap);
  Serial.println(WiFi.localIP());

  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
    String index = FPSTR (Main_Page);
    request->send(200, "text/html", index);
  });

  server.on("/wifi", HTTP_GET, [](AsyncWebServerRequest *request) {
    handleRequestScanWifi(request);
  });

  server.on("/login-ssid", HTTP_POST, [](AsyncWebServerRequest *request) {
    handleRequestLoginWifi(request);
  });

  server.on("/disconnect", HTTP_GET, [](AsyncWebServerRequest *request) {
    disconnectWifi(request);    
  });

  server.on("/get-status-wifi", HTTP_GET, [](AsyncWebServerRequest *request) {
    JsonDocument doc;
    AsyncResponseStream *response = request->beginResponseStream("application/json");
    JsonArray data = doc.createNestedArray("data");

    if(WiFi.status() == WL_CONNECTED){
      JsonObject item = data.createNestedObject();
      item["response"] = 200;
      item["ssid"] = WiFi.SSID();
      item["isConnectWifi"] = isConnectWifi;
      serializeJson(doc, *response);
      request->send(response);
    }
    
    JsonObject item = data.createNestedObject();
    item["response"] = 200;
    item["isConnectWifi"] = false;              
    serializeJson(doc, *response);
    request->send(response);
  });

  server.begin();  
}

void loop() {
  WiFiClientSecure client;
  if(isLoginWifi){
    wifiLogin(get_ssid, get_pass);
  }  

  if(WiFi.status() != WL_CONNECTED) {
    isConnectWifi = false;
    timeout(millis());    
  }else if (WiFi.status() == WL_DISCONNECTED) {
    isConnectWifi = false;
    timeout(millis());    
  }else if(WiFi.status() == WL_CONNECTED){
    isConnectWifi = true;
  }

	// Reset the loop if no new card present on the sensor/reader. This saves the entire process when idle.
	if ( !rfid.PICC_IsNewCardPresent()) {
		return;
	}

	// Select one of the cards.
	if ( !rfid.PICC_ReadCardSerial()) {
		return;
	}

 if(rfid.uid.uidByte[0] != nuidPICC[0] || rfid.uid.uidByte[1] != nuidPICC[1] || rfid.uid.uidByte[2] != nuidPICC[2] || rfid.uid.uidByte[3] != nuidPICC[3]) {
    Serial.println("Kartu tag baru terdeteksi.");
  }

  ApiAbsensi(printHex(rfid.uid.uidByte, rfid.uid.size));
  Serial.println(printHex(rfid.uid.uidByte, rfid.uid.size));

  if(WiFi.status() == WL_CONNECTED){
    client.setCACert(IRG_Root_X1);
    HTTPClient https;

    if (https.begin(client, "https://flockbase.my.id/api/absensi")) {  // HTTPS
      https.addHeader("Content-Type", "application/x-www-form-urlencoded");
      String httpData = "rfid_tag="+ printHex(rfid.uid.uidByte, rfid.uid.size);        
      int httpCode = https.POST(httpData);
      Serial.println(httpCode);
      
      // httpCode will be negative on error
      if (httpCode > 0) {
        // HTTP header has been send and Server response header has been handled
        Serial.printf("[HTTPS] GET... code: %d\n", httpCode);

        // file found at server
        if (httpCode == HTTP_CODE_OK || httpCode == HTTP_CODE_MOVED_PERMANENTLY) {
          String payload = https.getString();
          Serial.println(httpCode);
          // sound(httpCode);
          Serial.println(payload);
          StaticJsonDocument<200> doc2;
          deserializeJson(doc2, payload);
          jsonText = doc2["message2"];
          jsonText4 = doc2["message"];
          jsonText2 = doc2["name"];  
          jsonText3 = doc2["status"]; 
          Serial.println(jsonText3);
          String jsonText3String = jsonText3;            
        }
      } else {
        Serial.printf("[HTTPS] GET... failed, error: %s\n", https.errorToString(httpCode).c_str());
      }

      https.end();
    } else {
      Serial.printf("[HTTPS] Unable to connect\n");
    }
  }

  rfid.PICC_HaltA();

  rfid.PCD_StopCrypto1();
}

String printHex(byte *buffer, byte bufferSize) {
  String uidHex = "";
  if(uidHex != "") {
    uidHex= "";
  }
  for(byte i = 0; i < bufferSize; i++) {
    uidHex.concat(String(buffer[i] < 0x10 ? " 0" : ""));
    uidHex.concat(String(buffer[i], HEX));
  }
  return uidHex;
}

void wifiLogin(String ssid, String password){
  const char* ssid_sta = ssid.c_str();
  const char* pass_sta = password.c_str();
  WiFi.begin(ssid_sta, pass_sta);
    
  if(WiFi.status() == WL_CONNECTED){
    isConnectWifi = true;
    Serial.println("Koneksi Wifi Terhubung");
    message = "Koneksi Wifi Terhubung";
  }else if (WiFi.status() != WL_CONNECTED) {
    isConnectWifi = false;
    Serial.print(".");
    delay(50);
  }
  isLoginWifi = false;
}

void wifiStatus(){
  switch (WiFi.status()) {
  case WL_CONNECTED:
    Serial.println("Koneksi Wifi Terhubung");
    message = "Koneksi Wifi Terhubung";
    break;
  case WL_NO_SSID_AVAIL:
    Serial.println("SSID Tidak Tersedia");
    message = "SSID Tidak Tersedia";
    break;
  case WL_CONNECTION_LOST:
    Serial.println("Koneksi Terputus");
    message = "Koneksi Terputus";
    break;
  }    
}

void timeout(unsigned long currentMillis){
  if(isConnectWifi == false ){
    if(currentMillis - previousMillis >= interval){
    switch (WiFi.status()) {
    case WL_NO_SSID_AVAIL:
      Serial.println("Configured SSID cannot be reached");
      message = "Configured SSID cannot be reached";
      break;
    case WL_CONNECTED:
      Serial.println("Connection successfully established");
      Serial.println(WiFi.localIP().toString());
      isConnectWifi = true;
      break;
    case WL_CONNECT_FAILED:
      Serial.println("Connetion failed");      
      message = "Connetion failed";
      break;
    }

    Serial.printf("Connection status: %d\n", WiFi.status());
    previousMillis = currentMillis;
    Serial.println(previousMillis);
    Serial.println(message);
  }
  } 
}

void ApiAbsensi(String uidHex) {
  HTTPClient http;
  String data = "rfid_tag="+uidHex;

  http.begin(client, "https://flockbase.my.id/api/absensi");
  http.addHeader("Content-Type", "application/x-www-form-urlencoded");
  int httpResponseCode = http.POST(data);
  Serial.println(httpResponseCode);
}

void handleRequestScanWifi(AsyncWebServerRequest *request) {
  JsonDocument doc;
  int n = WiFi.scanNetworks();
  AsyncResponseStream *response = request->beginResponseStream("application/json");
  JsonArray data = doc.createNestedArray("data");
  Serial.println(data); 
  Serial.print("Wifi :");
  Serial.println(n);
  
  if(n == 0){       
    JsonObject item = data.createNestedObject();
    item["status"] = "Wifi Tidak Dapat Ditemukan..";
    
    serializeJson(doc, *response);
    Serial.println(data); 
    request->send(response);
  }
  
    for(int i = 0; i < n; ++i){
    JsonObject item = data.createNestedObject();
      item["status"] = "ok";
      item["SSID"] = WiFi.SSID(i);
      item["RSSI"] = WiFi.RSSI(i);
      item["Encrypt"] = WiFi.encryptionType(i);
    }
    serializeJson(doc, *response);
    Serial.println(data); 
    request->send(response);
  
}

void handleRequestLoginWifi(AsyncWebServerRequest *request) {
  WiFi.disconnect();  
  get_ssid = request->arg("ssid");
  get_pass = request->arg("pass");
  
  Serial.println(get_ssid);
  Serial.println(get_pass);
  isLoginWifi = true;
  JsonDocument doc;
  AsyncResponseStream *response = request->beginResponseStream("application/json");
  JsonArray data = doc.createNestedArray("data");
  JsonObject item = data.createNestedObject();  
  item["response"] = 200;
  item["isLogin"] = true;            
  serializeJson(doc, *response);
  request->send(response);
  }

void disconnectWifi(AsyncWebServerRequest *request){
  WiFi.disconnect();
  isConnectWifi = false;
  JsonDocument doc;
  AsyncResponseStream *response = request->beginResponseStream("application/json");
  JsonArray data = doc.createNestedArray("data");
  JsonObject item = data.createNestedObject();  
  item["response"] = 200;
  item["isConnectWifi"] = isConnectWifi;          
  serializeJson(doc, *response);
  request->send(response);
}