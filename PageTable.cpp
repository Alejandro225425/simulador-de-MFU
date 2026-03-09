#include "PageTable.h"
#include <iomanip>
#include <algorithm>
#include <sstream>

PageTable::PageTable(int numPages, int numFrames)
    : totalPages(numPages), totalFrames(numFrames), 
      frameOccupancy(numFrames, false), frameToPage(numFrames, -1), 
      activePages(), isPageActive(numPages, false), faultCount(0), hitCount(0) {
    
    table.resize(numPages);
    for (int i = 0; i < numPages; i++) {
        table[i] = MemoryPage(i);
    }
}

MemoryPage& PageTable::getPageEntry(int pageNumber) {
    if (pageNumber >= 0 && pageNumber < totalPages) {
        return table[pageNumber];
    }
    throw std::out_of_range("Número de página inválido");
}

const MemoryPage& PageTable::getPageEntry(int pageNumber) const {
    if (pageNumber >= 0 && pageNumber < totalPages) {
        return table[pageNumber];
    }
    throw std::out_of_range("Número de página inválido");
}

int PageTable::getPageInFrame(int frameNumber) const {
    if (frameNumber >= 0 && frameNumber < totalFrames) {
        return frameToPage[frameNumber];
    }
    return -1;
}

int PageTable::assignFrame(int pageNumber, int frameNumber) {
    MemoryPage& page = getPageEntry(pageNumber);
    
    if (!isPageActive[pageNumber]) {
        isPageActive[pageNumber] = true;
        activePages.push_back(pageNumber);
    }
    
    if (frameNumber == -1) {
        frameNumber = getAvailableFrame();
        if (frameNumber == -1) {
            return -1;  // No hay marcos disponibles
        }
    }
    
    // Verificar que el marco no esté ligado a otra página
    int oldPage = frameToPage[frameNumber];
    if (oldPage != -1 && oldPage != pageNumber) {
        table[oldPage].lastFrameNumber = table[oldPage].frameNumber; // Guardar el último marco
        table[oldPage].frameNumber = -1;
        table[oldPage].presentBit = false;
    }
    
    page.lastFrameNumber = frameNumber; // Actualizar su último marco al actual
    page.frameNumber = frameNumber;
    page.presentBit = true;
    frameOccupancy[frameNumber] = true;
    frameToPage[frameNumber] = pageNumber;
    
    return frameNumber;
}

void PageTable::freeFrame(int frameNumber) {
    if (frameNumber >= 0 && frameNumber < totalFrames) {
        frameOccupancy[frameNumber] = false;
        
        // Buscar la página asociada a este marco y actualizar su estado
        int pageNumber = frameToPage[frameNumber];
        if (pageNumber != -1) {
            table[pageNumber].lastFrameNumber = table[pageNumber].frameNumber;
            table[pageNumber].frameNumber = -1;
            table[pageNumber].presentBit = false;
            frameToPage[frameNumber] = -1;
        }
    }
}

int PageTable::getAvailableFrame() {
    for (int i = 0; i < totalFrames; i++) {
        if (!frameOccupancy[i]) {
            return i;
        }
    }
    return -1;
}

bool PageTable::isPageInMemory(int pageNumber) const {
    const MemoryPage& page = getPageEntry(pageNumber);
    return page.presentBit;
}

void PageTable::markDirty(int pageNumber) {
    getPageEntry(pageNumber).dirtyBit = true;
}

void PageTable::markReferenced(int pageNumber) {
    getPageEntry(pageNumber).referenceBit = true;
}

void PageTable::displayTable() {
    std::stringstream ss;
    ss << "0x" << std::hex << std::uppercase << reinterpret_cast<uintptr_t>(this);
    std::string ptbrStr = "PTBR (Hardware Register): " + ss.str();
    
    std::cout << "\n╔═════════════════════════════════════════════════════════════════════════════════════════════════════════════════╗" << std::endl;
    std::cout << "║                                       TABLA DE PAGINACION COMPLETA                                              ║" << std::endl;
    std::cout << "║ " << ptbrStr;
    int spaces = 113 - 1 - ptbrStr.length();
    for(int i = 0; i < spaces; i++) std::cout << " ";
    std::cout << "║" << std::endl;
    std::cout << "╠═════════════════════════════════════════════════════════════════════════════════════════════════════════════════╣" << std::endl;
    std::cout << "║ Pagina │ Marco │ Presencia │ Modificada │ Referencia │ Permisos │ Cache │ Accesos/Hits │ Estado               ║" << std::endl;
    std::cout << "╠═════════════════════════════════════════════════════════════════════════════════════════════════════════════════╣" << std::endl;
    
    // Ordenar páginas activas para visualización
    std::vector<int> sortedPages = activePages;
    std::sort(sortedPages.begin(), sortedPages.end());
    
    // Mostrar todas las páginas asignadas (ya sea en RAM o SWAP)
    for (int pageNumber : sortedPages) {
        const MemoryPage& page = table[pageNumber];
        
        std::cout << "║ " << std::setw(6) << page.pageNumber << " │ ";
        if (page.presentBit) {
            std::cout << std::setw(5) << page.frameNumber << " │ ";
        } else {
            if (page.lastFrameNumber != -1) {
                // Mostrar el último marco (anterior)
                std::string prevFrame = "Ant[" + std::to_string(page.lastFrameNumber) + "]";
                std::cout << std::setw(5) << prevFrame << " │ ";
            } else {
                std::cout << std::setw(5) << "-" << " │ ";
            }
        }
        
        std::cout << std::setw(9) << (page.presentBit ? "SI" : "NO") << " │ ";
        std::cout << std::setw(10) << (page.dirtyBit ? "SI" : "NO") << " │ ";
        std::cout << std::setw(10) << (page.referenceBit ? "SI" : "NO") << " │ ";
        
        std::string perms = (page.readBit?"R":"-");
        perms += (page.writeBit?"W":"-");
        perms += (page.execBit?"X":"-");
        std::cout << std::setw(8) << perms << " │ ";
        std::cout << std::setw(5) << (page.cacheBit ? "SI" : "NO") << " │ ";
        
        std::cout << std::setw(12) << page.accessCount << " │ ";
        
        std::string estado = (page.presentBit ? "[EN RAM]" : "[SWAP / DISCO]");
        std::cout << std::left << std::setw(20) << estado << std::right << " ║" << std::endl;
    }
    
    // Mostrar si hay marcos físicos vacíos (opcional pero ayuda a mostrar el estado completo)
    for (int frame = 0; frame < totalFrames; frame++) {
        if (frameToPage[frame] == -1) {
            std::cout << "║ " << std::setw(6) << "-" << " │ ";
            std::cout << std::setw(5) << frame << " │ ";
            std::cout << std::setw(9) << "NO" << " │ ";
            std::cout << std::setw(10) << "-" << " │ ";
            std::cout << std::setw(10) << "-" << " │ ";
            std::cout << std::setw(8) << "-" << " │ ";
            std::cout << std::setw(5) << "-" << " │ ";
            std::cout << std::setw(12) << "0" << " │ ";
            std::cout << std::left << std::setw(20) << "[VACIO]" << std::right << " ║" << std::endl;
        }
    }
    
    // Si queremos podemos imprimir una línea informativa
    std::cout << "╠═════════════════════════════════════════════════════════════════════════════════════════════════════════════════╣" << std::endl;
    
    std::string notaStr = "Nota: Las paginas virtuales no asignadas (" + std::to_string(totalPages - activePages.size()) + " pags) no han sido tocadas aun.";
    std::cout << "║ " << notaStr;
    int notaSpaces = 113 - 1 - notaStr.length();
    for(int i = 0; i < notaSpaces; i++) std::cout << " ";
    std::cout << "║" << std::endl;
    
    std::cout << "╚═════════════════════════════════════════════════════════════════════════════════════════════════════════════════╝" << std::endl;
}

void PageTable::displayMemoryPages() {
    std::cout << "\n╔═══════════════════════════════════════════════════════════════════════════════════════════════════════╗" << std::endl;
    std::cout << "║                      PAGINAS EN MEMORIA PRINCIPAL (RAMIFICATION TABLE)                                ║" << std::endl;
    std::cout << "╠═══════════════════════════════════════════════════════════════════════════════════════════════════════╣" << std::endl;
    
    bool foundAny = false;
    std::cout << "║ Pagina │ Marco │ Modificada │ Referencia │ Permisos │ Cache │ Accesos/Hits │                      ║" << std::endl;
    std::cout << "╠═══════════════════════════════════════════════════════════════════════════════════════════════════════╣" << std::endl;
    
    for (int frame = 0; frame < totalFrames; frame++) {
        int pageNumber = frameToPage[frame];
        if (pageNumber != -1) {
            foundAny = true;
            const MemoryPage& page = table[pageNumber];
            
            std::string perms = (page.readBit?"R":"-");
            perms += (page.writeBit?"W":"-");
            perms += (page.execBit?"X":"-");
            
            std::cout << "║ " << std::setw(6) << page.pageNumber << " │ "
                      << std::setw(5) << page.frameNumber << " │ "
                      << std::setw(10) << (page.dirtyBit ? "SI" : "NO") << " │ "
                      << std::setw(10) << (page.referenceBit ? "SI" : "NO") << " │ "
                      << std::setw(8) << perms << " │ "
                      << std::setw(5) << (page.cacheBit ? "SI" : "NO") << " │ "
                      << std::setw(12) << page.accessCount << " │ "
                      << std::left << std::setw(20) << " " << std::right << " ║" << std::endl;
        }
    }
    
    if (!foundAny) {
        std::cout << "║ [NO HAY PAGINAS EN MEMORIA PRINCIPAL]" << std::setw(66) << " ║" << std::endl;
    }
    
    std::cout << "╚═══════════════════════════════════════════════════════════════════════════════════════════════════════╝" << std::endl;
}

void PageTable::displayStatistics() {
    std::cout << "\n╔═══════════════════════════════════════════════════════════════════════════════════╗" << std::endl;
    std::cout << "║                  ESTADISTICAS DE LA TABLA DE PAGINACION                         ║" << std::endl;
    std::cout << "╠═══════════════════════════════════════════════════════════════════════════════════╣" << std::endl;
    std::cout << "║ Total de paginas virtuales: " << std::setw(48) << totalPages << " ║" << std::endl;
    std::cout << "║ Total de marcos fisicos: " << std::setw(52) << totalFrames << " ║" << std::endl;
    std::cout << "║ Marcos disponibles: " << std::setw(56) << getAvailableFrameCount() << " ║" << std::endl;
    std::cout << "║ Marcos ocupados: " << std::setw(59) << (totalFrames - getAvailableFrameCount()) << " ║" << std::endl;
    std::cout << "║" << std::setw(85) << "║" << std::endl;
    std::cout << "║ Fallos de página (Page Faults): " << std::setw(50) << faultCount << " ║" << std::endl;
    std::cout << "║ Aciertos (Hits): " << std::setw(64) << hitCount << " ║" << std::endl;
    
    int total = faultCount + hitCount;
    if (total > 0) {
        double faultRate = (double)faultCount / total * 100.0;
        std::cout << "║ Tasa de fallos: " << std::fixed << std::setprecision(2);
        std::cout << std::setw(58) << faultRate << "% ║" << std::endl;
    }
    
    std::cout << "╚═══════════════════════════════════════════════════════════════════════════════════╝" << std::endl;
}

int PageTable::getAvailableFrameCount() const {
    int count = 0;
    for (bool occupied : frameOccupancy) {
        if (!occupied) count++;
    }
    return count;
}
