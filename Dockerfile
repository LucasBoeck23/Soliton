# =============================================================================
# Soliton - Dockerfile para build e testes
# =============================================================================
# Compila o firmware ESP32-C3 e roda testes unitarios/PBT.
#
# Uso:
#   docker build -t soliton .
#   docker run --rm soliton              # build firmware + testes
#   docker run --rm soliton test         # so testes (rapido)
#   docker run --rm soliton build        # so build firmware
#
# Para extrair o binario compilado:
#   docker run --rm -v ${PWD}/output:/output soliton build-copy
# =============================================================================

FROM espressif/idf:v5.1

# Copiar projeto para dentro do container
COPY . /project
WORKDIR /project

# Instalar GCC nativo para testes em host
RUN apt-get update && apt-get install -y gcc && rm -rf /var/lib/apt/lists/*

# Script de entrada que decide o que fazer
RUN echo '#!/bin/bash\n\
set -e\n\
\n\
case "${1:-all}" in\n\
  test)\n\
    echo "=== Rodando testes em host ==="\n\
    cd /project/test\n\
    cmake -B build -DCMAKE_C_COMPILER=gcc\n\
    cmake --build build\n\
    ctest --test-dir build --output-on-failure\n\
    echo ""\n\
    echo "=== Todos os testes passaram! ==="\n\
    ;;\n\
  build)\n\
    echo "=== Compilando firmware ESP32-C3 ==="\n\
    cd /project\n\
    idf.py set-target esp32c3\n\
    idf.py build\n\
    echo ""\n\
    echo "=== Build concluido! ==="\n\
    ls -lh /project/build/*.bin 2>/dev/null || echo "Binario em build/"\n\
    ;;\n\
  build-copy)\n\
    echo "=== Compilando e copiando binario ==="\n\
    cd /project\n\
    idf.py set-target esp32c3\n\
    idf.py build\n\
    cp /project/build/*.bin /output/ 2>/dev/null || true\n\
    cp /project/build/*.elf /output/ 2>/dev/null || true\n\
    echo "Binarios copiados para /output/"\n\
    ;;\n\
  all)\n\
    echo "=== Rodando testes + build ==="\n\
    echo ""\n\
    echo "--- TESTES ---"\n\
    cd /project/test\n\
    cmake -B build -DCMAKE_C_COMPILER=gcc\n\
    cmake --build build\n\
    ctest --test-dir build --output-on-failure\n\
    echo ""\n\
    echo "--- BUILD FIRMWARE ---"\n\
    cd /project\n\
    idf.py set-target esp32c3\n\
    idf.py build\n\
    echo ""\n\
    echo "=== Tudo pronto! Testes OK + Firmware compilado ==="\n\
    ;;\n\
  *)\n\
    echo "Uso: docker run soliton [test|build|build-copy|all]"\n\
    exit 1\n\
    ;;\n\
esac' > /entrypoint.sh && chmod +x /entrypoint.sh

ENTRYPOINT ["/entrypoint.sh"]
CMD ["all"]
