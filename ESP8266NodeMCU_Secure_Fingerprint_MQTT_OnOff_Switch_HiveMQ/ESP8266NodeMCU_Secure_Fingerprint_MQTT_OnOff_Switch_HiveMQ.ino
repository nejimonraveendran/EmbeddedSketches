//esp8266 board library version 3.1.1

#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>
#include <PubSubClient.h>

const char* _ssid = "your_wifi_ssid";//type your ssid
const char* _password = "your_wifi_password";//type your password

//MQTT server settings.
const char* _mqttServer = "your_hive_mq_host_name"; //eg. 13efdrer.s2.eu.hivemq.cloud
const int _mqttPort = 8883; //change to your hive mqtt server port
const char* _mqttUserName = "your_hive_mq_username";
const char* _mqttPassword = "your_hive_mq_password";
const char* _clientId = "myesp8266";
const char* _topic = "myapp/topic1";

//get certificate: openssl s_client -showcerts -connect <your_mqtt_server>:<port> > server_cert.pem    
//get fingerprint and dates from certificate: openssl x509 -in server_cert.pem -noout -sha1 -fingerprint -dates
const char* _sha1CertFingerPrint = "AB:E6:FC:82:94:69:B9:22:EF:09:AF:22:4C:61:76:37:62:F8:1A:6C"; 

WiFiClientSecure _wifiClient;
PubSubClient _mqttClient(_wifiClient);

const int _relayPin = D1;

void setup() {
  pinMode(_relayPin, OUTPUT);
  digitalWrite(_relayPin, HIGH); //keep relay off

  Serial.begin(115200);
  
  connectToWifi();

  //_wifiClient.setInsecure(); //accept all certs sent by server.  NodeMCU does not support certificate verification because of low memory.
    _wifiClient.setFingerprint(_sha1CertFingerPrint);

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
    if (_mqttClient.connect(_clientId, _mqttUserName, _mqttPassword)) {
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

void publishMessage(const char* topic, const char* message){
  _mqttClient.publish(topic, message); 
}

