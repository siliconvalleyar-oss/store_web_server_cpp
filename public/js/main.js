// ============================================================
//  main.js - Lógica completa del frontend TechStore
// ============================================================
//  Maneja: navegación SPA, catálogo, carrito, login, checkout
// ============================================================

// ─── CONFIGURACIÓN ───
const API_BASE = '/api';
const TOKEN_KEY = 'techstore_token';

// ─── ESTADO GLOBAL ───
let productosCache = [];
let productosFiltrados = [];
let tokenSesion = localStorage.getItem(TOKEN_KEY) || '';

// ============================================================
//  INICIALIZACIÓN
// ============================================================
document.addEventListener('DOMContentLoaded', () => {
    cargarProductos();
    configurarFormularios();

    if (tokenSesion) {
        verificarSesion();
    }

    actualizarBadgeCarrito();
});

// ============================================================
//  NAVEGACIÓN SPA
// ============================================================
function mostrarSeccion(id) {
    // Ocultar todas las secciones
    document.querySelectorAll('.seccion').forEach(s => s.classList.remove('activa'));

    // Mostrar la sección solicitada
    const seccion = document.getElementById(`seccion-${id}`);
    if (seccion) {
        seccion.classList.add('activa');
    }

    // Actualizar nav links
    document.querySelectorAll('.nav-link[data-seccion]').forEach(link => {
        link.classList.toggle('active', link.dataset.seccion === id);
    });

    // Actualizar carrito si se navega a esa sección
    if (id === 'carrito') {
        cargarCarrito();
    }

    window.scrollTo({ top: 0, behavior: 'smooth' });
}

// ============================================================
//  TOAST NOTIFICATIONS
// ============================================================
function mostrarToast(mensaje, tipo = 'info') {
    const container = document.getElementById('toast-container');
    const toast = document.createElement('div');
    toast.className = `toast toast-${tipo}`;
    toast.innerHTML = `<span>${mensaje}</span>`;
    container.appendChild(toast);

    setTimeout(() => {
        toast.style.opacity = '0';
        toast.style.transform = 'translateX(100px)';
        toast.style.transition = 'all 0.3s ease';
        setTimeout(() => toast.remove(), 300);
    }, 3500);
}

// ============================================================
//  PETICIONES HTTP (fetch wrapper)
// ============================================================
async function apiFetch(url, options = {}) {
    const headers = {
        'Content-Type': 'application/json',
        ...options.headers,
    };

    if (tokenSesion) {
        headers['Authorization'] = `Bearer ${tokenSesion}`;
    }

    try {
        const res = await fetch(`${API_BASE}${url}`, {
            ...options,
            headers,
        });
        const data = await res.json();
        return { ok: res.ok, status: res.status, data };
    } catch (err) {
        return { ok: false, status: 0, data: { error: 'Error de conexión' } };
    }
}

// ============================================================
//  PRODUCTOS
// ============================================================
async function cargarProductos() {
    const grid = document.getElementById('productos-grid');
    grid.innerHTML = '<div class="loader">Cargando productos...</div>';

    const { ok, data } = await apiFetch('/productos');

    if (!ok || !Array.isArray(data)) {
        grid.innerHTML = '<div class="loader">❌ Error al cargar productos</div>';
        return;
    }

    productosCache = data;
    productosFiltrados = data;
    renderizarProductos(data);
}

function renderizarProductos(productos) {
    const grid = document.getElementById('productos-grid');

    if (productos.length === 0) {
        grid.innerHTML = '<div class="loader">No hay productos en esta categoría</div>';
        return;
    }

    grid.innerHTML = productos.map(p => `
        <div class="producto-card">
            <img class="producto-img"
                 src="${p.imagen || 'https://images.unsplash.com/photo-1555041469-a586c61ea9bc?w=400'}"
                 alt="${p.nombre}"
                 loading="lazy"
                 onerror="this.src='https://images.unsplash.com/photo-1555041469-a586c61ea9bc?w=400'">
            <div class="producto-body">
                <span class="producto-categoria">${p.categoria || 'General'}</span>
                <h3 class="producto-nombre">${p.nombre}</h3>
                <p class="producto-descripcion">${p.descripcion || ''}</p>
                <div class="producto-footer">
                    <span class="producto-precio">${p.precio?.toFixed(2)}</span>
                    <button class="btn btn-primary btn-sm" onclick="agregarAlCarrito(${p.id})">
                        🛒 Agregar
                    </button>
                </div>
                <div class="producto-stock" style="margin-top:8px;">
                    ${renderizarStock(p.stock)}
                </div>
            </div>
        </div>
    `).join('');
}

function renderizarStock(stock) {
    if (stock > 10) return `<span class="stock-disponible">✅ ${stock} en stock</span>`;
    if (stock > 0) return `<span class="stock-bajo">⚠️ Solo ${stock} restantes</span>`;
    return `<span class="stock-agotado">❌ Agotado</span>`;
}

function filtrarPorCategoria(categoria) {
    // Actualizar botones de filtro
    document.querySelectorAll('.filtro-btn').forEach(btn => {
        btn.classList.toggle('activo', btn.dataset.categoria === categoria);
    });

    if (categoria === 'todas') {
        productosFiltrados = productosCache;
    } else {
        productosFiltrados = productosCache.filter(p => p.categoria === categoria);
    }

    renderizarProductos(productosFiltrados);
}

// ============================================================
//  CARRITO
// ============================================================
async function agregarAlCarrito(productoId) {
    if (!tokenSesion) {
        mostrarToast('🔑 Debes iniciar sesión primero', 'error');
        mostrarSeccion('login');
        return;
    }

    const { ok, data } = await apiFetch('/carrito/agregar', {
        method: 'POST',
        body: JSON.stringify({ producto_id: productoId, cantidad: 1 }),
    });

    if (ok) {
        mostrarToast(`✅ ${data.producto || 'Producto'} agregado al carrito`, 'success');
        actualizarBadgeCarrito();
    } else {
        mostrarToast(`❌ ${data.error || 'Error al agregar'}`, 'error');
    }
}

async function cargarCarrito() {
    const contenedor = document.getElementById('carrito-contenido');

    if (!tokenSesion) {
        contenedor.innerHTML = `
            <div class="carrito-vacio">
                <div class="vacio-icono">🔑</div>
                <h2>Inicia sesión para ver tu carrito</h2>
                <p>Necesitas una cuenta para comprar</p>
                <a href="#" onclick="mostrarSeccion('login')" class="btn btn-primary">Iniciar Sesión</a>
            </div>`;
        return;
    }

    const { ok, data } = await apiFetch('/carrito');

    if (!ok || !data.items || data.items.length === 0) {
        contenedor.innerHTML = `
            <div class="carrito-vacio">
                <div class="vacio-icono">🛒</div>
                <h2>Tu carrito está vacío</h2>
                <p>Explora nuestro catálogo y agrega productos</p>
                <a href="#" onclick="mostrarSeccion('catalogo')" class="btn btn-primary">Ver Productos</a>
            </div>`;
        return;
    }

    contenedor.innerHTML = `
        <div class="carrito-items">
            ${data.items.map(item => `
                <div class="carrito-item">
                    <img class="carrito-item-img"
                         src="${item.imagen || 'https://images.unsplash.com/photo-1555041469-a586c61ea9bc?w=100'}"
                         alt="${item.nombre}"
                         onerror="this.src='https://images.unsplash.com/photo-1555041469-a586c61ea9bc?w=100'">
                    <div class="carrito-item-info">
                        <div class="carrito-item-nombre">${item.nombre}</div>
                        <div class="carrito-item-detalle">$${item.precio?.toFixed(2)} c/u</div>
                    </div>
                    <div class="carrito-item-acciones">
                        <div class="cantidad-control">
                            <button class="cantidad-btn" onclick="cambiarCantidad(${item.id}, -1)">−</button>
                            <span class="cantidad-valor">${item.cantidad}</span>
                            <button class="cantidad-btn" onclick="cambiarCantidad(${item.id}, 1)">+</button>
                        </div>
                        <span class="carrito-item-subtotal">$${item.subtotal?.toFixed(2)}</span>
                    </div>
                </div>
            `).join('')}
        </div>
        <div class="carrito-footer">
            <span class="carrito-total">Total: <strong>$${data.total?.toFixed(2)}</strong></span>
            <button class="btn btn-success btn-lg" onclick="irACheckout()">
                📦 Proceder al Pago
            </button>
        </div>`;
}

async function cambiarCantidad(productoId, delta) {
    const { ok } = await apiFetch('/carrito/agregar', {
        method: 'POST',
        body: JSON.stringify({ producto_id: productoId, cantidad: delta }),
    });

    if (ok) {
        await cargarCarrito();
        actualizarBadgeCarrito();
    }
}

async function actualizarBadgeCarrito() {
    const badge = document.getElementById('cart-badge');
    if (!badge) return;

    if (!tokenSesion) {
        badge.textContent = '0';
        badge.style.display = 'inline-flex';
        return;
    }

    const { ok, data } = await apiFetch('/carrito');
    if (ok && data.cantidad_items) {
        badge.textContent = data.cantidad_items;
    } else {
        badge.textContent = '0';
    }
}

// ============================================================
//  CHECKOUT
// ============================================================
function irACheckout() {
    mostrarSeccion('checkout');
    cargarResumenCheckout();
}

async function cargarResumenCheckout() {
    const { ok, data } = await apiFetch('/carrito');
    if (!ok || !data.items || data.items.length === 0) {
        mostrarToast('❌ Tu carrito está vacío', 'error');
        mostrarSeccion('carrito');
        return;
    }

    const resumen = document.getElementById('checkout-resumen');
    resumen.innerHTML = `
        <h3>📋 Resumen del pedido</h3>
        ${data.items.map(item => `
            <div class="resumen-item">
                <span>${item.nombre} × ${item.cantidad}</span>
                <span>$${item.subtotal?.toFixed(2)}</span>
            </div>
        `).join('')}
        <div class="resumen-total">
            <span>Total</span>
            <span>$${data.total?.toFixed(2)}</span>
        </div>
    `;
}

// ─── Manejar envío del formulario de checkout ───
function configurarCheckout() {
    const form = document.getElementById('checkout-form');
    if (!form) return;

    form.addEventListener('submit', async (e) => {
        e.preventDefault();

        const nombre = document.getElementById('checkout-nombre').value.trim();
        const direccion = document.getElementById('checkout-direccion').value.trim();
        const tarjeta = document.getElementById('checkout-tarjeta').value.replace(/\s/g, '');

        if (!nombre || !direccion || !tarjeta) {
            mostrarToast('❌ Completa todos los campos', 'error');
            return;
        }

        if (tarjeta.length < 13) {
            mostrarToast('❌ Número de tarjeta inválido', 'error');
            return;
        }

        const btn = form.querySelector('button[type="submit"]');
        btn.disabled = true;
        btn.textContent = '⏳ Procesando...';

        const { ok, data } = await apiFetch('/checkout', {
            method: 'POST',
            body: JSON.stringify({ nombre, direccion, tarjeta }),
        });

        btn.disabled = false;
        btn.textContent = '✅ Pagar y Confirmar';

        if (ok && data.exito) {
            form.style.display = 'none';
            document.getElementById('checkout-exito').style.display = 'block';
            document.getElementById('checkout-pedido').textContent = `📄 Pedido #${data.pedido_id}`;
            document.getElementById('checkout-total').textContent = `💰 Total pagado: $${data.total?.toFixed(2)}`;
            document.getElementById('checkout-tarjeta').textContent = `💳 Tarjeta: ${data.tarjeta}`;
            mostrarToast('🎉 ¡Compra realizada con éxito!', 'success');
            actualizarBadgeCarrito();
        } else {
            mostrarToast(`❌ ${data.error || 'Error al procesar la compra'}`, 'error');
        }
    });
}

// ============================================================
//  LOGIN
// ============================================================
async function iniciarSesion(usuario, password) {
    const { ok, data } = await apiFetch('/auth/login', {
        method: 'POST',
        body: JSON.stringify({ usuario, password }),
    });

    if (ok && data.token) {
        tokenSesion = data.token;
        localStorage.setItem(TOKEN_KEY, tokenSesion);
        mostrarToast(`✅ Bienvenido, ${data.usuario}`, 'success');
        await verificarSesion();
        mostrarSeccion('catalogo');
        actualizarBadgeCarrito();
        return true;
    } else {
        mostrarToast(`❌ ${data.error || 'Credenciales inválidas'}`, 'error');
        return false;
    }
}

async function verificarSesion() {
    if (!tokenSesion) return;

    const { ok, data } = await apiFetch('/auth/session');

    if (ok && data.autenticado) {
        document.getElementById('nav-login').style.display = 'none';
        document.getElementById('nav-perfil').style.display = 'inline-flex';
        document.getElementById('nav-usuario').textContent = data.usuario || data.nombre || 'Usuario';
    } else {
        cerrarSesion();
    }
}

function cerrarSesion() {
    tokenSesion = '';
    localStorage.removeItem(TOKEN_KEY);
    document.getElementById('nav-login').style.display = 'inline-flex';
    document.getElementById('nav-perfil').style.display = 'none';
    document.getElementById('cart-badge').textContent = '0';
    mostrarToast('👋 Sesión cerrada', 'info');
    mostrarSeccion('catalogo');
}

// ============================================================
//  CONFIGURAR FORMULARIOS
// ============================================================
function configurarFormularios() {
    // Login
    const loginForm = document.getElementById('login-form');
    if (loginForm) {
        loginForm.addEventListener('submit', async (e) => {
            e.preventDefault();
            const usuario = document.getElementById('login-usuario').value.trim();
            const password = document.getElementById('login-password').value.trim();
            await iniciarSesion(usuario, password);
        });
    }

    // Checkout
    configurarCheckout();

    // Formatear tarjeta
    const tarjetaInput = document.getElementById('checkout-tarjeta');
    if (tarjetaInput) {
        tarjetaInput.addEventListener('input', (e) => {
            let val = e.target.value.replace(/\D/g, '');
            val = val.replace(/(\d{4})/g, '$1 ').trim();
            e.target.value = val.substring(0, 19);
        });
    }
}

// ============================================================
//  INICIO RÁPIDO - Permitir login con Enter
// ============================================================
document.addEventListener('keydown', (e) => {
    if (e.key === 'Escape') {
        // Cerrar toasts al presionar Escape
        document.querySelectorAll('.toast').forEach(t => t.remove());
    }
});
