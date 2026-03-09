#ifndef MMU_H
#define MMU_H

#include "PageTable.h"
#include "TLB.h"
#include <string>

/**
 * Memory Management Unit (MMU)
 * Gestiona el acceso a memoria virtual y física
 * Realiza la traducción de direcciones virtuales a direcciones físicas
 * Coordina la tabla de paginación y el TLB
 */
class MMU {
private:
    PageTable pageTable;
    TLB tlb;
    
    // Configuración de memoria
    int virtualAddressBits;
    int physicalAddressBits;
    int pageSize;
    int virtualMemorySize;
    int physicalMemorySize;
    
    // Estadísticas
    int totalPageFaults;
    int totalMemoryAccesses;
    
public:
    /**
     * Constructor
     * @param numPages Número de páginas virtuales
     * @param numFrames Número de marcos físicos
     * @param pageSize Tamaño de cada página en bytes
     */
    MMU(int numPages, int numFrames, int pageSize = 4096);
    
    /**
     * Accede a una página virtual
     * @param pageNumber Número de página virtual
     * @param write true si es escritura, false si es lectura
     * @return true si el acceso fue exitoso, false si hay fallo de página
     */
    bool accessPage(int pageNumber, bool write = false);
    
    /**
     * Obtiene la dirección física a partir de una dirección virtual
     * @param virtualAddress Dirección virtual
     * @return Dirección física, -1 si no está mapeada
     */
    long long translateAddress(long long virtualAddress);
    
    /**
     * Obtiene la tabla de paginación
     */
    PageTable& getPageTable() { return pageTable; }
    const PageTable& getPageTable() const { return pageTable; }
    
    /**
     * Obtiene el TLB
     */
    TLB& getTLB() { return tlb; }
    const TLB& getTLB() const { return tlb; }
    
    /**
     * Muestra la información completa de la MMU
     */
    void displayMMUInfo();
    
    /**
     * Muestra estad­ísticas de la MMU
     */
    void displayStatistics();
    
    /**
     * Obtiene la tasa de fallos de página
     */
    double getPageFaultRate() const;
    
    /**
     * Obtiene el número total de fallos de página
     */
    int getTotalPageFaults() const { return totalPageFaults; }
    
    /**
     * Obtiene información de configuración
     */
    void displayConfiguration();
    
    /**
     * Resetea las estadísticas
     */
    void resetStatistics();
};

#endif // MMU_H
