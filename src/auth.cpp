// ============================================================
//  auth.cpp - Implementación del módulo de autenticación
// ============================================================

#include "auth.hpp"
#include <fstream>
#include <iostream>
#include <random>
#include <sstream>
#include <iomanip>
#include <stdexcept>

// ============================================================
//  Constructor
// ============================================================

AuthManager::AuthManager(const std::string& ruta)
    : ruta_usuarios(ruta) {
    // Cargar usuarios desde el archivo JSON
    std::ifstream archivo(ruta_usuarios);
    if (!archivo.is_open()) {
        std::cerr << "⚠️  Advertencia: No se pudo abrir " << ruta_usuarios
                  << ". Se usará autenticación vacía." << std::endl;
        return;
    }

    json lista_usuarios;
    try {
        archivo >> lista_usuarios;
    } catch (const json::parse_error& e) {
        std::cerr << "⚠️  Error al parsear usuarios.json: " << e.what() << std::endl;
        return;
    }

    for (const auto& u : lista_usuarios) {
        std::string usuario = u["usuario"].get<std::string>();
        usuarios[usuario] = u;
    }

    std::cout << "🔐  Usuarios cargados: " << usuarios.size() << std::endl;
}

// ============================================================
//  login()
// ============================================================

std::string AuthManager::login(const std::string& usuario, const std::string& password) {
    std::lock_guard<std::mutex> lock(mtx);

    auto it = usuarios.find(usuario);
    if (it == usuarios.end()) {
        return ""; // Usuario no encontrado
    }

    if (it->second["password"].get<std::string>() != password) {
        return ""; // Contraseña incorrecta
    }

    // Generar token único
    std::string token = generar_token();

    // Crear sesión
    Sesion sesion;
    sesion.usuario_id = it->second["id"].get<int>();
    sesion.nombre_usuario = usuario;

    sesiones[token] = sesion;
    return token;
}

// ============================================================
//  validar_token()
// ============================================================

bool AuthManager::validar_token(const std::string& token) {
    std::lock_guard<std::mutex> lock(mtx);
    return sesiones.find(token) != sesiones.end();
}

// ============================================================
//  obtener_sesion()
// ============================================================

Sesion& AuthManager::obtener_sesion(const std::string& token) {
    std::lock_guard<std::mutex> lock(mtx);
    auto it = sesiones.find(token);
    if (it == sesiones.end()) {
        throw std::runtime_error("Token de sesión inválido");
    }
    return it->second;
}

// ============================================================
//  obtener_info_usuario()
// ============================================================

json AuthManager::obtener_info_usuario(const std::string& token) {
    Sesion& sesion = obtener_sesion(token);

    auto it = usuarios.find(sesion.nombre_usuario);
    if (it == usuarios.end()) {
        return json::object();
    }

    // Devolver datos sin la contraseña
    json info = it->second;
    info.erase("password");
    return info;
}

// ============================================================
//  agregar_al_carrito()
// ============================================================

void AuthManager::agregar_al_carrito(const std::string& token,
                                      int producto_id, int cantidad) {
    Sesion& sesion = obtener_sesion(token);

    // Buscar si el producto ya está en el carrito
    for (auto& item : sesion.carrito) {
        if (item.producto_id == producto_id) {
            item.cantidad += cantidad;
            return;
        }
    }

    // Si no está, agregarlo
    sesion.carrito.push_back({producto_id, cantidad});
}

// ============================================================
//  obtener_carrito()
// ============================================================

json AuthManager::obtener_carrito(const std::string& token) {
    Sesion& sesion = obtener_sesion(token);

    json carrito_json = json::array();
    for (const auto& item : sesion.carrito) {
        json item_json;
        item_json["producto_id"] = item.producto_id;
        item_json["cantidad"] = item.cantidad;
        carrito_json.push_back(item_json);
    }

    return carrito_json;
}

// ============================================================
//  limpiar_carrito()
// ============================================================

void AuthManager::limpiar_carrito(const std::string& token) {
    Sesion& sesion = obtener_sesion(token);
    sesion.carrito.clear();
}

// ============================================================
//  generar_token()
// ============================================================

std::string AuthManager::generar_token() {
    // Generar 16 bytes aleatorios → 32 caracteres hexadecimales
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(0, 255);

    std::stringstream ss;
    for (int i = 0; i < 16; ++i) {
        ss << std::hex << std::setw(2) << std::setfill('0') << dis(gen);
    }

    return ss.str();
}
