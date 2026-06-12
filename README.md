# Lampada Inteligente – ESP32-S3 N16R8

Sistema de lampada inteligente controlado localmente e remotamente via Arduino IoT Cloud.
Desenvolvido como parte da avaliação AV2 da disciplina Praticas Integradas: Camada de Aplicacao – SENAI CIMATEC.

---

## Sobre o Projeto

O sistema utiliza um ESP32-S3 para monitorar temperatura e luminosidade do ambiente e controlar
um LED RGB automaticamente. O usuario tambem pode enviar comandos remotos pelo painel do Arduino IoT Cloud.

---

## Hardware Utilizado

| Componente       | Quantidade | Pino ESP32-S3                  |
|------------------|------------|--------------------------------|
| ESP32-S3 N16R8   | 1          | –                              |
| LED RGB          | 1          | R: GPIO4, G: GPIO5, B: GPIO6   |
| Buzzer           | 1          | GPIO7                          |
| Botao            | 1          | GPIO9 (INPUT_PULLUP)           |
| Sensor DHT22     | 1          | GPIO10                         |
| Fotorresistor    | 1          | GPIO1 (ADC)                    |
| Potenciometro    | 1          | GPIO2 (ADC)                    |
| Resistor 220 ohm | 3          | Serie nos canais do LED RGB    |
| Resistor 10k ohm | 1          | Divisor de tensao do LDR       |

---

## Esquema de Ligacoes

```
ESP32-S3
  GPIO4  --[220 ohm]-- LED_R --+
  GPIO5  --[220 ohm]-- LED_G --+-- GND
  GPIO6  --[220 ohm]-- LED_B --+

  GPIO7  -- Buzzer (+) -- GND

  GPIO9  -- Botao -- GND   (INPUT_PULLUP interno, sem resistor externo)

  GPIO10 -- DHT22 DATA
  3V3    -- DHT22 VCC
  GND    -- DHT22 GND

  3V3 --[LDR]--+-- GPIO1 (ADC)
               +--[10k ohm]-- GND

  3V3 -- Potenciometro extremidade 1
  GND -- Potenciometro extremidade 2
  GPIO2 (ADC) -- Potenciometro cursor (pino central)
```

---

## Funcionalidades

**Automaticas (sem intervencao do usuario):**
- LED RGB liga automaticamente quando o ambiente esta escuro (LDR abaixo do limite)
- LED RGB apaga quando o ambiente esta claro
- LED RGB apaga quando a temperatura esta fora da faixa segura (abaixo de 0 oC ou acima de 25 oC)
- Buzzer dispara quando a temperatura esta fora da faixa segura
- Potenciometro controla a cor do LED seguindo o espectro de temperatura de cor

**Escala de cores do potenciometro:**

| Posicao     | Cor           |
|-------------|---------------|
| Minima      | Vermelho      |
| Baixa       | Laranja       |
| Media       | Branco quente |
| Alta        | Azul turquesa |
| Maxima      | Azul          |

**Botao fisico:**
- Primeiro aperto: desliga LED e buzzer
- Segundo aperto: reativa o sistema
- Implementado via interrupcao externa

**Painel Arduino IoT Cloud:**

| Comando     | Acao                        |
|-------------|-----------------------------|
| Ligar       | Liga o sistema              |
| Desligar    | Desliga o sistema           |
| Vermelho    | LED vermelho por 1 segundo  |
| Amarelo     | LED amarelo por 1 segundo   |
| Azul        | LED azul por 1 segundo      |

**Variaveis monitoradas no painel:**
- Temperatura em graus Celsius
- Luminosidade (valor bruto do ADC)
- Posicao do potenciometro
- Status do LED (ligado/desligado)

---

## Como Montar o Projeto

1. Monte os componentes na protoboard conforme o esquema de ligacoes acima
2. Conecte o ESP32-S3 ao computador via USB

---

## Como Configurar a Arduino IDE

1. Baixe a Arduino IDE 2.x em arduino.cc/en/software
2. Va em File -> Preferences e adicione a URL abaixo em "Additional boards manager URLs":
   https://raw.githubusercontent.com/espressif/arduino-esp32/gh-pages/package_esp32_index.json
3. Va em Tools -> Board -> Boards Manager, pesquise "esp32" (Espressif) e instale
4. Selecione: Tools -> Board -> ESP32 Arduino -> ESP32S3 Dev Module

---

## Como Instalar as Bibliotecas

Va em Sketch -> Manage Libraries e instale:

- ArduinoIoTCloud (autor: Arduino)
- Arduino_ConnectionHandler (autor: Arduino)

Quando perguntar sobre dependencias, clique em "Install All".

---

## Como Configurar as Credenciais

No topo do arquivo `lampada_iot_cloud.ino`, preencha com seus dados:

```cpp
const char DEVICE_LOGIN_NAME[] = "SEU_DEVICE_ID";
const char SSID[]              = "SEU_SSID";
const char PASS[]              = "SUA_SENHA_WIFI";
const char DEVICE_KEY[]        = "SUA_SECRET_KEY";
```

O Device ID e a Secret Key sao gerados ao criar o dispositivo no Arduino IoT Cloud
em create.arduino.cc/iot.

---

## Como Gravar o Firmware

1. Abra o arquivo `lampada_iot_cloud.ino` na Arduino IDE
2. Preencha as credenciais conforme instrucoes acima
3. Conecte o ESP32-S3 via USB
4. Clique em Upload
5. Abra o Serial Monitor em 115200 baud para acompanhar as leituras

---

## Monitor Serial

A cada 500 ms o sistema imprime no Serial Monitor:

```
─────────────────────────────────
LED RGB  : ON (ligado)
Temp     : 23.4 oC
Lumin.   : 842
Pot      : 2100
Sistema  : ATIVO
Buzzer: DESLIGADO
```

Se a temperatura estiver fora da faixa segura, aparece tambem:
```
PERIGO! Desligar!
```

---

## Estrutura do Repositorio

```
lampada-iot/
├── firmware/
│   └── lampada_iot_cloud.ino
├── .gitignore
└── README.md
```

---

## Desafios Encontrados

- Leitura do DHT22 implementada manualmente sem biblioteca, respeitando o protocolo de temporização do sensor
- Uso de interrupcao externa no botao para garantir resposta imediata independente do estado do loop principal
- Intervalo minimo de 2 segundos entre leituras do DHT22 controlado via millis() para nao bloquear o loop

---

## Referencias

- Espressif Systems. ESP32-S3 Technical Reference Manual. 2022.
- Arduino. Arduino IoT Cloud. Disponivel em: create.arduino.cc/iot
- Wokwi. Online ESP32 and Arduino Simulator. Disponivel em: wokwi.com
- Random Nerd Tutorials. ESP32 Tutorials. Disponivel em: randomnerdtutorials.com

---
