## 🚀 GUÍA RÁPIDA DE INSTALACIÓN Y USO

### 📥 Requisitos
- Compilador C++ (g++, clang, o MSVC)
- Soporte para C++11 o superior
- Windows, Linux o macOS

### ⚡ Inicio Rápido (3 pasos)

#### 1. **Compilar (Opción recomendada: Makefile)**
```bash
# Windows (PowerShell)
make

# Linux/macOS
make
```

#### 2. **Ejecutar el Programa**
```bash
# Windows
MFUSimulator.exe

# Linux/macOS
./MFUSimulator
```

#### 3. **Seleccionar opciones del menú**
Una vez abierto, verás un menú interactivo con 9 opciones.

---

### 🎯 Ejemplo Práctico Rápido

**Objetivo**: Ver cómo funciona el algoritmo MFU

1. **Ejecutar el programa**
   ```bash
   MFUSimulator.exe
   ```

2. **Realizar una simulación automática** (Opción 2)
   ```
   Seleccione: 2
   Número de accesos: 100
   ```

3. **Ver resultados** (Opción 7)
   ```
   Seleccione: 7
   ```

4. **Interpretar la salida**:
   - **Tasa de fallo**: % de accesos que no encontraron página en memoria
   - **Frecuencia de acceso**: Cuántas veces se accedió cada página
   - **Bits de control**: Estado de presencia, modificación, referencia

---

### 📊 Significado de Métricas

| Métrica | Significado |
|---------|------------|
| **Page Faults** | Accesos a páginas no en memoria principal |
| **Tasa de Fallo** | (Fallos / Total accesos) × 100% |
| **TLB Hits** | Traducción encontrada en caché TLB |
| **Presencia (P)** | 1=En RAM, 0=En disco |
| **Dirty (D)** | 1=Modificada, 0=Sin cambios |
| **Referencia (R)** | 1=Accedida recientemente |

---

### 🔧 Alternativas de Compilación

#### Opción 1: Con Makefile (Recomendado)
```bash
make run      # Compila y ejecuta directamente
```

#### Opción 2: Compilación Manual
```bash
g++ -std=c++11 -o MFUSimulator.exe main.cpp MMU.cpp PageTable.cpp TLB.cpp MFUAlgorithm.cpp
```

#### Opción 3: Con CMake
```bash
mkdir build
cd build
cmake ..
cmake --build .
```

---

### 💡 Casos de Uso Recomendados

#### 1. **Entender conceptos básicos**
   - Opción 1: Acceso manual
   - Opción 3: Ver estructura de MMU

#### 2. **Ver el algoritmo en acción**
   - Opción 2: Simular 50 accesos
   - Opción 5: Ver frecuencia (MFU)

#### 3. **Analizar rendimiento**
   - Opción 2: Simular 200+ accesos
   - Opción 7: Ver estadísticas completas

---

### ❓ Preguntas Frecuentes

**P: ¿Por qué hay tantos "page faults" al principio?**
- R: Es normal. Inicialmente toda la memoria virtual está en disco. Conforme se accede, las páginas se cargan a RAM.

**P: ¿Qué es MFU exactamente?**
- R: **Most Frequently Used** - Reemplaza la página con MENOR número de accesos, manteniendo las más usadas.

**P: ¿Puedo cambiar el número de marcos/páginas?**
- R: Sí, edita en `main.cpp`:
  ```cpp
  int NUM_PAGES = 16;      // Cambia esto
  int NUM_FRAMES = 4;      // Cambia esto
  ```

**P: ¿Qué tamaño debe tener una página?**
- R: Típicamente 4KB (4096 bytes), que es lo que usa el simulador. Puedes cambiarlo en `main.cpp`.

---

### 🎓 Conceptos Clave

```
MEMORIA VIRTUAL vs FÍSICA

┌─────────────────────┐
│  Dirección Virtual  │
│  (32 bits típico)   │
└──────────┬──────────┘
           │
           │ MMU + TLB
           │ Traducción
           ↓
┌─────────────────────┐
│ Dirección Física    │
│ (24 bits típico)    │
└─────────────────────┘

Tabla de Paginación: Mapeo virtual → física
TLB: Caché de traducciones
MFU: Algoritmo para elegir qué página reemplazar
```

---

### 📚 Flujo de Acceso a Memoria

```
1. CPU solicita página
   ↓
2. Buscar en TLB (caché)
   - Si está → ¡Hit! (dirección física)
   - Si no está → Miss
   ↓
3. Buscar en Tabla de Paginación
   - Si está en memoria → ¡Acierto!
   - Si no está → Page Fault
   ↓
4. Si hay espacio → Cargar página
   Si no hay espacio → MFU elige qué reemplazar
   ↓
5. Actualizar TLB
   ↓
6. Devolver dirección física
```

---

### 🔍 Cómo Leer la Salida

#### Tabla de Paginación:
```
Página │ Marco │ Presencia │ Modificada │ ...
  0    │   0   │    SÍ     │     NO      │
  1    │   1   │    SÍ     │     SÍ      │
  2    │  -1   │    NO     │     NO      │ <- En disco
```

#### Frecuencia (MFU):
```
Página │ Marco │ Frecuencia │ Posición │
  3    │   3   │     5      │    1     │ <- Más accedida
  0    │   0   │     3      │    2     │
  1    │   1   │     1      │    3     │ <- Menos accedida (será reemplazada)
```

---

### ⚠️ Solución de Problemas

| Problema | Solución |
|----------|----------|
| No compila | Asegúrate de tener g++ instalado y usar `-std=c++11` |
| Caracteres rotos | Cambia encoding a UTF-8 en terminal |
| Ejecutable no se crea | Revisa carpeta, debe estar en la misma donde están los .cpp |
| TLB siempre vacío | Simula más accesos (50+) para ver resultados |

---

### 📖 Referencias Útiles

- **Virtual Memory**: Simula RAM física mayor a través de software
- **Paging**: División en bloques fijos para administrar memoria
- **Page Replacement**: Decidir qué página sacar cuando no hay espacio
- **TLB**: Pequeña caché que acelera traducciones de direcciones

---

**¡Ahora estás listo para explorar el simulador MFU!** 🚀
