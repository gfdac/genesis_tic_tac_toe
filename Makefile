# Define os nomes dos arquivos
PROGRAM := rom
TARGET := $(PROGRAM).out
ROM := $(PROGRAM).bin

# Define o compilador
GCC := m68k-elf-gcc
LD := m68k-elf-ld
OBJCOPY := m68k-elf-objcopy

# Define o arquivo do linker
LINK_SCRIPT := link.ld

# Flags de compilação
# Caminhos corrigidos para o SGDK
CFLAGS := -Wall -fshort-enums -fno-builtin -m68000 -I./inc -I./res -I/sgdk/inc/
# Flags do linker com o caminho corrigido
LDFLAGS := -T$(LINK_SCRIPT) -nostdlib -Wl,--gc-sections -L/sgdk/lib/ -lsgdk -lgcc

# Define os arquivos de código
C_SRCS := src/main.c
S_SRCS := src/boot/sega.s

# Define os arquivos objeto
OBJS := $(C_SRCS:.c=.o) $(S_SRCS:.s=.o)

.PHONY: all clean

all: $(ROM)

$(ROM): $(TARGET)
	@$(OBJCOPY) -O binary $(TARGET) $(ROM)
	@echo "ROM built successfully!"

$(TARGET): $(OBJS) $(LINK_SCRIPT)
	@echo "Linking..."
	@$(GCC) $(LDFLAGS) $(OBJS) -o $(TARGET)
	@echo "Linking complete!"

%.o: %.c
	@echo "Compiling $<..."
	@$(GCC) $(CFLAGS) -c $< -o $@

%.o: %.s
	@echo "Assembling $<..."
	@$(GCC) $(CFLAGS) -c $< -o $@

clean:
	@echo "Cleaning up..."
	@rm -f $(OBJS) $(TARGET) $(ROM)
	@echo "Clean complete!"