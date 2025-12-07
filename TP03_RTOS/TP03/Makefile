# Makefile final, corrigido para usar caminho absoluto com sudo

PRJ_DIR := $(shell basename "$(CURDIR)")

BIN_DIR := .
INC_DIR := inc
SRC_DIR := src
OBJ_DIR := obj

EXE := $(BIN_DIR)/$(PRJ_DIR)_rt
SRC := $(wildcard $(SRC_DIR)/*.c)
OBJ := $(patsubst $(SRC_DIR)/%.c, $(OBJ_DIR)/%.o, $(SRC))

CC       := gcc
CPPFLAGS := -I. -I$(SRC_DIR) -I$(INC_DIR) -MMD -MP -D_GNU_SOURCE
CFLAGS   := -Wall -Wextra -O2 -std=gnu17 -g3
LDFLAGS  :=
LDLIBS   := -lm -pthread -lrt

.PHONY: all clean vars run run-with-load

all: $(EXE)

vars:
	@echo " "
	@echo "INC_DIR = $(INC_DIR)"
	@echo "SRC_DIR = $(SRC_DIR)"
	@echo "EXE = $(EXE)"
	@echo " "

$(EXE): $(OBJ) | $(BIN_DIR)
	@echo "Linkando o executável..."
	$(CC) $(LDFLAGS) $^ $(LDLIBS) -o $@
	@echo "Executável $(EXE) criado com sucesso."

$(BIN_DIR) $(OBJ_DIR):
	mkdir -p $@

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c | $(OBJ_DIR)
	@echo "Compilando $<..."
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@

clean:
	-@$(RM) -rv $(EXE) $(OBJ_DIR) output_*.txt

# ALTERAÇÃO: Usando caminho absoluto $(CURDIR) para o sudo
run: $(EXE)
	@echo "================================================="
	@echo "=== EXECUTANDO EM MODO TEMPO REAL (SEM CARGA) ==="
	@echo "================================================="
	@sudo $(CURDIR)/$<

# ALTERAÇÃO: Usando caminho absoluto $(CURDIR) para o sudo
run-with-load: $(EXE)
	@echo "=============================================================="
	@echo "=== EXECUTANDO EM MODO TEMPO REAL (COM CARGA NA CPU) ==="
	@echo "=============================================================="
	@command -v stress >/dev/null 2>&1 || { echo >&2 "Instalando 'stress'..."; sudo apt-get update && sudo apt-get install -y stress; }
	@echo "Iniciando carga na CPU em segundo plano..."
	@stress --cpu 4 &
	@STRESS_PID=$$!
	@echo "Iniciando a simulação (requer sudo)..."
	@sudo $(CURDIR)/$<
	@echo "Finalizando a carga da CPU (PID: $$STRESS_PID)..."
	@sudo kill $$STRESS_PID
	@echo "Teste com carga finalizado."

-include $(OBJ:.o=.d)