#include "MFUAlgorithm.h"
#include <iostream>
#include <iomanip>

MFUAlgorithm::MFUAlgorithm(PageTable* table) 
    : pageTable(table), timestamp(0) {}

int MFUAlgorithm::selectPageForReplacement() {
    int selectedPage = -1;
    int minAccessCount = INT_MAX;
    
    // Buscar la página con el contador de accesos más bajo entre las que están en memoria
    for (int frame = 0; frame < pageTable->getTotalFrames(); frame++) {
        int pageNum = pageTable->getPageInFrame(frame);
        if (pageNum != -1) {
            const MemoryPage& page = pageTable->getPageEntry(pageNum);
            if (page.accessCount < minAccessCount) {
                minAccessCount = page.accessCount;
                selectedPage = pageNum;
            }
        }
    }
    
    return selectedPage;
}

bool MFUAlgorithm::replacePage(int oldPageNumber, int newPageNumber) {
    if (oldPageNumber < 0 || oldPageNumber >= pageTable->getTotalPages()) {
        return false;
    }
    
    if (newPageNumber < 0 || newPageNumber >= pageTable->getTotalPages()) {
        return false;
    }
    
    // Obtener el marco de la página antigua
    int frameNumber = pageTable->getPageEntry(oldPageNumber).frameNumber;
    
    if (frameNumber == -1) {
        return false;  // Página antigua no está en memoria
    }
    
    // Marcar la página antigua como no presente
    pageTable->getPageEntry(oldPageNumber).presentBit = false;
    pageTable->getPageEntry(oldPageNumber).frameNumber = -1;
    pageTable->getPageEntry(oldPageNumber).referenceBit = false;
    
    // Asignar el marco a la nueva página
    pageTable->getPageEntry(newPageNumber).frameNumber = frameNumber;
    pageTable->getPageEntry(newPageNumber).presentBit = true;
    pageTable->getPageEntry(newPageNumber).referenceBit = true;
    pageTable->getPageEntry(newPageNumber).accessCount = 1;
    
    return true;
}

void MFUAlgorithm::recordPageAccess(int pageNumber) {
    if (pageNumber >= 0 && pageNumber < pageTable->getTotalPages()) {
        pageTable->getPageEntry(pageNumber).accessCount++;
        pageTable->getPageEntry(pageNumber).referenceBit = true;
        timestamp++;
    }
}

void MFUAlgorithm::displayPagesByFrequency() {
    std::cout << "\n╔══════════════════════════════════════════════════════════════════════════════════════╗" << std::endl;
    std::cout << "║           PAGINA ORDENADAS POR FRECUENCIA DE ACCESO (ALGORITMO MFU)                 ║" << std::endl;
    std::cout << "╠══════════════════════════════════════════════════════════════════════════════════════╣" << std::endl;
    
    std::vector<std::pair<int, int>> pageFrequency;  // (pageNumber, accessCount)
    
    for (int frame = 0; frame < pageTable->getTotalFrames(); frame++) {
        int pageNum = pageTable->getPageInFrame(frame);
        if (pageNum != -1) {
            const MemoryPage& page = pageTable->getPageEntry(pageNum);
            pageFrequency.push_back({pageNum, page.accessCount});
        }
    }
    
    // Ordenar de mayor a menor frecuencia
    std::sort(pageFrequency.begin(), pageFrequency.end(),
        [](const std::pair<int, int>& a, const std::pair<int, int>& b) { 
            return a.second > b.second; 
        });
    
    std::cout << "║ P.Virt │ Frame │ Frecuencia │ Presencia │ Dirty Bit  │ Ref. Bit   │ Posicion │" << std::endl;
    std::cout << "╠══════════════════════════════════════════════════════════════════════════════════════╣" << std::endl;
    
    if (pageFrequency.empty()) {
        std::cout << "║ [NO HAY PAGINAS EN MEMORIA PRINCIPAL]" << std::setw(51) << "║" << std::endl;
    } else {
        for (size_t i = 0; i < pageFrequency.size(); i++) {
            int pageNum = pageFrequency[i].first;
            int frequency = pageFrequency[i].second;
            const MemoryPage& page = pageTable->getPageEntry(pageNum);
            
            std::cout << "║ " << std::setw(6) << pageNum << " │ "
                      << std::setw(5) << page.frameNumber << " │ "
                      << std::setw(10) << frequency << " │ "
                      << std::setw(9) << (page.presentBit ? "SI" : "NO") << " │ "
                      << std::setw(10) << (page.dirtyBit ? "SI" : "NO") << " │ "
                      << std::setw(10) << (page.referenceBit ? "SI" : "NO") << " │ "
                      << std::setw(8) << (i + 1) << " │" << std::endl;
        }
    }
    
    std::cout << "╠══════════════════════════════════════════════════════════════════════════════════════╣" << std::endl;
    std::cout << "║ Nota: Las paginas con MAYOR frecuencia se mantienen en RAM (Page Hit)               ║" << std::endl;
    std::cout << "║       Las paginas con MENOR frecuencia seran elegidas para Swap Out (MFU Reemplazo) ║" << std::endl;
    std::cout << "╚══════════════════════════════════════════════════════════════════════════════════════╝" << std::endl;
}

void MFUAlgorithm::reset() {
    timestamp = 0;
    for (int i = 0; i < pageTable->getTotalPages(); i++) {
        pageTable->getPageEntry(i).accessCount = 0;
        pageTable->getPageEntry(i).referenceBit = false;
    }
}
