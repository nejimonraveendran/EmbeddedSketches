#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <WiFiClient.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClientSecureBearSSL.h>

const char* _ssid = "your_wifi_ssid";//type your ssid
const char* _password = "your_wifi_password";//type your password

IPAddress _ip(192, 168, 1, 180); //static IP address to be used in the current subnet 
IPAddress _gateway(192, 168, 1, 1); //wifi router gateway address
IPAddress _subnet(255, 255, 255, 0); //wifi subnet
IPAddress _dns(8, 8, 8, 8); //set google as DNS

//REST API TLS SHA-1 fingerprint
const uint8_t _apiFingerprint[20] = {0xc2, 0x47, 0x0e, 0xcd, 0xb5, 0x3c, 0x01, 0x70, 0x2e, 0x80, 0x68, 0xf6, 0x70, 0x62, 0x7a, 0xf7, 0xe6, 0xee, 0x86, 0xf3};
String _apiUrl = "https://v2.jokeapi.dev/joke/Any";

ESP8266WebServer _server(80);

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  
  connectToWifi();

  _server.on("/", handleHome);

  //lambda function to handle 404s
  _server.onNotFound([]() {
    _server.send(404, "text/plain", "Not Found");
  });

  Serial.println();
  Serial.println("Starting HTTP Server...");
  _server.begin();
  Serial.println("HTTP server started");

}

void loop() {
  // put your main code here, to run repeatedly:
  _server.handleClient();

 // getDataFromApi();
}

void handleHome() {
  String result = getDataFromApi();
  _server.send(200, "text/html", result); 
}


void connectToWifi(){
  // Connect to WiFi network
  Serial.println();
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(_ssid);

  WiFi.mode(WIFI_STA);
  WiFi.config(_ip, _gateway, _subnet, _dns); 
  WiFi.begin(_ssid, _password);
   
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected, IP Address: ");
  Serial.print(WiFi.localIP());  
  
  delay(100);
}

String getDataFromApi(){
  if(WiFi.status() != WL_CONNECTED){
    Serial.println("Unable to invoke API, WIFI not connected.");
    return "{\"result\":\"error\"}";
  }
  
  Serial.println("Beginning HTTP Client");

  std::unique_ptr<BearSSL::WiFiClientSecure> wifiClient(new BearSSL::WiFiClientSecure);
  wifiClient->setFingerprint(_apiFingerprint);

  HTTPClient httpClient;
  
  bool isConnected = httpClient.begin(*wifiClient, _apiUrl);

  if(!isConnected){
    Serial.println("HttpClient failed to connect");
    return "{\"result\":\"error\"}";
  }  

  Serial.println("Trying to fetch response from URL");  
  int httpCode = httpClient.GET(); //send a GET request

  if(httpCode != HTTP_CODE_OK){
    Serial.printf("[HTTP] GET... failed, error: %d, %s\n", httpCode, httpClient.errorToString(httpCode).c_str());
    return "{\"result\":\"error\"}";
  }

  String result = httpClient.getString();
  httpClient.end();

  Serial.println(result);

  return result;  
}
