#include <WiFi.h>
#include <PubSubClient.h>

const char* ssid = "Wokwi-GUEST";
const char* password = "";

const char* mqttServer = "broker.hivemq.com";
const int mqttPort = 1883;


const char* topic = "oficina/led";

WiFiClient espClient;
PubSubClient client(espClient);

const int botao = 15;

bool ultimoEstado = HIGH;

void conectarWiFi() {
  Serial.print("Conectando ao WiFi");

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("\nWiFi conectado!");
}

void conectarMQTT() {

  while (!client.connected()) {

    Serial.print("Conectando ao MQTT... ");

    String clientId = "ESP32Publisher-";
    clientId += String(random(1000));

    if (client.connect(clientId.c_str())) {
      Serial.println("Conectado!");
    } else {
      Serial.print("Erro. Código: ");
      Serial.println(client.state());
      delay(2000);
    }
  }
}

void setup() {

  Serial.begin(115200);

  pinMode(botao, INPUT_PULLUP);

  conectarWiFi();

  client.setServer(mqttServer, mqttPort);
}

void loop() {

  if (!client.connected()) {
    conectarMQTT();
  }

  client.loop();

  bool estadoAtual = digitalRead(botao);

  if (estadoAtual != ultimoEstado) {

    if (estadoAtual == LOW) {
      client.publish(topic, "ON");
      Serial.println("Enviado: ON");
    } else {
      client.publish(topic, "OFF");
      Serial.println("Enviado: OFF");
    }

    ultimoEstado = estadoAtual;

    delay(50); // debounce
  }
}
