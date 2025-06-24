#include "paredes.h"

void Paredes::crear() {

    ///BORDES DEL MAPA
    agregarPared({0, 0, 2996, 10}); //superior
    agregarPared({0, 0, 10, 2036});//lateral izq
    agregarPared({0, 2036 - 10, 2996, 10});//lateral der
    agregarPared({2996 - 10, 0, 10, 2036});//inferior

    ///PAREDES INTERNAS

    agregarPared({0, 530, 345, 5});//H1
    agregarPared({470, 530, 300, 5});//h1.2
    agregarPared({960, 530, 550, 5});//h1.3
    agregarPared({1700, 530, 950, 5});//h1.4
    agregarPared({2800, 530, 110, 5});//h1.5
    agregarPared({0, 1015, 800, 5});//H2
    agregarPared({960, 1015, 510, 5});//h2.2
    agregarPared({1700, 1015, 350, 5});//h2.3
    agregarPared({2150, 1015, 240, 5});//h2.4
    agregarPared({0, 1500, 280, 5});//H3
    agregarPared({420, 1500, 320, 5});//h3.2
    agregarPared({960, 1500, 500, 5});//h3.3
    agregarPared({1690, 1500, 490, 5});//h3.3
    agregarPared({2250, 1500, 430, 5});//h3.4
    agregarPared({2750, 1500, 280, 5});//h3.5


    agregarPared({1120, 0, 5, 150});//V1
    agregarPared({1120, 350, 5, 400});//V1.1
    agregarPared({1120, 850, 5, 350});//V1.2
    agregarPared({1120, 1350, 5, 170});//V1.2

    agregarPared({680, 560, 5, 150});//V2.1
    agregarPared({680, 900, 5, 340});//V2.2
    agregarPared({680, 1320, 5, 400});//V2.3
    agregarPared({680, 1850, 5, 200});//V2.4


    agregarPared({2000, 0, 5, 150});//V3
    agregarPared({2000, 300, 5, 880});//V3.2
    agregarPared({2000, 1350, 5, 280});//V3.3
    agregarPared({2000, 1800, 5, 200});//V3.4


    agregarPared({2420, 600, 5, 1300});//V4.2
    agregarPared({2470, 0, 5, 200});//V4.2
    agregarPared({2470, 350, 5, 200});//V4.2

    agregarPared({2000, 355, 450, 5});//V4.2
    agregarPared({130, 1700, 430, 150});//V4.2
}

void Paredes::agregarPared(const sf::FloatRect& rect) {
    colisiones.push_back(rect);
}

bool Paredes::colisiona(const sf::FloatRect& entidad) const {
    for (const auto& muro : colisiones) {
        if (muro.intersects(entidad))
            return true;
    }
    return false;
}

void Paredes::dibujarDebug(sf::RenderWindow& ventana) const {
    for (const auto& muro : colisiones) {
        sf::RectangleShape shape;
        shape.setPosition(muro.left, muro.top);
        shape.setSize({ muro.width, muro.height });
        shape.setFillColor(sf::Color(255, 0, 0, 0)); ///(255, 0, 0, 100)); para ver las paredes
        ventana.draw(shape);
    }
}

