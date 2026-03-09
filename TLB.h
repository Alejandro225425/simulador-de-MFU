#ifndef TLB_H
#define TLB_H

#include <unordered_map>
#include <vector>
#include <iostream>
#include <iomanip>

/**
 * Translation Lookaside Buffer (TLB)
 * Caché de pequeño tamaño que almacena mapeos recientes de página virtual a marco físico
 * Acelera la traducción de direcciones virtuales a físicas
 */
class TLB {
private:
    struct TLBEntry {
        int pageNumber;
        int frameNumber;
        int accessCount;
        long long lastAccessTime;

        TLBEntry(int page = -1, int frame = -1) 
            : pageNumber(page), frameNumber(frame), accessCount(0), lastAccessTime(0) {}
    };

    std::vector<TLBEntry> entries;
    int maxSize;
    long long accessCounter;
    int hits;
    int misses;

public:
    TLB(int size = 16);  // Tamaño típico de TLB
    
    /**
     * Busca un mapeo de página a marco en el TLB
     * @param pageNumber Número de página virtual
     * @return Número de marco si lo encuentra, -1 si no está
     */
    int lookup(int pageNumber);
    
    /**
     * Inserta un nuevo mapeo en el TLB
     * Si está lleno, reemplaza la entrada menos usada recientemente
     * @param pageNumber Número de página virtual
     * @param frameNumber Número de marco físico
     */
    void insert(int pageNumber, int frameNumber);
    
    /**
     * Invalida una entrada del TLB (se usa al reemplazar una página)
     * @param pageNumber Número de página virtual
     */
    void invalidate(int pageNumber);
    
    /**
     * Limpia todo el TLB
     */
    void clear();
    
    /**
     * Muestra el contenido actual del TLB
     */
    void display();
    
    /**
     * Obtiene estadísticas del TLB
     */
    void displayStatistics();
    
    /**
     * Obtiene la tasa de aciertos
     */
    double getHitRate() const;
    
    /**
     * Resetea las estadísticas
     */
    void resetStatistics();
};

#endif // TLB_H
