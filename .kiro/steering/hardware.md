# Hardware — Componentes e Especificações

## Lista de Componentes

| Componente | Modelo Recomendado | Função | Custo (BRL) |
|---|---|---|---|
| Microcontrolador | ESP32-C3 | Processamento central, PWM, I2C, BLE | R$ 18,00 |
| Sensor de Distância Laser | VL53L0X (ToF) | Detecção ToF de alta precisão para obstáculos | R$ 12,00 |
| Módulo de Vibração + Transistor | Mini Motor ERM + 2N2222 | Feedback tátil ajustável via PWM | R$ 5,00 |
| Módulo de Áudio | DFPlayer Mini + MicroSD | Reprodução de alertas e mensagens .mp3 | R$ 12,00 |
| Bateria Recarregável | Lítio 3.7V (18650 ou Polímero) | Fonte principal de energia (~800mAh) | R$ 15,00 |
| Gerenciador Solar / Carga | CN3065 ou TP4056 USB-C | Regulação de carga USB-C + Painel Solar | R$ 8,00 |
| Mini Painel Solar | Painel 5V / 100mA (PET Laminado) | Carga flutuante com luz solar/ambiente | R$ 7,00 |
| Case e Estrutura | Impressão 3D (PETG / PLA) | Caixa de proteção e clipe de fixação universal | R$ 10,00 |

## Interfaces de Comunicação

- **I2C:** Sensor VL53L0X (endereço padrão 0x29)
- **PWM:** Motor de vibração ERM (controle de intensidade proporcional)
- **UART:** DFPlayer Mini (comandos de reprodução)
- **BLE:** Integrado ao ESP32-C3 (comunicação com app companion)
- **ADC:** Leitura de tensão da bateria (monitoramento VBat)
- **GPIO:** Botão de ação do usuário

## Requisitos Elétricos

- Tensão de operação: 3.3V (regulada internamente)
- Bateria: 3.7V Li-Po/Li-Ion
- Consumo médio estimado: ~50-80mA ativo, <1mA em deep sleep
- Carregamento: USB-C 5V + Solar 5V/100mA
