# Sóliton — Módulo Adaptativo de Acessibilidade 360°

Ecossistema de hardware e software embarcado de baixo custo (~R$ 95,00) projetado para devolver autonomia, segurança e mobilidade urbana para pessoas cegas ou com baixa visão.

O dispositivo integra sensoriamento espacial laser, feedback háptico (vibração) e áudio por condução óssea, funcionando **sem exigir smartphones ou conexões com a internet**.

---

## Índice

- [Pré-requisitos](#pré-requisitos)
- [Quick Start](#quick-start)
- [Instalação Detalhada](#instalação-detalhada)
- [Comandos de Build](#comandos-de-build)
- [Execução de Testes](#execução-de-testes)
- [Estrutura do Projeto](#estrutura-do-projeto)
- [Regras de Dependência entre Camadas](#regras-de-dependência-entre-camadas)
- [Contribuindo](#contribuindo)
- [Pipeline CI](#pipeline-ci)

---

## Pré-requisitos

### Versões Mínimas de Ferramentas

| Ferramenta | Versão Mínima | Verificação |
|-----------|---------------|-------------|
| Python | ≥ 3.8 | `python --version` |
| Git | ≥ 2.30 | `git --version` |
| CMake | ≥ 3.16 | `cmake --version` |
| ESP-IDF | ≥ 5.1.0 | `idf.py --version` |

### Linux (Ubuntu ≥ 22.04)

| Requisito | Como instalar |
|-----------|---------------|
| Python 3.8+ | `sudo apt install python3 python3-pip python3-venv` |
| Git 2.30+ | `sudo apt install git` |
| CMake 3.16+ | `sudo apt install cmake` |
| Dependências de build | `sudo apt install gcc git wget flex bison gperf ninja-build ccache libffi-dev libssl-dev dfu-util libusb-1.0-0` |

### Windows (≥ 10)

| Requisito | Como instalar |
|-----------|---------------|
| Python 3.8+ | [python.org/downloads](https://www.python.org/downloads/) — marcar "Add to PATH" |
| Git 2.30+ | [git-scm.com](https://git-scm.com/download/win) |
| CMake 3.16+ | Incluído no instalador do ESP-IDF |
| ESP-IDF Tools Installer | [dl.espressif.com/dl/esp-idf](https://dl.espressif.com/dl/esp-idf/) |

---

## Quick Start

```bash
# 1. Clonar o repositório
git clone https://github.com/seu-usuario/soliton.git && cd soliton

# 2. Configurar o ambiente (instala ESP-IDF e toolchain)
./tools/setup.sh        # Linux/macOS
# .\tools\setup.ps1    # Windows (PowerShell)

# 3. Compilar o firmware
idf.py build
```

---

## Instalação Detalhada

### Linux (Ubuntu ≥ 22.04)

1. Instalar dependências do sistema:
   ```bash
   sudo apt update
   sudo apt install -y git wget flex bison gperf python3 python3-pip python3-venv \
       cmake ninja-build ccache libffi-dev libssl-dev dfu-util libusb-1.0-0
   ```

2. Clonar o repositório:
   ```bash
   git clone https://github.com/seu-usuario/soliton.git
   cd soliton
   ```

3. Executar o script de setup (instala ESP-IDF v5.x e toolchain RISC-V):
   ```bash
   chmod +x tools/setup.sh
   ./tools/setup.sh
   ```

4. Ativar o ambiente ESP-IDF (necessário em cada novo terminal):
   ```bash
   source ~/.espressif/esp-idf/export.sh
   ```

5. Verificar a instalação:
   ```bash
   idf.py --version
   ```

### Windows (≥ 10)

1. Instalar **Python ≥ 3.8** pelo instalador oficial. Marcar a opção "Add Python to PATH".

2. Instalar **Git ≥ 2.30** pelo instalador oficial (git-scm.com).

3. Clonar o repositório:
   ```powershell
   git clone https://github.com/seu-usuario/soliton.git
   cd soliton
   ```

4. Executar o script de setup no PowerShell (como Administrador):
   ```powershell
   .\tools\setup.ps1
   ```

5. Abrir o **ESP-IDF PowerShell** (atalho criado pelo instalador) e navegar até o projeto:
   ```powershell
   cd C:\caminho\para\soliton
   ```

6. Verificar a instalação:
   ```powershell
   idf.py --version
   ```

---

## Comandos de Build

| Comando | Descrição |
|---------|-----------|
| `idf.py build` | Compila o firmware para ESP32-C3 (target RISC-V) |
| `idf.py flash` | Grava o firmware no dispositivo conectado via USB |
| `idf.py flash monitor` | Grava e abre o monitor serial (Ctrl+] para sair) |
| `idf.py monitor` | Abre o monitor serial sem regravar |
| `idf.py clean` | Limpa artefatos de build |
| `idf.py fullclean` | Limpa tudo (incluindo configuração) |
| `idf.py set-target esp32c3` | Define o target (já configurado em sdkconfig.defaults) |

### Build com Warnings como Erros (CI mode)

```bash
idf.py build --ccflags="-Werror"
```

---

## Execução de Testes

### Testes em Host (sem hardware)

Os testes unitários e de propriedade rodam no host (x86/x64) usando a Mock Layer no lugar do hardware real.

```bash
# Compilar e executar testes via CMake (host)
cd test
cmake -B build -DCMAKE_C_COMPILER=gcc
cmake --build build
ctest --test-dir build --output-on-failure
```

### Testes via ESP-IDF

```bash
# Executar todos os testes
idf.py -T test

# Executar testes de um componente específico
idf.py -T test -t hal
idf.py -T test -t drivers
```

### Análise Estática

```bash
# Verificar formatação (dry-run)
clang-format --dry-run --Werror components/**/*.c components/**/*.h

# Aplicar formatação automaticamente
clang-format -i components/**/*.c components/**/*.h

# Análise estática com clang-tidy
clang-tidy components/**/*.c -- -I components/*/include
```

---

## Estrutura do Projeto

```
soliton/
├── CMakeLists.txt              # CMake raiz (project(soliton))
├── sdkconfig.defaults          # Configuração base ESP32-C3
├── README.md                   # Este arquivo
├── .clang-format               # Regras de formatação automática
├── .clang-tidy                 # Regras de análise estática
│
├── .github/
│   └── workflows/
│       └── ci.yml              # Pipeline CI (GitHub Actions)
│
├── components/
│   ├── README.md               # Documentação de camadas e regras
│   ├── hal/                    # Hardware Abstraction Layer
│   │   ├── include/            #   Interfaces públicas (hal_*.h)
│   │   ├── src/                #   Implementação real (ESP32-C3)
│   │   └── mock/              #   Mock Layer para testes em host
│   │       ├── include/        #     hal_mock_ctrl.h
│   │       └── src/            #     Implementações mock
│   ├── drivers/                # Drivers de periféricos
│   │   ├── vl53l0x/            #   Sensor ToF laser (I2C)
│   │   ├── dfplayer/           #   Módulo de áudio (UART)
│   │   ├── motor_erm/          #   Motor de vibração (PWM)
│   │   └── battery/            #   Monitoramento de bateria (ADC)
│   ├── modules/                # Lógica de aplicação (radar, áudio, etc.)
│   ├── config/                 # Configurações, pinout, thresholds
│   │   └── include/            #   soliton_pinout.h, constantes
│   └── utils/                  # Utilitários (math, ring buffers)
│
├── main/
│   ├── CMakeLists.txt
│   └── main.c                  # Ponto de entrada (app_main)
│
├── test/
│   ├── CMakeLists.txt          # Build de testes para host
│   ├── hal/                    # Testes da HAL e Mock Layer
│   ├── drivers/                # Testes dos drivers
│   ├── modules/                # Testes dos módulos
│   ├── utils/                  # PBT runner e utilitários de teste
│   └── build/                  # Testes de validação de camadas (CMake)
│
├── tools/
│   ├── setup.sh                # Script de setup (Linux/macOS)
│   ├── setup.ps1               # Script de setup (Windows)
│   ├── check_layers.cmake      # Validação de dependências entre camadas
│   └── templates/              # Templates para novos componentes
│
├── docs/                       # Documentação técnica
│
├── hardware/
│   └── wokwi/
│       └── diagram.json        # Configuração do emulador Wokwi
│
└── vendor/                     # Dependências externas (Unity, etc.)
```

---

## Regras de Dependência entre Camadas

O projeto segue regras estritas de dependência para manter baixo acoplamento:

| Camada | Pode depender de | Não pode depender de |
|--------|------------------|---------------------|
| `modules/` | `hal/`, `utils/`, `config/` | `drivers/` |
| `drivers/` | `hal/`, ESP-IDF APIs | `modules/`, `utils/`, outros drivers |
| `hal/` | C stdlib, ESP-IDF tipos | `drivers/`, `modules/` |
| `utils/` | C stdlib | `hal/`, `drivers/`, `modules/` |
| `config/` | nada (só dados) | tudo |

As regras são validadas em tempo de build pelo script `tools/check_layers.cmake`. Violações geram `FATAL_ERROR` no CMake com identificação do componente e da regra violada.

Cada componente declara sua camada no `CMakeLists.txt`:
```cmake
set(COMPONENT_LAYER "drivers")  # ou "hal", "modules", "utils", "config"
```

---

## Contribuindo

### Convenções de Código

- **Variáveis e funções:** `snake_case`
- **Constantes:** `UPPER_SNAKE_CASE`
- **Funções públicas:** prefixo de módulo (ex: `radar_read_distance()`, `audio_play_track()`, `power_read_battery()`)
- **Include guards:** formato `SOLITON_<MODULO>_<ARQUIVO>_H`
- **Comentários:** em português
- **Indentação:** 4 espaços (configurado no `.clang-format`)
- **Largura máxima de linha:** 100 caracteres
- **Estilo de chave:** K&R (abertura na mesma linha)

### Criando um Novo Componente

Use os templates disponíveis em `tools/templates/component/`:

1. Copiar o template para o diretório apropriado em `components/`
2. Renomear arquivos e substituir placeholders
3. Declarar a camada no `CMakeLists.txt` com `set(COMPONENT_LAYER "<camada>")`
4. Declarar dependências com `REQUIRES` ou `PRIV_REQUIRES`

### Workflow de Contribuição

1. Criar branch a partir de `main`
2. Implementar mudanças seguindo as convenções
3. Executar formatação: `clang-format -i <arquivos>`
4. Executar testes: `cd test && cmake -B build && cmake --build build && ctest --test-dir build`
5. Criar Pull Request — o CI validará automaticamente

---

## Pipeline CI

O pipeline de Integração Contínua é executado automaticamente em todo push e pull request para a branch `main` via GitHub Actions.

### Etapas (em ordem)

1. **Análise Estática** — Compilação com `-Werror`, verificação de formatação (`clang-format`), análise de código (`clang-tidy`)
2. **Build do Firmware** — `idf.py build` para o target ESP32-C3
3. **Testes Unitários** — `idf.py -T test` no target host

### Comportamento

- Se qualquer etapa falhar, as subsequentes são canceladas
- Pull Requests com falha no CI são bloqueados para merge
- Imagem Docker utilizada: `espressif/idf:v5.1`
- Cache de dependências ESP-IDF entre execuções para reduzir tempo de build
- Tempo máximo total: ≤ 10 minutos (build limpo)

---

## Licença

Open-source hardware e software. Consulte o arquivo `LICENSE` para detalhes.
