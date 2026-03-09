## 🪟 GUÍA DE COMPILACIÓN EN WINDOWS

### ✅ Requisitos Previos

1. **Compilador C++** (elige UNO):
   - **MinGW-w64** (Recomendado para g++)
   - **MSVC** (Visual Studio)
   - **Clang**

2. **Verificar instalación**:
   Abre PowerShell y ejecuta:
   ```powershell
   g++ --version
   ```
   Deberías ver algo como:
   ```
   g++ (MinGW-W64 15.2.0, built by MinGW-W64 project) 15.2.0
   ```

---

## 🔧 Opción 1: MinGW-w64 (RECOMENDADO)

### Instalación de MinGW-w64

1. **Descargar**:
   - Ve a: https://www.mingw-w64.org/
   - Click en "Downloads"
   - Descarga el instalador

2. **Instalar**:
   - Ejecuta el instalador
   - Elige versión: **x86_64** (para 64-bit)
   - Elige compiladores: **gcc, g++, gdb**
   - Instala en: `C:\mingw64`

3. **Agregar a PATH**:
   - Click derecho en "Este equipo" → "Propiedades"
   - Click en "Configuración avanzada del sistema"
   - "Variables de entorno"
   - Agregar a PATH: `C:\mingw64\bin`

4. **Verificar**:
   ```powershell
   g++ --version
   ```

### Compilar el Proyecto

1. **Abre PowerShell** en la carpeta del proyecto:
   ```powershell
   cd "C:\Users\PREDATOR\Documents\aplicación con Algoritmo de reemplazo"
   ```

2. **Compilar**:
   ```powershell
   g++ -std=c++11 -Wall -Wextra -o MFUSimulator.exe main.cpp MMU.cpp PageTable.cpp TLB.cpp MFUAlgorithm.cpp
   ```

3. **Ejecutar**:
   ```powershell
   .\MFUSimulator.exe
   ```

---

## 🔧 Opción 2: Con Makefile

### Requisitos
- MinGW instalado (como arriba)
- `make` instalado

### Instalar Make en Windows

1. **Opción A**: Descargar GnuWin32
   - Go to: http://gnuwin32.sourceforge.net/packages/make.htm
   - Descargar e instalar
   - Agregar a PATH: `C:\Program Files (x86)\GnuWin32\bin`

2. **Opción B**: Usar package manager
   ```powershell
   choco install make  # Si tienes Chocolatey
   ```

### Compilar con Makefile

1. **Abre PowerShell**:
   ```powershell
   cd "C:\Users\PREDATOR\Documents\aplicación con Algoritmo de reemplazo"
   ```

2. **Compilar**:
   ```powershell
   make
   ```
   Output:
   ```
   g++ -std=c++11 -Wall -Wextra -c main.cpp
   g++ -std=c++11 -Wall -Wextra -c MMU.cpp
   ...
   g++ -std=c++11 -Wall -Wextra -o MFUSimulator main.o ...
   ```

3. **Ejecutar**:
   ```powershell
   .\MFUSimulator.exe
   ```

4. **Limpiar archivos compilados**:
   ```powershell
   make clean
   ```

---

## 🔧 Opción 3: Visual Studio

### Compilar con MSVC

1. **Abre Visual Studio**
2. **Create → New → Project**
3. **Elige: C++ → Empty Project**
4. **Agregar archivos**:
   - Drag & drop todos los `.cpp` y `.h`  
   - O: Project → Add Existing Item

5. **Configurar C++ Standard**:
   - Project Properties → C/C++
   - Language Standard: ISO C++11 o superior

6. **Compilar**:
   - Ctrl+F7 (Compile)
   - O: Build → Build Solution

7. **Ejecutar**:
   - F5 o Ctrl+F5

---

## 🔧 Opción 4: CMake

### Requisitos
- CMake instalado
- MinGW o MSVC configurado

### Pasos

1. **Abre PowerShell** en carpeta del proyecto

2. **Crear carpeta build**:
   ```powershell
   mkdir build
   cd build
   ```

3. **Generar archivos de compilación**:
   ```powershell
   cmake -G "MinGW Makefiles" ..
   # O para Visual Studio:
   # cmake -G "Visual Studio 17 2022" ..
   ```

4. **Compilar**:
   ```powershell
   cmake --build .
   ```

5. **Ejecutar**:
   ```powershell
   .\Debug\MFUSimulator.exe
   ```

---

## 🚀 Troubleshooting

### Problema: "g++ no se reconoce"
**Solución**:
1. MinGW no está en PATH
2. Verifica: `C:\mingw64\bin\g++.exe` existe
3. Agrega a PATH manualmente en PowerShell:
   ```powershell
   $env:Path += ";C:\mingw64\bin"
   ```

### Problema: "No se puede encontrar -lstdc++"
**Solución**:
- Usa compilador de MinGW completo
- O agrega flag: `-static-libgcc -static-libstdc++`

### Problema: Caracteres especiales no se muestran
**Solución**:
1. En PowerShell:
   ```powershell
   chcp 65001
   ```
2. O usa Command Prompt antiguo (cmd.exe)

### Problema: El programa no inicia / Falta DLL
**Solución**:
1. Opción A: Compilar estáticamente
   ```powershell
   g++ -std=c++11 -static -o MFUSimulator.exe main.cpp ...
   ```

2. Opción B: Copiar DLLs
   ```powershell
   Copy-Item C:\mingw64\bin\libstdc++-6.dll -Destination .\
   ```

---

## 📋 Script Automatizado

Crear archivo `compilar.ps1`:
```powershell
# compilar.ps1
Write-Host "╔════════════════════════════════════════╗"
Write-Host "║   Compilando MFU Simulator             ║"
Write-Host "╚════════════════════════════════════════╝"

$files = @(
    "main.cpp",
    "MMU.cpp",
    "PageTable.cpp",
    "TLB.cpp",
    "MFUAlgorithm.cpp"
)

$fileString = $files -join " "

Write-Host "Procesando archivos..."
g++ -std=c++11 -Wall -Wextra -o MFUSimulator.exe $fileString

if ($LASTEXITCODE -eq 0) {
    Write-Host "✓ Compilación exitosa!"
    Write-Host "Ejecutando..."
    .\MFUSimulator.exe
} else {
    Write-Host "✗ Error en compilación"
}
```

Ejecutar:
```powershell
Set-ExecutionPolicy -ExecutionPolicy Bypass -Scope Process
.\compilar.ps1
```

---

## 🎯 Resumen de la Compiled

| Método | Dificultad | Velocidad | Recomendado |
|--------|-----------|-----------|------------|
| Direct g++ | ⭐ | ⭐⭐⭐ | ✅ Para empezar |
| Makefile | ⭐⭐ | ⭐⭐⭐ | ✅ Para proyecto |
| Visual Studio | ⭐⭐ | ⭐⭐ | IDE tradicional |
| CMake | ⭐⭐⭐ | ⭐ | Proyectos grandes |

**RECOMENDACIÓN**: Comienza con la compilación directa con g++

---

## ✅ Verificación de Compilación Exitosa

Después de compilar, ejecuta:
```powershell
.\MFUSimulator.exe
```

Deberías ver:
```
╔════════════════════════════════════════════════════════════════════════════════════════╗
║                         ¡BIENVENIDO AL SIMULADOR MFU!                                 ║
║                                                                                        ║
║ Este programa simula el algoritmo de reemplazo MFU (Most Frequently Used)             ║
║ Incluye visualización de: MMU, TLB, Tabla de Paginación y Bits de Control            ║
╚════════════════════════════════════════════════════════════════════════════════════════╝
```

Si ves esto, ¡La compilación fue exitosa! 🎉

---

## 📞 Soporte

Si tienes problemas:

1. **Verifica rutas completas**: Usa la carpeta correcta
2. **Verifica compilador**: `g++ --version`
3. **Verifica archivos**: Los 5 .cpp y 5 .h deben estar presentes
4. **Lee mensajes de error**: Son muy informativos

**Archivos que DEBEN estar**:
- ✅ main.cpp
- ✅ MMU.cpp
- ✅ MMU.h
- ✅ PageTable.cpp
- ✅ PageTable.h
- ✅ TLB.cpp
- ✅ TLB.h
- ✅ MFUAlgorithm.cpp
- ✅ MFUAlgorithm.h
- ✅ MemoryPage.h

---

**¡Ahora estás listo para compilar en Windows!** 🚀
