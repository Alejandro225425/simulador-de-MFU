#ifndef MFU_ALGORITHM_H
#define MFU_ALGORITHM_H

#include "PageTable.h"
#include <vector>
#include <algorithm>
#include <iostream>

/**
 * Algoritmo de Reemplazo MFU (Most Frequently Used)
 * Reemplaza la página que ha sido menos accedida (menor contador de accesos)
 * Basado en la suposición de que las páginas más frecuentemente usadas seguirán siendo usadas
 */
class MFUAlgorithm {
private:
    PageTable* pageTable;
    long long timestamp;  // Para rastrear el orden de accesos
    
public:
    MFUAlgorithm(PageTable* table);
    
    /**
     * Selecciona una página para ser reemplazada usando el algoritmo MFU
     * Elige la página con el menor contador de accesos entre las páginas en memoria
     * @return Número de página a reemplazar, -1 si no hay páginas que reemplazar
     */
    int selectPageForReplacement();
    
    /**
     * Reemplaza una página con otra usando el algoritmo MFU
     * @param oldPageNumber Número de página antigua (será reemplazada)
     * @param newPageNumber Número de página nueva
     * @return true si el reemplazo fue exitoso, false si no
     */
    bool replacePage(int oldPageNumber, int newPageNumber);
    
    /**
     * Registra un acceso a una página
     * Incrementa el contador de accesos y actualiza el timestamp
     * @param pageNumber Número de página accedida
     */
    void recordPageAccess(int pageNumber);
    
    /**
     * Muestra el estado de todas las páginas ordenadas por frecuencia de acceso
     */
    void displayPagesByFrequency();
    
    /**
     * Resets estadísticas del algoritmo
     */
    void reset();
};

#endif // MFU_ALGORITHM_H
