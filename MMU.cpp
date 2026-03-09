#include "MMU.h"
#include <cmath>
#include <iomanip>
#include <sstream>

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
        // NO HAY MARCOS DISPONIBLES - se debe reemplazar una página
        // Este es donde interviene el algoritmo de reemplazo (MFU)
        // Por ahora, simplemente encontramos la página con menos accesos
        int pageToReplace = -1;
        int minAccesses = INT_MAX;
        
        for (int i = 0; i < pageTable.getTotalPages(); i++) {
            if (pageTable.getPageEntry(i).presentBit) {
                if (pageTable.getPageEntry(i).accessCount < minAccesses) {
                    minAccesses = pageTable.getPageEntry(i).accessCount;
                    pageToReplace = i;
                }
            }
        }
        
        if (pageToReplace != -1) {
            // Reemplazar la página
            int frameToUse = pageTable.getPageEntry(pageToReplace).frameNumber;
            pageTable.getPageEntry(pageToReplace).presentBit = false;
            pageTable.getPageEntry(pageToReplace).frameNumber = -1;
            tlb.invalidate(pageToReplace);
            
            pageTable.assignFrame(pageNumber, frameToUse);
        } else {
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
