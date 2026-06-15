// ============================================================
//  database.cpp - Implementación del módulo de acceso a datos
// ============================================================

#include "database.hpp"
#include <fstream>
#include <iostream>
#include <stdexcept>

// ============================================================
//  Constructor
// ============================================================

Database::Database(const std::string& ruta)
    : ruta_productos(ruta) {
    // Abrir y parsear el archivo JSON de productos
    std::ifstream archivo(ruta_productos);
    if (!archivo.is_open()) {
        throw std::runtime_error(
            "No se pudo abrir el archivo de productos: " + ruta_productos
        );
    }

    try {
        archivo >> productos;
    } catch (const json::parse_error& e) {
        throw std::runtime_error(
            "Error al parsear productos.json: " + std::string(e.what())
        );
    }

    if (!productos.is_array()) {
        throw std::runtime_error(
            "productos.json debe contener un array JSON"
        );
    }

    std::cout << "📦  Catálogo cargado: " << productos.size()
              << " productos" << std::endl;
}

// ============================================================
//  obtener_todos()
// ============================================================

json Database::obtener_todos() {
    return productos;
}

// ============================================================
//  obtener_por_id()
// ============================================================

json Database::obtener_por_id(int id) {
    for (const auto& prod : productos) {
        if (prod["id"].get<int>() == id) {
            return prod;
        }
    }
    return json(); // Objeto JSON vacío → no encontrado
}

// ============================================================
//  verificar_stock()
// ============================================================

bool Database::verificar_stock(int producto_id, int cantidad) {
    for (const auto& prod : productos) {
        if (prod["id"].get<int>() == producto_id) {
            int stock = prod["stock"].get<int>();
            return stock >= cantidad;
        }
    }
    return false;
}

// ============================================================
//  descontar_stock()
// ============================================================

bool Database::descontar_stock(int producto_id, int cantidad) {
    for (auto& prod : productos) {
        if (prod["id"].get<int>() == producto_id) {
            int stock_actual = prod["stock"].get<int>();
            if (stock_actual >= cantidad) {
                prod["stock"] = stock_actual - cantidad;
                return true;
            }
            return false;
        }
    }
    return false;
}

// ============================================================
//  guardar()
// ============================================================

void Database::guardar() {
    std::ofstream archivo(ruta_productos);
    if (!archivo.is_open()) {
        std::cerr << "❌  Error: No se pudo guardar productos.json" << std::endl;
        return;
    }
    archivo << productos.dump(2) << std::endl;
}
