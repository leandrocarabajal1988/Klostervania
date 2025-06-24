#include "enemigo.h"
#include <cstdlib>   // std::rand
#include <cmath>     // std::hypot
#include <iostream>

// Constructor: delega en personaje(posInicial, rutaSpritesheet, escala)
// para que ese constructor fije escala y origin en la base automáticamente.
enemigo::enemigo(const sf::Vector2f& posInicial,
                 const std::string& rutaSpritesheet,
                 const sf::Vector2f& escala)
    : personaje(posInicial, rutaSpritesheet, escala)
    , _posInicial(posInicial)
{
    // Definir puntos de patrulla de ejemplo
    _puntosPatrulla = {{100.f, 200.f}, {300.f, 200.f}};
}

// Activa o desactiva el enemigo; si se desactiva, arranca el reloj de respawn.
void enemigo::setActivo(bool activo) {
    _activo = activo;
    if (!_activo) {
        _respawnClock.restart();
    }
}

bool enemigo::estaActivo() const {
    return _activo;
}

// Dibuja el enemigo solo si está activo
void enemigo::draw(sf::RenderWindow& window) {
    if (_activo) {
        personaje::draw(window);
    }
}

// getSprite, getPosition y getBounds simplemente delegan a la clase base:
const sf::Sprite& enemigo::getSprite() const {
    return sprite;
}

sf::Vector2f enemigo::getPosition() const {
    return sprite.getPosition();
}

sf::FloatRect enemigo::getBounds() const {
    return sprite.getGlobalBounds();
}

// update: controla respawn, combate por turnos y patrulla fuera de combate
void enemigo::update(float deltaTime,
                     bool movDer,
                     bool movIzq,
                     bool movArr,
                     bool movAbj)
{
    // 1) Respawn si está inactivo
    if (!_activo) {
        if (_respawnClock.getElapsedTime() >= _respawnDelay) {
            // Reactivar enemigo y restaurar estado inicial
            _activo = true;
            setSalud(_maxSalud);
            sprite.setPosition(_posInicial);

            // Restaurar escala normal y origin en la base
            sprite.setScale(baseScaleX, baseScaleY);
            {
                sf::FloatRect local = sprite.getLocalBounds();
                sprite.setOrigin(0.f, local.height);
            }

            // Resetear flags internos de combate
            estado          = estadoPersonaje::quieto;
            atacando        = false;
            proyectilActivo = false;
            ataqueLlegado   = false;
            ataqueFase      = 0;
            _modoBatalla    = false;
        }
        // Salir sin hacer nada más mientras esté inactivo
        return;
    }

    // 2) Si está en combate por turnos, solo animar mediante personaje::update
    if (_modoBatalla) {
        personaje::update(deltaTime, movDer, movIzq, movArr, movAbj);
        return;
    }
/*         DESACTIVO TEMPORALMENTE
    // 3) IA de patrulla aleatoria fuera de combate
    _tiempoDesdeUltimoMovimiento += deltaTime;
    if (_tiempoDesdeUltimoMovimiento >= 0.7f && !_puntosPatrulla.empty()) {
        // Elegir un punto de patrulla al azar
        int idx = std::rand() % static_cast<int>(_puntosPatrulla.size());
        sf::Vector2f destino = _puntosPatrulla[idx];

        // Calcular dirección y mover ligeramente hacia ese punto
        sf::Vector2f dir = destino - sprite.getPosition();
        float dist       = std::hypot(dir.x, dir.y);
        const float speedP = 4.f; // velocidad de patrulla
        if (dist > 1.f) {
            dir /= dist; // normalizar
            sprite.move(dir * speedP);
        }
        _tiempoDesdeUltimoMovimiento = 0.f;
    }*/

    // 4) Delegar animaciones de caminata/respiración a personaje::update
    personaje::update(deltaTime, false, false, false, false);
}

// ataque: elige ataque aleatorio y lo ejecuta; devuelve el daño causado
int enemigo::ataque(const sf::Vector2f& destino)
{
    // 1) Guardar la posición base del “pie” antes de modificar el origen
    ataqueStartPos = sprite.getPosition();

    // 2) Obtener dimensiones reales del sprite (ya escalado)
    sf::FloatRect local = sprite.getLocalBounds();
    float ancho = local.width;
    float alto  = local.height;
    float escX  = std::abs(sprite.getScale().x);
    float escY  = sprite.getScale().y;

    // 3) Flip horizontal manteniendo origen en la base (Y = alto)
    if (destino.x > ataqueStartPos.x) {
        // Atacar a la derecha
        sprite.setScale(+escX, escY);
        sprite.setOrigin(0.f, alto);
    } else {
        // Atacar a la izquierda (flip)
        sprite.setScale(-escX, escY);
        sprite.setOrigin(ancho, alto);
    }

    // 4) Elegir tipo de ataque (ligero, pesado, especial) y lanzar animación
    int r = std::rand() % 3; // 0 = ligero, 1 = pesado, 2 = especial
    int danio = 0;
    switch (r) {
        case 0:
            danio = getAtaqueLigero();
            ataqueLigero(destino);
            break;
        case 1:
            danio = getAtaquePesado();
            ataquePesado(destino);
            break;
        case 2:
            danio = getHabilidadEspecial();
            habilidadEspecial(destino);
            break;
    }
    return danio;
}
