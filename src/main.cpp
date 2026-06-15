// ============================================================
//  main.cpp - Punto de entrada del servidor web
// ============================================================
//  Tienda Electrónica - Servidor en C++ con cpp-httplib
//
//  Compilación:
//    make
//  Ejecución:
//    make run
//
//  Servidor en: http://localhost:8080
//
//  Dependencias:
//    - cpp-httplib (incluido como header en include/httplib.h)
//    - nlohmann-json3-dev (sudo apt install nlohmann-json3-dev)
//    - g++ con soporte C++17
// ============================================================

#include <iostream>
#include <string>
#include "httplib.h"
#include "database.hpp"
#include "auth.hpp"
#include "routes.hpp"

// ============================================================
//  Configuración del servidor
// ============================================================

const int PUERTO_DEFECTO = 8080;
const std::string HOST = "0.0.0.0";
const std::string CARPETA_PUBLIC = "./public";
const std::string CARPETA_TEMPLATES = "./templates";

// ============================================================
//  Ruta del archivo de productos (data/productos.json)
// ============================================================

const std::string RUTA_PRODUCTOS = "./data/productos.json";
const std::string RUTA_USUARIOS = "./data/usuarios.json";

// ============================================================
//  parsear_argumentos()
// ============================================================
//  Analiza argv en busca de --port <numero>.
//  Si no se encuentra, retorna el puerto por defecto.
// ============================================================

int parsear_argumentos(int argc, char* argv[]) {
    int puerto = PUERTO_DEFECTO;

    for (int i = 1; i < argc - 1; ++i) {
        std::string arg = argv[i];
        if (arg == "--port" || arg == "-p") {
            try {
                puerto = std::stoi(argv[i + 1]);
                if (puerto < 1 || puerto > 65535) {
                    std::cerr << "⚠️  Puerto inválido: " << argv[i + 1]
                              << ". Usando puerto " << PUERTO_DEFECTO << std::endl;
                    puerto = PUERTO_DEFECTO;
                }
            } catch (const std::exception&) {
                std::cerr << "⚠️  Argumento inválido para --port: '"
                          << argv[i + 1] << "'. Usando puerto "
                          << PUERTO_DEFECTO << std::endl;
                puerto = PUERTO_DEFECTO;
            }
            break;
        }
    }

    return puerto;
}

// ============================================================
//  main()
// ============================================================

int main(int argc, char* argv[]) {
    int PUERTO = parsear_argumentos(argc, argv);

    std::cout << std::endl;
    std::cout << "==========================================" << std::endl;
    std::cout << "  🛒  TIENDA ELECTRÓNICA - SERVIDOR C++"    << std::endl;
    std::cout << "==========================================" << std::endl;
    std::cout << std::endl;

    // ── 1. Inicializar módulos ──
    std::cout << "📂  Inicializando módulos..." << std::endl;

    Database db(RUTA_PRODUCTOS);
    AuthManager auth(RUTA_USUARIOS);
    RouteManager routes(db, auth);

    // ── 2. Crear servidor HTTP ──
    std::cout << "🌐  Creando servidor HTTP..." << std::endl;
    httplib::Server svr;

    // ── 3. Configurar rutas de la API ──
    routes.registrar_rutas(svr);

    // ── 4. Configurar archivos estáticos ──
    // Servir archivos desde ./public (index.html, css/, js/)
    svr.set_mount_point("/", CARPETA_PUBLIC);
    std::cout << "📁  Archivos estáticos: " << CARPETA_PUBLIC << std::endl;

    // ── 5. Configurar manejo de errores ──
    svr.set_error_handler([](const httplib::Request& /*req*/, httplib::Response& res) {
        if (res.status == 404) {
            res.set_content(
                "<html><body><h1>404 - Página no encontrada</h1>"
                "<p>La ruta solicitada no existe.</p>"
                "<a href='/'>Volver al inicio</a></body></html>",
                "text/html"
            );
        }
    });

    // ── 6. Mensaje de inicio ──
    std::cout << std::endl;
    std::cout << "✅  Servidor listo!" << std::endl;
    std::cout << "──────────────────────────────" << std::endl;
    std::cout << "   URL:     http://localhost:"
              << PUERTO << std::endl;
    std::cout << "   API:     http://localhost:"
              << PUERTO << "/api/productos" << std::endl;
    std::cout << "   Usuarios: admin/admin, cliente/cliente" << std::endl;
    std::cout << "──────────────────────────────" << std::endl;
    std::cout << "   Presiona Ctrl+C para detener el servidor" << std::endl;
    std::cout << std::endl;

    // ── 7. Iniciar servidor ──
    svr.listen(HOST, PUERTO);

    return 0;
}
