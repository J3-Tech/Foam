#include <SPI.h>
#include <Ethernet.h>
#include <ArduinoJson.h>

// Enter a MAC address and IP address for your controller below.
// The IP address will be dependent on your local network:
byte mac[] = {
  0x90, 0xA2, 0XDA, 0x00, 0x45, 0xBA
};
IPAddress ip(192, 168, 1, 170);

// Enter the IP address of the server you're connecting to:
IPAddress server(192, 168, 1, 2);
//char server[] = "192.168.1.2"; 

// Initialize the Ethernet client library
EthernetClient client;

JsonObject& createJsonObject(){
  StaticJsonBuffer<200> jsonBuffer;

  JsonObject&  contextElement = jsonBuffer.createObject();
  contextElement["type"] = "Room";
  contextElement["isPattern"] = "false";
  contextElement["id"] = "Room1";
  JsonArray& attributes = contextElement.createNestedArray("attributes");


  JsonObject& attribute = jsonBuffer.createObject();
  attribute["name"] = "distance";
  attribute["type"] = "float";
  attribute["value"] = "10";
  attributes.add(attribute);

  JsonObject& root = jsonBuffer.createObject();
  root["updateAction"]="APPEND";
  JsonArray& contextElements = root.createNestedArray("contextElements");
  contextElements.add(contextElement);

  return root;
}


void setup() {
  
  // start the Ethernet connection:
  Ethernet.begin(mac, ip);
  // Open serial communications and wait for port to open:
  Serial.begin(9600);
  if (Ethernet.begin(mac) == 0) {
    Serial.println("Failed to configure Ethernet using DHCP");
    // no point in carrying on, so do nothing forevermore:
    // try to congifure using IP address instead of DHCP:
    Ethernet.begin(mac, ip);
  }
  // give the Ethernet shield a second to initialize:
  delay(1000);

  Serial.println("connecting...");

  // if you get a connection, report back via serial:
  if (client.connect(server, 80)) {
    Serial.println("connected");
    client.println("POST /v1/updateContext HTTP/1.1");
    client.println("Host: 192.168.1.2:1026");
    client.println("User-Agent: Arduino/1.1");
    client.println("Connection: close");
    client.println("Content-Type: application/json");
    client.print("Content-Length: ");
    client.println("227");
    client.println();
    JsonObject& x = createJsonObject();
    x.printTo(client);
    client.println();
    Serial.println("JSON Sent");
  }
  else {
    // if you didn't get a connection to the server:
    Serial.println("connection failed");
  }
}

void loop()
{
  // if there are incoming bytes available
  // from the server, read them and print them:
  if (client.available()) {
    char c = client.read();
    Serial.print(c);
  }

  // as long as there are bytes in the serial queue,
  // read them and send them out the socket if it's open:
  /*while (Serial.available() > 0) {
    char inChar = Serial.read();
    if (client.connected()) {
      client.print(inChar);
    }
  }*/

  // if the server's disconnected, stop the client:
  if (!client.connected()) {
    Serial.println();
    Serial.println("disconnecting.");
    client.stop();
    // do nothing:
    while (true);
  }else{
    client.println("from ard");
  }
}