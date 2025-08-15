# Inicia a partir da imagem base que você estava usando
FROM maciekbaron/sgdk

# Instala o gdb e outras ferramentas de depuração
USER root
RUN apt-get update && apt-get install -y gdb gdb-multiarch
USER m68k