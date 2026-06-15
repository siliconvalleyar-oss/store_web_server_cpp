// ============================================================
//  routes.hpp - Módulo de rutas del servidor web
// ============================================================
//  Define los handlers para cada endpoint de la API REST.
//  Usa cpp-httplib para el manejo de peticiones HTTP.
// ============================================================

#ifndef ROUTES_HPP
#define ROUTES_HPP

#include <string>
#include <memory>
#include "httplib.h"
#include "database.hpp"
#include "auth.hpp"

// ============================================================
//  Clase RouteManager
// ============================================================

class RouteManager {
private:
    Database& db;
    AuthManager& auth;

public:
    /**
     * Constructor.
     * @param db Referencia a la instancia de Database
     * @param auth Referencia a la instancia de AuthManager
     */
    RouteManager(Database& db, AuthManager& auth);

    /**
     * Registra todas las rutas en el servidor.
     * @param svr Referencia al servidor httplib
     */
    void registrar_rutas(httplib::Server& svr);

private:
    // ---- Handlers de API ----

    /// GET /api/productos → lista todos los productos
    void handle_listar_productos(const httplib::Request& req, httplib::Response& res);

    /// GET /api/productos/:id → detalle de un producto
    void handle_detalle_producto(const httplib::Request& req, httplib::Response& res);

    /// POST /api/auth/login → iniciar sesión
    void handle_login(const httplib::Request& req, httplib::Response& res);

    /// GET /api/auth/session → verificar sesión actual
    void handle_verificar_sesion(const httplib::Request& req, httplib::Response& res);

    /// POST /api/carrito/agregar → agregar producto al carrito
    void handle_agregar_carrito(const httplib::Request& req, httplib::Response& res);

    /// GET /api/carrito → ver el carrito actual
    void handle_ver_carrito(const httplib::Request& req, httplib::Response& res);

    /// POST /api/checkout → procesar compra
    void handle_checkout(const httplib::Request& req, httplib::Response& res);

    /**
     * Obtiene el token de sesión desde los headers o cookies.
     * @param req Petición HTTP
     * @return Token de sesión o cadena vacía
     */
    std::string extraer_token(const httplib::Request& req);
};

#endif // ROUTES_HPP
