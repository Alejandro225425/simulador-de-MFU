## 🔧 DOCUMENTACIÓN TÉCNICA - DETALLES DE IMPLEMENTACIÓN

### 📦 Estructura del Proyecto

```
MFUSimulator/
│
├── main.cpp                 # Programa principal - Menú interactivo
│
├── Header Files:
│   ├── MemoryPage.h         # Estructura de datos de páginas
│   ├── TLB.h                # Interface de Translation Lookaside Buffer
│   ├── PageTable.h          # Interface de Tabla de Paginación
│   ├── MMU.h                # Interface de Memory Management Unit
│   └── MFUAlgorithm.h       # Interface del algoritmo MFU
│
├── Implementation Files:
│   ├── TLB.cpp              # Implementación de caché TLB
│   ├── PageTable.cpp        # Implementación de tabla de paginación
│   ├── MMU.cpp              # Implementación de gestor de memoria
│   └── MFUAlgorithm.cpp     # Implementación del algoritmo MFU
│
└── Build Files:
    ├── CMakeLists.txt       # Configuración CMake
    ├── Makefile             # Makefile para compilación
    └── MFUSimulator.exe     # Ejecutable compilado
```

---

### 🏗️ Descripción de Clases

#### 1. **MemoryPage (MemoryPage.h)**
Estructura que representa una página de memoria virtual.

```cpp
struct MemoryPage {
    int pageNumber;              // Identificador de página (0-15)
    int frameNumber;             // Marco físico asignado (-1 si no está en RAM)
    bool presentBit;             // Bit de presencia
    bool dirtyBit;               // Bit de modificación
    bool referenceBit;           // Bit de referencia
    int accessCount;             // Contador de accesos (para MFU)
    long long lastAccessTime;    // Timestamp del último acceso
    string data;                 // Datos almacenados
};
```

**Métodos principales**:
- `displayStatus()`: Mostrar estado completo de la página

---

#### 2. **TLB (TLB.h / TLB.cpp)**
Cache de traducción de direcciones virtuales a físicas.

```cpp
class TLB {
    struct TLBEntry { 
        int pageNumber;          // Página virtual
        int frameNumber;         // Marco físico
        int accessCount;         // Veces consultada
        long long lastAccessTime; // Cuándo fue la última consulta
    };
    
    vector<TLBEntry> entries;
    int maxSize = 16;            // Entradas máximas (típico)
    int hits, misses;            // Estadísticas
};
```

**Métodos principales**:
- `lookup(int page)`: Buscar traducción en TLB → retorna marco o -1
- `insert(int page, int frame)`: Insertar nueva traducción
- `invalidate(int page)`: Invalidar entrada (cuando se reemplaza página)
- `display()`: Mostrar contenido del TLB
- `displayStatistics()`: Mostrar tasa de aciertos

**Algoritmo de reemplazo**: LRU (Least Recently Used)
- Cuando TLB está lleno, se reemplaza la entrada menos recientemente usada
- Basado en `lastAccessTime`

---

#### 3. **PageTable (PageTable.h / PageTable.cpp)**
Tabla de paginación que mapea todas las páginas virtuales.

```cpp
class PageTable {
    vector<MemoryPage> table;     // Tabla completa
    int totalPages = 256;         // Páginas virtuales
    int totalFrames = 4;          // Marcos físicos disponibles
    vector<bool> frameOccupancy;  // Qué marcos están en uso
    int faultCount = 0;           // Contador de fallos de página
    int hitCount = 0;             // Contador de aciertos
};
```

**Métodos principales**:
- `getPageEntry(int)`: Acceder a entrada específica
- `assignFrame(int page, int frame)`: Asignar marco a página
- `freeFrame(int frame)`: Liberar marco
- `getAvailableFrame()`: Encontrar marco libre
- `isPageInMemory(int page)`: Verificar presencia
- `markDirty(int page)`: Marcar como modificada
- `markReferenced(int page)`: Marcar como accedida
- `displayTable()`: Mostrar tabla completa

---

#### 4. **MMU (MMU.h / MMU.cpp)**
Unidad de gestión de memoria que coordina TLB y tabla de paginación.

```cpp
class MMU {
    PageTable pageTable;         // La tabla
    TLB tlb;                     // La caché
    int virtualAddressBits = 32; // Espacioy virtual
    int physicalAddressBits = 24;// Espacio físico
    int pageSize = 4096;         // Tamaño página (4 KB típico)
};
```

**Métodos principales**:
- `accessPage(int page, bool write)`: Acceder a página
  - Retorna `true` si éxito, `false` si error
  - Maneja TLB miss → Page table hit → Page fault
- `translateAddress(long long vAddr)`: Traducir dirección virtual
- `displayMMUInfo()`: Mostrar estado completo
- `displayStatistics()`: Mostrar métricas

**Flujo de accessPage()**:
```cpp
1. totalMemoryAccesses++
2. Buscar en TLB
   ├─ SI (TLB HIT):
   │  ├─ incrementHitCount()
   │  ├─ accessCount++
   │  ├─ referenceBit = true
   │  └─ return true
   └─ NO (TLB MISS):
      3. Buscar en PageTable
         ├─ SI (PAGE TABLE HIT):
         │  ├─ incrementHitCount()
         │  ├─ accessCount++
         │  ├─ referenceBit = true
         │  ├─ Insertar en TLB
         │  └─ return true
         └─ NO (PAGE FAULT):
            4. totalPageFaults++
            5. ¿Marcos disponibles?
               ├─ SI:
               │  └─ assignFrame(page, availableFrame)
               └─ NO:
                  └─ Usar MFUAlgorithm para reemplazar
```

---

#### 5. **MFUAlgorithm (MFUAlgorithm.h / MFUAlgorithm.cpp)**
Implementación del algoritmo de reemplazo Most Frequently Used.

```cpp
class MFUAlgorithm {
    PageTable* pageTable;        // Puntero a tabla (para modificarla)
    long long timestamp;         // Contador de tiempo lógico
};
```

**Métodos principales**:
- `selectPageForReplacement()`: Encontrar qué página reemplazar
  ```cpp
  // Pseudocódigo
  for (cada página en memoria) {
      if (accesos < mínimo) {
          mínimo = accesos
          selectedPage = página
      }
  }
  return selectedPage;  // Página con MENOS accesos
  ```
  
- `replacePage(int old, int new)`: Reemplazar página antigua por nueva
- `recordPageAccess(int page)`: Incrementar contador de acceso
- `displayPagesByFrequency()`: Mostrar páginas ordenadas por frecuencia
- `reset()`: Limpiar estadísticas

**Características MFU**:
- ✅ Simple de implementar
- ✅ Bajo overhead computacional
- ✅ Bueno para cargas con localidad
- ❌ Puede causar thrashing con cambios de fase
- ❌ Requiere mantener contador de accesos

---

### 🔄 Flujo de Ejecución Completo

```
┌─ INICIALIZACIÓN (main())
│  ├─ MMU mmu(16 páginas, 4 marcos)
│  ├─ MFUAlgorithm mfuAlgorithm(&mmu.pageTable)
│  └─ Mostrar menú
│
├─ OPCIÓN 1: Acceso Manual
│  ├─ Pedir número de página
│  ├─ mmu.accessPage(pageNum)
│  │  ├─ TLB.lookup()
│  │  ├─ PageTable.isPageInMemory()
│  │  ├─ MFUAlgorithm.selectPageForReplacement() [si necesario]
│  │  ├─ Actualizar bits
│  │  └─ TLB.insert()
│  └─ Mostrar estado de página
│
├─ OPCIÓN 2: Simulación
│  ├─ Pedir número de accesos
│  ├─ For (i=0 hasta N)
│  │  ├─ randomPage = rand() % NUM_PAGES
│  │  ├─ mmu.accessPage(randomPage)
│  │  └─ mfuAlgorithm.recordPageAccess()
│  └─ Mostrar resumen
│
└─ OPCIÓN 7: Todas las Estadísticas
   └─ Mostrar todo el estado del sistema
```

---

### 📊 Estructura de Datos en Memoria

```
┌──────────────────────────────────────────────┐
│         MEMORY MANAGEMENT UNIT               │
│  ┌────────────────────────────────────────┐  │
│  │          PAGE TABLE (16 páginas)       │  │
│  ├────────────────────────────────────────┤  │
│  │ Página │ Marco │ P │ D │ R │ Accesos  │  │
│  ├────────────────────────────────────────┤  │
│  │    0   │  0    │ 1 │ 0 │ 1 │    5     │  │
│  │    1   │  1    │ 1 │ 1 │ 1 │    3     │  │
│  │    2   │ -1    │ 0 │ 0 │ 0 │    0     │  │
│  │   ...  │ ...   │..│..│..│  ...     │  │
│  └────────────────────────────────────────┘  │
│                                               │
│  ┌────────────────────────────────────────┐  │
│  │  TLB (Caché - 16 entradas máx)         │  │
│  ├────────────────────────────────────────┤  │
│  │ Página │ Marco │ Accesos │ LastAccess  │  │
│  ├────────────────────────────────────────┤  │
│  │    0   │  0    │    2    │     100     │  │
│  │    1   │  1    │    1    │      95     │  │
│  │   ...  │ ...   │  ...    │     ...     │  │
│  └────────────────────────────────────────┘  │
│                                               │
│  ESTADÍSTICAS:                                │
│  • Page Faults: 12                            │
│  • TLB Hits: 75                               │
│  • TLB Misses: 25                             │
└──────────────────────────────────────────────┘
```

---

### 💾 Ciclo de Vida de una Página

```
1. CREACIÓN
   └─ Se crea MemoryPage con presentBit=false
      (página está en disco virtual)

2. PRIMER ACCESO (PAGE FAULT)
   ├─ Buscar marco disponible
   ├─ Si no hay → Ejecutar MFU
   │  └─ Seleccionar página menos accedida
   │  └─ Reemplazarla
   ├─ Cargar página en marco
   ├─ Establecer presentBit = true
   ├─ accessCount = 1
   └─ Insertar en TLB

3. ACCESOS POSTERIORES (HITS)
   ├─ Si en TLB → Rápido (TLB HIT)
   ├─ Si en RAM → Medio (PAGE HIT)
   │  └─ Actualizar TLB
   └─ accessCount++

4. MODIFICACIÓN
   ├─ Si es escritura
   ├─ dirtyBit = true
   └─ Marcar para escribir en disco después

5. REEMPLAZO (si otro necesita el marco)
   ├─ Si dirtyBit = true → Guardar en disco
   ├─ presentBit = false
   ├─ frameNumber = -1
   └─ Invalidar entrada en TLB

6. VUELVE A MEMORIA
   └─ Repetir desde paso 2
```

---

### 🎯 Parámetros Configurables

**En main.cpp**:
```cpp
int NUM_PAGES = 16;      // Páginas virtuales (0-15)
int NUM_FRAMES = 4;      // Marcos físicos disponibles
int PAGE_SIZE = 4096;    // Bytes por página
```

**En TLB.h (TLB constructor)**:
```cpp
TLB(int size = 16);  // Tamaño máximo de TLB
```

**Recomendaciones**:
- `NUM_PAGES > NUM_FRAMES` (para ver reemplazos)
- Mantener `NUM_FRAMES` pequeño (4-8) para ver efecto
- `PAGE_SIZE` típicamente 4KB (4096)
- `TLB size` típicamente 16-64 entradas

---

### 🧪 Complejidad Computacional

| Operación | Complejidad | Notas |
|-----------|-------------|-------|
| `TLB.lookup()` | O(n) | n = tamaño TLB (típicamente 16) |
| `PageTable.assignFrame()` | O(m) | m = total de páginas |
| `MFU.selectPageForReplacement()` | O(m) | Buscar página min |
| `MMU.accessPage()` | O(n+m) | Peor caso: TLB miss + búsqueda |
| `displayTable()` | O(m) | Mostrar todas las páginas |

**Optimizaciones posibles**:
- Usar hash table para TLB (O(1) lookup)
- Usar heap para MFU (O(log n) selección)
- Bitmap para marcos libres (O(1) búsqueda)

---

### 🔍 Debugging y Pruebas

**Cómo verificar correctitud**:

1. **TLB Correctness**:
   - Ejecutar opción 1 (acceso manual)
   - Repetir mismo acceso
   - Segundo acceso debe ser TLB HIT

2. **Page Fault Handling**:
   - Simular acceso a página 10 (no en memoria)
   - Ver cómo se asigna marco
   - Ver actualización en tabla

3. **MFU Algorithm**:
   - Acceder a página 0 cinco veces
   - Acceder a página 1 dos veces
   - Si hay reemplazo, página 1 debería ser reemplazada

4. **Statistics**:
   - Total hits + total faults = total accesos
   - TLB hit rate debe ser razonable (>50%)
   - Page fault rate baja después de stabilización

---

### 📚 Conceptos Implementados

#### Virtual Memory
- Permite usar más memoria virtual que física
- Transparente al programa (CPU usa direcciones virtuales)
- Sistema operativo mapea virtual→física

#### Paging
- División de memoria en páginas fijas (4KB típico)
- Páginas pueden estar en RAM o disco
- Cargarse bajo demanda (demand paging)

#### Page Replacement
- ¿Qué hacer cuando RAM está llena?
- Elegir página menos importante para reemplazar
- MFU: Reemplazar menos accedida

#### Translation Lookaside Buffer
- Caché de pequeño tamaño (~16 entradas)
- Almacena mapeos recientes
- Acelera traducción de direcciones

#### Working Set
- Conjunto de páginas actualmente en uso
- Si working set > RAM disponible → thrashing
- MFU intenta mantener working set en RAM

---

**Esta documentación proporciona una visión completa del simulador** 🎓
