#include <WiFi.h>
#include <PubSubClient.h>

const char* ssid = "Wokwi-GUEST";
const char* password = "";

const char* mqttServer = "broker.hivemq.com";
const int mqttPort = 1883;



const char* topic = "oficina/led";

WiFiClient espClient;
PubSubClient client(espClient);

const int led = 4;

void conectarWiFi() {
  Serial.print("Conectando ao WiFi");

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("\nWiFi conectado!");
}

void callback(char* topic, byte* payload, unsigned int length) {

  String mensagem = "";

  for (int i = 0; i < length; i++) {
    mensagem += (char)payload[i];
  }

  Serial.print("Mensagem recebida: ");
  Serial.println(mensagem);

  if (mensagem == "ON") {
    digitalWrite(led, HIGH);
  }
  else if (mensagem == "OFF") {
    digitalWrite(led, LOW);
  }
}

void conectarMQTT() {

  while (!client.connected()) {

    Serial.print("Conectando ao MQTT... ");

    String clientId = "ESP32Subscriber-";
    clientId += String(random(1000));

    if (client.connect(clientId.c_str())) {

      Serial.println("Conectado!");

      client.subscribe(topic);

    } else {

      Serial.print("Erro. Código: ");
      Serial.println(client.state());

      delay(2000);
    }
  }
}

void setup() {

  Serial.begin(115200);

  pinMode(led, OUTPUT);
  digitalWrite(led, LOW);

  conectarWiFi();

  client.setServer(mqttServer, mqttPort);
  client.setCallback(callback);
}

void loop() {

  if (!client.connected()) {
    conectarMQTT();
  }

  client.loop();
}
