//board manager: https://dl.espressif.com/dl/package_esp32_index.json
//esp32 board lib ver: 1.0.6 (esp32 by Espressif Systems)

#include <WiFiClientSecure.h>
#include <PubSubClient.h>

const char* _ssid = "your_wifi_ssid";//type your ssid
const char* _password = "your_wifi_password";//type your password

//MQTT server settings.
const char* _mqttServer = "your_mqtt_server"; //eg. 13efdrer.s2.eu.hivemq.cloud
const int _mqttPort = 8883; //change to your hive mqtt server port
const char* _mqttUserName = "your_mqtt_username";
const char* _mqttPassword = "your_mqtt_password";
const char* _clientId = "your_client_id";
const char* _topic = "myapp/topic1";

//root server certificate
//get certificate: openssl s_client -showcerts -connect <your_mqtt_server>:<port> > server_cert.pem  
//following example shows HiveMQ root certificate  
const char* _rootCert PROGMEM = R"CERT(
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

// You can use x.509 client certificates if you want
//const char* test_client_key = "";   //to verify the client
//const char* test_client_cert = "";  //to verify the client

WiFiClientSecure _wifiClient;
PubSubClient _mqttClient(_wifiClient);

int _flashLightPin = 4; //the buil-in flash light LED

void setup() {
  pinMode(_flashLightPin, OUTPUT);
  digitalWrite(_flashLightPin, LOW); //keep the light off

  Serial.begin(115200);
  
  connectToWifi();

  //_wifiClient.setInsecure();
  _wifiClient.setCACert(_rootCert);
  //_wifiClient.setCertificate(test_client_key); // for client verification
  //_wifiClient.setPrivateKey(test_client_cert);	// for client verification

  _mqttClient.setServer(_mqttServer, _mqttPort);
  _mqttClient.setCallback(messageReceivedHandler);

}


void loop() {
  if(!_mqttClient.connected()){
    connectToMqttServer();
  }

  _mqttClient.loop();
}


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
    digitalWrite(_flashLightPin, HIGH); //turn light on
    Serial.println("Light turned on");
  } else if(message == "c:off") //command:off
  {
    digitalWrite(_flashLightPin, LOW); //turn light off
    Serial.println("Light turned off");
  }
   
  Serial.println();

}

void publishMessage(const char* topic, const char* message){
  _mqttClient.publish(topic, message); 
}

