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
    std::cout << "║         ESTADO DE RAM - PAGINAS POR FRECUENCIA (ALGORITMO MFU)                     ║" << std::endl;
    std::cout << "╠══════════════════════════════════════════════════════════════════════════════════════╣" << std::endl;
    std::cout << "║ Como funciona MFU: cuando la RAM se llena y llega una pagina nueva, se expulsa      ║" << std::endl;
    std::cout << "║ la pagina con MENOR frecuencia de acceso (es la menos usada = mas prescindible).    ║" << std::endl;
    std::cout << "╠══════════════════════════════════════════════════════════════════════════════════════╣" << std::endl;

    std::vector<std::pair<int, int>> pageFrequency;  // (pageNumber, accessCount)

    for (int frame = 0; frame < pageTable->getTotalFrames(); frame++) {
        int pageNum = pageTable->getPageInFrame(frame);
        if (pageNum != -1) {
            const MemoryPage& page = pageTable->getPageEntry(pageNum);
            pageFrequency.push_back({pageNum, page.accessCount});
        }
    }

    // Ordenar de mayor a menor frecuencia (MFU: la ultima = victima potencial)
    std::sort(pageFrequency.begin(), pageFrequency.end(),
        [](const std::pair<int, int>& a, const std::pair<int, int>& b) {
            return a.second > b.second;
        });

    std::cout << "║                                                                                      ║" << std::endl;
    std::cout << "║  Pos  │ P.Virtual │ Marco │ Frecuencia │ Dirty │ Ref. │ Rol en MFU                  ║" << std::endl;
    std::cout << "║ ──────┼───────────┼───────┼────────────┼───────┼──────┼───────────────────────────  ║" << std::endl;

    if (pageFrequency.empty()) {
        std::cout << "║  [SIN DATOS] Ejecuta primero la simulacion (opcion 1)                            ║" << std::endl;
    } else {
        for (size_t i = 0; i < pageFrequency.size(); i++) {
            int pageNum   = pageFrequency[i].first;
            int frequency = pageFrequency[i].second;
            const MemoryPage& page = pageTable->getPageEntry(pageNum);

            // Rol explicativo segun posicion en el ranking
            std::string rol;
            if (pageFrequency.size() == 1) {
                rol = "<-- UNICA (victima si hay fallo)";
            } else if (i == 0) {
                rol = "   MAS USADA - protegida";
            } else if (i == pageFrequency.size() - 1) {
                rol = "<== VICTIMA MFU (menos usada)";
            } else {
                rol = "   intermedia";
            }

            std::cout << "║  " << std::setw(4) << (i + 1)
                      << " │ " << std::setw(9) << pageNum
                      << " │ " << std::setw(5)  << page.frameNumber
                      << " │ " << std::setw(10) << frequency
                      << " │ " << std::setw(5)  << (page.dirtyBit     ? "SI" : "NO")
                      << " │ " << std::setw(4)  << (page.referenceBit ? "SI" : "NO")
                      << " │ " << std::left << std::setw(29) << rol << std::right << " ║" << std::endl;
        }
    }

    std::cout << "╠══════════════════════════════════════════════════════════════════════════════════════╣" << std::endl;

    // Mostrar proxima victima
    if (!pageFrequency.empty()) {
        int victima    = pageFrequency.back().first;
        int freqVict   = pageFrequency.back().second;
        int frameVict  = pageTable->getPageEntry(victima).frameNumber;
        std::cout << "║ PROXIMA VICTIMA si entra una pagina nueva:                                           ║" << std::endl;
        std::cout << "║   Pagina " << std::setw(6) << victima
                  << " (Marco " << frameVict << ", frecuencia=" << freqVict
                  << ") sera expulsada." << std::setw(20) << " ║" << std::endl;
        std::cout << "║   Razon: menor frecuencia = menos necesaria para mantener en RAM.                    ║" << std::endl;
    }

    std::cout << "╠══════════════════════════════════════════════════════════════════════════════════════╣" << std::endl;
    std::cout << "║ LEYENDA DE COLUMNAS:                                                                 ║" << std::endl;
    std::cout << "║  Frecuencia : num. de veces que la pagina fue accedida (criterio de decision MFU)   ║" << std::endl;
    std::cout << "║  Dirty      : SI = pagina modificada (necesita escribirse a disco antes de salir)   ║" << std::endl;
    std::cout << "║  Ref.       : SI = fue accedida recientemente (Reference Bit activo)                ║" << std::endl;
    std::cout << "╚══════════════════════════════════════════════════════════════════════════════════════╝" << std::endl;
}

void MFUAlgorithm::reset() {
    timestamp = 0;
    for (int i = 0; i < pageTable->getTotalPages(); i++) {
        pageTable->getPageEntry(i).accessCount = 0;
        pageTable->getPageEntry(i).referenceBit = false;
    }
}
