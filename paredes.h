#pragma once
#include <SFML/Graphics.hpp>
#include <vector>

class Paredes {
public:
    void crear();
    void agregarPared(const sf::FloatRect& rect);
    bool colisiona(const sf::FloatRect& entidad) const;
    void dibujarDebug(sf::RenderWindow& ventana) const;

private:
    std::vector<sf::FloatRect> colisiones;
};

