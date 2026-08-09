# Arquitetura do Firmware — Projeto Sóliton

Este documento descreve a arquitetura em camadas do firmware, as regras de dependência entre componentes, e os fluxos de dados, build e testes do projeto.

---

## Diagrama de Camadas

O firmware é organizado em camadas com responsabilidades bem definidas. Cada camada só pode depender das camadas abaixo dela, seguindo regras estritas.

```mermaid
graph TD
    subgraph "Application Layer"
        M[modules/]
    end

    subgraph "Service Layer"
        U[utils/]
        C[config/]
    end

    subgraph "Abstraction Layer"
        H[hal/]
    end

    subgraph "Driver Layer"
        D[drivers/]
    end

    subgraph "Platform"
        ESP[ESP-IDF APIs]
        HW[ESP32-C3 Hardware]
    end

    M -->|allowed| H
    M -->|allowed| U
    M -->|allowed| C
    D -->|allowed| H
    D -->|allowed| ESP
    H -->|allowed| ESP
    ESP --> HW

    %% Forbidden dependencies (shown as comments for clarity)
    %% M -x-> D (PROIBIDO)
    %% D -x-> M (PROIBIDO)
    %% D -x-> U (PROIBIDO)
    %% D -x-> other D (PROIBIDO)
    %% H -x-> D (PROIBIDO)
    %% H -x-> M (PROIBIDO)

    style M fill:#4CAF50,color:#fff
    style U fill:#2196F3,color:#fff
    style C fill:#2196F3,color:#fff
    style H fill:#FF9800,color:#fff
    style D fill:#9C27B0,color:#fff
    style ESP fill:#607D8B,color:#fff
    style HW fill:#455A64,color:#fff
```

---

## Regras de Dependência

### Dependências Permitidas (✅) e Proibidas (❌)

| Camada | Pode depender de | NÃO pode depender de |
|--------|-----------------|---------------------|
| **modules/** | `hal/`, `utils/`, `config/` | `drivers/` |
| **drivers/** | `hal/`, ESP-IDF APIs | `modules/`, `utils/`, outros `drivers/` |
| **hal/** | tipos C stdlib, ESP-IDF tipos | `drivers/`, `modules/` |
| **utils/** | tipos C stdlib | `hal/`, `drivers/`, `modules/` |
| **config/** | tipos C stdlib | tudo (contém apenas dados/constantes) |

### Diagrama de Dependências Permitidas vs Proibidas

```mermaid
graph LR
    subgraph "Dependências PERMITIDAS"
        direction LR
        M1[modules] -->|✅| H1[hal]
        M1 -->|✅| U1[utils]
        M1 -->|✅| C1[config]
        D1[drivers] -->|✅| H1
        D1 -->|✅| E1[ESP-IDF]
        H1 -->|✅| E1
    end

    style M1 fill:#4CAF50,color:#fff
    style H1 fill:#FF9800,color:#fff
    style U1 fill:#2196F3,color:#fff
    style C1 fill:#2196F3,color:#fff
    style D1 fill:#9C27B0,color:#fff
    style E1 fill:#607D8B,color:#fff
```

```mermaid
graph LR
    subgraph "Dependências PROIBIDAS"
        direction LR
        M2[modules] -.->|❌| D2[drivers]
        D2 -.->|❌| M2
        D2 -.->|❌| U2[utils]
        D2 -.->|❌| D3[outro driver]
        H2[hal] -.->|❌| D2
        H2 -.->|❌| M2
        U2 -.->|❌| H2
        U2 -.->|❌| M2
    end

    style M2 fill:#f44336,color:#fff
    style D2 fill:#f44336,color:#fff
    style D3 fill:#f44336,color:#fff
    style H2 fill:#f44336,color:#fff
    style U2 fill:#f44336,color:#fff
```

---

## Fluxo de Dados do Firmware

O loop principal do firmware segue um ciclo contínuo de sensoriamento, processamento e feedback:

```mermaid
flowchart TD
    subgraph "1. Sensoriamento"
        VL[VL53L0X<br/>Sensor ToF] -->|I2C| READ[Leitura de distância<br/>em mm]
        BAT[Bateria<br/>ADC] -->|ADC| VBAT[Leitura de tensão<br/>em mV]
        BLE_RX[BLE<br/>Recepção] -->|UART/BLE| CMD[Comandos do App]
    end

    subgraph "2. Processamento"
        READ --> CALC[Calcula duty-cycle<br/>proporcional à distância]
        VBAT --> CHECK[Verifica nível<br/>< 20% → alerta]
        CMD --> PARSE[Interpreta comando<br/>linha de ônibus / config]
    end

    subgraph "3. Feedback"
        CALC --> MOTOR[Motor ERM<br/>Vibração PWM]
        CHECK --> AUDIO[DFPlayer<br/>Alerta sonoro]
        PARSE --> AUDIO
        PARSE --> MOTOR
    end

    subgraph "4. Economia de Energia"
        MOTOR --> SLEEP[Light Sleep<br/>entre ciclos]
        AUDIO --> SLEEP
        SLEEP --> VL
    end

    style VL fill:#4CAF50,color:#fff
    style BAT fill:#4CAF50,color:#fff
    style BLE_RX fill:#4CAF50,color:#fff
    style CALC fill:#FF9800,color:#fff
    style CHECK fill:#FF9800,color:#fff
    style PARSE fill:#FF9800,color:#fff
    style MOTOR fill:#9C27B0,color:#fff
    style AUDIO fill:#9C27B0,color:#fff
    style SLEEP fill:#607D8B,color:#fff
```

---

## Fluxo de Build

O sistema de build suporta dois targets: ESP32-C3 (hardware real) e Host (testes em x86/x64).

```mermaid
flowchart LR
    A[idf.py build] --> B{Target?}
    B -->|esp32c3| C[Compila com<br/>toolchain RISC-V<br/>riscv32-esp-elf-gcc]
    B -->|linux / host| D[Compila com<br/>GCC nativo<br/>x86/x64]
    C --> E[Linka implementação<br/>HAL real<br/>hal/src/]
    D --> F[Linka Mock Layer<br/>hal/mock/src/]
    E --> G[Binário .bin<br/>flashável para ESP32-C3]
    F --> H[Executável de teste<br/>host]

    style A fill:#2196F3,color:#fff
    style B fill:#FF9800,color:#fff
    style C fill:#4CAF50,color:#fff
    style D fill:#4CAF50,color:#fff
    style E fill:#9C27B0,color:#fff
    style F fill:#9C27B0,color:#fff
    style G fill:#4CAF50,color:#fff
    style H fill:#4CAF50,color:#fff
```

### Seleção de Implementação HAL

A flag de compilação `SOLITON_USE_MOCK` controla qual implementação da HAL é linkada:

| Flag | Implementação | Target | Uso |
|------|--------------|--------|-----|
| `SOLITON_USE_MOCK=0` (padrão) | `hal/src/` (real) | ESP32-C3 | Firmware de produção |
| `SOLITON_USE_MOCK=1` | `hal/mock/src/` | Host (x86/x64) | Testes unitários |

---

## Fluxo de Testes

```mermaid
flowchart TD
    T[idf.py -T test] --> TC[Compila testes<br/>para host x86/x64]
    TC --> ML[Linka Mock Layer<br/>hal/mock/]
    ML --> UNITY[Executa testes Unity<br/>+ PBT Runner]
    UNITY --> R{Resultado}
    R -->|Todos passam| P[Exit code 0<br/>✅ Build aprovado]
    R -->|Algum falha| F[Exit code ≠ 0<br/>❌ Relatório de falha:<br/>arquivo, linha, esperado vs obtido]

    style T fill:#2196F3,color:#fff
    style TC fill:#FF9800,color:#fff
    style ML fill:#9C27B0,color:#fff
    style UNITY fill:#4CAF50,color:#fff
    style P fill:#4CAF50,color:#fff
    style F fill:#f44336,color:#fff
```

### Tipos de Teste

| Tipo | Framework | Objetivo | Iterações |
|------|-----------|----------|-----------|
| **Unit Tests** | Unity + CMock | Exemplos específicos e edge cases | 1 por caso |
| **Property-Based Tests** | Unity + PBT Runner (C) | Propriedades universais com inputs aleatórios | 100 por propriedade |

---

## Estrutura de Diretórios

```
soliton/
├── CMakeLists.txt              # Root CMake (project())
├── sdkconfig.defaults          # Configuração base ESP32-C3
├── .clang-format               # Formatação automática
├── .clang-tidy                 # Análise estática
├── .github/workflows/ci.yml    # Pipeline CI
├── components/
│   ├── hal/                    # Camada de Abstração de Hardware
│   │   ├── include/            # Interfaces (hal_i2c.h, hal_pwm.h, etc.)
│   │   ├── src/                # Implementação real (ESP32-C3)
│   │   └── mock/              # Mock Layer para testes em host
│   ├── drivers/                # Drivers de periféricos
│   │   ├── vl53l0x/           # Sensor ToF (I2C)
│   │   ├── dfplayer/          # Módulo de áudio (UART)
│   │   ├── motor_erm/         # Motor de vibração (PWM)
│   │   └── battery/           # Leitura de bateria (ADC)
│   ├── modules/               # Lógica de aplicação (futuro)
│   ├── config/                # Constantes, pinout, thresholds
│   └── utils/                 # Utilitários compartilhados
├── main/                       # Ponto de entrada (app_main)
├── test/                       # Testes unitários e PBT
│   ├── hal/                   # Testes da HAL e Mock Layer
│   ├── drivers/               # Testes dos drivers
│   ├── modules/               # Testes dos módulos
│   ├── utils/                 # Testes de utilitários e PBT runner
│   └── build/                 # Testes de validação de camadas (CMake)
├── docs/                       # Documentação técnica
├── tools/                      # Scripts e templates
│   ├── setup.sh               # Setup do ambiente (Linux/macOS)
│   ├── setup.ps1              # Setup do ambiente (Windows)
│   ├── check_layers.cmake     # Validação de dependências entre camadas
│   └── templates/             # Templates para novos componentes
└── hardware/                   # Esquemáticos, PCB, Wokwi
    └── wokwi/diagram.json     # Configuração do emulador
```

---

## Aplicação das Regras de Camada (Build-Time)

As regras de dependência são aplicadas automaticamente em tempo de compilação pelo script CMake `tools/check_layers.cmake`. Qualquer violação interrompe o build com `FATAL_ERROR`:

```
LAYER VIOLATION: <componente_origem> (<camada>) cannot depend on <componente_destino> (<camada_proibida>)
```

Componentes sem `COMPONENT_LAYER` declarado no `CMakeLists.txt` também são rejeitados:

```
LAYER ERROR: <componente> does not have COMPONENT_LAYER defined
```
