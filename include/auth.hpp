// ============================================================
//  auth.hpp - Módulo de autenticación y sesiones
// ============================================================
//  Maneja login de usuarios y sesiones mediante tokens.
//  Las sesiones se almacenan en memoria (unordered_map).
// ============================================================

#ifndef AUTH_HPP
#define AUTH_HPP

#include <string>
#include <unordered_map>
#include <vector>
#include <mutex>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

// ============================================================
//  Estructuras de datos
// ============================================================

/// Representa un ítem en el carrito de compras
struct ItemCarrito {
    int producto_id;
    int cantidad;
};

/// Representa una sesión de usuario activa
struct Sesion {
    int usuario_id;
    std::string nombre_usuario;
    std::vector<ItemCarrito> carrito;
};

// ============================================================
//  Clase AuthManager
// ============================================================

class AuthManager {
private:
    // Mapa: token → datos de sesión
    std::unordered_map<std::string, Sesion> sesiones;

    // Mapa: usuario_id → contraseña (cargado desde usuarios.json)
    std::unordered_map<std::string, json> usuarios;

    // Mutex para operaciones thread-safe
    std::mutex mtx;

    // Ruta al archivo de usuarios
    std::string ruta_usuarios;

public:
    /**
     * Constructor: carga los usuarios desde el archivo JSON.
     * @param ruta Ruta a data/usuarios.json
     */
    AuthManager(const std::string& ruta);

    /**
     * Intenta autenticar un usuario.
     * @param usuario Nombre de usuario
     * @param password Contraseña
     * @return Token de sesión si es exitoso, cadena vacía si falla
     */
    std::string login(const std::string& usuario, const std::string& password);

    /**
     * Valida que un token de sesión sea válido.
     * @param token Token a validar
     * @return true si el token es válido
     */
    bool validar_token(const std::string& token);

    /**
     * Obtiene los datos de una sesión por token.
     * @param token Token de sesión
     * @return Referencia a la sesión (lanza excepción si no existe)
     */
    Sesion& obtener_sesion(const std::string& token);

    /**
     * Obtiene la información del usuario autenticado.
     * @param token Token de sesión
     * @return JSON con datos del usuario (sin contraseña)
     */
    json obtener_info_usuario(const std::string& token);

    /**
     * Agrega un producto al carrito de una sesión.
     * @param token Token de sesión
     * @param producto_id ID del producto
     * @param cantidad Cantidad a agregar
     */
    void agregar_al_carrito(const std::string& token, int producto_id, int cantidad);

    /**
     * Obtiene el carrito de una sesión como JSON.
     * @param token Token de sesión
     * @return JSON array con los items del carrito
     */
    json obtener_carrito(const std::string& token);

    /**
     * Limpia el carrito de una sesión (después del checkout).
     * @param token Token de sesión
     */
    void limpiar_carrito(const std::string& token);

private:
    /**
     * Genera un token de sesión aleatorio.
     * @return String de 32 caracteres hexadecimales
     */
    std::string generar_token();
};

#endif // AUTH_HPP
