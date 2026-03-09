#ifndef MEMORY_PAGE_H
#define MEMORY_PAGE_H

#include <string>
#include <iostream>

/**
 * Estructura que representa una pagina de memoria
 * Incluye todos los bits de control necesarios para la administracion de memoria
 */
struct MemoryPage {
    int pageNumber;              // Numero de pagina
    int frameNumber;             // Numero de marco fisico (-1 si no esta en memoria principal)
    int lastFrameNumber;         // Último marco que tuvo antes de ser enviada al disco (-1 si nunca tuvo uno)
    bool presentBit;             // Bit de presencia (1 = en memoria principal, 0 = en disco)
    bool dirtyBit;               // Bit sucio/modificado (1 = modificada, 0 = sin cambios)
    bool referenceBit;           // Bit de referencia (1 = accedida recientemente, 0 = no accedida)
    int accessCount;             // Contador de accesos (para MFU)
    long long lastAccessTime;    // Tiempo del ultimo acceso
    bool readBit;                // Bit de lectura (Read)
    bool writeBit;               // Bit de escritura (Write)
    bool execBit;                // Bit de ejecucion (Execute)
    bool cacheBit;               // Bit de cache (Cacheable / Uncacheable)
    std::string data;            // Datos almacenados en la pagina

    MemoryPage() : pageNumber(-1), frameNumber(-1), lastFrameNumber(-1), presentBit(false), 
                   dirtyBit(false), referenceBit(false), accessCount(0), 
                   lastAccessTime(0), readBit(true), writeBit(true), execBit(false), cacheBit(true), data("") {}

    MemoryPage(int pageNum, int frameNum = -1) 
        : pageNumber(pageNum), frameNumber(frameNum), lastFrameNumber(frameNum), presentBit(frameNum != -1),
          dirtyBit(false), referenceBit(false), accessCount(0), 
          lastAccessTime(0), readBit(true), writeBit(true), execBit(false), cacheBit(true), data("") {}

    // Metodo para mostrar el estado de la pagina
    void displayStatus() const {
        std::cout << "Pagina #" << pageNumber 
                  << " | Marco: " << frameNumber << " (Ultimo: " << lastFrameNumber << ")"
                  << " | Presencia: " << (presentBit ? "SI" : "NO")
                  << " | Modificada: " << (dirtyBit ? "SI" : "NO")
                  << " | Referencia: " << (referenceBit ? "SI" : "NO")
                  << " | Permisos: " << (readBit?"R":"-") << (writeBit?"W":"-") << (execBit?"X":"-")
                  << " | Cache: " << (cacheBit ? "SI" : "NO")
                  << " | Accesos: " << accessCount
                  << " | Datos: " << (data.empty() ? "[VACIO]" : data) << std::endl;
    }
};

#endif // MEMORY_PAGE_H
