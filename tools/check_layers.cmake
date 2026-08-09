# tools/check_layers.cmake — Validação de regras de dependência entre camadas
# Invocado durante o configure do CMake (include() no root CMakeLists.txt).
#
# Regras de dependência:
#   modules/ -> hal, utils, config        (proibido: drivers)
#   drivers/ -> hal                        (proibido: modules, utils, outros drivers)
#   hal/     -> stdlib, ESP-IDF types      (proibido: drivers, modules)
#   utils/   -> stdlib                     (proibido: hal, drivers, modules)
#   config/  -> stdlib                     (proibido: tudo, é só dados)
#
# Funções exportadas:
#   get_component_layer(<component_name> <out_var>)
#   check_layer_declaration(<component_name> <component_dir>)
#   enforce_layer_rules(<component_name> <component_layer> <requires_list>)

# ---------------------------------------------------------------------------
# get_component_layer(<component_name> <out_var>)
#
# Determina a camada de um componente baseado em seu caminho ou variável
# COMPONENT_LAYER previamente definida. Componentes desconhecidos (ESP-IDF
# internos) retornam "external" e não são validados.
# ---------------------------------------------------------------------------
function(get_component_layer component_name out_var)
    # Verificar se é um componente conhecido do projeto pelo caminho no diretório
    set(_known_layers "")

    # Mapear componentes conhecidos para suas camadas baseado na estrutura do projeto
    # hal
    if(component_name STREQUAL "hal")
        set(${out_var} "hal" PARENT_SCOPE)
        return()
    endif()

    # drivers — cada driver é registrado pelo nome do subdiretório
    foreach(_drv vl53l0x dfplayer motor_erm battery)
        if(component_name STREQUAL "${_drv}")
            set(${out_var} "drivers" PARENT_SCOPE)
            return()
        endif()
    endforeach()

    # modules
    if(component_name STREQUAL "modules")
        set(${out_var} "modules" PARENT_SCOPE)
        return()
    endif()

    # utils
    if(component_name STREQUAL "utils")
        set(${out_var} "utils" PARENT_SCOPE)
        return()
    endif()

    # config
    if(component_name STREQUAL "config")
        set(${out_var} "config" PARENT_SCOPE)
        return()
    endif()

    # Componentes externos (ESP-IDF, etc.) — não validados
    set(${out_var} "external" PARENT_SCOPE)
endfunction()

# ---------------------------------------------------------------------------
# check_layer_declaration(<component_name> <component_dir>)
#
# Verifica que um componente do projeto possui COMPONENT_LAYER declarado
# em seu CMakeLists.txt. Emite FATAL_ERROR se não tiver.
# Componentes externos (fora de components/) são ignorados.
# ---------------------------------------------------------------------------
function(check_layer_declaration component_name component_dir)
    # Só validar componentes dentro de components/ do projeto
    string(FIND "${component_dir}" "components/" _pos)
    if(_pos EQUAL -1)
        return()
    endif()

    # Ler o CMakeLists.txt do componente e verificar se declara COMPONENT_LAYER
    set(_cmakelists "${component_dir}/CMakeLists.txt")
    if(NOT EXISTS "${_cmakelists}")
        return()
    endif()

    file(READ "${_cmakelists}" _content)
    string(FIND "${_content}" "COMPONENT_LAYER" _layer_pos)
    if(_layer_pos EQUAL -1)
        message(FATAL_ERROR
            "LAYER VIOLATION: Component '${component_name}' at ${component_dir} "
            "does not declare COMPONENT_LAYER. Every project component must "
            "explicitly set(COMPONENT_LAYER \"<layer>\") in its CMakeLists.txt.")
    endif()
endfunction()

# ---------------------------------------------------------------------------
# enforce_layer_rules(<component_name> <component_layer> <requires_list>)
#
# Valida que as dependências declaradas (REQUIRES) de um componente respeitam
# as regras de camada. Emite FATAL_ERROR com detalhes da violação.
# ---------------------------------------------------------------------------
function(enforce_layer_rules component_name component_layer requires_list)
    if(component_layer STREQUAL "modules")
        # modules/ -> pode depender de: hal, utils, config
        # modules/ -> NÃO pode depender de: drivers
        foreach(dep IN LISTS requires_list)
            get_component_layer(${dep} dep_layer)
            if(dep_layer STREQUAL "external")
                continue()
            endif()
            if(dep_layer STREQUAL "drivers")
                message(FATAL_ERROR
                    "LAYER VIOLATION: ${component_name} (modules) cannot depend on "
                    "${dep} (drivers). Rule: modules/ must not depend on drivers/.")
            endif()
        endforeach()

    elseif(component_layer STREQUAL "drivers")
        # drivers/ -> pode depender de: hal
        # drivers/ -> NÃO pode depender de: modules, utils, outros drivers
        foreach(dep IN LISTS requires_list)
            get_component_layer(${dep} dep_layer)
            if(dep_layer STREQUAL "external")
                continue()
            endif()
            if(dep_layer STREQUAL "modules")
                message(FATAL_ERROR
                    "LAYER VIOLATION: ${component_name} (drivers) cannot depend on "
                    "${dep} (modules). Rule: drivers/ must not depend on modules/.")
            endif()
            if(dep_layer STREQUAL "utils")
                message(FATAL_ERROR
                    "LAYER VIOLATION: ${component_name} (drivers) cannot depend on "
                    "${dep} (utils). Rule: drivers/ must not depend on utils/.")
            endif()
            if(dep_layer STREQUAL "drivers" AND NOT dep STREQUAL component_name)
                message(FATAL_ERROR
                    "LAYER VIOLATION: ${component_name} (drivers) cannot depend on "
                    "${dep} (drivers). Rule: drivers/ must not depend on other drivers.")
            endif()
        endforeach()

    elseif(component_layer STREQUAL "hal")
        # hal/ -> pode depender de: stdlib, ESP-IDF types (external)
        # hal/ -> NÃO pode depender de: drivers, modules
        foreach(dep IN LISTS requires_list)
            get_component_layer(${dep} dep_layer)
            if(dep_layer STREQUAL "external")
                continue()
            endif()
            if(dep_layer STREQUAL "drivers")
                message(FATAL_ERROR
                    "LAYER VIOLATION: ${component_name} (hal) cannot depend on "
                    "${dep} (drivers). Rule: hal/ must not depend on drivers/.")
            endif()
            if(dep_layer STREQUAL "modules")
                message(FATAL_ERROR
                    "LAYER VIOLATION: ${component_name} (hal) cannot depend on "
                    "${dep} (modules). Rule: hal/ must not depend on modules/.")
            endif()
        endforeach()

    elseif(component_layer STREQUAL "utils")
        # utils/ -> pode depender de: stdlib apenas
        # utils/ -> NÃO pode depender de: hal, drivers, modules
        foreach(dep IN LISTS requires_list)
            get_component_layer(${dep} dep_layer)
            if(dep_layer STREQUAL "external")
                continue()
            endif()
            if(dep_layer STREQUAL "hal")
                message(FATAL_ERROR
                    "LAYER VIOLATION: ${component_name} (utils) cannot depend on "
                    "${dep} (hal). Rule: utils/ must only depend on stdlib.")
            endif()
            if(dep_layer STREQUAL "drivers")
                message(FATAL_ERROR
                    "LAYER VIOLATION: ${component_name} (utils) cannot depend on "
                    "${dep} (drivers). Rule: utils/ must only depend on stdlib.")
            endif()
            if(dep_layer STREQUAL "modules")
                message(FATAL_ERROR
                    "LAYER VIOLATION: ${component_name} (utils) cannot depend on "
                    "${dep} (modules). Rule: utils/ must only depend on stdlib.")
            endif()
        endforeach()

    elseif(component_layer STREQUAL "config")
        # config/ -> pode depender de: stdlib apenas (é só dados)
        # config/ -> NÃO pode depender de: nenhum componente do projeto
        foreach(dep IN LISTS requires_list)
            get_component_layer(${dep} dep_layer)
            if(dep_layer STREQUAL "external")
                continue()
            endif()
            message(FATAL_ERROR
                "LAYER VIOLATION: ${component_name} (config) cannot depend on "
                "${dep} (${dep_layer}). Rule: config/ must not depend on any project component.")
        endforeach()
    endif()
endfunction()
