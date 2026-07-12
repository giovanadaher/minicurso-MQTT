#include <WiFi.h>
#include <PubSubClient.h>

const char* ssid = "Wokwi-GUEST";
const char* password = "";

const char* mqttServer = "broker.hivemq.com";
const int mqttPort = 1883;

const char* topic = "oleoduto/pressao";

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

  float pressao = mensagem.toFloat();

  Serial.print("Pressão recebida: ");
  Serial.print(pressao);
  Serial.println(" bar");

  if (pressao > 80) {

    digitalWrite(led, HIGH);

    Serial.println("⚠ ALERTA: PRESSÃO ACIMA DO LIMITE!");

  } else {

    digitalWrite(led, LOW);

    Serial.println("Sistema operando normalmente.");
  }

  Serial.println("-------------------------");
}

void conectarMQTT() {

  while (!client.connected()) {

    Serial.print("Conectando ao MQTT... ");

    String clientId = "CentralMonitoramento-";
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
