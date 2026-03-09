#include "TLB.h"
#include <algorithm>

TLB::TLB(int size) : maxSize(size), accessCounter(0), hits(0), misses(0) {}

int TLB::lookup(int pageNumber) {
    accessCounter++;
    
    for (auto& entry : entries) {
        if (entry.pageNumber == pageNumber) {
            entry.accessCount++;
            entry.lastAccessTime = accessCounter;
            hits++;
            return entry.frameNumber;
        }
    }
    
    misses++;
    return -1;
}

void TLB::insert(int pageNumber, int frameNumber) {
    // Si ya existe, actualizar
    for (auto& entry : entries) {
        if (entry.pageNumber == pageNumber) {
            entry.frameNumber = frameNumber;
            entry.lastAccessTime = accessCounter;
            return;
        }
    }
    
    // Si está lleno, remover la entrada menos usada recientemente
    if ((int)entries.size() >= maxSize) {
        auto minElement = std::min_element(entries.begin(), entries.end(),
            [](const TLBEntry& a, const TLBEntry& b) {
                return a.lastAccessTime < b.lastAccessTime;
            });
        entries.erase(minElement);
    }
    
    // Insertar nueva entrada
    TLBEntry newEntry(pageNumber, frameNumber);
    newEntry.lastAccessTime = accessCounter;
    entries.push_back(newEntry);
}

void TLB::invalidate(int pageNumber) {
    auto it = std::find_if(entries.begin(), entries.end(),
        [pageNumber](const TLBEntry& e) { return e.pageNumber == pageNumber; });
    
    if (it != entries.end()) {
        entries.erase(it);
    }
}

void TLB::clear() {
    entries.clear();
}

void TLB::display() {
    std::cout << "\n╔════════════════════════════════════════════════════════════╗" << std::endl;
    std::cout << "║          TRANSLATION LOOKASIDE BUFFER (TLB)                 ║" << std::endl;
    std::cout << "╠════════════════════════════════════════════════════════════╣" << std::endl;
    std::cout << "║ Tamano maximo: " << std::setw(45) << maxSize << " ║" << std::endl;
    std::cout << "║ Entradas en uso: " << std::setw(43) << entries.size() << " ║" << std::endl;
    std::cout << "╠════════════════════════════════════════════════════════════╣" << std::endl;
    
    if (entries.empty()) {
        std::cout << "║ [VACIO]" << std::setw(52) << "║" << std::endl;
    } else {
        std::cout << "║ PagVirtual │ Marco │ Accesos │ UltimoAcceso │ " << std::endl;
        std::cout << "╠════════════════════════════════════════════════════════════╣" << std::endl;
        for (const auto& entry : entries) {
            std::cout << "║ " << std::setw(10) << entry.pageNumber
                      << " │ " << std::setw(5) << entry.frameNumber
                      << " │ " << std::setw(7) << entry.accessCount
                      << " │ " << std::setw(12) << entry.lastAccessTime << " │" << std::endl;
        }
    }
    
    std::cout << "╚════════════════════════════════════════════════════════════╝" << std::endl;
}

void TLB::displayStatistics() {
    std::cout << "\n╔════════════════════════════════════════════════════════════╗" << std::endl;
    std::cout << "║             ESTADISTICAS DEL TLB                           ║" << std::endl;
    std::cout << "╠════════════════════════════════════════════════════════════╣" << std::endl;
    std::cout << "║ Aciertos (Hits): " << std::setw(40) << hits << " ║" << std::endl;
    std::cout << "║ Fallos (Misses): " << std::setw(40) << misses << " ║" << std::endl;
    
    double hitRate = getHitRate();
    std::cout << "║ Tasa de aciertos: " << std::fixed << std::setprecision(2);
    std::cout << std::setw(38) << (hitRate * 100.0) << "% ║" << std::endl;
    
    std::cout << "║ Total de accesos: " << std::setw(40) << (hits + misses) << " ║" << std::endl;
    std::cout << "╚════════════════════════════════════════════════════════════╝" << std::endl;
}

double TLB::getHitRate() const {
    int total = hits + misses;
    if (total == 0) return 0.0;
    return (double)hits / total;
}

void TLB::resetStatistics() {
    hits = 0;
    misses = 0;
    accessCounter = 0;
}
