#include <WiFiClient.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>

const char* _ssid = "your_wifi_ssid";//type your ssid
const char* _password = "your_wifi_password";//type your password

//MQTT server settings.
const char* _mqttServer = "your_mqtt_server"; //eg. m23.cloudmqtt.com
const int _mqttPort = 15038; //change to your mqtt server port
const char* _mqttUserName = "your_mqtt_username";
const char* _mqttPassword = "your_mqtt_password";
const char* _topic = "your_mqtt_topic";

WiFiClient _wifiClient;
PubSubClient _mqttClient(_wifiClient);

const int _relayPin = D1;

void setup() {
  pinMode(_relayPin, OUTPUT);
  digitalWrite(_relayPin, HIGH); //keep relay off

  Serial.begin(115200);
  
  connectToWifi();
  
  _mqttClient.setServer(_mqttServer, _mqttPort);
  _mqttClient.setCallback(messageReceivedHandler);

}

void loop() {
  // put your main code here, to run repeatedly:
  if(!_mqttClient.connected()){
    connectToMqttServer();
  }

  _mqttClient.loop();
}

//
void connectToWifi(){
  // Connect to WiFi network
  Serial.println();
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(_ssid);

  //WiFi.mode(WIFI_STA);
  //WiFi.config(_ip, _gateway, _subnet, _dns); 
  WiFi.begin(_ssid, _password);
   
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected, IP Address: ");
  Serial.print(WiFi.localIP());  
  Serial.println();
  
  delay(100);
}

//
void connectToMqttServer() {
  // Loop until we're reconnected
  while (!_mqttClient.connected()) {
    Serial.println("Attempting MQTT connection...");
    // Attempt to connect
    if (_mqttClient.connect("myesp8266", _mqttUserName, _mqttPassword)) {
      Serial.println("MQTT connected.");
      _mqttClient.subscribe(_topic);
    } else {
      Serial.print("Failed to connect, connection state = ");
      Serial.println(_mqttClient.state());
      Serial.println(". Trying again in 5 seconds...");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}


void messageReceivedHandler(char* topic, byte* payload, unsigned int length) {
  if(length <= 0){
    Serial.println("Empty message received");
    return;    
  }
  
  String message((char*)payload);
  message = message.substring(0, length);

  Serial.printf("Message received, topic: [%s]. Message: [%s]", topic, message);
  Serial.println();  

  if(message == "c:on") //command:on
  {
    digitalWrite(_relayPin, LOW); //turn relay on
    Serial.println("Relay turned on");
  } else if(message == "c:off") //command:off
  {
    digitalWrite(_relayPin, HIGH); //turn relay on
    Serial.println("Relay turned off");
  }
   
  Serial.println();

}
