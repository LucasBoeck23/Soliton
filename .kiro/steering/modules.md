# Módulos do Sistema Sóliton

O sistema é dividido em duas partes: **Hardware autônomo** (funciona sem celular) e **App companion** (funções que precisam de internet).

---

## HARDWARE AUTÔNOMO (3 módulos embarcados)

### 1. Radar Anti-Acidentes (Sensoriamento Aéreo)

**Problema:** A bengala tradicional só detecta o chão plano. Obstáculos na altura do peito e cabeça causam acidentes e lesões frequentes.

**Solução:** Sensor laser de distância Time-of-Flight (ToF) varre o espaço aéreo em feixe focado (cone de ~25°).

**Feedback:** Conforme obstáculo aéreo se aproxima (< 1,5m), o sistema altera o ciclo de trabalho (PWM) de um motor de vibração ERM, fazendo a mão ou peito do usuário vibrar proporcionalmente à proximidade.

---

### 2. Sistema Sonoro Seguro (Condução Óssea / Módulo Áudio)

**Problema:** Fones de ouvido comuns bloqueiam o canal auditivo, anulando percepção do trânsito (extremamente perigoso).

**Solução:** Áudio informativo via fones de condução óssea (ouvido 100% livre) ou mini alto-falante direcionado no ombro/gola.

**Usos:**
- Alertas do radar ("obstáculo à frente")
- Informações recebidas do app (linhas de ônibus, etc.)
- Alerta de bateria baixa

---

### 3. Alimentação Híbrida e Carga Solar de Emergência

**Problema:** Risco de o dispositivo descarregar na rua, deixando usuário sem assistência.

**Solução:** Mini painel solar (5V / 100mA) integrado ao case com gerenciador de carga CN3065 / TP4056.

**Benefício:** Carga flutuante contínua com luz solar/ambiente durante caminhada, estendendo autonomia por semanas. Porta USB-C para recarga rápida na tomada.

---

## APP COMPANION (celular — funções que precisam de internet)

O app se comunica com o hardware via BLE e serve como:

### 4. Identificador de Linhas de Ônibus

**Problema:** A pessoa cega no ponto não sabe qual ônibus está se aproximando.

**Solução:** O app consulta API pública de transporte (ex: SPTrans, GTFS Realtime) usando GPS do celular para detectar que o usuário está em um ponto de ônibus. Quando um ônibus se aproxima, envia a informação via BLE pro hardware, que fala pelo áudio.

**Fluxo:**
1. App detecta que usuário está num ponto de ônibus (GPS + dados GTFS)
2. App consulta API pública com previsão de chegada
3. App envia via BLE pro dispositivo: linha + tempo estimado
4. Hardware fala via DFPlayer: "Linha 472 — 3 minutos"

---

### 5. Configuração do Hardware

**Funções do app:**
- Ajustar sensibilidade do radar (distância de alerta)
- Configurar linhas de ônibus favoritas
- Atualizar áudios no MicroSD (futuramente OTA)
- Ver status da bateria
- Ativar/desativar módulos

---

## Comunicação Hardware ↔ App

- **Protocolo:** BLE (Bluetooth Low Energy) — integrado no ESP32-C3
- **Direção:** Bidirecional
  - App → Hardware: comandos, configurações, informações de ônibus
  - Hardware → App: status de bateria, logs de uso
- **Offline:** Hardware funciona 100% sozinho para radar + áudio + energia. App é opcional/complementar.
