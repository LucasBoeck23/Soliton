# Requirements Document

## Introduction

Este documento define os requisitos para a **base/fundação** do projeto Sóliton — ou seja, toda a infraestrutura de desenvolvimento, arquitetura de código, emulação de hardware e manutenção que deve existir ANTES da implementação dos módulos funcionais (Radar, Áudio, Energia, BLE). O objetivo é garantir que o projeto tenha uma estrutura sólida, testável e reproduzível desde o início.

## Glossary

- **Build_System**: Sistema de compilação baseado em CMake integrado ao ESP-IDF que gera os binários do firmware
- **HAL**: Hardware Abstraction Layer — camada de abstração que isola o código de aplicação do hardware físico, expondo interfaces genéricas para periféricos
- **Driver**: Componente de software que controla diretamente um periférico de hardware específico (VL53L0X, DFPlayer, motor ERM, ADC)
- **Mock_Layer**: Implementação simulada de um driver ou HAL que substitui o hardware real para fins de teste em ambiente host
- **Emulador**: Ambiente de simulação (Wokwi ou QEMU) que executa o firmware ESP32-C3 sem hardware físico
- **ESP_IDF**: Framework oficial da Espressif para desenvolvimento no ESP32-C3, incluindo toolchain, bibliotecas e ferramentas de build
- **Toolchain**: Conjunto de compilador (riscv32-esp-elf-gcc), linker e ferramentas necessárias para gerar binários para ESP32-C3
- **CI_Pipeline**: Pipeline de Integração Contínua que executa build, testes e análise estática automaticamente a cada commit
- **Unity_Framework**: Framework de testes unitários em C utilizado para validação de componentes isolados
- **CMock**: Ferramenta de geração automática de mocks a partir de headers C, complementar ao Unity
- **Componente_ESP_IDF**: Unidade modular do ESP-IDF com seu próprio CMakeLists.txt, código-fonte e dependências declaradas
- **Target_Hardware**: Plataforma ESP32-C3 onde o firmware será executado em produção
- **Target_Host**: Máquina de desenvolvimento (x86/x64) onde testes unitários e de integração são executados sem hardware embarcado

## Requirements

### Requisito 1: Estrutura do Projeto ESP-IDF

**User Story:** Como desenvolvedor, eu quero uma estrutura de projeto ESP-IDF configurada e funcional, para que eu possa compilar e flashear firmware no ESP32-C3 desde o primeiro dia.

#### Critérios de Aceitação

1. THE Build_System SHALL gerar um arquivo binário flashável (.bin) para ESP32-C3 com tamanho maior que 0 bytes no diretório de saída do build, a partir do código-fonte do projeto utilizando ESP-IDF v5.x e CMake
2. WHEN o comando `idf.py build` é executado na raiz do projeto, THE Build_System SHALL compilar todos os componentes com exit code 0, zero erros de compilação, e gerar o binário final em menos de 120 segundos em uma máquina com 4 núcleos
3. THE Build_System SHALL organizar o código-fonte em no mínimo 4 Componentes_ESP_IDF separados: drivers (hardware), modules (lógica de aplicação), config (configurações e thresholds) e utils (utilitários), cada um com seu próprio diretório e arquivo CMakeLists.txt contendo idf_component_register()
4. WHEN um novo componente é adicionado ao projeto com dependências declaradas no campo REQUIRES ou PRIV_REQUIRES do idf_component_register() em seu CMakeLists.txt, THE Build_System SHALL compilar o projeto com sucesso (exit code 0) resolvendo a ordem de compilação e inclusão de headers entre componentes automaticamente
5. THE Build_System SHALL manter um arquivo `sdkconfig.defaults` versionado no repositório contendo no mínimo: o target do chip (CONFIG_IDF_TARGET="esp32c3"), tamanho da flash, e frequência de clock do cristal
6. IF o comando `idf.py build` falha com erros de compilação, THEN THE Build_System SHALL retornar um exit code diferente de 0 e exibir mensagens indicando o arquivo e linha onde o erro ocorreu

---

### Requisito 2: Ambiente de Desenvolvimento Reproduzível

**User Story:** Como desenvolvedor, eu quero um ambiente de desenvolvimento documentado e reproduzível, para que qualquer colaborador consiga compilar o projeto sem configuração manual extensa.

#### Critérios de Aceitação

1. THE ESP_IDF SHALL ser configurado na versão ≥ 5.1.0 (major 5) com Toolchain riscv32-esp-elf para o target ESP32-C3
2. THE Build_System SHALL fornecer um arquivo de configuração de ambiente (script de setup ou devcontainer) que instala todas as dependências necessárias para que o comando `idf.py build` complete com sucesso sem instalações manuais adicionais
3. WHEN um desenvolvedor clona o repositório e executa o script de setup, THE Build_System SHALL deixar o ambiente pronto para compilação em no máximo 3 comandos, sendo que a verificação de sucesso é a execução de `idf.py build` completar sem erros
4. THE Build_System SHALL documentar no README os pré-requisitos para pelo menos 2 dos seguintes sistemas operacionais: Linux (Ubuntu ≥ 22.04), Windows (≥ 10) ou macOS (≥ 12), incluindo versões mínimas de ferramentas (Python, Git, CMake) e passos de instalação numerados
5. IF o script de setup falhar durante a execução, THEN THE Build_System SHALL exibir uma mensagem de erro indicando qual dependência ou etapa falhou e encerrar com código de saída diferente de zero

---

### Requisito 3: Hardware Abstraction Layer (HAL)

**User Story:** Como desenvolvedor, eu quero uma camada de abstração de hardware, para que o código dos módulos (radar, áudio, energia) seja independente do hardware físico e testável em isolamento.

#### Critérios de Aceitação

1. THE HAL SHALL expor interfaces em C (headers com protótipos de função) para cada periférico: I2C, PWM, UART, ADC e GPIO, onde cada interface define no mínimo as operações de inicialização, leitura ou escrita, e de-inicialização
2. THE HAL SHALL ter exatamente duas implementações para cada interface: uma para o Target_Hardware (ESP32-C3 real) e uma Mock_Layer para o Target_Host, onde a Mock_Layer registra cada chamada recebida (nome da função e parâmetros) em um histórico consultável por código de teste
3. WHEN o código de um módulo utiliza a HAL, THE Build_System SHALL selecionar a implementação correta (hardware ou mock) via flag de compilação sem alteração no código do módulo
4. THE HAL SHALL definir um tipo de retorno padronizado para todas as operações de periférico contendo: um código de sucesso (valor zero), códigos de erro por categoria (timeout, parâmetro inválido, periférico não inicializado, falha de comunicação), e cada código SHALL ser um valor inteiro distinto e documentado no header
5. WHEN uma operação de periférico falha, THE HAL SHALL retornar o código de erro correspondente à categoria da falha, preservar o estado dos demais periféricos, e retornar o controle ao chamador em no máximo 50 ms
6. IF um módulo invoca uma função da HAL para um periférico que não foi inicializado, THEN THE HAL SHALL retornar o código de erro de periférico não inicializado sem acessar o hardware

---

### Requisito 4: Camada de Drivers

**User Story:** Como desenvolvedor, eu quero drivers isolados para cada componente de hardware, para que cada driver possa ser desenvolvido, testado e substituído independentemente.

#### Critérios de Aceitação

1. THE Driver SHALL ser implementado como um Componente_ESP_IDF independente para cada periférico: VL53L0X (I2C), DFPlayer Mini (UART), Motor ERM (PWM) e Bateria (ADC)
2. THE Driver SHALL depender exclusivamente da interface HAL, sem acessar registradores de hardware diretamente
3. THE Driver SHALL expor um header público com funções seguindo o padrão `<prefixo>_init()`, `<prefixo>_deinit()` e no mínimo as seguintes operações: `vl53l0x_read_distance_mm()`, `dfplayer_play_track()`, `motor_erm_set_duty()`, `battery_read_voltage_mv()`
4. WHEN o Driver é compilado para Target_Host, THE Build_System SHALL linkar a Mock_Layer correspondente da HAL automaticamente
5. WHEN `<prefixo>_init()` é chamado com sucesso, THE Driver SHALL retornar código de sucesso da HAL e aceitar uma struct de configuração específica do periférico (endereço I2C, canal UART, canal PWM ou canal ADC)
6. IF `<prefixo>_init()` falha por indisponibilidade do periférico, THEN THE Driver SHALL retornar o código de erro correspondente da HAL sem modificar estado global
7. THE Driver SHALL incluir um stub mínimo funcional onde cada função de operação retorna um valor fixo válido dentro da faixa operacional do periférico (distância: 500mm para VL53L0X, tensão: 3700mV para Bateria) e código de sucesso

---

### Requisito 5: Emulação de Hardware para Testes

**User Story:** Como desenvolvedor, eu quero emular o ESP32-C3 e seus periféricos, para que eu possa testar o firmware sem depender de hardware físico.

#### Critérios de Aceitação

1. THE Emulador SHALL executar o binário do firmware ESP32-C3 utilizando Wokwi ou QEMU com suporte a periféricos I2C, UART e GPIO
2. WHEN o firmware é executado no Emulador, THE Emulador SHALL fornecer um arquivo de configuração (diagram.json para Wokwi ou device tree para QEMU) que mapeia os periféricos do projeto
3. THE Mock_Layer SHALL simular o sensor VL53L0X retornando valores de distância configuráveis para testes (faixa de 0 a 2000mm)
4. THE Mock_Layer SHALL simular o DFPlayer Mini aceitando comandos UART e registrando as faixas de áudio requisitadas sem reprodução real
5. THE Mock_Layer SHALL simular o motor ERM registrando valores de duty-cycle PWM aplicados (faixa de 0 a 100%) para validação em testes
6. THE Mock_Layer SHALL simular leitura ADC da bateria retornando valores de tensão configuráveis (faixa de 3.0V a 4.2V)
7. WHEN um teste unitário utiliza a Mock_Layer, THE Mock_Layer SHALL permitir injeção de valores de retorno e verificação de chamadas (número de invocações e parâmetros passados) via interface programática
8. THE Mock_Layer SHALL permitir simulação de erros de comunicação (timeout, NACK) para validação do comportamento dos drivers em condições de falha
9. WHEN a Mock_Layer é inicializada sem configuração explícita, THE Mock_Layer SHALL retornar valores padrão seguros (distância: 2000mm, tensão: 3700mV, duty-cycle: 0%) e código de sucesso

---

### Requisito 6: Framework de Testes

**User Story:** Como desenvolvedor, eu quero um framework de testes unitários integrado ao projeto, para que eu possa validar cada componente isoladamente durante o desenvolvimento.

#### Critérios de Aceitação

1. THE Unity_Framework SHALL ser integrado ao projeto como Componente_ESP_IDF para execução de testes unitários em C
2. THE CMock SHALL ser integrado ao projeto para geração automática de mocks a partir dos headers da HAL e dos Drivers durante a etapa de build dos testes
3. WHEN o comando `idf.py -T test` é executado na raiz do projeto, THE Build_System SHALL compilar e executar todos os testes unitários no Target_Host e reportar resultados com contagem de testes passados, falhados e ignorados
4. THE Build_System SHALL suportar execução de testes individuais por componente via argumento de linha de comando no formato `idf.py -T test -t <nome_do_componente>`
5. WHEN um teste falha, THE Unity_Framework SHALL reportar o nome do teste, arquivo, linha e valores esperado versus obtido
6. THE Build_System SHALL manter os testes em diretório `/test` na raiz do projeto com subdiretórios espelhando a estrutura de `components/` (hal/, drivers/, modules/, utils/)
7. IF a compilação dos testes falha, THEN THE Build_System SHALL retornar código de saída diferente de zero e reportar o erro de compilação com arquivo e linha
8. WHEN todos os testes são executados com sucesso, THE Build_System SHALL retornar código de saída zero; WHEN pelo menos um teste falha, THE Build_System SHALL retornar código de saída diferente de zero

---

### Requisito 7: Convenções de Código e Análise Estática

**User Story:** Como desenvolvedor, eu quero convenções de código definidas e verificáveis automaticamente, para que o código mantenha consistência e qualidade ao longo do desenvolvimento.

#### Critérios de Aceitação

1. THE Build_System SHALL aplicar as seguintes convenções de nomenclatura: snake_case para variáveis e funções, UPPER_SNAKE_CASE para constantes, prefixo de módulo em funções públicas (ex: `radar_`, `audio_`, `power_`, `ble_`), verificáveis via regras configuradas na ferramenta de análise estática
2. THE Build_System SHALL incluir um arquivo de configuração para análise estática (clang-tidy ou cppcheck) que verifica conformidade com as convenções de nomenclatura e boas práticas de C/C++
3. WHEN o comando de análise estática é executado, THE Build_System SHALL reportar violações de convenção com arquivo, linha e descrição do problema, e retornar um código de saída diferente de zero se ao menos uma violação for encontrada
4. THE Build_System SHALL incluir um arquivo `.clang-format` que formata código automaticamente com indentação de 4 espaços, largura máxima de linha de 100 caracteres e abertura de chave na mesma linha (estilo Attach/K&R)
5. THE Build_System SHALL incluir include guards em todos os headers no formato `SOLITON_<MODULO>_<ARQUIVO>_H`
6. IF o desenvolvedor submete código com violações de formatação detectáveis pelo `.clang-format`, THEN THE Build_System SHALL reportar as diferenças de formatação e retornar código de saída diferente de zero

---

### Requisito 8: Pipeline de Integração Contínua (CI)

**User Story:** Como desenvolvedor, eu quero um pipeline de CI que execute build e testes automaticamente, para que erros sejam detectados antes de serem integrados à branch principal.

#### Critérios de Aceitação

1. WHEN um push ou pull request é criado para a branch principal, THE CI_Pipeline SHALL ser acionado automaticamente e iniciar a execução das etapas configuradas
2. WHEN o CI_Pipeline é acionado, THE CI_Pipeline SHALL executar as seguintes etapas em ordem: análise estática (compilação com warnings tratados como erros e verificação de formatação), build do firmware para o target ESP32-C3, execução de testes unitários no Target_Host
3. IF qualquer etapa do CI_Pipeline falha, THEN THE CI_Pipeline SHALL interromper as etapas subsequentes, marcar o commit com status de falha no GitHub, e reportar no pull request a etapa que falhou e a saída de erro correspondente
4. THE CI_Pipeline SHALL utilizar GitHub Actions com imagem Docker contendo ESP-IDF e Toolchain pré-instalados
5. THE CI_Pipeline SHALL completar todas as etapas em no máximo 10 minutos para um build limpo (definido como execução sem cache de artefatos de compilação anterior)
6. THE CI_Pipeline SHALL cachear dependências do ESP-IDF e Toolchain entre execuções para reduzir tempo de build
7. IF o CI_Pipeline reporta status de falha em um pull request, THEN THE CI_Pipeline SHALL bloquear o merge do pull request na branch principal até que todas as etapas sejam concluídas com sucesso

---

### Requisito 9: Estrutura de Diretórios do Projeto

**User Story:** Como desenvolvedor, eu quero uma estrutura de diretórios clara e padronizada, para que o código seja fácil de navegar e novos contribuidores encontrem rapidamente o que precisam.

#### Critérios de Aceitação

1. THE Build_System SHALL organizar o projeto na seguinte estrutura de diretórios de primeiro nível: `components/` (HAL, drivers, módulos), `main/` (ponto de entrada), `test/` (testes unitários), `docs/` (documentação), `tools/` (scripts de utilidade), `hardware/` (esquemáticos e PCB)
2. THE Build_System SHALL separar dentro de `components/` os subdiretórios: `hal/` (abstrações de hardware), `drivers/` (drivers de periféricos), `modules/` (lógica de aplicação), `utils/` (utilitários compartilhados)
3. WHEN um novo componente é criado, THE Build_System SHALL fornecer um template com CMakeLists.txt, header público e arquivo fonte inicial
4. THE Build_System SHALL manter um arquivo `components/README.md` documentando a responsabilidade de cada subdiretório e regras de dependência entre camadas

---

### Requisito 10: Separação de Camadas e Regras de Dependência

**User Story:** Como desenvolvedor, eu quero regras claras de dependência entre camadas, para que o código mantenha baixo acoplamento e alta coesão ao longo do desenvolvimento.

#### Critérios de Aceitação

1. THE Build_System SHALL impor que componentes da camada de aplicação (modules/) dependam apenas de interfaces da camada HAL (hal/), utilitários (utils/) e configurações (config/), sem dependência direta de componentes da camada de drivers (drivers/)
2. THE Build_System SHALL impor que componentes da camada de drivers (drivers/) dependam apenas de interfaces da camada HAL (hal/) e de APIs do ESP-IDF, sem dependência de componentes da camada de aplicação (modules/), utilitários (utils/) ou de outros drivers
3. THE Build_System SHALL impor que a camada HAL (hal/) declare apenas interfaces que dependam de tipos da biblioteca padrão C ou de tipos definidos pelo ESP-IDF, sem referência a implementações de drivers ou módulos de aplicação
4. IF um componente declara uma dependência que viola as regras de camada definidas nos critérios 1 a 3, THEN THE Build_System SHALL interromper a compilação e emitir mensagem de erro que identifique o componente de origem, o componente proibido referenciado e a regra de camada violada
5. THE Build_System SHALL documentar as regras de dependência entre camadas em um diagrama que identifique cada camada (drivers, HAL, modules, utils, config) e as direções de dependência permitidas
6. IF um novo componente é adicionado ao projeto sem declaração explícita de camada no sistema de build, THEN THE Build_System SHALL rejeitar a compilação com mensagem de erro indicando que o componente não possui camada atribuída
