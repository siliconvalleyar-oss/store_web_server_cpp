// ============================================================
//  database.hpp - Módulo de acceso a datos
// ============================================================
//  Lee/escribe archivos JSON (productos, usuarios) y provee
//  funciones de consulta para el catálogo de productos.
// ============================================================

#ifndef DATABASE_HPP
#define DATABASE_HPP

#include <string>
#include <vector>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

// ============================================================
//  Clase Database
// ============================================================

class Database {
private:
    json productos;       // Catálogo completo de productos
    std::string ruta_productos;

public:
    /**
     * Constructor: carga los productos desde el archivo JSON.
     * @param ruta Ruta a data/productos.json
     */
    Database(const std::string& ruta);

    /**
     * Obtiene la lista completa de productos.
     * @return JSON array con todos los productos
     */
    json obtener_todos();

    /**
     * Obtiene un producto por su ID.
     * @param id ID del producto
     * @return JSON con el producto, o nullptr si no existe
     */
    json obtener_por_id(int id);

    /**
     * Verifica que un producto tenga stock suficiente.
     * @param producto_id ID del producto
     * @param cantidad Cantidad solicitada
     * @return true si hay stock suficiente
     */
    bool verificar_stock(int producto_id, int cantidad);

    /**
     * Reduce el stock de un producto (simula la compra).
     * @param producto_id ID del producto
     * @param cantidad Cantidad a descontar
     * @return true si se pudo descontar
     */
    bool descontar_stock(int producto_id, int cantidad);

    /**
     * Guarda los cambios en el archivo productos.json.
     */
    void guardar();
};

#endif // DATABASE_HPP
