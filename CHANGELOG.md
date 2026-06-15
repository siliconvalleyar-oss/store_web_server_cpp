# Changelog

## [1.0.0] - 2026-06-15

### Added
- Servidor web C++ con cpp-httplib (header-only)
- API RESTful para tienda electrónica
- Catálogo de productos con filtros por categoría
- Autenticación por tokens de sesión (Bearer + Cookie)
- Carrito de compras por sesión (en memoria)
- Checkout con validación de stock y descuento
- Frontend SPA con tema oscuro y glassmorphism
- Archivos de datos en JSON (productos + usuarios)
- Páginas HTML standalone (alternativas a SPA)
- Soporte para flag `--port` / `-p` en CLI
- Manejo de errores 404 personalizado
- Makefile con targets: all, clean, run, debug

### Security
- Las contraseñas se almacenan en texto plano (solo demo)
- Los tokens se generan con `std::random_device` + `std::mt19937`
- No hay expiración de sesiones (viven hasta reiniciar el server)
