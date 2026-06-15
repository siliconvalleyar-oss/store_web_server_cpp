// ============================================================
//  routes.cpp - Implementación de los handlers de rutas
// ============================================================

#include "routes.hpp"
#include <iostream>
#include <sstream>
#include <cstdlib>   // srand, rand
#include <ctime>     // time

// ============================================================
//  Constructor
// ============================================================

RouteManager::RouteManager(Database& database, AuthManager& auth_manager)
    : db(database), auth(auth_manager) {}

// ============================================================
//  registrar_rutas()
// ============================================================

void RouteManager::registrar_rutas(httplib::Server& svr) {
    // ---- Rutas de productos ----
    svr.Get("/api/productos", [this](const httplib::Request& req, httplib::Response& res) {
        handle_listar_productos(req, res);
    });

    svr.Get(R"(/api/productos/(\d+))", [this](const httplib::Request& req, httplib::Response& res) {
        handle_detalle_producto(req, res);
    });

    // ---- Rutas de autenticación ----
    svr.Post("/api/auth/login", [this](const httplib::Request& req, httplib::Response& res) {
        handle_login(req, res);
    });

    svr.Get("/api/auth/session", [this](const httplib::Request& req, httplib::Response& res) {
        handle_verificar_sesion(req, res);
    });

    // ---- Rutas de carrito ----
    svr.Post("/api/carrito/agregar", [this](const httplib::Request& req, httplib::Response& res) {
        handle_agregar_carrito(req, res);
    });

    svr.Get("/api/carrito", [this](const httplib::Request& req, httplib::Response& res) {
        handle_ver_carrito(req, res);
    });

    // ---- Ruta de checkout ----
    svr.Post("/api/checkout", [this](const httplib::Request& req, httplib::Response& res) {
        handle_checkout(req, res);
    });

    std::cout << "🛣️   Rutas registradas: productos, auth, carrito, checkout" << std::endl;
}

// ============================================================
//  extraer_token()
// ============================================================

std::string RouteManager::extraer_token(const httplib::Request& req) {
    // Intentar obtener token del header Authorization
    auto auth_header = req.get_header_value("Authorization");
    if (!auth_header.empty()) {
        // Formato esperado: "Bearer <token>"
        if (auth_header.size() > 7 && auth_header.substr(0, 7) == "Bearer ") {
            return auth_header.substr(7);
        }
    }

    // Intentar obtener token de la cookie
    auto cookie = req.get_header_value("Cookie");
    if (!cookie.empty()) {
        // Buscar "token=<valor>"
        size_t pos = cookie.find("token=");
        if (pos != std::string::npos) {
            size_t start = pos + 6;
            size_t end = cookie.find(";", start);
            if (end == std::string::npos) {
                return cookie.substr(start);
            }
            return cookie.substr(start, end - start);
        }
    }

    return "";
}

// ============================================================
//  handle_listar_productos()  -  GET /api/productos
// ============================================================

void RouteManager::handle_listar_productos(const httplib::Request& req, httplib::Response& res) {
    res.set_header("Access-Control-Allow-Origin", "*");
    res.set_header("Content-Type", "application/json");

    json productos = db.obtener_todos();
    res.status = 200;
    res.body = productos.dump();
}

// ============================================================
//  handle_detalle_producto()  -  GET /api/productos/:id
// ============================================================

void RouteManager::handle_detalle_producto(const httplib::Request& req, httplib::Response& res) {
    res.set_header("Access-Control-Allow-Origin", "*");
    res.set_header("Content-Type", "application/json");

    // Extraer el ID desde la URL (capturado por el regex)
    int id = std::stoi(req.matches[1]);

    json producto = db.obtener_por_id(id);
    if (producto.is_null() || producto.empty()) {
        json error;
        error["error"] = "Producto no encontrado";
        error["codigo"] = 404;
        res.status = 404;
        res.body = error.dump();
        return;
    }

    res.status = 200;
    res.body = producto.dump();
}

// ============================================================
//  handle_login()  -  POST /api/auth/login
// ============================================================

void RouteManager::handle_login(const httplib::Request& req, httplib::Response& res) {
    res.set_header("Access-Control-Allow-Origin", "*");
    res.set_header("Content-Type", "application/json");

    try {
        json body = json::parse(req.body);
        std::string usuario = body["usuario"].get<std::string>();
        std::string password = body["password"].get<std::string>();

        std::string token = auth.login(usuario, password);

        if (token.empty()) {
            json error;
            error["error"] = "Credenciales inválidas";
            res.status = 401;
            res.body = error.dump();
            return;
        }

        json respuesta;
        respuesta["token"] = token;
        respuesta["usuario"] = usuario;
        respuesta["mensaje"] = "Inicio de sesión exitoso";
        res.status = 200;
        res.body = respuesta.dump();

    } catch (const json::parse_error& e) {
        json error;
        error["error"] = "JSON inválido: " + std::string(e.what());
        res.status = 400;
        res.body = error.dump();
    } catch (const std::exception& e) {
        json error;
        error["error"] = e.what();
        res.status = 500;
        res.body = error.dump();
    }
}

// ============================================================
//  handle_verificar_sesion()  -  GET /api/auth/session
// ============================================================

void RouteManager::handle_verificar_sesion(const httplib::Request& req, httplib::Response& res) {
    res.set_header("Access-Control-Allow-Origin", "*");
    res.set_header("Content-Type", "application/json");

    std::string token = extraer_token(req);

    if (token.empty() || !auth.validar_token(token)) {
        json error;
        error["autenticado"] = false;
        error["error"] = "No autenticado";
        res.status = 401;
        res.body = error.dump();
        return;
    }

    json info = auth.obtener_info_usuario(token);
    info["autenticado"] = true;
    res.status = 200;
    res.body = info.dump();
}

// ============================================================
//  handle_agregar_carrito()  -  POST /api/carrito/agregar
// ============================================================

void RouteManager::handle_agregar_carrito(const httplib::Request& req, httplib::Response& res) {
    res.set_header("Access-Control-Allow-Origin", "*");
    res.set_header("Content-Type", "application/json");

    std::string token = extraer_token(req);
    if (token.empty() || !auth.validar_token(token)) {
        json error;
        error["error"] = "Debes iniciar sesión primero";
        res.status = 401;
        res.body = error.dump();
        return;
    }

    try {
        json body = json::parse(req.body);
        int producto_id = body["producto_id"].get<int>();
        int cantidad = body.value("cantidad", 1);

        // Verificar que el producto existe y tiene stock
        json producto = db.obtener_por_id(producto_id);
        if (producto.empty()) {
            json error;
            error["error"] = "Producto no encontrado";
            res.status = 404;
            res.body = error.dump();
            return;
        }

        if (!db.verificar_stock(producto_id, cantidad)) {
            json error;
            error["error"] = "Stock insuficiente";
            error["stock_disponible"] = producto["stock"];
            res.status = 400;
            res.body = error.dump();
            return;
        }

        auth.agregar_al_carrito(token, producto_id, cantidad);

        json respuesta;
        respuesta["mensaje"] = "Producto agregado al carrito";
        respuesta["producto"] = producto["nombre"];
        respuesta["cantidad"] = cantidad;
        res.status = 200;
        res.body = respuesta.dump();

    } catch (const json::parse_error& e) {
        json error;
        error["error"] = "JSON inválido: " + std::string(e.what());
        res.status = 400;
        res.body = error.dump();
    }
}

// ============================================================
//  handle_ver_carrito()  -  GET /api/carrito
// ============================================================

void RouteManager::handle_ver_carrito(const httplib::Request& req, httplib::Response& res) {
    res.set_header("Access-Control-Allow-Origin", "*");
    res.set_header("Content-Type", "application/json");

    std::string token = extraer_token(req);
    if (token.empty() || !auth.validar_token(token)) {
        json error;
        error["error"] = "Debes iniciar sesión primero";
        res.status = 401;
        res.body = error.dump();
        return;
    }

    // Obtener items del carrito
    json items_carrito = auth.obtener_carrito(token);

    // Expandir con información completa de cada producto
    json carrito_completo = json::array();
    double total = 0.0;

    for (const auto& item : items_carrito) {
        int prod_id = item["producto_id"].get<int>();
        json producto = db.obtener_por_id(prod_id);

        if (!producto.empty()) {
            json entry = producto;
            entry["cantidad"] = item["cantidad"];
            entry["subtotal"] = producto["precio"].get<double>() * item["cantidad"].get<int>();
            total += entry["subtotal"].get<double>();
            carrito_completo.push_back(entry);
        }
    }

    json respuesta;
    respuesta["items"] = carrito_completo;
    respuesta["total"] = total;
    respuesta["cantidad_items"] = items_carrito.size();

    res.status = 200;
    res.body = respuesta.dump();
}

// ============================================================
//  handle_checkout()  -  POST /api/checkout
// ============================================================

void RouteManager::handle_checkout(const httplib::Request& req, httplib::Response& res) {
    res.set_header("Access-Control-Allow-Origin", "*");
    res.set_header("Content-Type", "application/json");

    std::string token = extraer_token(req);
    if (token.empty() || !auth.validar_token(token)) {
        json error;
        error["error"] = "Debes iniciar sesión primero";
        res.status = 401;
        res.body = error.dump();
        return;
    }

    try {
        // Obtener datos del formulario de checkout
        json body = json::parse(req.body);
        std::string nombre = body.value("nombre", "");
        std::string direccion = body.value("direccion", "");
        std::string tarjeta = body.value("tarjeta", "");

        if (nombre.empty() || direccion.empty() || tarjeta.empty()) {
            json error;
            error["error"] = "Todos los campos son obligatorios";
            res.status = 400;
            res.body = error.dump();
            return;
        }

        // Obtener carrito
        json items_carrito = auth.obtener_carrito(token);
        if (items_carrito.empty()) {
            json error;
            error["error"] = "El carrito está vacío";
            res.status = 400;
            res.body = error.dump();
            return;
        }

        // Verificar stock y descontar
        for (const auto& item : items_carrito) {
            int prod_id = item["producto_id"].get<int>();
            int cantidad = item["cantidad"].get<int>();

            if (!db.descontar_stock(prod_id, cantidad)) {
                json error;
                error["error"] = "Error al procesar el stock";
                error["producto_id"] = prod_id;
                res.status = 500;
                res.body = error.dump();
                return;
            }
        }

        // Guardar cambios en productos.json
        db.guardar();

        // Calcular total
        double total = 0.0;
        for (const auto& item : items_carrito) {
            int prod_id = item["producto_id"].get<int>();
            json producto = db.obtener_por_id(prod_id);
            if (!producto.empty()) {
                total += producto["precio"].get<double>() * item["cantidad"].get<int>();
            }
        }

        // Generar número de pedido simulado (seed única por ejecución)
        static bool seed_inicializada = false;
        if (!seed_inicializada) {
            srand(static_cast<unsigned>(time(nullptr)));
            seed_inicializada = true;
        }
        std::string pedido_id = "PED-" + std::to_string(rand() % 100000 + 10000);

        // Limpiar carrito
        auth.limpiar_carrito(token);

        // Respuesta exitosa
        json respuesta;
        respuesta["exito"] = true;
        respuesta["mensaje"] = "¡Compra realizada con éxito!";
        respuesta["pedido_id"] = pedido_id;
        respuesta["total"] = total;
        respuesta["nombre"] = nombre;
        respuesta["direccion"] = direccion;

        // Ocultar los últimos 4 dígitos de la tarjeta
        std::string tarjeta_oculta = "**** **** **** " + tarjeta.substr(std::max(0, (int)tarjeta.size() - 4));
        respuesta["tarjeta"] = tarjeta_oculta;

        res.status = 200;
        res.body = respuesta.dump();

    } catch (const json::parse_error& e) {
        json error;
        error["error"] = "JSON inválido: " + std::string(e.what());
        res.status = 400;
        res.body = error.dump();
    }
}
