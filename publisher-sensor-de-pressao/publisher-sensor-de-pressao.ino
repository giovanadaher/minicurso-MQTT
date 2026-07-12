#include <WiFi.h>
#include <PubSubClient.h>

const char* ssid = "Wokwi-GUEST";
const char* password = "";

const char* mqttServer = "broker.hivemq.com";
const int mqttPort = 1883;

const char* topic = "oleoduto/pressao";

WiFiClient espClient;
PubSubClient client(espClient);

const int potenciometro = 34;

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

    String clientId = "SensorPressao-";
    clientId += String(random(1000));

    if (client.connect(clientId.c_str())) {

      Serial.println("Conectado!");

    } else {

      Serial.print("Erro: ");
      Serial.println(client.state());

      delay(2000);
    }
  }
}

void setup() {

  Serial.begin(115200);

  conectarWiFi();

  client.setServer(mqttServer, mqttPort);
}

void loop() {

  if (!client.connected()) {
    conectarMQTT();
  }

  client.loop();

  int leitura = analogRead(potenciometro);

  float pressao = map(leitura, 0, 4095, 0, 100);

  String mensagem = String(pressao);

  client.publish(topic, mensagem.c_str());

  Serial.print("Pressão enviada: ");
  Serial.print(pressao);
  Serial.println(" bar");

  delay(1000);
}
