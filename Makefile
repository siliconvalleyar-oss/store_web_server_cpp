# ============================================================
#  Makefile - Tienda Electrónica C++
# ============================================================
#  Targets:
#    make all    → Compila el proyecto (por defecto)
#    make clean  → Elimina objetos y binario
#    make run    → Compila y ejecuta el servidor
#    make debug  → Compila con flags de depuración
#
#  Dependencias:
#    - nlohmann-json3-dev: sudo apt install nlohmann-json3-dev
#    - cpp-httplib: incluido en include/httplib.h
#    - g++ (C++17)
# ============================================================

# ---- Compilador y flags ----
CXX        := g++
CXXFLAGS   := -std=c++17 -Wall -Wextra -pedantic -O2 -pthread

# ---- Directorios ----
SRC_DIR    := src
INC_DIR    := include
OBJ_DIR    := obj
BIN_DIR    := bin

# ---- Archivos ----
SRCS       := $(wildcard $(SRC_DIR)/*.cpp)
OBJS       := $(patsubst $(SRC_DIR)/%.cpp,$(OBJ_DIR)/%.o,$(SRCS))
TARGET     := $(BIN_DIR)/server

# ---- Librerías externas ----
# nlohmann/json es header-only (en /usr/include/)
JSON_INC   := -I/usr/include

# cpp-httplib es header-only (en include/)
HTTPLIB_INC := -I$(INC_DIR)

# Flags totales de compilación
CXXFLAGS   += $(JSON_INC) $(HTTPLIB_INC)

# Flags de linkeo (pthread ya está incluido en CXXFLAGS)
LDFLAGS    := -lpthread

# ---- Colores (para output) ----
GREEN      := \033[92m
CYAN       := \033[96m
YELLOW     := \033[93m
RED        := \033[91m
RESET      := \033[0m

# ============================================================
#  TARGETS
# ============================================================

# ---- Target por defecto ----
all: $(TARGET)

# ---- Linkeo: genera el binario final ----
$(TARGET): $(OBJS) | $(BIN_DIR)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LDFLAGS)
	@echo ""
	@echo "$(GREEN)✅  Compilación exitosa: $@$(RESET)"

# ---- Compilación: genera los .o desde .cpp ----
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp | $(OBJ_DIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@
	@echo "$(CYAN)📦  Compilado: $< -> $@$(RESET)"

# ---- Crear directorios si no existen ----
$(BIN_DIR):
	mkdir -p $(BIN_DIR)

$(OBJ_DIR):
	mkdir -p $(OBJ_DIR)

# ---- Limpieza ----
.PHONY: clean
clean:
	rm -rf $(OBJ_DIR)/*.o $(TARGET)
	@echo "$(YELLOW)🧹  Limpieza completa: objetos y binario eliminados.$(RESET)"

# ---- Ejecución ----
# Pasa los argumentos extra (ej: make run ARGS="--port 9090")
.PHONY: run
run: all
	@echo ""
	@echo "$(GREEN)🚀  Iniciando servidor...$(RESET)"
	@echo "$(CYAN)🌐  Servidor en http://localhost:$(or $(firstword $(ARGS:--port=)),8080)$(RESET)"
	@echo "$(YELLOW)🔑  Usuarios: admin/admin | cliente/cliente$(RESET)"
	@echo "$(RED)⏹️   Presiona Ctrl+C para detener$(RESET)"
	@echo ""
	./$(TARGET) $(ARGS)

# ---- Depuración ----
.PHONY: debug
debug: CXXFLAGS := -std=c++17 -g -O0 -Wall -Wextra -pedantic -DDEBUG -pthread $(JSON_INC) $(HTTPLIB_INC)
debug: clean all
	@echo ""
	@echo "$(YELLOW)🐛  Modo depuración activado.$(RESET)"
	@echo "    Ejecuta: gdb ./$(TARGET)"
