


# TechStore - Tienda Electrónica Web Server

> Servidor web en C++ para tienda electrónica con SPA frontend.
> API RESTful, carrito de compras, autenticación por tokens y checkout simulado.

## Stack

| Capa | Tecnología |
|------|-----------|
| Backend | C++17 + cpp-httplib |
| Frontend | HTML5 + CSS3 + Vanilla JS (SPA) |
| Datos | JSON (nlohmann/json) |
| Build | Makefile |

## Requisitos

```bash
sudo apt install nlohmann-json3-dev g++
# cpp-httplib ya está incluido en include/httplib.h
```

## Compilar y Ejecutar

```bash
make              # Compilar
make run          # Compilar + ejecutar en http://localhost:8080
make run ARGS="--port 9090"   # Puerto personalizado
make clean        # Limpiar objetos y binario
make debug        # Compilar con -g -O0 para depuración
```

## API

| Método | Ruta | Descripción |
|--------|------|-------------|
| GET | `/api/productos` | Listar todos los productos |
| GET | `/api/productos/:id` | Detalle de un producto |
| POST | `/api/auth/login` | Iniciar sesión |
| GET | `/api/auth/session` | Verificar sesión actual |
| POST | `/api/carrito/agregar` | Agregar al carrito |
| GET | `/api/carrito` | Ver carrito |
| POST | `/api/checkout` | Procesar compra |

## Usuarios Demo

| Usuario | Contraseña | Tipo |
|---------|-----------|------|
| `admin` | `admin` | Administrador |
| `cliente` | `cliente` | Cliente |

## Arquitectura

```
Cliente (SPA) → HTTP → cpp-httplib Server
                           ├── RouteManager (dispatcher)
                           ├── Database (productos.json)
                           └── AuthManager (sesiones en memoria)
```

### Flujo de una compra

1. Usuario navega catálogo (`GET /api/productos`)
2. Inicia sesión (`POST /api/auth/login`) → recibe token
3. Agrega productos al carrito (`POST /api/carrito/agregar`)
4. Revisa carrito (`GET /api/carrito`)
5. Completa checkout (`POST /api/checkout`) → stock se descuenta

## Productos

8 productos en 3 categorías (Electrónica, Informática, Muebles).
Precios desde $59.99 hasta $449.99.

## Licencia

MIT
