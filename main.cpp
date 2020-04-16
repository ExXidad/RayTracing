#include <cmath>
#include <iostream>
#include <SFML/Graphics.hpp>
#include "myTypes.h"
#include <cmath>

typedef sf::Vector2f Vector2f;
typedef sf::Vector2i Vector2i;

class Line;

class Boundary;

class Scene;

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

double det(const mapDouble &map) {
    return (map[0][0] * map[1][1] - map[0][1] * map[1][0]);
}

double det(Vector2f a, Vector2f l) {
    return (a.x * l.y - a.y * l.x);
}

float abs(Vector2f a) {
    return sqrt(a.x * a.x + a.y * a.y);
}

//
//sf::Vector2f Vector2f(const vectorDouble &vector){
//    return sf::Vector2f(vector[0],vector[1]);
//}

Vector2f prod(Vector2f vector, double num) {
    return Vector2f(vector.x * num, vector.y * num);
}

class Line {
protected:
    sf::VertexArray line2P(const Vector2f &p1, const Vector2f &p2) {
        sf::VertexArray line(sf::LinesStrip, 2);
        line[0].position = p1;
        line[1].position = p2;
        line[0].color = sf::Color::White;
        line[1].color = sf::Color::White;
        return line;
    }

public:
    Vector2f p2;
    Vector2f p1;
    Vector2f direction;
    sf::VertexArray line;

    Line(Vector2f p1, Vector2f p2) {
        this->p1 = p1;
        this->p2 = p2;
        this->direction = p2 - p1;// prod(p2 - p1, 1 / abs(p2 - p1));
        this->line = line2P(p1, p2);
    }
};

class Boundary : public Line {
protected:

public:
    Boundary(Vector2f p1, Vector2f p2) : Line(p1, p2) {
    }
};

class Ray : public Line {
private:
    double currentMinDistance = 10000;
protected:

public:
    Ray(Vector2f p1, Vector2f p2) : Line(p1, p2) {

    }

    void computeBoundaryIntersection(Boundary &boundary) {
        Vector2f a = direction, l = boundary.direction;
        if (det(a, l) != 0) {
            double x1 = p1.x, y1 = p1.y, x2 = p2.x, y2 = p2.y;
            double x3 = boundary.p1.x, y3 = boundary.p1.y, x4 = boundary.p2.x, y4 = boundary.p2.y;

            double t =
                    ((x1 - x3) * (y3 - y4) - (y1 - y3) * (x3 - x4)) / ((x1 - x2) * (y3 - y4) - (y1 - y2) * (x3 - x4));
            double u =
                    -((x1 - x2) * (y1 - y3) - (y1 - y2) * (x1 - x3)) / ((x1 - x2) * (y3 - y4) - (y1 - y2) * (x3 - x4));
            std::cout << "t, u:" << t << " " << u << std::endl;
            if (abs(prod(direction, t)) <= currentMinDistance && t >= 0 && u <= 1) {
                currentMinDistance = abs(prod(direction, t));
                this->line = line2P(p1, p1 + prod(direction, t));
            }
        }
    }
};


class Scene {
protected:
    sf::RenderWindow *window;

public:
    std::vector<Boundary> boundaries;

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
    sf::RenderWindow *window;
    Vector2f origin;
    std::vector<Ray> rays;

public:
    LightSource(sf::RenderWindow &window, Vector2f origin) {
        this->window = &window;
        this->origin = origin;
        for (int i = 0; i < 20; ++i) {
            rays.emplace_back(Ray(origin, origin + Vector2f(cos(i * 3.14 / 10), sin(i * 3.14 / 10))));
        }
    }

    void drawSource() {
        for (const auto &ray : rays) {
            window->draw(ray.line);
        }
    }

    void computeSceneInteraction(Scene &scene) {
        for (auto &ray : rays) {
            for (auto &boundary : scene.boundaries) {
                ray.computeBoundaryIntersection(boundary);
            }
        }
    }
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
            boundary2(Vector2f(200, 800), Vector2f(800, 50)),
            boundary3(Vector2f(0, 0), Vector2f(150, 400));


    Scene scene(window);
    scene.addBoundary(boundary1);
    scene.addBoundary(boundary2);
    scene.addBoundary(boundary3);

    LightSource source(window, Vector2f(200, 400));
    source.computeSceneInteraction(scene);

    scene.draw();
    source.drawSource();

    //////////////////////////////////////////////////////////////////////////
    window.display();
    keepWindowOpened(window);
    return 0;
}
