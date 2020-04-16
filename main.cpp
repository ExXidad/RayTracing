#include <iostream>
#include <SFML/Graphics.hpp>
#include "myTypes.h"
#include <cmath>

typedef sf::Vector2f Vector2f;
typedef sf::Vector2i Vector2i;

void keepWindowOpened(sf::RenderWindow &window) {
    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
                break;
            }
        }
    }
}
//
//sf::Vector2f Vector2f(const vectorDouble &vector){
//    return sf::Vector2f(vector[0],vector[1]);
//}

sf::VertexArray line2P(const Vector2f &p1, const Vector2f &p2) {
    sf::VertexArray line(sf::LinesStrip, 2);
    line[0].position = p1;
    line[1].position = p2;
    line[0].color = sf::Color::White;
    line[1].color = sf::Color::White;
    return line;
}

class Line {
protected:
    Vector2f p2;
    Vector2f p1;
    Vector2f direction;
public:
    sf::VertexArray line;

    Line(Vector2f p1, Vector2f p2) {
        this->p1 = p1;
        this->p2 = p2;
        this->direction = p2 - p1;
        this->line = line2P(p1, p2);
    }
};

class Boundary : public Line {
protected:

public:
    Boundary(Vector2f p1, Vector2f p2) : Line(p1, p2) {
    }
};

class Scene {
protected:
    sf::RenderWindow *window;
    std::vector<Boundary> boundaries;

public:
    Scene(sf::RenderWindow &window) {
        sf::Vector2u windowSize = window.getSize();
        boundaries.emplace_back(Vector2f(0, 0), Vector2f(windowSize.x, 0));
        boundaries.emplace_back(Vector2f(0, 0), Vector2f(windowSize.y, 0));
        boundaries.emplace_back(Vector2f(windowSize.x, 0), Vector2f(windowSize.x, windowSize.y));
        boundaries.emplace_back(Vector2f(0, windowSize.y), Vector2f(windowSize.x, windowSize.y));

        this->window = &window;
    }

    void addBoundary(Boundary &boundary) {
        boundaries.push_back(boundary);
    }

    void draw() {
        for (const auto &boundary : boundaries) {
            window->draw(boundary.line);
        }
    }
};

class LightSource {
protected:
    Vector2f origin;
public:

};

int main() {
    int windowWidth = 900;
    int windowHeight = 900;

    sf::ContextSettings settings;
    settings.majorVersion = 3.1;
    settings.antialiasingLevel = 8;

    sf::RenderWindow window(sf::VideoMode(windowWidth, windowHeight), "Maze",
                            sf::Style::Default, settings);

    window.clear(sf::Color::Black);

    //sf::View view;
    //view.reset(sf::FloatRect())

    //////////////////////////////////////////////////////////////////////////

    Boundary boundary1(Vector2f(100, 100), Vector2f(500, 300)),
            boundary2(Vector2f(200, 800), Vector2f(800, 50));


    Scene scene(window);
    scene.addBoundary(boundary1);
    scene.addBoundary(boundary2);

    scene.draw();

    //////////////////////////////////////////////////////////////////////////
    window.display();
    keepWindowOpened(window);
    return 0;
}
