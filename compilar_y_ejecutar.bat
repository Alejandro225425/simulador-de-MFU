@echo off
chcp 65001 > nul
echo ===========================================
echo   Compilando el Simulador MFU...
echo ===========================================

g++ -std=c++11 -Wall -Wextra main.cpp MMU.cpp PageTable.cpp TLB.cpp MFUAlgorithm.cpp -o MFUSimulator.exe

if %errorlevel% neq 0 (
    echo.
    echo [ERROR] La compilacion ha fallado. Revisa los errores arriba.
    pause
    exit /b %errorlevel%
)

echo.
echo [EXITO] Compilacion completada correctamente.
echo.
echo ===========================================
echo   Ejecutando el Simulador MFU...
echo ===========================================
echo.

MFUSimulator.exe

echo.
echo ===========================================
echo   Ejecucion finalizada. Presiona una tecla para salir.
echo ===========================================
pause
