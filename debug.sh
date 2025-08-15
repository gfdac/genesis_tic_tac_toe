#!/bin/bash
# Ignora o argumento que o VS Code passa
shift

# Usa o docker exec para rodar o gdb dentro do contêiner
docker exec -i sgdk-debug /usr/bin/gdb-multiarch --interpreter=mi

