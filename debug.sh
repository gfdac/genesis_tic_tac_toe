#!/bin/bash
# Ignora o argumento que o VS Code passa
shift

# Executa o gdb com todos os comandos de inicialização, na ordem correta
docker exec -i sgdk-debug /usr/bin/gdb-multiarch --interpreter=mi \
-ex "set architecture m68k" \
-ex "set targetArchitecture m68k" \
-ex "file /m68k/out/rom.out" \
-ex "target remote host.docker.internal:6868"
