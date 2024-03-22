#include <Arduino.h>
#include <WiFi.h>
#include <PubSubClient.h>

#define trigPin 15
#define echoPin 2
int distance;
long duration;


// Deklarasi prototipe fungsi
void setup_wifi();

// Masukkan informasi WiFi
const char* ssid = "KONTRAKAN ALUMNI";
const char* password = "TMUA1443H";

// Masukkan informasi broker MQTT
const char* mqttServer = "192.168.1.20";
const int mqttPort = 1883;
// const char* mqttUser = "username_mqtt";
// const char* mqttPassword = "password_mqtt";

// Inisialisasi klien WiFi dan MQTT
WiFiClient espClient;
PubSubClient client(espClient);

// Deklarasi fungsi callback yang akan dipanggil saat pesan diterima
void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();
}

void parkingSystem(){
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  duration = pulseIn(echoPin, HIGH);
  distance = ((duration/2) * 0.343) / 10;
  Serial.print("Jarak Mobil :");
  Serial.println(distance);
  if (distance >= 140 && distance <= 180)
  {
    client.publish("testMode", "1");
  }else{
    client.publish("testMode", "0");
  }
}
  

void setup() {
  Serial.begin(115200);
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  setup_wifi(); // Panggil fungsi setup_wifi()
  client.setServer(mqttServer, mqttPort);
  client.setCallback(callback); // Tetapkan fungsi callback
}

void setup_wifi() {
  delay(100);
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}


void reconnect() {
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    if (client.connect("ESP32Client")) {
      Serial.println("connected");
      client.subscribe("testModeSubscribe"); // Subscribe ke topik "testMode"
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      delay(5000);
    }
  }
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();
  // Kirim pesan ke topik "testMode"
  parkingSystem();
  delay(5000); // Delay 5 detik sebelum mengirim pesan lagi
}