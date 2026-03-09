# Makefile para el Simulador MFU
# Compilador
CXX = g++
CXXFLAGS = -std=c++11 -Wall -Wextra

# Archivos
SOURCES = main.cpp MMU.cpp PageTable.cpp TLB.cpp MFUAlgorithm.cpp
HEADERS = MMU.h PageTable.h TLB.h MFUAlgorithm.h MemoryPage.h
OBJECTS = $(SOURCES:.cpp=.o)
EXECUTABLE = MFUSimulator

# Objetivo por defecto
all: $(EXECUTABLE)

# Regla para crear el ejecutable
$(EXECUTABLE): $(OBJECTS)
	$(CXX) $(CXXFLAGS) -o $@ $^

# Regla para archivos objeto
%.o: %.cpp $(HEADERS)
	$(CXX) $(CXXFLAGS) -c $<

# Limpiar archivos compilados
clean:
	del /Q *.o $(EXECUTABLE).exe

# Ejecutar el programa
run: $(EXECUTABLE)
	./$(EXECUTABLE)

# Ayuda
help:
	@echo "Comandos disponibles:"
	@echo "  make          - Compilar el proyecto"
	@echo "  make run      - Compilar y ejecutar"
	@echo "  make clean    - Eliminar archivos compilados"
	@echo "  make help     - Mostrar esta ayuda"

.PHONY: all clean run help
