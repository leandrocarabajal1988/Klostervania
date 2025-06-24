#include "personaje.h"
#include <cmath>  // Para std::sin y constantes matemáticas

// --- Setters de estadísticas ---
void personaje::setSalud(int salud)
{
    _salud = salud;  // Actualiza puntos de vida
}

void personaje::setAtaqueLigero(int ataqueLigero)
{
    _ataqueLigero = ataqueLigero;  // Actualiza daño ataque ligero
}

void personaje::setAtaquePesado(int ataquePesado)
{
    _ataquePesado = ataquePesado;  // Actualiza daño ataque pesado
}

void personaje::setHabilidadEspecial(int habilidadEspecial)
{
    _habilidadEspecial = habilidadEspecial;  // Actualiza poder habilidad especial
}

int personaje::getSalud() const
{
    return _salud;  // Devuelve puntos de vida actuales
}

int personaje::getAtaqueLigero() const
{
    return _ataqueLigero;  // Devuelve daño de ataque ligero
}

int personaje::getAtaquePesado() const
{
    return _ataquePesado;  // Devuelve daño de ataque pesado
}

int personaje::getHabilidadEspecial() const
{
    return _habilidadEspecial;  // Devuelve valor de habilidad especial
}

sf::FloatRect personaje::getBounds() const
{
    return sprite.getGlobalBounds();
}

// 1) Constructor por defecto
// ———————————————————————————————
personaje::personaje()
    : estado(estadoPersonaje::quieto),
      _salud(1000),
      _ataqueLigero(10),
      _ataquePesado(15),
      _habilidadEspecial(25),
      baseScaleX(0.3f),
      baseScaleY(0.3f),
      breathAmplitude(0.01f),    // pulso muy sutil por defecto
      breathSpeed(0.5f)          // ciclo por segundo
{
    // Si hubiera un sprite ya asociado, ajustamos escala y origin a la base
    sprite.setScale(baseScaleX, baseScaleY);
    {
        sf::FloatRect local = sprite.getLocalBounds();
        sprite.setOrigin(0.f, local.height);
    }
    breathClock.restart();
}

// ———————————————————————————————
// 2) Nuevo constructor: recibe parámetros
// ———————————————————————————————
personaje::personaje(const sf::Vector2f& posInicial,
                     const std::string& rutaSpritesheet,
                     const sf::Vector2f& escala)
    : estado(estadoPersonaje::quieto),
      _salud(1000),
      _ataqueLigero(10),
      _ataquePesado(15),
      _habilidadEspecial(25),
      baseScaleX(escala.x),
      baseScaleY(escala.y),
      breathAmplitude(0.02f),
      breathSpeed(0.2f)
{
    // 1) Intentamos cargar la textura desde rutaSpritesheet
    std::cout << "Intentando cargar textura: " << rutaSpritesheet << "\n";
    if (!textura.loadFromFile(rutaSpritesheet))
    {
        std::cerr << "ERROR: No pude cargar " << rutaSpritesheet << "\n";
    }
    else
    {
        std::cout << "Textura cargada OK: " << rutaSpritesheet << "\n";
    }

    // 2) Asignar textura al sprite y definir rectángulo inicial
    sprite.setTexture(textura);
    sprite.setTextureRect({0, 0, frameWidth, frameHeight});

    // 3) Aplicar escala (actualiza baseScaleX/Y) y fijar origen en la base
    sprite.setScale(baseScaleX, baseScaleY);
    {
        sf::FloatRect local = sprite.getLocalBounds();
        sprite.setOrigin(0.f, local.height);
    }

    // 4) Fijar posición inicial del personaje
    sprite.setPosition(posInicial);

    // 5) Reiniciar reloj de respiración
    breathClock.restart();
}

// Mueve el sprite según los desplazamientos dados
void personaje::mover(float offsetX, float offsetY)
{
    sprite.move(offsetX, offsetY);
}
/*
sf::Vector2f personaje::getPosition() const {
    return sprite.getPosition();
}

void personaje::setPosition(const sf::Vector2f& pos) {
    sprite.setPosition(pos);
}

sf::FloatRect personaje::getBounds() const {
    return sprite.getGlobalBounds();
}

*/



// Detiene la animación, resetea frame y temporizador
void personaje::detener()
{
    currentFrame = 0;
    frameTimer   = 0.f;
    sprite.setTextureRect(frameActual);
}

// Dibujo: personaje y posible proyectil
void personaje::draw(sf::RenderWindow& window)
{
    window.draw(sprite);
    if (proyectilActivo)
        window.draw(spriteProyectil);
}

// --- Lógica de actualización: animación, movimiento y respiración ---
void personaje::update(float deltaTime,
                       bool movDer,
                       bool movIzq,
                       bool movArriba,
                       bool movAbajo)
{
    // A) Secuencia de ataque ligero
    if (estado == estadoPersonaje::ataqueLigero)
    {
        // Orientación horizontal para el ataque ligero
        {
            sf::FloatRect local = sprite.getLocalBounds();
            float ancho = local.width;
            float alto  = local.height;

            if (ataqueTargetPos.x < ataqueStartPos.x)
            {
                // Atacar hacia la izquierda (flip horizontal)
                sprite.setScale(-baseScaleX, baseScaleY);
                sprite.setOrigin(ancho, alto);
            }
            else
            {
                // Atacar hacia la derecha
                sprite.setScale(baseScaleX, baseScaleY);
                sprite.setOrigin(0.f, alto);
            }
        }

        // — Fase de movimiento —
        if (!ataqueLlegado)
        {
            // siempre dibuja el primer frame de la fila de ataque:
            frameActual = {
                0,
                filaFrameAtaqueLigero * frameHeight,
                frameWidth,
                frameHeight
            };
            sprite.setTextureRect(frameActual);

            // mueve hacia el target a velocidad constante
            sf::Vector2f dir = ataqueTargetPos - sprite.getPosition();
            float dist       = std::hypot(dir.x, dir.y);
            if (dist > ataqueSpeed * deltaTime)
            {
                dir /= dist;  // normalizar
                sprite.move(dir * ataqueSpeed * deltaTime);
            }
            else
            {
                // llegó definitivamente
                sprite.setPosition(ataqueTargetPos);
                ataqueLlegado = true;
                currentFrame  = 0;    // arrancamos los frames
                frameTimer    = 0.f;
            }
        }
        // — Fase de animación de frames —
        else if (currentFrame < cantidadFrameAtaqueLigero)
        {
            frameTimer += deltaTime;
            if (frameTimer >= frameTime)
            {
                frameTimer -= frameTime;
                currentFrame++;
            }
            frameActual = {
                currentFrame * frameWidth,
                filaFrameAtaqueLigero * frameHeight,
                frameWidth,
                frameHeight
            };
            sprite.setTextureRect(frameActual);
        }
        // — Fase de reset —
        else
        {
            // 1) Volver a la posición base del ataque
            sprite.setPosition(ataqueStartPos);

            // 2) Restaurar escala normal y origen en la base
            sprite.setScale(baseScaleX, baseScaleY);
            {
                sf::FloatRect local = sprite.getLocalBounds();
                sprite.setOrigin(0.f, local.height);
            }

            // 3) Cambiar estado a quieto
            estado   = estadoPersonaje::quieto;
            atacando = false;
        }

        return; // ya procesamos ataque completo
    }

    // B) Secuencia de ataque pesado
    if (estado == estadoPersonaje::ataquePesado)
    {
        // Orientación horizontal para el ataque pesado
        {
            sf::FloatRect local = sprite.getLocalBounds();
            float ancho = local.width;
            float alto  = local.height;

            if (ataqueTargetPos.x < ataqueStartPos.x)
            {
                // Atacar hacia la izquierda (flip horizontal)
                sprite.setScale(-baseScaleX, baseScaleY);
                sprite.setOrigin(ancho, alto);
            }
            else
            {
                // Atacar hacia la derecha
                sprite.setScale(baseScaleX, baseScaleY);
                sprite.setOrigin(0.f, alto);
            }
        }

        if (!ataqueLlegado)
        {
            frameActual = sf::IntRect(
                0,
                filaFrameAtaquePesado * frameHeight,
                frameWidth,
                frameHeight
            );
            sprite.setTextureRect(frameActual);
            sf::Vector2f dir = ataqueTargetPos - sprite.getPosition();
            float dist = std::hypot(dir.x, dir.y);
            if (dist > ataqueSpeed * deltaTime)
            {
                dir /= dist;
                sprite.move(dir * ataqueSpeed * deltaTime);
            }
            else
            {
                sprite.setPosition(ataqueTargetPos);
                ataqueLlegado = true;
                currentFrame   = 0;
                frameTimer     = 0.f;
            }
        }
        else if (currentFrame < cantidadFrameAtaquePesado)
        {
            frameTimer += deltaTime;
            if (frameTimer >= frameTime)
            {
                frameTimer -= frameTime;
                currentFrame++;
            }
            frameActual = sf::IntRect(
                currentFrame * frameWidth,
                filaFrameAtaquePesado * frameHeight,
                frameWidth,
                frameHeight
            );
            sprite.setTextureRect(frameActual);
        }
        else
        {
            // 1) Volver a la posición base del ataque
            sprite.setPosition(ataqueStartPos);

            // 2) Restaurar escala normal y origen en la base
            sprite.setScale(baseScaleX, baseScaleY);
            {
                sf::FloatRect local = sprite.getLocalBounds();
                sprite.setOrigin(0.f, local.height);
            }

            // 3) Cambiar estado a quieto
            estado   = estadoPersonaje::quieto;
            atacando = false;
        }
        return;
    }

    // C) Habilidad Especial
    if (estado == estadoPersonaje::habilidadEspecial)
    {
        // Fase 0: MOVER EL PROYECTIL
        if (ataqueFase == 0 && !ataqueLlegado)
        {
            // ─── ORIENTAR PROYECTIL ───
            sf::Vector2f dir0 = ataqueTargetPos - spriteProyectil.getPosition();
            float signoX = (dir0.x >= 0.f) ? +1.f : -1.f;

            // Fijar origen en la base del proyectil (mismo esquema que sprite principal)
            {
                sf::FloatRect localP = sprite.getLocalBounds();
                float anchoP = localP.width;
                float altoP  = localP.height;
                spriteProyectil.setScale(signoX * baseScaleX * 3.f,
                                         baseScaleY * 5.f);
                if (signoX < 0.f)
                    spriteProyectil.setOrigin(anchoP, altoP);
                else
                    spriteProyectil.setOrigin(0.f, altoP);
            }

            // fijamos el frame 2 del spritesheet (columna 1)
            projectileFrame = 1;
            spriteProyectil.setTextureRect(
                sf::IntRect(
                    projectileFrame * frameWidth,
                    filaFrameHabilidadEspecial * frameHeight,
                    frameWidth,
                    frameHeight
                )
            );

            sf::Vector2f dir = ataqueTargetPos - spriteProyectil.getPosition();
            float dist       = std::hypot(dir.x, dir.y);
            if (dist > ataqueSpeed * deltaTime)
            {
                dir /= dist;  // normalizar
                spriteProyectil.move(dir * ataqueSpeed * deltaTime);
            }
            else
            {
                spriteProyectil.setPosition(ataqueTargetPos);
                ataqueLlegado   = true;
                ataqueFase      = 1;
                projectileFrame = 1;
                frameTimer      = 0.f;
            }
        }
        // Fase 1: ANIMAR FRAMES 2..6 DEL PROYECTIL
        else if (ataqueFase == 1 && projectileFrame < cantidadFrameHabilidadEspecial)
        {
            frameTimer += deltaTime * 1.8f;
            if (frameTimer >= frameTime)
            {
                frameTimer -= frameTime;
                projectileFrame++;
            }
            spriteProyectil.setTextureRect(
                sf::IntRect(
                    projectileFrame * frameWidth,
                    filaFrameHabilidadEspecial * frameHeight,
                    frameWidth,
                    frameHeight
                )
            );
        }
        // Fase 2: RESET
        else
        {
            // 1) Apagar proyectil
            proyectilActivo = false;

            // 2) Volver a posición base
            sprite.setPosition(ataqueStartPos);

            // 3) Restaurar escala normal y origen en la base
            sprite.setScale(baseScaleX, baseScaleY);
            {
                sf::FloatRect local = sprite.getLocalBounds();
                sprite.setOrigin(0.f, local.height);
            }

            // 4) Cambiar estado a quieto
            estado   = estadoPersonaje::quieto;
            atacando = false;
        }

        return;
    }

    // D) ¿Está caminando en alguna dirección?
    bool caminando = movDer || movIzq || movArriba || movAbajo;

    if (caminando)
    {
        // 1) Elegir fila y totalFrames según dirección
        int fila        = 0;
        int totalFrames = 0;

        if (movArriba)
        {
            estado      = estadoPersonaje::caminandoArriba;
            fila        = filaFrameCaminarArriba;
            totalFrames = cantidadFrameCaminarArriba;
        }
        else if (movAbajo)
        {
            estado      = estadoPersonaje::caminandoAbajo;
            fila        = filaFrameCaminarAbajo;
            totalFrames = cantidadFrameCaminarAbajo;
        }
        else
        {
            estado      = estadoPersonaje::caminando;
            fila        = filaFrameCaminar;
            totalFrames = cantidadFrameCaminar;
        }

        // 2) Avanzar el temporizador de animación
        frameTimer += deltaTime;
        if (frameTimer >= frameTime)
        {
            frameTimer  -= frameTime;
            currentFrame = (currentFrame + 1) % totalFrames;
        }

        // 3) Calcular y aplicar el sub-rectángulo
        int left = currentFrame * frameWidth;
        int top  = fila * frameHeight;
        sprite.setTextureRect({ left, top, frameWidth, frameHeight });

        // 4) Mover al personaje
        if (movArriba) sprite.move(0, -speed * deltaTime);
        if (movAbajo)  sprite.move(0,  speed * deltaTime);
        if (movIzq)    sprite.move(-speed * deltaTime, 0);
        if (movDer)    sprite.move( speed * deltaTime, 0);

        // 5) Flip horizontal solo para la caminata lateral,
        //    pero manteniendo el origen en la base
        {
            sf::FloatRect local = sprite.getLocalBounds();
            float ancho = local.width;
            float alto  = local.height;

            if (movIzq)
            {
                sprite.setScale(-baseScaleX, baseScaleY);
                sprite.setOrigin(ancho, alto);
            }
            else
            {
                sprite.setScale(baseScaleX, baseScaleY);
                sprite.setOrigin(0.f, alto);
            }
        }
    }
    else
    {
        // --- Estado quieto ---
        estado       = estadoPersonaje::quieto;
        currentFrame = 0;

        // Frame fijo de quieto en la fila 6
        sprite.setTextureRect(
            { 0,
              filaFrameQuieto * frameHeight,
              frameWidth,
              frameHeight }
        );

        // Restaurar escala y origen en la base
        sprite.setScale(baseScaleX, baseScaleY);
        {
            sf::FloatRect local = sprite.getLocalBounds();
            sprite.setOrigin(0.f, local.height);
        }
    }

    // D) Respiración / pulso  (opcional, comentado si no se quiere)
     float t = breathClock.getElapsedTime().asSeconds();
     float factor = 1.f + breathAmplitude * std::sin(2.f * 3.14159265f * breathSpeed * t);
     float signX = (sprite.getScale().x < 0) ? -1.f : 1.f;
     sprite.setScale(signX * baseScaleX * factor, baseScaleY * factor);
}

// --- IMPLEMENTACIÓN de los métodos de ataque (solo asignan estado y guardan posición) ---
void personaje::ataqueLigero(const sf::Vector2f& destino)
{
    if (estado != estadoPersonaje::ataqueLigero)
    {
        // Guardar la posición “base” para retornar luego
        ataqueStartPos = sprite.getPosition();
        atacando       = true;
        currentFrame   = 0;
        frameTimer     = 0.f;
        ataqueFase     = 0;                     // empezamos fase de movimiento
        ataqueTargetPos   = destino;            // destino fijo
        ataqueLlegado     = false;              // reiniciamos aquí
        estado         = estadoPersonaje::ataqueLigero;
    }
}

// Ataque Pesado
void personaje::ataquePesado(const sf::Vector2f& destino)
{
    if (estado != estadoPersonaje::ataquePesado)
    {
        // Guardar la posición “base” para retornar luego
        ataqueStartPos = sprite.getPosition();
        atacando       = true;
        currentFrame   = 0;
        frameTimer     = 0.f;
        ataqueFase     = 0;
        ataqueTargetPos= destino;
        ataqueLlegado  = false;
        estado         = estadoPersonaje::ataquePesado;
    }
}

// Habilidad Especial
void personaje::habilidadEspecial(const sf::Vector2f& destino)
{
    if (estado != estadoPersonaje::habilidadEspecial)
    {
        // Guardar la posición “base” para retornar luego
        ataqueStartPos = sprite.getPosition();
        estado          = estadoPersonaje::habilidadEspecial;
        atacando        = true;
        ataqueFase      = 0;
        ataqueLlegado   = false;
        proyectilActivo = true;
        projectileFrame = 0;

         //Ajustar el destino del proyectil 100px más arriba
        sf::Vector2f targetProy = { destino.x, destino.y  +200.f };
        ataqueTargetPos = targetProy;

        // Fijar el primer frame de la habilidad en el personaje
        sprite.setTextureRect(
            { 0,
              filaFrameHabilidadEspecial * frameHeight,
              frameWidth,
              frameHeight }
        );

        // Asignar textura al proyectil y fijar su frame inicial
        spriteProyectil.setTexture(textura);
        spriteProyectil.setTextureRect(
            { 0,
              filaFrameHabilidadEspecial * frameHeight,
              frameWidth,
              frameHeight }
        );

        // Inicializar el proyectil en la X del personaje, Y=0
        float xPersonaje = ataqueStartPos.x;
        spriteProyectil.setPosition(xPersonaje, 0.f);
        spriteProyectil.setScale(baseScaleX, baseScaleY);
    }
}

sf::Vector2f personaje::getPosition() const
{
    return sprite.getPosition();
}

const sf::Sprite& personaje::getSprite() const
{
    return sprite;
}

void personaje::setPosition(float x, float y)
{
    sprite.setPosition(x, y);
}

void personaje::setPosition(const sf::Vector2f& pos)
{
    sprite.setPosition(pos.x, pos.y);
}

void personaje::setOrigin(float x, float y)
{
    sprite.setOrigin(x, y);
}

void personaje::setScale(float x, float y)
{
    baseScaleX = x;
    baseScaleY = y;

    sprite.setScale(baseScaleX, baseScaleY);
    // Al cambiar escala “manualmente” también reposicionamos el origen en la base:
    {
        sf::FloatRect local = sprite.getLocalBounds();
        sprite.setOrigin(0.f, local.height);
    }
}

bool personaje::estaAtacando()
{
    return atacando;
}

void personaje::setScale(const sf::Vector2f& s)
{
    setScale(s.x, s.y);
}

void personaje::setEstado(estadoPersonaje nuevoEstado)
{
    estado = nuevoEstado;
}

sf::Sprite& personaje::getSprite() {
    return sprite;
}

void personaje::setFrameAtaque(int frame)
{
    currentFrame = frame % cantidadFrameAtaqueLigero;

    frameActual.left = currentFrame * frameWidth;
    frameActual.top  = filaFrameAtaqueLigero * frameHeight;
    frameActual.width  = frameWidth;
    frameActual.height = frameHeight;

    sprite.setTextureRect(frameActual);
}

