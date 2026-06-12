# 💡 Sistema de Lâmpada Inteligente IoT

Projeto desenvolvido para a disciplina **Práticas Integradas: Camada de Aplicação**, com o objetivo de implementar uma lâmpada inteligente utilizando ESP32, sensores e integração com Arduino IoT Cloud.

---

## 📋 Descrição do Projeto

O sistema consiste em uma lâmpada inteligente capaz de:

* Monitorar temperatura ambiente.
* Monitorar luminosidade do ambiente.
* Controlar automaticamente um LED RGB.
* Emitir alertas sonoros através de um buzzer.
* Permitir controle remoto através do Arduino IoT Cloud.
* Armazenar dados em banco de dados para monitoramento e análise.

O projeto integra hardware, software, computação em nuvem e banco de dados, proporcionando uma experiência completa de desenvolvimento IoT.

---

# 🎯 Objetivos

* Desenvolver um sistema IoT utilizando ESP32.
* Integrar sensores e atuadores.
* Enviar dados para a nuvem.
* Armazenar dados em banco de dados.
* Criar dashboards para monitoramento remoto.
* Aplicar versionamento de código utilizando Git e GitHub.

---

# 🛠️ Componentes Utilizados

| Componente            | Quantidade |
| --------------------- | ---------- |
| ESP32                 | 1          |
| Sensor de Temperatura | 1          |
| Fotorresistor (LDR)   | 1          |
| LED RGB               | 1          |
| Buzzer                | 1          |
| Potenciômetro         | 1          |
| Botão                 | 1          |

---

# 🏗️ Arquitetura do Sistema

```text
Sensores e Atuadores
        │
        ▼
      ESP32
        │
        ▼
 Arduino IoT Cloud
        │
        ▼
     Backend API
        │
        ▼
   Banco de Dados
        │
        ▼
 Dashboard Online
```

---

# ⚙️ Funcionalidades

## Sensor de Temperatura

* Realiza leituras contínuas.
* Exibe temperatura no monitor serial.
* Aciona alerta quando:

```text
Temperatura < 0°C
ou
Temperatura > 25°C
```

* Ativa o buzzer em situação de risco.

---

## Fotorresistor

* Monitora a luminosidade do ambiente.
* Aciona automaticamente o LED RGB quando o ambiente estiver escuro.

---

## LED RGB

### Controle Automático

Liga quando:

* Ambiente escuro.
* Temperatura entre 0°C e 25°C.

Desliga quando:

* Ambiente claro.
* Temperatura fora da faixa segura.
* Sistema for desligado pelo botão.

### Controle de Cor

A cor é definida pelo potenciômetro:

| Faixa | Cor      |
| ----- | -------- |
| Baixa | Vermelho |
| Média | Amarelo  |
| Alta  | Azul     |

Seguindo o espectro:

```text
Vermelho → Laranja → Branco → Azul Turquesa → Azul
```

---

## Botão

Implementado utilizando interrupção externa.

Funções:

* Primeiro clique:

  * Desliga LED RGB.
  * Desliga buzzer.

* Segundo clique:

  * Reativa LED RGB.
  * Reativa buzzer.

---

## Buzzer

Emite alerta sonoro quando:

```text
Temperatura < 0°C
ou
Temperatura > 25°C
```

---

# ☁️ Integração com Arduino IoT Cloud

O sistema envia e recebe informações através do Arduino IoT Cloud.

## Comandos Disponíveis

### LED

* Ligar
* Desligar

### Cores

* Vermelho
* Amarelo
* Azul

### Sensor de Temperatura

* Ativar Temperatura
* Desativar Temperatura

### Fotorresistor

* Ativar Detector
* Desativar Detector

### Buzzer

* Ativar Buzzer
* Desativar Buzzer

---

# 🗄️ Banco de Dados

O sistema registra informações coletadas pelos sensores.

## Dados armazenados

* Timestamp
* Temperatura
* Luminosidade
* Estado do LED
* Cor Atual
* Estado do Buzzer

---

# 🚀 Como Montar o Projeto

## 1. Montagem Física

Conectar ao ESP32:

* Sensor de temperatura
* Fotorresistor
* LED RGB
* Potenciômetro
* Buzzer
* Botão

Consultar os datasheets dos componentes para definição dos pinos.

---

## 2. Instalar Dependências

### Arduino IDE

Baixar:

https://www.arduino.cc/en/software

### Biblioteca ESP32

Instalar pelo Gerenciador de Placas da Arduino IDE.

### Bibliotecas Necessárias

```cpp
WiFi.h
ArduinoIoTCloud.h
Arduino_ConnectionHandler.h
```

Além das bibliotecas específicas dos sensores utilizados.

---

## 3. Configurar Arduino IoT Cloud

1. Criar uma conta.
2. Registrar o dispositivo ESP32.
3. Criar variáveis de nuvem.
4. Gerar credenciais.
5. Inserir as credenciais no firmware.

---

# ▶️ Como Executar

## Firmware ESP32

1. Abrir o projeto na Arduino IDE.
2. Selecionar a placa ESP32.
3. Compilar.
4. Fazer upload.

---

## Backend

Exemplo utilizando Node.js:

```bash
npm install
npm start
```

Servidor disponível em:

```text
http://localhost:3000
```

---

# 📊 Dashboard

O dashboard deve apresentar:

* Temperatura
* Luminosidade
* Estado do LED
* Cor Atual
* Estado do Botão
* Estado do Buzzer

Com atualização automática em tempo real.

---

# 📂 Estrutura do Projeto

```text
📦 projeto-lampada-inteligente
├── firmware/
│   └── esp32
│
├── backend/
│   ├── api
│   └── database
│
├── docs/
│   ├── esquematico
│   └── imagens
│
├── README.md
└── LICENSE
```

---

# 🧪 Testes Realizados

* Teste de leitura de temperatura.
* Teste de leitura de luminosidade.
* Teste de acionamento do LED RGB.
* Teste do buzzer.
* Teste do botão com interrupção.
* Teste de comunicação com Arduino IoT Cloud.
* Teste de persistência no banco de dados.

---

# ⚠️ Possíveis Problemas

## ESP32 não conecta ao Wi-Fi

Verifique:

* SSID
* Senha
* Alcance da rede

## Dados não aparecem no Dashboard

Verifique:

* Conexão com Arduino IoT Cloud.
* Variáveis configuradas.
* Token de autenticação.

## Sensores sem leitura

Verifique:

* Alimentação.
* Pinos conectados.
* Bibliotecas instaladas.

---

# 👥 Equipe

* Vinicius Sousa Fernandes
* Rafael Guerra de Oliveira
* Lucca Badaró

---
