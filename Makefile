# Define os nomes dos arquivos
PROGRAM := rom
TARGET := $(PROGRAM).out
ROM := $(PROGRAM).bin

# Define o compilador
GCC := m68k-elf-gcc
LD := m68k-elf-ld
OBJCOPY := m68k-elf-objcopy
SIZEBND := java -jar /sgdk/bin/bin/sizebnd.jar

# Diretórios
INC_DIR := inc
SRC_DIR := src
RES_DIR := res
OUT_DIR := out

# Adiciona os diretórios de fontes ao VPATH
# VPATH := $(SRC_DIR):$(SRC_DIR)/boot

# Flags de compilação
CFLAGS := -DSGDK_GCC -m68000 -fdiagnostics-color=always -Wall -Wextra -Wno-shift-negative-value -Wno-main -Wno-unused-parameter -fno-builtin -ffunction-sections -fdata-sections -fms-extensions -I. -I$(INC_DIR) -I$(SRC_DIR) -I$(RES_DIR) -I/sgdk/inc -I/sgdk/res -B/sgdk/bin
# Flags de otimização
OPTIMIZATION_FLAGS := -O3 -fuse-linker-plugin -fno-web -fno-gcse -fomit-frame-pointer -flto -flto=auto -ffat-lto-objects
# Flags do linker
LDFLAGS := -m68000 -B/sgdk/bin -n -T /sgdk/md.ld -nostdlib -Wl,--gc-sections -flto -flto=auto -ffat-lto-objects
# Bibliotecas
LIBS := /sgdk/lib/libmd.a -lgcc

# Define os arquivos de código
C_SRCS := $(wildcard $(SRC_DIR)/**/*.c) $(wildcard $(SRC_DIR)/*.c)
S_SRCS := $(wildcard $(SRC_DIR)/**/*.s) $(wildcard $(SRC_DIR)/*.s)

# Define os arquivos objeto
OBJS := $(patsubst $(SRC_DIR)/%.c,$(OUT_DIR)/%.o,$(filter %.c,$(C_SRCS)))
OBJS += $(patsubst $(SRC_DIR)/%.s,$(OUT_DIR)/%.o,$(filter %.s,$(S_SRCS)))

# Alvos
.PHONY: all clean debug release

all: $(ROM)

release: all

debug: CFLAGS += -ggdb -DDEBUG=1
debug: all

$(ROM): $(TARGET)
	@echo "Creating final ROM image..."
	@mkdir -p $(OUT_DIR)
	@$(OBJCOPY) -O binary $(TARGET) $(ROM)
	@$(SIZEBND) $(ROM) -sizealign 131072 -checksum
	@echo "ROM built successfully!"

$(TARGET): $(OBJS)
	@echo "Linking object files..."
	@mkdir -p $(OUT_DIR)
	@$(GCC) $(LDFLAGS) $(LIBS) $(OBJS) -o $(TARGET)
	@m68k-elf-nm -n -l $(TARGET) > $(OUT_DIR)/symbol.txt
	@echo "Linking complete!"

$(OUT_DIR)/%.o: $(SRC_DIR)/%.c
	@echo "Compiling $<..."
	@mkdir -p $(@D)
	@$(GCC) $(CFLAGS) $(OPTIMIZATION_FLAGS) -MMD -c $< -o $@

$(OUT_DIR)/%.o: $(SRC_DIR)/%.s
	@echo "Assembling $<..."
	@mkdir -p $(@D)
	@$(GCC) $(CFLAGS) $(OPTIMIZATION_FLAGS) -x assembler-with-cpp -Wa,--register-prefix-optional,--bitwise-or -c $< -o $@

clean:
	@echo "Cleaning up..."
	@rm -rf $(OUT_DIR)
	@echo "Clean complete!"
