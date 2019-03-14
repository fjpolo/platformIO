/*
 Basic Espressif ESP32 devkit v4 WROOM MQTT example

 This sketch demonstrates the capabilities of the pubsub library in combination
 with the ESP8266 board/library.

 It connects to an MQTT server then:
  - publishes "hello world" to the topic "outTopic" every two seconds
  - subscribes to the topic "inTopic", printing out any messages
    it receives. NB - it assumes the received payloads are strings not binary
  - If the first character of the topic "inTopic" is an 1, switch ON the ESP Led,
    else switch it off

 It will reconnect to the server if the connection is lost using a blocking
 reconnect function. See the 'mqtt_reconnect_nonblocking' example for how to
 achieve the same result without blocking the main loop.

 To install the Espressif ESP32 devkit v4 WROOM board, (using Arduino 1.6.4+):
  - Add the following 3rd party board manager under "File -> Preferences -> Additional Boards Manager URLs":
       http://arduino.esp8266.com/stable/package_esp8266com_index.json
  - Open the "Tools -> Board -> Board Manager" and click install for the ESP8266"
  - Select your Espressif ESP32 devkit v4 WROOM in "Tools -> Board"

*/
// Includes
#include <WiFi.h>
#include <PubSubClient.h>

/*
 * WiFi configuration variables
 */
const char* ssid = "VoCore2";
const char* password = "...";
const char* mqtt_server = "192.168.61.1";    // IP Broker/Server being VoCore2


/*
          MQTT Topic


            CASA
             |
          Oficina
           |   |
          Luz Temp


/casa/oficina/luz     // actuador, debe suscribirse
/casa/oficina/temp    // sensor, debe publicar

Comodines:
  + (sustituye cualquier nivel): /casa/+/temp
  # (cubre niveles hacia abajo): /casa/#

*/

/*
 * Global variables
 */
WiFiClient espClient;
PubSubClient client(espClient);
long lastMsg = 0;
char msg[50];
int value = 0;

/*
 * WiFi setup
 */
 void setup_wifi() {
   delay(10);
   //
   Serial.begin(115200);
   // Connect to Wi-Fi network with SSID and password
   Serial.print("Connecting to ");
   Serial.println(ssid);
   WiFi.begin(ssid);
   while (WiFi.status() != WL_CONNECTED) {
     delay(500);
     Serial.print(".");
   }
   randomSeed(micros());

   Serial.println("");
   Serial.println("WiFi connected");
   Serial.println("IP address: ");
   Serial.println(WiFi.localIP());
 }

 /*
  * MQTT callback function
  */
 void callback(char* topic, byte* payload, unsigned int length) {
   Serial.print("Message arrived [");
   Serial.print(topic);
   Serial.print("] ");
   for (int i = 0; i < length; i++) {
     Serial.print((char)payload[i]);
   }
   Serial.println();

   // Switch on the LED if an 1 was received as first character
   if ((char)payload[0] == '1') {
     //digitalWrite(BUILTIN_LED, LOW);   // Turn the LED on (Note that LOW is the voltage level
    Serial.println("LED off");
     // but actually the LED is on; this is because
     // it is active low on the ESP-01)
   } else {
     //digitalWrite(BUILTIN_LED, HIGH);  // Turn the LED off by making the voltage HIGH
     Serial.println("LED on");
   }

 }

 /*
  * MQTT connection
  */
 void reconnect() {
   // Loop until we're reconnected
   while (!client.connected()) {
     Serial.print("Attempting MQTT connection...");
     // Create a random client ID
     String clientId = "ESP32Client-01";
     clientId += String(random(0xffff), HEX);
     // Attempt to connect
     if (client.connect(clientId.c_str())) {
       Serial.println("connected");
       // Once connected, publish an announcement...
       client.publish("casa/oficina/temp", "Enviando el primer mensaje");
       // ... and resubscribe
       client.subscribe("casa/oficina/luz");
     } else {
       Serial.print("failed, rc=");
       Serial.print(client.state());
       Serial.println(" try again in 5 seconds");
       // Wait 5 seconds before retrying
       delay(5000);
     }
   }
 }

 /*
 * Initialisation and setup
 */
void setup() {
  Serial.begin(115200);
  setup_wifi();
  client.setServer(mqtt_server, 1883);  // Use Wireshark to monitor this port
  client.setCallback(callback);         // void callback(char* topic, byte* payload, unsigned int length);
}

/*
 * MAIN
 */
void loop() {
  /**/
if (!client.connected()) {
  reconnect();
}
client.loop();

long now = millis();
if (now - lastMsg > 2000) {
  lastMsg = now;
  ++value;
  snprintf (msg, 50, "hello world #%ld", value);    // Should replace this for a temp measurement
  Serial.print("Publish message: ");
  Serial.println(msg);
  client.publish("casa/oficina/temp", msg);
}
}
