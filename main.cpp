#include <iostream>
#include <iomanip>
#include <cstdlib>
#include <ctime>
#include <vector>
#include <string>
#include <climits>
#ifdef _WIN32
#include <windows.h>
#endif
#include "MMU.h"
#include "MFUAlgorithm.h"

using namespace std;

// Función para pausar la ejecución y permitir al usuario leer la salida
void pausarParaLeer() {
    cout << "\nPresione Enter para continuar...";
    cin.ignore(10000, '\n');
    cin.get();
}

// Función para limpiar la consola
void limpiarPantalla() {
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
}

// Función para mostrar el menú principal
void displayMainMenu() {
    cout << "\n+==== SIMULADOR MFU (Most Frequently Used) - MAXIMUM RAM ACTIVO ====" << endl;
    cout << "|    CON VISUALIZACION DE METRICAS DE MEMORIA" << endl;
    cout << "|    BASE: TODA LA RAM DISPONIBLE DEL SISTEMA ASIGNADA" << endl;
    cout << "+========================================================" << endl;
    cout << "|" << endl;
    cout << "|  1. Simular secuencia de accesos aleatorios" << endl;
    cout << "|  2. Ver informacion de la MMU (Unidad de Manejo de Memoria)" << endl;
    cout << "|  3. Ver tabla de paginacion (Page Table) completa" << endl;
    cout << "|  4. Ver paginas por frecuencia (Algoritmo de Reemplazo MFU)" << endl;
    cout << "|  5. Ver estadisticas del TLB (Translation Lookaside Buffer)" << endl;
    cout << "|  6. Ver todas las estadisticas" << endl;
    cout << "|  7. Resetear simulacion" << endl;
    cout << "|  8. Salir" << endl;
    cout << "|" << endl;
    cout << "+========================================================" << endl;
    cout << "Seleccione una opcion: ";
}

void showExplanation() {
    cout << "\n+===== EXPLICACION DE LOS COMPONENTES =====" << endl;
    cout << "|" << endl;
    cout << "| MMU (Memory Management Unit):" << endl;
    cout << "|   - Traduce direcciones virtuales a fisicas" << endl;
    cout << "|   - Coordina tabla de paginacion y TLB" << endl;
    cout << "|" << endl;
    cout << "| TLB (Translation Lookaside Buffer):" << endl;
    cout << "|   - Cache de mapeos recientes" << endl;
    cout << "|   - Mejora rendimiento evitando accesos a tabla" << endl;
    cout << "|" << endl;
    cout << "| TABLA DE PAGINACION (PAGE TABLE):" << endl;
    cout << "|   - PTBR (Page Table Base Register): Registro con la direccion base en RAM" << endl;
    cout << "|   - Mapea paginas virtuales (Virtual Pages) a marcos fisicos (Physical Frames)" << endl;
    cout << "|   - Bits de control (Control Bits):" << endl;
    cout << "|     - Presencia (Present Bit): en RAM (1) o en disco/swap (0)" << endl;
    cout << "|     - Modificado (Dirty Bit): cambios no guardados en disco (1/0)" << endl;
    cout << "|     - Referencia (Reference Bit): accedida recientemente (1/0)" << endl;
    cout << "|" << endl;
    cout << "| ALGORITMO MFU (Most Frequently Used):" << endl;
    cout << "|   - Reemplaza pagina con MENOR frecuencia" << endl;
    cout << "|   - Si pagina se usara again, best reemplazo" << endl;
    cout << "|   - Metrica: contador de accesos" << endl;
    cout << "|" << endl;
    cout << "+==========================================" << endl;
}

int main() {
#ifdef _WIN32
    system("chcp 65001 > nul");
#endif

    // Inicializar generador aleatorio
    srand(time(nullptr));
    
    // Determinar la RAM disponible en el sistema (Windows)
    long long MEMORIA_TOTAL = 4LL * 1024LL * 1024LL * 1024LL; // Default 4GB fallback
#ifdef _WIN32
    MEMORYSTATUSEX status;
    status.dwLength = sizeof(status);
    if (GlobalMemoryStatusEx(&status)) {
        // Usamos el 90% de la RAM disponible para evitar crashear el sistema por completo
        // o la memoria total física si se quiere ser más agresivo. Optamos por la memoria física total (ullTotalPhys)
        // para el cálculo de porcentaje, y asignamos basándonos en ullAvailPhys (RAM libre actual).
        MEMORIA_TOTAL = status.ullAvailPhys; 
    }
#endif

    cout << "\n+====== INICIANDO SIMULADOR MFU CON MEMORIA MAXIMA =====" << endl;
    cout << "| RAM Disponible detectada: " << (MEMORIA_TOTAL / (1024*1024)) << " MB" << endl;
    cout << "| Asignando toda la memoria real disponible..." << endl;
    cout << "+======================================================+" << endl;
    
    // Asignar memoria real ANTES de todo
    vector<char*> memoryBuffer;
    const long long CHUNK_SIZE = 250*1024*1024; // 250 MB chunks para ser más rápidos
    long long asignado = 0;
    int numeroChunks = 0;
    
    cout << "\n";
    while (asignado < MEMORIA_TOTAL) {
        try {
            char* chunk = new char[CHUNK_SIZE];
            memoryBuffer.push_back(chunk);
            asignado += CHUNK_SIZE;
            numeroChunks++;
            
            float porcentaje = (float)asignado / MEMORIA_TOTAL * 100.0f;
            cout << "  [" << numeroChunks << "] " 
                 << (asignado / (1024*1024)) << " MB / " << (MEMORIA_TOTAL / (1024*1024)) << " MB ("
                 << fixed << setprecision(1) << porcentaje << "%)" << endl;
        } catch (bad_alloc& e) {
            cout << "\n  [ERROR] Limite del sistema alcanzado en: " 
                 << (asignado / (1024*1024)) << " MB" << endl;
            break;
        }
    }
    
    cout << "\n+===== MEMORIA MAXIMA ASIGNADA - SIMULACION ACTIVA =====" << endl;
    cout << "| Memoria total retenida: " << (asignado / (1024*1024)) << " MB" << endl;
    cout << "+======================================================+" << endl;
    
    // Configuración del simulador
    int PAGE_SIZE = 4096;    // 4 KB por página
    
    // Calcular el número de páginas virtuales en base a la RAM asignada (para cubrir todo el espacio)
    // Usamos uint64_t para evitar overflow si hay mucha RAM
    unsigned long long NUM_PAGES_LL = (unsigned long long)asignado / PAGE_SIZE;
    
    // Limitamos a INT_MAX / 2 para evitar overflows de signed int en el resto de la simulación
    int NUM_PAGES = (NUM_PAGES_LL > (INT_MAX / 2)) ? (INT_MAX / 2) : (int)NUM_PAGES_LL;
    
    // Forzar 5 marcos físicos y asegurar al menos NUM_FRAMES + 1 páginas
    int NUM_FRAMES = 5;      
    
    if (NUM_PAGES <= NUM_FRAMES) {
        NUM_PAGES = NUM_FRAMES + 10;
    }
    
    // Crear MMU y algoritmo MFU
    MMU mmu(NUM_PAGES, NUM_FRAMES, PAGE_SIZE);
    MFUAlgorithm mfuAlgorithm(&mmu.getPageTable());
    
    int choice;
    bool running = true;
    
    cout << "\n+====== BIENVENIDO AL SIMULADOR MFU ======" << endl;
    cout << "|" << endl;
    cout << "| Algoritmo: MFU (Most Frequently Used)" << endl;
    cout << "| Estado: USANDO TODA LA RAM DISPONIBLE (" << (asignado / (1024*1024)) << " MB)" << endl;
    cout << "| Monitoreeable: En Administrador de tareas" << endl;
    cout << "| Visualiza: MMU, TLB, Tabla Paginacion" << endl;
    cout << "|" << endl;
    cout << "+==========================================" << endl;
    
    showExplanation();
    pausarParaLeer();
    
    while (running) {
        limpiarPantalla();
        displayMainMenu();
        cin >> choice;
        
        if (cin.fail()) {
            cin.clear();
            cin.ignore(10000, '\n');
            cout << "Entrada inválida. Por favor intente de nuevo." << endl;
            continue;
        }
        
        switch(choice) {
            case 1: {
                limpiarPantalla();
                // Simular accesos aleatorios
                int numAccesos;
                cout << "\nIngrese el numero de accesos a simular (1-1000): ";
                cin >> numAccesos;
                
                if (numAccesos > 0 && numAccesos <= 1000) {
                    cout << "\n+===== SIMULACION DE ACCESOS ALEATORIOS =====" << endl;
                    cout << "| Config: " << NUM_PAGES << " paginas, " << NUM_FRAMES << " marcos" << endl;
                    cout << "+==========================================" << endl;
                    cout << "\nMarcos fisicos disponibles (Physical Frames): " << NUM_FRAMES << endl;
                    cout << "Paginas virtuales (Virtual Pages): " << NUM_PAGES << endl;
                    cout << "\nEjecutando simulacion...\n" << endl;
                    
                    int reemplazosCount = 0;
                    
                    for (int i = 0; i < numAccesos; i++) {
                        int randomPage;
                        
                        // 35% de probabilidad de forzar un "Hit" (acertar en una pagina que ya esta en RAM)
                        // Esto hace que la simulacion sea mas realista y muestre aciertos, no solo fallos.
                        bool forceHit = (rand() % 100) < 35;
                        
                        if (forceHit && mmu.getPageTable().getAvailableFrameCount() < NUM_FRAMES) {
                            // Buscar una página que ya esté cargada en memoria
                            vector<int> loadedPages;
                            for (int frame = 0; frame < NUM_FRAMES; frame++) {
                                int pageNum = mmu.getPageTable().getPageInFrame(frame);
                                if (pageNum != -1) {
                                    loadedPages.push_back(pageNum);
                                }
                            }
                            if (!loadedPages.empty()) {
                                randomPage = loadedPages[rand() % loadedPages.size()];
                            } else {
                                randomPage = rand() % NUM_PAGES;
                            }
                        } else {
                            // Acceso completamente aleatorio
                            randomPage = rand() % NUM_PAGES;
                        }
                        
                        bool isWrite = (rand() % 10) < 3;  // 30% de escritura
                        
                        // Guardar estado actual
                        int fallosAntes = mmu.getTotalPageFaults();
                        int hitsAntes = mmu.getPageTable().getHitCount();
                        
                        // Realizar acceso
                        mmu.accessPage(randomPage, isWrite);
                        mfuAlgorithm.recordPageAccess(randomPage);
                        
                        // Detectar si hubo nuevo fallo de página (reemplazo) o acierto (Hit)
                        int fallosDespues = mmu.getTotalPageFaults();
                        int hitsDespues = mmu.getPageTable().getHitCount();
                        
                        if (hitsDespues > hitsAntes) {
                            if (numAccesos <= 100) {
                                cout << "  [PAGE HIT] Acceso a pagina " << randomPage << endl;
                            }
                        } else if (fallosDespues > fallosAntes) {
                            reemplazosCount++;
                            if (numAccesos <= 100) {
                                cout << "  [PAGE FAULT] Acceso a pagina " << randomPage << endl;
                            }
                        }
                        
                        if (numAccesos <= 100) {
                            cout << "    [ESTADO TABLA RAM]: ";
                            for (int f = 0; f < NUM_FRAMES; f++) {
                                int pNum = mmu.getPageTable().getPageInFrame(f);
                                if (pNum != -1) cout << "Marco " << f << ":[Pag " << pNum << "]   ";
                                else cout << "Marco " << f << ":[VACIO]   ";
                            }
                            cout << endl;
                        } else {
                            if ((i + 1) % 20 == 0 && (i+1) % 100 != 0) {
                                cout << ".";
                                cout.flush();
                            }
                            if ((i + 1) % 100 == 0) {
                                cout << " [" << (i+1) << "/" << numAccesos << "]" << endl;
                            }
                        }
                    }
                    
                    cout << "\n\n+===== RESULTADOS FINALES DE SIMULACION =====" << endl;
                    cout << "| Accesos a memoria solicitados: " << numAccesos << endl;
                    cout << "| Aciertos en memoria (Page Hits): " << mmu.getPageTable().getHitCount() << endl;
                    cout << "| Fallos de pagina (Page Faults): " << mmu.getTotalPageFaults() << endl;
                    cout << "| Total de reemplazos de pagina (Swaps): " << reemplazosCount << endl;
                    cout << "| Tasa de aciertos (Hit Rate): " << fixed << setprecision(2) 
                         << ((double)mmu.getPageTable().getHitCount() / numAccesos * 100.0) << "%" << endl;
                    cout << "| Tasa de fallos (Miss Rate): " << fixed << setprecision(2) 
                         << (mmu.getPageFaultRate() * 100.0) << "%" << endl;
                    cout << "+============================================" << endl;
                } else {
                    cout << "\n[ERROR] Numero de accesos invalido." << endl;
                }
                pausarParaLeer();
                break;
            }
            
            case 2: {
                limpiarPantalla();
                // Ver información de la MMU
                mmu.displayMMUInfo();
                pausarParaLeer();
                break;
            }
            
            case 3: {
                limpiarPantalla();
                // Ver tabla de paginación completa
                mmu.getPageTable().displayTable();
                pausarParaLeer();
                break;
            }
            
            case 4: {
                limpiarPantalla();
                // Ver páginas por frecuencia (MFU)
                mfuAlgorithm.displayPagesByFrequency();
                pausarParaLeer();
                break;
            }
            
            case 5: {
                limpiarPantalla();
                // Ver estadísticas del TLB
                mmu.getTLB().displayStatistics();
                pausarParaLeer();
                break;
            }
            
            case 6: {
                limpiarPantalla();
                // Ver todas las estadísticas
                mmu.displayStatistics();
                pausarParaLeer();
                limpiarPantalla();
                mmu.getPageTable().displayTable();
                pausarParaLeer();
                limpiarPantalla();
                mfuAlgorithm.displayPagesByFrequency();
                pausarParaLeer();
                break;
            }
            
            case 7: {
                limpiarPantalla();
                // Resetear simulación
                mmu = MMU(NUM_PAGES, NUM_FRAMES, PAGE_SIZE);
                mfuAlgorithm = MFUAlgorithm(&mmu.getPageTable());
                cout << "\n[OK] Simulacion reseteada." << endl;
                cout << "[OK] " << (asignado / (1024*1024)) << " MB de memoria sigue asignada y disponible." << endl;
                pausarParaLeer();
                break;
            }
            
            case 8: {
                limpiarPantalla();
                // Salir
                cout << "\n+===== GRACIAS POR USAR EL SIMULADOR MFU =====" << endl;
                cout << "+=============================================+" << endl;
                running = false;
                break;
            }
            
            default:
                cout << "\n[ERROR] Opcion no valida. Por favor intente de nuevo." << endl;
                pausarParaLeer();
        }
    }
    
    // Liberar memoria asignada
    cout << "\n+===== LIBERANDO " << (asignado / (1024*1024)) << " MB DE MEMORIA =====" << endl;
    for (char* chunk : memoryBuffer) {
        delete[] chunk;
    }
    memoryBuffer.clear();
    cout << "[OK] Memoria liberada correctamente." << endl;
    cout << "+====================================+" << endl;
    
    return 0;
}
