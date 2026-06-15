# Skill: tienda_web_server

C++ web server for an electronics store (TechStore) using cpp-httplib + nlohmann/json.

## Project Structure

```
tienda_web_server/
├── src/
│   ├── main.cpp        # Entry, CLI args, server bootstrap
│   ├── routes.cpp      # API handlers (products, auth, cart, checkout)
│   ├── database.cpp    # JSON file product catalog
│   └── auth.cpp        # In-memory sessions, login, cart
├── include/
│   ├── routes.hpp
│   ├── database.hpp
│   ├── auth.hpp
│   └── httplib.h       # cpp-httplib (header-only HTTP library)
├── public/
│   ├── index.html      # SPA frontend
│   ├── css/styles.css  # Dark theme + glassmorphism
│   └── js/main.js      # SPA logic (fetch, render, cart, login)
├── templates/          # Standalone HTML pages (alternative to SPA)
├── data/
│   ├── productos.json  # 8 products, 3 categories
│   └── usuarios.json   # 2 users (admin, cliente)
├── bin/                # Compiled binary
├── Makefile
└── README.md
```

## Build & Run

```bash
# Install dependency
sudo apt install nlohmann-json3-dev

# Build
make

# Run (port 8080)
make run

# Custom port
make run ARGS="--port 9090"

# Debug mode
make debug
```

## API Endpoints

### Products
- `GET  /api/productos`       → List all
- `GET  /api/productos/:id`   → Get by ID

### Auth
- `POST /api/auth/login`      → Login (body: {usuario, password})
- `GET  /api/auth/session`    → Verify session (Bearer or Cookie)

### Cart
- `POST /api/carrito/agregar` → Add item (body: {producto_id, cantidad})
- `GET  /api/carrito`         → View cart

### Checkout
- `POST /api/checkout`        → Process purchase (body: {nombre, direccion, tarjeta})

## Key Patterns

### Session management
```cpp
// Token generated via std::random_device + std::mt19937 (16 bytes → hex)
// Stored in unordered_map<string, Sesion> (in-memory, lost on restart)
// Auth via Bearer header or Cookie: token=<value>
```

### Product data format
```json
{
  "id": 1,
  "nombre": "Auriculares Bluetooth Pro",
  "precio": 89.99,
  "descripcion": "...",
  "categoria": "Electrónica",
  "stock": 25,
  "imagen": "https://..."
}
```

### Adding a new endpoint
1. Add handler method in `routes.hpp`
2. Implement in `routes.cpp`
3. Register in `RouteManager::registrar_rutas()`

### Adding a new data source
1. Create JSON file in `data/`
2. Add loading in `main.cpp` constructor chain
3. Add methods in `database.hpp`/`database.cpp`
