# Arquitetura de Firmware

## Linguagem e Abordagem

- **Linguagem:** C/C++ Bare-Metal
- **Sem sistema operacional pesado** (sem RTOS, exceto se necessário futuramente)
- **Tempo de resposta:** Microssegundos (tempo real)
- **Consumo mínimo de energia**

## Plataformas Alvo

- ESP32-C3 (preferencial — BLE integrado)
- Raspberry Pi Pico (alternativa)

## Arquitetura do Loop Principal

```text
┌───────────────────────────────┐
│     INICIALIZAÇÃO DO CHIP     │
│ Configura Pinos GPIO, I2C, PWM│
└───────────────┬───────────────┘
                │
                ▼
┌───────────────────────────────────────────────────────────────────────────┐
│                            LOOP PRINCIPAL (Bare-Metal)                    │
├───────────────────────────────────────────────────────────────────────────┤
│  1. Sensing Laser (I2C) ──► Lê distância mm  ──► Ajusta PWM Motor (Radar)│
│  2. BLE Comm             ──► Recebe dados App ──► Dispara Áudio/Vibração  │
│  3. Sensor de Bateria   ──► Leitura ADC VBat ──► Alerta Sonoro se < 20%  │
│  4. Power Management    ──► Light Sleep (ms) ──► Economia de Bateria     │
└───────────────────────────────────────────────────────────────────────────┘
```

## Etapas do Loop

1. **Sensing Laser (I2C):** Leitura do VL53L0X, converte distância em duty-cycle PWM para motor ERM
2. **BLE Communication:** Recebe comandos/dados do app companion (linhas de ônibus, configurações), envia status
3. **Sensor de Bateria:** ADC lê divisor resistivo na bateria, alerta se < 20%
4. **Power Management:** Entra em light sleep entre ciclos para economia

## Convenções de Código

- Nomes de variáveis e funções em snake_case
- Constantes em UPPER_SNAKE_CASE
- Prefixo de módulo nos nomes de funções (ex: `radar_read_distance()`, `bus_check_rf()`, `audio_play_track()`)
- Comentários em português
- Headers (.h) com include guards
- Separação clara entre drivers de hardware e lógica de aplicação

## Estrutura de Diretórios Esperada

```
/src
  /drivers        # Drivers de hardware (VL53L0X, DFPlayer, BLE, ADC)
  /modules        # Lógica dos módulos (radar, ble_comm, audio, power)
  /config         # Configurações, thresholds, IDs cadastrados
  /utils          # Utilitários (math helpers, ring buffers, etc.)
  main.c          # Inicialização e loop principal
/include          # Headers públicos
/docs             # Documentação técnica
/hardware         # Esquemáticos, PCB, 3D prints
```
