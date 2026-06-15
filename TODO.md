# TODO — TechStore: Tienda Electrónica Web Server

## Corto plazo

- [ ] Agregar expiración de sesiones (timeout configurable)
- [ ] Hash de contraseñas (bcrypt o SHA-256) en lugar de texto plano
- [ ] Validar stock antes de mostrar "Agregar al carrito" (deshabilitar si stock=0)
- [ ] Agregar endpoint `DELETE /api/carrito/:id` para eliminar items individuales
- [ ] Manejo de CORS para todos los orígenes (ya hay headers parciales)

## Mediano plazo

- [ ] Base de datos SQLite en lugar de archivos JSON
- [ ] Registro de usuarios (`POST /api/auth/register`)
- [ ] Búsqueda de productos por nombre/descripción (`GET /api/productos?q=...`)
- [ ] Paginación en catálogo (`?page=1&limit=10`)
- [ ] Órdenes persistentes (historial de compras por usuario)
- [ ] Subida de imágenes de productos
- [ ] Rate limiting para endpoints de autenticación
- [ ] Logging a archivo con rotación

## Largo plazo

- [ ] Panel de administración web (CRUD de productos/usuarios)
- [ ] Pasarela de pago real (Stripe/MercadoPago)
- [ ] Notificaciones por email de confirmación de pedido
- [ ] API GraphQL como alternativa a REST
- [ ] Tests automatizados (Google Test para C++, Jest para frontend)
- [ ] Dockerizar la aplicación
- [ ] HTTPS con certificado Let's Encrypt

## Bugs conocidos

- [ ] `handle_agregar_carrito` no verifica si la cantidad es negativa
- [ ] `descontar_stock` no persiste cambios si `guardar()` falla silenciosamente
- [ ] Las sesiones no persisten entre reinicios del servidor
- [ ] La cookie `token=` no tiene flags `HttpOnly` ni `Secure`

## Arquitectura

- [ ] Separar `routes.cpp` en módulos: `productos_routes.cpp`, `auth_routes.cpp`, `carrito_routes.cpp`
- [ ] Agregar capa de middleware (logging, CORS, autenticación)
- [ ] Migrar a framework web más completo (crow, oatpp, drogon)
- [ ] Agregar tests unitarios con Google Test
- [ ] CI/CD con GitHub Actions
