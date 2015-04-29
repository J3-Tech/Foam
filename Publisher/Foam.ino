#include <SPI.h>
#include <Ethernet.h>
#include <ArduinoJson.h>

const int pingPin = 7;
const int bufferSize = 160;

// Enter a MAC address and IP address for your controller below.
byte mac[] = {
  0x90, 0xA2, 0XDA, 0x00, 0x45, 0xBA
};

// The IP address will be dependent on your local network:
IPAddress ip(192, 168, 1, 170);

// Enter the IP address of the server you're connecting to:
IPAddress server(192, 168, 1, 254);

// Initialize the Ethernet client library
EthernetClient client;

JsonObject& createJsonObject(float value){
  StaticJsonBuffer<500> jsonBuffer;

  JsonObject&  contextElement = jsonBuffer.createObject();
  contextElement["type"] = "Room";
  contextElement["isPattern"] = "false";
  contextElement["id"] = "Room1";
  JsonArray& attributes = contextElement.createNestedArray("attributes");

  JsonObject& attribute = jsonBuffer.createObject();
  attribute["name"] = "distance";
  attribute["type"] = "float";
  attribute["value"].set(value,4);
  //
  attributes.add(attribute);

  JsonObject& root = jsonBuffer.createObject();
  root["updateAction"]="APPEND";
  JsonArray& contextElements = root.createNestedArray("contextElements");
  contextElements.add(contextElement);

  return root;
}

float readSensor(){
  float cm;
  // The PING))) is triggered by a HIGH pulse of 2 or more microseconds.
  // Give a short LOW pulse beforehand to ensure a clean HIGH pulse:
  pinMode(pingPin, OUTPUT);
  digitalWrite(pingPin, LOW);
  delayMicroseconds(2);
  digitalWrite(pingPin, HIGH);
  delayMicroseconds(5);
  digitalWrite(pingPin, LOW);

  // The same pin is used to read the signal from the PING))): a HIGH
  // pulse whose duration is the time (in microseconds) from the sending
  // of the ping to the reception of its echo off of an object.
  pinMode(pingPin, INPUT);

  float distance = pulseIn(pingPin, HIGH);

  return microsecondsToCentimeters(distance);
}

float microsecondsToCentimeters(float microseconds)
{
  // The speed of sound is 340 m/s or 29 microseconds per centimeter.
  // The ping travels out and back, so to find the distance of the
  // object we take half of the distance travelled.
  return (microseconds / 29 / 2) / 100;
}

void setup() {
  
  // Open serial communications and wait for port to open:
  Serial.begin(9600);
  
  // start the Ethernet connection:
  if (Ethernet.begin(mac) == 0) {
    Serial.println("Failed to configure Ethernet using DHCP");
    // no point in carrying on, so do nothing forevermore:
    // try to congifure using IP address instead of DHCP:
    Ethernet.begin(mac, ip);
  }
  
  // give the Ethernet shield a second to initialize:
  delay(1000);
}

void loop()
{
  // if there are incoming bytes available
  // from the server, read them and print them:
  Serial.println("connecting...");
  if (client.connect(server, 1026)) {
    //
      float value = readSensor();
      Serial.println(value);
      delay(1000);
      JsonObject& data = createJsonObject(value);
      client.println("POST /v1/updateContext HTTP/1.1");
      client.println("Host: 192.168.1.254:1026");
      client.println("User-Agent: Arduino/1.1");
      client.println("Connection: close");
      client.println("Accept: application/json;");
      client.println("Content-Type: application/json;");
      client.print("Content-Length: ");
      client.println(bufferSize);
      client.println();
      data.printTo(client);
      client.println();
      Serial.println("JSON Sent");
      
      if (client.available()) {
        char c = client.read();
        Serial.print(c);
      }
      client.stop();
  }
}
