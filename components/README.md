# Componentes — Projeto Sóliton

Este diretório contém todos os componentes ESP-IDF do projeto, organizados em camadas com regras de dependência bem definidas.

## Estrutura de Subdiretórios

### `hal/` — Hardware Abstraction Layer

**Responsabilidade:** Define interfaces puras (headers C) para cada periférico (I2C, PWM, UART, ADC, GPIO). Fornece duas implementações: uma real para o ESP32-C3 e uma Mock Layer para testes em host.

- `include/` — Headers de interface pública (`hal_i2c.h`, `hal_pwm.h`, etc.)
- `src/` — Implementação real para o target ESP32-C3
- `mock/include/` — Header de controle do mock (`hal_mock_ctrl.h`)
- `mock/src/` — Implementação mock para testes em host

### `drivers/` — Drivers de Periféricos

**Responsabilidade:** Implementa a lógica de controle de cada componente de hardware externo. Cada driver é um componente ESP-IDF independente que acessa o hardware exclusivamente através da HAL.

- `vl53l0x/` — Sensor de distância laser Time-of-Flight (I2C)
- `dfplayer/` — Módulo de áudio DFPlayer Mini (UART)
- `motor_erm/` — Motor de vibração ERM (PWM)
- `battery/` — Monitoramento de tensão da bateria (ADC)

### `modules/` — Lógica de Aplicação

**Responsabilidade:** Implementa os módulos funcionais de alto nível do sistema (radar anti-acidentes, sistema de áudio, gerenciamento de energia, comunicação BLE). Contém a lógica de negócio que orquestra os drivers através da HAL.

### `config/` — Configurações e Constantes

**Responsabilidade:** Armazena apenas dados de configuração — pinout, thresholds, constantes do sistema. Não contém lógica executável. Serve como fonte única de verdade para parâmetros do hardware.

- `include/` — Headers com `#define` de pinout, thresholds, IDs

### `utils/` — Utilitários Compartilhados

**Responsabilidade:** Funções utilitárias genéricas reutilizáveis (math helpers, ring buffers, conversões). Dependem apenas da biblioteca padrão C, sem referência a hardware ou lógica de aplicação.

- `include/` — Headers públicos dos utilitários
- `src/` — Implementação dos utilitários

---

## Regras de Dependência entre Camadas

```
┌─────────────────────────────────────────────────────────────────┐
│ CAMADA          │ PODE DEPENDER DE         │ NÃO PODE DEPENDER  │
├─────────────────┼──────────────────────────┼────────────────────┤
│ modules/        │ hal/, utils/, config/    │ drivers/            │
│ drivers/        │ hal/, ESP-IDF APIs       │ modules/, utils/,   │
│                 │                          │ outros drivers      │
│ hal/            │ tipos C stdlib, ESP-IDF  │ drivers/, modules/  │
│ utils/          │ tipos C stdlib           │ hal/, drivers/,     │
│                 │                          │ modules/            │
│ config/         │ nada (só dados)          │ tudo                │
└─────────────────────────────────────────────────────────────────┘
```

### Resumo Visual

```
    ┌──────────┐
    │ modules/ │  ← Camada de Aplicação
    └────┬─────┘
         │ depende de
    ┌────┴─────┐     ┌─────────┐     ┌─────────┐
    │   hal/   │     │ utils/  │     │ config/ │
    └────┬─────┘     └─────────┘     └─────────┘
         │ depende de
    ┌────┴─────┐
    │ drivers/ │  ← Camada de Hardware (também depende de hal/)
    └──────────┘
```

### Regras Detalhadas

1. **modules/** → Pode depender de `hal/`, `utils/`, `config/`. **NÃO PODE** depender de `drivers/`.
2. **drivers/** → Pode depender de `hal/` e APIs do ESP-IDF. **NÃO PODE** depender de `modules/`, `utils/`, ou de outros drivers.
3. **hal/** → Pode depender de tipos da C stdlib e tipos do ESP-IDF. **NÃO PODE** depender de `drivers/` ou `modules/`.
4. **utils/** → Pode depender apenas de tipos da C stdlib. **NÃO PODE** depender de `hal/`, `drivers/`, ou `modules/`.
5. **config/** → Contém apenas dados. **NÃO PODE** depender de nenhum outro componente.

### Enforcement

As regras de dependência são validadas em tempo de build pelo script `tools/check_layers.cmake`. Se um componente declarar uma dependência que viole as regras acima, o CMake emitirá um `FATAL_ERROR` identificando:
- O componente de origem
- O componente proibido referenciado
- A regra de camada violada

Cada componente deve declarar sua camada no `CMakeLists.txt`:
```cmake
set(COMPONENT_LAYER "drivers")  # ou "hal", "modules", "utils", "config"
```
