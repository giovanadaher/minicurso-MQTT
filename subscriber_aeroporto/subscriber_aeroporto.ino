#include <WiFi.h>
#include <PubSubClient.h>

// --- CONFIGURAÇÕES DE CONEXÃO ---
const char* ssid = "Wokwi-GUEST"; // Rede padrão do simulador Wokwi (mude se for usar placa física)
const char* password = "";
const char* mqtt_server = "broker.hivemq.com";
const int mqtt_port = 1883;

// --- CONFIGURAÇÃO DE PORTAS (HARDWARE) ---
#define PIN_TRIG 5
#define PIN_ECHO 18

// --- CONFIGURAÇÃO DOS TÓPICOS ---
// Usamos uma estrutura hierárquica profissional de tópicos
const char* topico_status = "minicurso/aeroporto/gate1/status";
const char* topico_info   = "minicurso/aeroporto/gate1/telemetria";

// --- VARIÁVEIS DE CONTROLE E ESTADO ---
WiFiClient espClient;
PubSubClient client(espClient);

String ultimo_estado = ""; // Armazena se estava "LIVRE" ou "OCUPADO"
unsigned long ultimo_tempo_leitura = 0;
const long intervalo_leitura = 2000; // Medição a cada 2 segundos (2000ms)

// --- FUNÇÃO PARA MEDIR A DISTÂNCIA ---
float medirDistancia() {
  // Garante o pino TRIG em nível baixo
  digitalWrite(PIN_TRIG, LOW);
  delayMicroseconds(2);
  
  // Envia um pulso de 10 microssegundos para disparar o sensor
  digitalWrite(PIN_TRIG, HIGH);
  delayMicroseconds(10);
  digitalWrite(PIN_TRIG, LOW);
  
  // Mede o tempo de resposta do pulso no pino ECHO (em microssegundos)
  long duracao = pulseIn(PIN_ECHO, HIGH, 30000); // Timeout de 30ms se não voltar
  
  // Calcula a distância em centímetros (Velocidade do som = ~340m/s ou 0.034cm/us)
  float distancia = duracao * 0.017; 
  
  return distancia;
}

// --- CONECTAR AO WI-FI ---
void setup_wifi() {
  delay(10);
  Serial.print("\nConectando ao Wi-Fi: ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\n[Wi-Fi] Conectado!");
}

// --- CONECTAR AO BROKER MQTT ---
void reconnect() {
  while (!client.connected()) {
    Serial.print("[MQTT] Tentando conexão...");
    String clientId = "AirportGate1-" + String(random(0, 9999));
    
    if (client.connect(clientId.c_str())) {
      Serial.println("Conectado com sucesso!");
      // O portão de embarque apenas publica dados, não precisa se inscrever (subscribe) em tópicos
    } else {
      Serial.print("Falhou, erro=");
      Serial.print(client.state());
      Serial.println(" Tentando novamente em 5 segundos...");
      delay(5000);
    }
  }
}

void setup() {
  Serial.begin(115200);
  
  // Configura os pinos do sensor
  pinMode(PIN_TRIG, OUTPUT);
  pinMode(PIN_ECHO, INPUT);
  
  setup_wifi();
  client.setServer(mqtt_server, mqtt_port);
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  unsigned long tempo_atual = millis();
  
  // Executa a leitura de tempos em tempos (não bloqueante)
  if (tempo_atual - ultimo_tempo_leitura >= intervalo_leitura) {
    ultimo_tempo_leitura = tempo_atual;
    
    float distancia = medirDistancia();
    Serial.printf("[SENSOR] Distância medida: %.2f cm\n", distancia);
    
    // Se a leitura falhar ou for fora do alcance prático, consideramos o gate vazio
    if (distancia <= 0 || distancia > 100) {
      distancia = 999; 
    }

    String estado_atual = "";
    
    // --- LÓGICA DE DETECÇÃO (Aeronave a menos de 15 cm do sensor) ---
    if (distancia < 15.0) {
      estado_atual = "OCUPADO";
    } else {
      estado_atual = "LIVRE";
    }

    // Só publica no Broker se o estado mudou (Evita spam de mensagens repetidas)
    if (estado_atual != ultimo_estado) {
      ultimo_estado = estado_atual;
      
      // 1. Publica o status direto (LIVRE ou OCUPADO)
      client.publish(topico_status, estado_atual.c_str(), true); // Retain flag ativa para salvar o último estado
      
      // 2. Publica uma mensagem detalhada de telemetria
      String mensagem_telemetria;
      if (estado_atual == "OCUPADO") {
        mensagem_telemetria = "Aviso aos Pilotos: Vaga Ocupada. Aeronave detectada no Gate 1 (" + String(distancia, 1) + "cm do acoplamento).";
      } else {
        mensagem_telemetria = "Aviso aos Pilotos: Pista Livre. Gate 1 liberado para pouso e acoplamento.";
      }
      
      client.publish(topico_info, mensagem_telemetria.c_str(), true);
      
      Serial.println("[MQTT] Novo estado publicado: " + estado_atual);
      Serial.println("[MQTT] Telemetria: " + mensagem_telemetria);
    }
  }
}
