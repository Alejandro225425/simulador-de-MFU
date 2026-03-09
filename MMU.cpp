#include "MMU.h"
#include <cmath>
#include <iomanip>
#include <sstream>
#include <climits>

MMU::MMU(int numPages, int numFrames, int pageSize)
    : pageTable(numPages, numFrames), tlb(16),
      virtualAddressBits(32), physicalAddressBits(24),
      pageSize(pageSize), 
      virtualMemorySize(numPages * pageSize),
      physicalMemorySize(numFrames * pageSize),
      totalPageFaults(0), totalMemoryAccesses(0) {}

bool MMU::accessPage(int pageNumber, bool write) {
    totalMemoryAccesses++;
    
    // Paso 1: Buscar en TLB
    int frameNumber = tlb.lookup(pageNumber);
    
    if (frameNumber != -1) {
        // TLB HIT - la traducción está en caché
        pageTable.incrementHitCount();
        pageTable.getPageEntry(pageNumber).accessCount++;
        pageTable.getPageEntry(pageNumber).referenceBit = true;
        
        if (write) {
            pageTable.markDirty(pageNumber);
        }
        
        return true;
    }
    
    // Paso 2: Buscar en tabla de paginación
    if (pageTable.isPageInMemory(pageNumber)) {
        // PAGE TABLE HIT - la página está en memoria principal
        pageTable.incrementHitCount();
        pageTable.getPageEntry(pageNumber).accessCount++;
        pageTable.getPageEntry(pageNumber).referenceBit = true;
        
        // Actualizar TLB con esta traducción
        frameNumber = pageTable.getPageEntry(pageNumber).frameNumber;
        tlb.insert(pageNumber, frameNumber);
        
        if (write) {
            pageTable.markDirty(pageNumber);
        }
        
        return true;
    }
    
    // Paso 3: PAGE FAULT - la página no está en memoria
    totalPageFaults++;
    pageTable.incrementFaultCount();
    
    // Intentar asignar un marco disponible
    int availableFrame = pageTable.getAvailableFrame();
    if (availableFrame != -1) {
        // Hay marco disponible
        pageTable.assignFrame(pageNumber, availableFrame);
    } else {
        // NO HAY MARCOS DISPONIBLES - se debe reemplazar una página con MFU
        int pageToReplace = -1;
        int minAccesses = INT_MAX;

        // ── TRAZA MFU: Candidatos a reemplazo ──────────────────────────────
        std::cout << "\n  ┌─── [MFU] DECISION DE REEMPLAZO ───────────────────────────────────┐" << std::endl;
        std::cout << "  │ Pagina nueva solicitada : " << std::setw(6) << pageNumber << " (no esta en RAM)" << std::endl;
        std::cout << "  │ RAM llena. Evaluando candidatos para expulsar:" << std::endl;
        std::cout << "  │" << std::endl;
        std::cout << "  │   Pagina │ Marco │ Frecuencia │ Victima?" << std::endl;
        std::cout << "  │  ────────┼───────┼────────────┼─────────" << std::endl;

        for (int i = 0; i < pageTable.getTotalPages(); i++) {
            if (pageTable.getPageEntry(i).presentBit) {
                int acc = pageTable.getPageEntry(i).accessCount;
                int frm = pageTable.getPageEntry(i).frameNumber;
                std::cout << "  │  " << std::setw(7) << i
                          << " │ " << std::setw(5) << frm
                          << " │ " << std::setw(10) << acc
                          << " │ ";
                if (acc < minAccesses) {
                    minAccesses = acc;
                    pageToReplace = i;
                }
                // Marca provisional (se imprime después de conocer la víctima real)
                std::cout << "..." << std::endl;
            }
        }

        // Reimprimir sólo la fila ganadora como víctima
        if (pageToReplace != -1) {
            std::cout << "  │" << std::endl;
            std::cout << "  │ ==> VICTIMA ELEGIDA: Pagina " << pageToReplace
                      << " (frecuencia=" << minAccesses << ", menor de todas)" << std::endl;
            std::cout << "  │ Criterio MFU: se expulsa la pagina con MENOR frecuencia" << std::endl;
            std::cout << "  │   porque es la que se uso MENOS y es la mas prescindible." << std::endl;

            int frameToUse = pageTable.getPageEntry(pageToReplace).frameNumber;

            std::cout << "  │" << std::endl;
            std::cout << "  │ [SWAP OUT] Pagina " << pageToReplace
                      << " -> sale del Marco " << frameToUse << std::endl;

            pageTable.getPageEntry(pageToReplace).presentBit = false;
            pageTable.getPageEntry(pageToReplace).frameNumber = -1;
            tlb.invalidate(pageToReplace);

            pageTable.assignFrame(pageNumber, frameToUse);

            std::cout << "  │ [SWAP IN ] Pagina " << pageNumber
                      << " -> entra en Marco " << frameToUse << std::endl;
            std::cout << "  └───────────────────────────────────────────────────────────────────┘" << std::endl;
        } else {
            std::cout << "  └─ [ERROR] No se encontro pagina para reemplazar." << std::endl;
            return false;
        }
    }
    
    // Configurar bits de la nueva página
    pageTable.getPageEntry(pageNumber).accessCount++;
    pageTable.getPageEntry(pageNumber).referenceBit = true;
    
    if (write) {
        pageTable.markDirty(pageNumber);
    }
    
    // Actualizar TLB
    frameNumber = pageTable.getPageEntry(pageNumber).frameNumber;
    tlb.insert(pageNumber, frameNumber);
    
    return true;
}

long long MMU::translateAddress(long long virtualAddress) {
    int pageNumber = virtualAddress / pageSize;
    int offset = virtualAddress % pageSize;
    
    if (pageNumber >= pageTable.getTotalPages()) {
        return -1;  // Direccion virtual fuera de rango
    }
    
    const MemoryPage& page = pageTable.getPageEntry(pageNumber);
    
    if (!page.presentBit) {
        return -1;  // Pagina no esta en memoria
    }
    
    long long physicalAddress = (page.frameNumber * (long long)pageSize) + offset;
    return physicalAddress;
}

void MMU::displayMMUInfo() {
    std::cout << "\n╔══════════════════════════════════════════════════════════════════════════════════════╗" << std::endl;
    std::cout << "║                  INFORMACION DE LA MEMORY MANAGEMENT UNIT (MMU)                    ║" << std::endl;
    std::cout << "╠══════════════════════════════════════════════════════════════════════════════════════╣" << std::endl;
    
    displayConfiguration();
    
    std::cout << "\n═══ TABLA DE PAGINACION ═══" << std::endl;
    pageTable.displayMemoryPages();
    
    std::cout << "\n═══ TLB (CACHÉ) ═══" << std::endl;
    tlb.display();
}

void MMU::displayStatistics() {
    std::cout << "\n╔══════════════════════════════════════════════════════════════════════════════════════╗" << std::endl;
    std::cout << "║                   ESTADÍSTICAS COMPLETAS DE LA MMU                                  ║" << std::endl;
    std::cout << "╠══════════════════════════════════════════════════════════════════════════════════════╣" << std::endl;
    
    std::cout << "║ Total de accesos a memoria (Memory Accesses): " << std::setw(38) << totalMemoryAccesses << " ║" << std::endl;
    std::cout << "║ Total de fallos de pagina (Page Faults): " << std::setw(43) << totalPageFaults << " ║" << std::endl;
    
    double faultRate = getPageFaultRate();
    std::cout << "║ Tasa de fallos de pagina (Miss Rate): " << std::fixed << std::setprecision(2);
    std::cout << std::setw(46) << (faultRate * 100.0) << "% ║" << std::endl;
    
    std::cout << "╚══════════════════════════════════════════════════════════════════════════════════════╝" << std::endl;
    
    // Mostrar estadisticas de componentes
    std::cout << "\n";
    pageTable.displayStatistics();
    std::cout << "\n";
    tlb.displayStatistics();
}

double MMU::getPageFaultRate() const {
    if (totalMemoryAccesses == 0) return 0.0;
    return (double)totalPageFaults / totalMemoryAccesses;
}

void MMU::displayConfiguration() {
    std::cout << "║ CONFIGURACION DE MEMORIA:" << std::endl;
    std::cout << "║  - Tamano de pagina (Page Size): " << std::setw(48) << pageSize << " bytes ║" << std::endl;
    std::cout << "║  - Paginas virtuales (Virtual Pages): " << std::setw(43) << pageTable.getTotalPages() << " ║" << std::endl;
    std::cout << "║  - Marcos fisicos (Physical Frames): " << std::setw(46) << pageTable.getTotalFrames() << " ║" << std::endl;
    std::cout << "║  - Espacio de Memoria Virtual (Virtual Address Space): " << std::setw(28) << virtualMemorySize << " bytes ║" << std::endl;
    std::cout << "║  - Espacio de Memoria Fisica (Physical Address Space): " << std::setw(28) << physicalMemorySize << " bytes ║" << std::endl;
    std::cout << "║  - Ratio Memoria Virtual/Fisica: " << std::setw(52) << (double)virtualMemorySize / physicalMemorySize << "x ║" << std::endl;
    
    // PTBR simulation using the pageTable object's memory address
    std::stringstream ss;
    ss << "0x" << std::hex << std::uppercase << reinterpret_cast<uintptr_t>(&pageTable);
    std::cout << "║  - Page Table Base Register (PTBR): " << std::setw(45) << ss.str() << " ║" << std::endl;
}

void MMU::resetStatistics() {
    totalPageFaults = 0;
    totalMemoryAccesses = 0;
    pageTable.incrementFaultCount();  // Reset
    tlb.resetStatistics();
}
