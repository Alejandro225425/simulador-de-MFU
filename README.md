# 🖥️ Simulador de Algoritmo MFU (Most Frequently Used)

## Descripción

Esta aplicación en **C++** es un simulador educativo completo que demuestra el funcionamiento del **algoritmo de reemplazo MFU** en sistemas de gestión de memoria. La aplicación visualiza de manera clara y entendible todas las métricas medibles de la memoria, incluyendo:

- **MMU** (Memory Management Unit)
- **TLB** (Translation Lookaside Buffer)
- **Tabla de Paginación**
- **Bits de Control** (presencia, sucio/modificado, referencia)
- **Estadísticas de Rendimiento**

## 📋 Características Principales

### 1. **Memory Management Unit (MMU)**
- Gestiona la traducción de direcciones virtuales a físicas
- Coordina la tabla de paginación y el TLB
- Rastreo de fallos de página (page faults)
- Cálculo de tasas de fallo

### 2. **Translation Lookaside Buffer (TLB)**
- Caché de 16 entradas que almacena mapeos recientes
- Mejora el rendimiento evitando accesos repetidos a tabla de paginación
- Estadísticas de aciertos y fallos
- Reemplazo LRU (Least Recently Used) cuando está lleno

### 3. **Tabla de Paginación**
- Mapeo completo de páginas virtuales a marcos físicos
- Visualización detallada de todas las páginas
- Estado de utilizaciónde memoria

### 4. **Bits de Control**
Cada página mantiene tres bits importantEs:

- **Bit de Presencia (P)**: 1 = Página en memoria principal, 0 = Página en disco
- **Bit Sucio/Modificado (D)**: 1 = Página ha sido modificada, 0 = Sin cambios
- **Bit de Referencia (R)**: 1 = Página accedida recientemente, 0 = No accedida

### 5. **Algoritmo MFU (Most Frequently Used)**
- Reemplaza la página con el **menor contador de accesos**
- Basado en el principio de localidad: páginas frecuentes seguirán siendo usadas
- Visualización de páginas ordenadas por frecuencia
- Prevención de thrashing en patrones de acceso con localidad

## 🏗️ Estructura de Archivos

```
.
├── main.cpp              # Programa principal con menú interactivo
├── MMU.h / MMU.cpp       # Implementación de Memory Management Unit
├── PageTable.h / PageTable.cpp  # Tabla de paginación
├── TLB.h / TLB.cpp       # Translation Lookaside Buffer
├── MFUAlgorithm.h / MFUAlgorithm.cpp  # Algoritmo de reemplazo MFU
├── MemoryPage.h          # Estructura de datos para páginas
├── CMakeLists.txt        # Configuración CMake
├── Makefile              # Makefile para compilación
└── README.md             # Este archivo
```

## 🔧 Compilación

### Opción 1: Usando Makefile (Windows/Linux)
```bash
make              # Compilar
make run          # Compilar y ejecutar
make clean        # Limpiar archivos compilados
```

### Opción 2: Usando CMake
```bash
mkdir build
cd build
cmake ..
cmake --build .
./MFUSimulator    # Linux
```

### Opción 3: Compilación directa (g++)
```bash
g++ -std=c++11 -o MFUSimulator main.cpp MMU.cpp PageTable.cpp TLB.cpp MFUAlgorithm.cpp
./MFUSimulator
```

## 📊 Configuración del Simulador

En `main.cpp` puedes ajustar estos parámetros:

```cpp
int NUM_PAGES = 16;      // Número de páginas virtuales
int NUM_FRAMES = 4;      // Número de marcos físicos
int PAGE_SIZE = 4096;    // Tamaño de cada página (bytes)
```

**Nota**: Mantener `NUM_FRAMES < NUM_PAGES` para observar reemplazos.

## 🎮 Uso del Simulador

Al ejecutar, verás un menú interactivo:

### Opciones Principales:

1. **Acceso Manual a Página**
   - Especificar número de página
   - Genera translation (virtual → física)
   - Actualiza contadores y bits de control

2. **Simular Accesos Aleatorios**
   - Carga de trabajo realista
   - Mezcla de lecturas (70%) y escrituras (30%)
   - Observar comportamiento del algoritmo MFU

3. **Ver Información de la MMU**
   - Tabla de paginación actual
   - Estado del TLB
   - Marcos ocupados vs disponibles

4. **Ver Tabla de Paginación Completa**
   - Todas las páginas virtuales
   - Mapeo a marcos físicos
   - Estado de bits de control

5. **Ver Páginas por Frecuencia (MFU)**
   - Ordenamiento por contador de accesos
   - Identifica qué páginas serían reemplazadas
   - Visualización clara del algoritmo

6. **Ver Estadísticas del TLB**
   - Tasa de aciertos (hit rate)
   - Número de accesos totales
   - Efectividad de la caché

7. **Ver Todas las Estadísticas**
   - Resumen completo de MMU
   - Fallos de página
   - Rendimiento general

8. **Resetear Simulación**
   - Limpiar todos los datos
   - Empezar nueva simulación

## 📈 Ejemplo de Uso

### Simulación Básica:

```
1. Seleccionar opción 2 (Simular accesos aleatorios)
2. Ingresar 100 accesos
3. Ver resultados en opción 7 (Todas las estadísticas)
4. Observar:
   - Tabla de paginación
   - TLB y su efectividad
   - Frecuencia de accesos (MFU)
   - Tasa de fallos de página
```

## 📊 Métricas Medibles

### Misses vs Hits

- **Page Fault (Fallo)**: Página no está en memoria principal
- **Hit**: Página estáen memoria principal (acierto)

### Tasas Importantes:

```
Tasa de Fallos = (Fallos de Página) / (Total de Accesos) × 100%
Tasa TLB Hit = (Aciertos TLB) / (Total de Accesos TLB) × 100%
Eficiencia = (Total de Accesos - Fallos) / Total de Accesos × 100%
```

### Bits de Control:

| Bit | Significado | Valor |
|-----|-------------|-------|
| P (Presencia) | En memoria | 1 = SÍ, 0 = NO |
| D (Dirty) | Modificada | 1 = SÍ, 0 = NO |
| R (Referencia) | Accedida | 1 = SÍ, 0 = NO |

## 🎯 Diseño Educativo

La aplicación está específicamente diseñada para **aprender** cómo funciona la administración de memoria:

✅ **Visualización Clara**: Tablas ASCII formatadas  
✅ **Interfaz Interactiva**: Explorar cada componente  
✅ **Explicaciones**: Descripción de cada métrica  
✅ **Ejemplos Reales**: Simulación con accesos aleatorios  
✅ **Algoritmo Visible**: Ver cómo MFU selecciona páginas  

## 💡 Conceptos Implementados

### MFU (Most Frequently Used)
- **Ventaja**: Bueno para cargas de trabajo con patrón de localidad
- **Desventaja**: Requiere mantener contador de accesos
- **Caso de Uso**: Aplicaciones con referencias concentradas en pocas páginas

### Tabla de Paginación
- Mapeo virtual → físico
- Bits de control para cada página
- Soporte para memoria virtual contra física

### TLB
- Caché de traducción de direcciones
- Mejora significativa de rendimiento
- Reemplazo LRU cuando se llena

## 🔍 Ejemplo de Salida

```
╔═════════════════════════════════════════╗
║  TABLA DE PAGINACIÓN COMPLETA           ║
╠═════════════════════════════════════════╣
║ Página │ Marco │ Presencia │ Modificada │
╠═════════════════════════════════════════╣
║   0    │   0   │    SÍ     │     NO     │
║   1    │   1   │    SÍ     │     SÍ     │
║   2    │  -1   │    NO     │     NO     │
║   3    │   2   │    SÍ     │     NO     │
║  ...   │  ...  │   ...     │    ...     │
╚═════════════════════════════════════════╝
```

## 🐛 Troubleshooting

**Problema**: No compila
- **Solución**: Asegúrate de usar C++11 o superior: `g++ -std=c++11`

**Problema**: Caracteres especiales no se muestran correctamente
- **Solución**: En Windows, configurar encoding UTF-8 o usar `chcp 65001`

**Problema**: TLB siempre vacío
- **Solución**: Normal si se hacen pocos accesos. Simula 50+ accesos para ver resultados.

## 📚 Referencias Educativas

- **Virtual Memory**: Concepto de memoria virtual vs física
- **Paging**: División de memoria en páginas de tamaño fijo
- **Page Replacement Algorithms**: Estrategias para elegir qué página reemplazar
- **Locality of Reference**: Principio en el que se basan los algoritmos
- **TLB**: Optimización de traducción de direcciones

## 📝 Licencia

Este proyecto es de código abierto para propósitos educativos.

---

**Autor**: Sistema de Administración de Memoria  
**Año**: 2025  
**Propósito**: Educativo - Enseñanza de Sistemas Operativos
