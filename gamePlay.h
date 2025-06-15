#pragma once
#include <SFML/Graphics.hpp>
#include "personaje.h"
#include "enemigo.h"
#include "boss.h"
#include "menu.h"
#include "item.h"
#include "batalla.h"
#include "popUpCartel.h"
#include <SFML/Audio.hpp>
#include <memory>
#include <vector>
#include <chrono>   // para std::chrono::seconds

class gamePlay
{
public:
    gamePlay();
    void ejecutar();
    void iniciarNuevaPartida();
    void seleccionPersonaje();
    void unlockPersonaje(int id);

private:
    void procesarEventos();
    void updatePersonaje(sf::Time dt);
    void drawMenuPrincipal();
    void drawExploracion();
    void mostrarGameOver();

    sf::Clock reloj;

    // ————— Ventana principal —————
    sf::RenderWindow window;

    // ————— Fondos y sprites —————
    sf::Texture       fondoPrincipal;
    sf::View vista;
    sf::Sprite        spriteFondo;
    sf::Texture       fondoNuevaPartida;
    sf::Sprite        spriteNuevaPartida;

    // ————— Transición y menú —————
    sf::RectangleShape pantallaNegra;
    sf::Font           fuente;
    menu               menuPrincipal;
    static constexpr int numOpcionesMenuPrincipal = 5;
    std::vector<std::string> opcionesVector = {"Nueva Partida", "Continuar Partida", "Record", "Creditos", "Salir"};
    // Velocidad de fade (cambios de alpha por segundo)
    float fadeSpeed = 100.f;
    // Alpha actual (0→255)
    float alphaFade   = 255.f;
    // Devuelve true cuando ya llegó a alpha = 0 (completamente transparente)
    void fadeInTransition(sf::Sprite& spriteFondo);
    void fadeInTransition(sf::Sprite& spriteFondo, sf::View* vista);

    // ————— Sonidos —————
    sf::SoundBuffer bufferFlecha;
    sf::SoundBuffer bufferEnter;
    sf::Sound       flecha;
    sf::Sound       enter;

    // ————— Entidades del juego —————
    std::vector<std::shared_ptr<personaje>> prototipos;
    std::vector<std::shared_ptr<personaje>> roster;
    std::shared_ptr<personaje>              jugadorActivo;
    void agregarPersonaje(const std::string& nombre, const std::string& ruta);
    void inicializarPrototipos();

    // ————— Enemigos del juego —————
    std::vector<enemigo*> enemigos;
    enemigo* enemigoSeleccionado = nullptr;
    void inicializarEnemigos();
    item       itemRecolectable;

    // ————— Flags de estado —————
    bool ejecutando     = false;
    bool juegoIniciado  = false;
    int  opcionSeleccionada = 0;

    enum class EstadoJuego { SeleccionPersonaje , MenuPrincipal, Exploracion, pause, Batalla, dialogoItem };
    EstadoJuego estado = EstadoJuego::MenuPrincipal;

    batalla* batallaGamePlay = nullptr;
    bool     batallaIniciada = false;
    sf::Vector2f posicionPreBatalla;
    PopUpCartel popupCartel;

    bool batallaPopupActive() const;
};
