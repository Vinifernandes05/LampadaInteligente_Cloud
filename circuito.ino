#include <ArduinoIoTCloud.h>
#include <Arduino_ConnectionHandler.h>

// ─────────────────────────────────────────────────────────────
//  CREDENCIAIS – preencha com os seus dados do Arduino IoT Cloud
// ─────────────────────────────────────────────────────────────
const char DEVICE_LOGIN_NAME[] = "";   // Device ID
const char SSID[]              = "";
const char PASS[]              = "";
const char DEVICE_KEY[]        = "";  // Secret Key

// ─── PINOS ───────────────────────────────────────────────────
#define PIN_LED_R   4
#define PIN_LED_G   5
#define PIN_LED_B   6
#define PIN_BUZZER  7
#define PIN_BOTAO   9
#define PIN_DHT     10
#define PIN_LDR     1
#define PIN_POT     2

// ─── LIMITES ─────────────────────────────────────────────────
#define TEMP_MIN      0.0f
#define TEMP_MAX     25.0f
#define LDR_ESCURO   1800
#define BUZZER_FREQ  1000

// ─── VARIÁVEIS GLOBAIS DO CIRCUITO ───────────────────────────
volatile bool sistemaAtivo = true;

float temperatura  = 0.0f;
int   luminosidade = 0;
int   valorPot     = 0;

// ─── VARIÁVEIS CLOUD ─────────────────────────────────────────
// Estas variáveis são sincronizadas automaticamente com o painel
CloudTemperature  cloudTemperatura;    // Gauge/Chart no painel
CloudLight        cloudLuminosidade;   // Gauge/Chart no painel
int               cloudPotenciometro;  // Value no painel
bool              cloudLed;            // LED indicator no painel
String            cloudComando;        // Text input no painel

// Protótipo do callback (chamado quando o painel envia comando)
void onCloudComandoChange();

// ─────────────────────────────────────────────────────────────
//  LEITURA MANUAL DO DHT22 (sem biblioteca) – inalterado
// ─────────────────────────────────────────────────────────────
bool lerDHT22(float* temp, float* umid) {
  uint8_t dados[5] = {0, 0, 0, 0, 0};

  // 1. Sinal de início: ESP32 puxa baixo por ~1 ms
  pinMode(PIN_DHT, OUTPUT);
  digitalWrite(PIN_DHT, LOW);
  delay(1);
  digitalWrite(PIN_DHT, HIGH);
  pinMode(PIN_DHT, INPUT_PULLUP);

  // 2. Aguarda o DHT22 responder (puxa baixo ~80 µs, depois alto ~80 µs)
  unsigned long t0 = micros();
  while (digitalRead(PIN_DHT) == HIGH) {
    if (micros() - t0 > 100) return false;
  }
  t0 = micros();
  while (digitalRead(PIN_DHT) == LOW) {
    if (micros() - t0 > 100) return false;
  }
  t0 = micros();
  while (digitalRead(PIN_DHT) == HIGH) {
    if (micros() - t0 > 100) return false;
  }

  // 3. Lê os 40 bits (5 bytes): umid_hi, umid_lo, temp_hi, temp_lo, checksum
  for (int i = 0; i < 40; i++) {
    t0 = micros();
    while (digitalRead(PIN_DHT) == LOW) {
      if (micros() - t0 > 100) return false;
    }
    t0 = micros();
    while (digitalRead(PIN_DHT) == HIGH) {
      if (micros() - t0 > 100) return false;
    }
    if (micros() - t0 > 40) {
      dados[i / 8] |= (1 << (7 - (i % 8)));
    }
  }

  // 4. Verifica checksum
  uint8_t soma = dados[0] + dados[1] + dados[2] + dados[3];
  if (soma != dados[4]) return false;

  // 5. Decodifica umidade
  *umid = ((dados[0] << 8) | dados[1]) * 0.1f;

  // 6. Decodifica temperatura
  int16_t tempRaw = ((dados[2] & 0x7F) << 8) | dados[3];
  *temp = tempRaw * 0.1f;
  if (dados[2] & 0x80) *temp = -(*temp);

  return true;
}

// ─── ISR – BOTÃO ─────────────────────────────────────────────
void IRAM_ATTR isr_botao() {
  sistemaAtivo = !sistemaAtivo;
}

// ─────────────────────────────────────────────────────────────
//  LED RGB – inalterado
// ─────────────────────────────────────────────────────────────
void setRGB(uint8_t r, uint8_t g, uint8_t b) {
  analogWrite(PIN_LED_R, r);
  analogWrite(PIN_LED_G, g);
  analogWrite(PIN_LED_B, b);
}

void desligarLED() {
  setRGB(0, 0, 0);
}

void aplicarCorPot(int pot) {
  if      (pot < 820)  setRGB(255,   0,   0);
  else if (pot < 1639) setRGB(255,  80,   0);
  else if (pot < 2458) setRGB(255, 200, 100);
  else if (pot < 3277) setRGB(  0, 210, 200);
  else                 setRGB(  0,   0, 255);
}

// ─────────────────────────────────────────────────────────────
//  LÓGICA DO LED – inalterado
// ─────────────────────────────────────────────────────────────
void atualizarLED() {
  if (!sistemaAtivo) { desligarLED(); return; }

  bool tempPerigosa   = (temperatura < TEMP_MIN || temperatura > TEMP_MAX);
  bool ambienteEscuro = (luminosidade < LDR_ESCURO);

  if (tempPerigosa)   { desligarLED(); return; }
  if (ambienteEscuro) { aplicarCorPot(valorPot); }
  else                { desligarLED(); }
}

// ─────────────────────────────────────────────────────────────
//  LÓGICA DO BUZZER – inalterado
// ─────────────────────────────────────────────────────────────
void atualizarBuzzer() {
  if (!sistemaAtivo) {
    noTone(PIN_BUZZER);
    Serial.println("Buzzer: DESLIGADO");
    return;
  }

  bool tempPerigosa = (temperatura < TEMP_MIN || temperatura > TEMP_MAX);
  if (tempPerigosa) {
    tone(PIN_BUZZER, BUZZER_FREQ);
    Serial.println("Buzzer: LIGADO");
  } else {
    noTone(PIN_BUZZER);
    Serial.println("Buzzer: DESLIGADO");
  }
}

// ─────────────────────────────────────────────────────────────
//  MONITOR SERIAL – inalterado
// ─────────────────────────────────────────────────────────────
void imprimirSerial() {
  bool ledLigado = sistemaAtivo &&
                   (luminosidade < LDR_ESCURO) &&
                   !(temperatura < TEMP_MIN || temperatura > TEMP_MAX);

  Serial.println("─────────────────────────────────");
  Serial.print("LED RGB  : ");
  Serial.println(ledLigado ? "ON (ligado)" : "OFF (desligado)");
  Serial.print("Temp     : ");
  Serial.print(temperatura, 1);
  Serial.println(" oC");
  if (temperatura < TEMP_MIN || temperatura > TEMP_MAX)
    Serial.println("PERIGO! Desligar!");
  Serial.print("Lumin.   : ");
  Serial.println(luminosidade);
  Serial.print("Pot      : ");
  Serial.println(valorPot);
  Serial.print("Sistema  : ");
  Serial.println(sistemaAtivo ? "ATIVO" : "DESATIVADO");
}

// ─────────────────────────────────────────────────────────────
//  CALLBACK – chamado pelo Cloud quando o painel envia um comando
//  Buzzer propositalmente ausente daqui (apenas local)
// ─────────────────────────────────────────────────────────────
void onCloudComandoChange() {
  String cmd = cloudComando;
  cmd.trim();

  Serial.print("[Cloud] Comando recebido: ");
  Serial.println(cmd);

  if      (cmd == "Ligar")    { sistemaAtivo = true;  atualizarLED(); }
  else if (cmd == "Desligar") { sistemaAtivo = false; atualizarLED(); }
  else if (cmd == "Vermelho") { setRGB(255, 0, 0);   delay(1000); atualizarLED(); }
  else if (cmd == "Amarelo")  { setRGB(255, 255, 0); delay(1000); atualizarLED(); }
  else if (cmd == "Azul")     { setRGB(0, 0, 255);   delay(1000); atualizarLED(); }

  cloudComando = "";  // limpa para aceitar o próximo comando
}

// ─────────────────────────────────────────────────────────────
//  REGISTRA AS PROPRIEDADES NO ARDUINO IOT CLOUD
// ─────────────────────────────────────────────────────────────
void initProperties() {
  ArduinoCloud.setBoardId(DEVICE_LOGIN_NAME);
  ArduinoCloud.setSecretDeviceKey(DEVICE_KEY);

  // Comando: painel pode escrever e ESP32 lê → callback disparado
  ArduinoCloud.addProperty(cloudComando,       READWRITE, ON_CHANGE, onCloudComandoChange);

  // Sensores: painel só lê, ESP32 envia
  ArduinoCloud.addProperty(cloudTemperatura,   READ, ON_CHANGE, NULL);
  ArduinoCloud.addProperty(cloudLuminosidade,  READ, ON_CHANGE, NULL);
  ArduinoCloud.addProperty(cloudPotenciometro, READ, ON_CHANGE, NULL);
  ArduinoCloud.addProperty(cloudLed,           READ, ON_CHANGE, NULL);
}

WiFiConnectionHandler ArduinoIoTPreferredConnection(SSID, PASS);

// ─────────────────────────────────────────────────────────────
//  SETUP
// ─────────────────────────────────────────────────────────────
void setup() {
  Serial.begin(115200);
  delay(1000);

  pinMode(PIN_LED_R,  OUTPUT);
  pinMode(PIN_LED_G,  OUTPUT);
  pinMode(PIN_LED_B,  OUTPUT);
  pinMode(PIN_BUZZER, OUTPUT);

  pinMode(PIN_BOTAO, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(PIN_BOTAO), isr_botao, FALLING);

  analogReadResolution(12);

  // Inicia o Arduino IoT Cloud
  initProperties();
  ArduinoCloud.begin(ArduinoIoTPreferredConnection);
  ArduinoCloud.printDebugInfo();

  Serial.println("Sistema iniciado.");
}

// ─────────────────────────────────────────────────────────────
//  LOOP
// ─────────────────────────────────────────────────────────────
#define INTERVALO_DHT 2000
unsigned long ultimaLeituraDHT = 0;

void loop() {
  // Mantém a conexão com o Arduino IoT Cloud
  ArduinoCloud.update();

  // Lê DHT22 a cada 2 segundos
  if (millis() - ultimaLeituraDHT >= INTERVALO_DHT) {
    float t, u;
    if (lerDHT22(&t, &u)) {
      temperatura = t;
    }
    ultimaLeituraDHT = millis();
  }

  // LDR e potenciômetro lidos a cada ciclo
  luminosidade = analogRead(PIN_LDR);
  valorPot     = analogRead(PIN_POT);

  atualizarLED();
  atualizarBuzzer();
  imprimirSerial();

  // Atualiza variáveis Cloud com os valores locais
  cloudTemperatura   = temperatura;
  cloudLuminosidade  = luminosidade;
  cloudPotenciometro = valorPot;
  cloudLed           = sistemaAtivo &&
                       (luminosidade < LDR_ESCURO) &&
                       !(temperatura < TEMP_MIN || temperatura > TEMP_MAX);

  delay(500);
}