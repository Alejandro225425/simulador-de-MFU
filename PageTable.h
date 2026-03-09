#ifndef PAGE_TABLE_H
#define PAGE_TABLE_H

#include "MemoryPage.h"
#include <vector>
#include <iostream>
#include <iomanip>

/**
 * Tabla de Paginación
 * Mantiene el mapeo de todas las páginas virtuales a marcos físicos
 * Cada entrada contiene información sobre si la página está en memoria principal o en disco
 */
class PageTable {
private:
    std::vector<MemoryPage> table;
    int totalPages;
    int totalFrames;
    std::vector<bool> frameOccupancy;  // Rastrear qué marcos están ocupados
    std::vector<int> frameToPage;      // Rastrear qué página está en cada marco (Optimización O(1))
    std::vector<int> activePages;      // Rastrear páginas que han sido asignadas en algún momento
    std::vector<bool> isPageActive;    // Verificación O(1) si la página ya fue asignada
    int faultCount;  // Contador de fallos de página (page faults)
    int hitCount;    // Contador de aciertos
    
public:
    /**
     * Obtiene el número de página almacenado en un marco específico
     * @param frameNumber Número de marco
     * @return Número de página, o -1 si está vacío
     */
    int getPageInFrame(int frameNumber) const;

    /**
     * Constructor
     * @param numPages Número total de páginas virtuales
     * @param numFrames Número total de marcos físicos
     */
    PageTable(int numPages, int numFrames);
    
    /**
     * Obtiene una entrada de la tabla de paginación
     * @param pageNumber Número de página
     * @return Referencia a la página
     */
    MemoryPage& getPageEntry(int pageNumber);
    const MemoryPage& getPageEntry(int pageNumber) const;
    
    /**
     * Asigna un marco a una página
     * @param pageNumber Número de página
     * @param frameNumber Número de marco (si es -1, busca uno disponible)
     * @return Número de marco asignado, -1 si no hay disponible
     */
    int assignFrame(int pageNumber, int frameNumber = -1);
    
    /**
     * Libera un marco
     * @param frameNumber Número de marco a liberar
     */
    void freeFrame(int frameNumber);
    
    /**
     * Obtiene el primer marco disponible
     * @return Número de marco disponible, -1 si no hay
     */
    int getAvailableFrame();
    
    /**
     * Verifica si una página está en memoria principal
     * @param pageNumber Número de página
     * @return true si está en memoria, false si está en disco
     */
    bool isPageInMemory(int pageNumber) const;
    
    /**
     * Marca una entrada como modificada
     * @param pageNumber Número de página
     */
    void markDirty(int pageNumber);
    
    /**
     * Marca una entrada como accedida
     * @param pageNumber Número de página
     */
    void markReferenced(int pageNumber);
    
    /**
     * Muestra la tabla completa
     */
    void displayTable();
    
    /**
     * Muestra solo las páginas en memoria principal
     */
    void displayMemoryPages();
    
    /**
     * Muestra estad­ísticas de la tabla
     */
    void displayStatistics();
    
    /**
     * Obtiene el número total de páginas
     */
    int getTotalPages() const { return totalPages; }
    
    /**
     * Obtiene el número total de marcos
     */
    int getTotalFrames() const { return totalFrames; }
    
    /**
     * Obtiene el número de marcos disponibles
     */
    int getAvailableFrameCount() const;
    
    /**
     * Incrementa el contador de fallos
     */
    void incrementFaultCount() { faultCount++; }
    
    /**
     * Incrementa el contador de aciertos
     */
    void incrementHitCount() { hitCount++; }
    
    /**
     * Obtiene el contador de fallos
     */
    int getFaultCount() const { return faultCount; }
    
    /**
     * Obtiene el contador de aciertos
     */
    int getHitCount() const { return hitCount; }
};

#endif // PAGE_TABLE_H
