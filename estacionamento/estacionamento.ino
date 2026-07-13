#include <WiFi.h>
#include <PubSubClient.h>
#include <ESP32Servo.h>

// --- CONFIGURAÇÕES DE REDE E BROKER ---
const char* ssid = "Wokwi-GUEST"; // Rede Wi-Fi padrão do simulador Wokwi
const char* password = "";
const char* mqtt_server = "broker.hivemq.com";
const int mqtt_port = 1883;

// --- CONFIGURAÇÃO DOS TÓPICOS ATUALIZADOS ---
// IMPORTANTE: Cada aluno deve alterar o "bancada01" para o número da sua mesa!
const char* topico_individual = "minicurso/bancada01/cancela"; 
const char* topico_geral      = "minicurso/central/todas_cancelas";

// --- OBJETOS ---
WiFiClient espClient;
PubSubClient client(espClient);
Servo cancelaServo;

const int servoPin = 18; // Pino GPIO do ESP32 conectado ao servo

// --- FUNÇÃO QUE RECEBE AS MENSAGENS DO BROKER ---
void callback(char* topic, byte* payload, unsigned int length) {
  String mensagem = "";
  for (int i = 0; i < length; i++) {
    mensagem += (char)payload[i];
  }
  
  Serial.print("Mensagem recebida no tópico [");
  Serial.print(topic);
  Serial.print("]: ");
  Serial.println(mensagem);

  // Lógica de controle do Servo Motor
  if (mensagem == "ABRIR") {
    Serial.println("-> Comando ABRIR detectado! Levantando cancela (90 graus)...");
    cancelaServo.write(90); 
  } 
  else if (mensagem == "FECHAR") {
    Serial.println("-> Comando FECHAR detectado! Abaixando cancela (0 graus)...");
    cancelaServo.write(0);
  }
}

// --- FUNÇÃO PARA CONECTAR/RECONECTAR AO BROKER MQTT ---
void reconnect() {
  while (!client.connected()) {
    Serial.print("Tentando conectar ao Broker MQTT...");
    
    // ID aleatório para permitir múltiplos computadores na mesma rede sem quedas
    String clientId = "ESP32Client-" + String(random(0, 10000));
    
    if (client.connect(clientId.c_str())) {
      Serial.println(" Conectado com sucesso!");
      
      // Inscrição nos dois canais de comando
      client.subscribe(topico_individual);
      client.subscribe(topico_geral);
      
      Serial.println("Inscrito nos tópicos individual e geral de broadcast.");
    } else {
      Serial.print(" Falhou, código de erro=");
      Serial.print(client.state());
      Serial.println(" Nova tentativa em 5 segundos...");
      delay(5000);
    }
  }
}

// --- CONFIGURAÇÃO INICIAL ---
void setup() {
  Serial.begin(115200);
  
  // Inicializa o servo e define o estado inicial como fechado
  cancelaServo.attach(servoPin, 500, 2400); 
  cancelaServo.write(0); 
  
  // Inicializa conexão Wi-Fi simulada do Wokwi
  Serial.print("Conectando-se ao Wi-Fi ");
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWi-Fi conectado com sucesso!");
  
  // Vincula o servidor e a função de processamento de mensagens (callback)
  client.setServer(mqtt_server, mqtt_port);
  client.setCallback(callback);
}

// --- LOOP PRINCIPAL ---
void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop(); // Mantém o cliente escutando novas mensagens em background
  
  delay(10); 
}
