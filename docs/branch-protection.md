# Configuração de Branch Protection — Sóliton CI

## Objetivo

Bloquear merge de pull requests na branch `main` quando o pipeline CI falha,
garantindo que apenas código validado (build, testes, análise estática) seja
integrado à branch principal.

## Regras de Proteção para a Branch `main`

### Configuração no GitHub

Acessar: **Settings → Branches → Add branch protection rule**

| Configuração | Valor |
|---|---|
| Branch name pattern | `main` |
| Require a pull request before merging | ✅ Habilitado |
| Require status checks to pass before merging | ✅ Habilitado |
| Status checks that are required | `build-and-test` |
| Require branches to be up to date before merging | ✅ Habilitado |
| Do not allow bypassing the above settings | ✅ Habilitado (opcional) |

### Passos para Configurar

1. No repositório GitHub, vá em **Settings** → **Branches**
2. Clique em **Add rule** (ou edite a regra existente para `main`)
3. Em **Branch name pattern**, digite: `main`
4. Marque **Require a pull request before merging**
5. Marque **Require status checks to pass before merging**
6. Na busca de status checks, selecione: **build-and-test**
   - Este é o nome do job definido em `.github/workflows/ci.yml`
   - O check só aparece após a primeira execução bem-sucedida do workflow
7. Marque **Require branches to be up to date before merging**
8. (Opcional) Marque **Do not allow bypassing the above settings** para aplicar a regra inclusive para admins
9. Clique em **Create** ou **Save changes**

## Comportamento Esperado

### Quando o CI Passa ✅

- O pull request mostra check verde "build-and-test — All checks have passed"
- O botão **Merge** fica habilitado
- O merge pode ser realizado normalmente

### Quando o CI Falha ❌

- O pull request mostra check vermelho "build-and-test — Some checks were not successful"
- O botão **Merge** fica **desabilitado** (bloqueado)
- O desenvolvedor deve corrigir os erros e fazer novo push
- O CI executa novamente automaticamente no novo push
- O merge só é desbloqueado quando todas as etapas do CI passam

## Etapas do CI que Bloqueiam o Merge

O job `build-and-test` executa as seguintes etapas em ordem. Se qualquer etapa falhar,
as subsequentes são interrompidas e o merge é bloqueado:

1. **Static Analysis - Format Check**: Verifica formatação com `clang-format --Werror`
2. **Static Analysis - Clang-Tidy**: Compila com `-Werror` e executa análise estática
3. **Build Firmware**: Compila o firmware completo para ESP32-C3
4. **Run Unit Tests**: Executa todos os testes unitários em host

## Notas

- O status check `build-and-test` só fica disponível para seleção após a primeira
  execução do workflow (faça um push inicial para `main` ou crie um PR de teste)
- A imagem Docker `espressif/idf:v5.1` garante ambiente reproduzível
- O cache de ferramentas ESP-IDF reduz o tempo de execução entre builds
- Tempo máximo esperado: < 10 minutos para build limpo (sem cache)
