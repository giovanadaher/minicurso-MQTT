# Minicurso MQTT com ESP32

Este repositório reúne os códigos e materiais utilizados no **Minicurso de MQTT com ESP32**, organizado pelo **IEEE IFF Campos Centro**.

## Objetivo

Apresentar os conceitos fundamentais do protocolo MQTT e sua aplicação em projetos de Internet das Coisas (IoT), utilizando o ESP32, o simulador Wokwi e o broker público HiveMQ.

## Tecnologias utilizadas

* ESP32
* Arduino IDE
* Wokwi
* MQTT
* HiveMQ Public Broker
* Biblioteca PubSubClient
* Biblioteca WiFi (ESP32)

## Estrutura do repositório

```
publisher-botao/
subscriber-led/
simulacao-oleoduto/
slides/
```

## Projetos

### Publisher (Botão)

O ESP32 lê o estado de um botão e publica mensagens MQTT em um tópico.

Exemplo de mensagens publicadas:

* ON
* OFF

### Subscriber (LED)

O ESP32 permanece inscrito em um tópico MQTT e controla um LED de acordo com as mensagens recebidas.

### Simulação de monitoramento de oleodutos

Exemplo de aplicação industrial utilizando MQTT para transmitir dados de sensores de um oleoduto para um sistema de monitoramento, permitindo a geração de alertas em tempo real.

## Broker MQTT

Durante a oficina foi utilizado o broker público da HiveMQ.

Host:

```
broker.hivemq.com
```

Porta:

```
1883
```

## Bibliotecas

Instale a seguinte biblioteca na Arduino IDE:

* PubSubClient

A biblioteca WiFi já acompanha o pacote do ESP32.

## Simulações

Os projetos também podem ser executados no Wokwi.

Link Projeto Publisher - Botão: https://wokwi.com/projects/469314570888818689

Link Projeto Subscriber - LED: https://wokwi.com/projects/469314655403004929

## Organização

IEEE IFF Campos Centro

Minicurso de MQTT com ESP32

