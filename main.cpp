#include <cmath>
#include <iostream>
#include <SFML/Graphics.hpp>

typedef sf::Vector2f Vector2f;
typedef sf::Vector2i Vector2i;

class Line;

class Boundary;

class Ray;

class Scene;

class LightSource;

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

double det(Vector2f a, Vector2f l) {
    return (a.x * l.y - a.y * l.x);
}

float abs(Vector2f a) {
    return sqrt(a.x * a.x + a.y * a.y);
}

Vector2f prod(Vector2f vector, double num) {
    return Vector2f(vector.x * num, vector.y * num);
}

Vector2f normalize(Vector2f &vector) {
    return prod(vector, 1 / abs(vector));
}

Vector2f normalVector(Vector2f &vector) {
    return Vector2f(-vector.y, vector.x);
}

bool isEqual(double num1, double num2) {
    double absoluteEpsilon = 0.0001;
    return fabs(num2 - num1) <= absoluteEpsilon;
}

double scalarProd(Vector2f v1, Vector2f v2) {
    return v1.x * v2.x + v1.y * v2.y;
}

//
//sf::Vector2f Vector2f(const vectorDouble &vector){
//    return sf::Vector2f(vector[0],vector[1]);
//}

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

    void drawLine(sf::RenderWindow &window) {
        window.draw(line);
    }
};

class Boundary : public Line {
private:
    int code;

public:
    int getCode() const {
        return code;
    }

    void setCode(int code) {
        Boundary::code = code;
    }

private:
    bool globalBoundaryQ;
public:
    bool getGlobalBoundaryQ() const {
        return globalBoundaryQ;
    }

    void setGlobalBoundaryQ(bool globalBoundary) {
        Boundary::globalBoundaryQ = globalBoundary;
    }

public:
    Boundary(Vector2f p1 = Vector2f(0, 0), Vector2f p2 = Vector2f(0, 0), bool globalBoundary = false) :
            Line(p1, p2) {
        this->globalBoundaryQ = globalBoundary;
    }

};

class Ray : public Line {
private:
    double currentMinDistance = -1;
    Boundary currentClosestBoundary;
    int reflectedBoundaryCode = -1;

public:
    const Boundary &getCurrentClosestBoundary() const {
        return currentClosestBoundary;
    }

    void setCurrentClosestBoundary(const Boundary &currentClosestBoundary) {
        Ray::currentClosestBoundary = currentClosestBoundary;
    }

public:
    Ray(Vector2f p1 = Vector2f(0, 0), Vector2f p2 = Vector2f(0, 0), int reflectedBoundaryCode = -1) : Line(p1, p2) {
        this->reflectedBoundaryCode = reflectedBoundaryCode;
    }

    void computeBoundaryIntersection(Boundary &boundary) {
        Vector2f a = direction, l = boundary.direction;
        if (!isEqual(det(a, l), 0)) {
            double x1 = p1.x, y1 = p1.y, x2 = p2.x, y2 = p2.y;
            double x3 = boundary.p1.x, y3 = boundary.p1.y, x4 = boundary.p2.x, y4 = boundary.p2.y;

            double t =
                    ((x1 - x3) * (y3 - y4) - (y1 - y3) * (x3 - x4)) / ((x1 - x2) * (y3 - y4) - (y1 - y2) * (x3 - x4));
            double u =
                    -((x1 - x2) * (y1 - y3) - (y1 - y2) * (x1 - x3)) / ((x1 - x2) * (y3 - y4) - (y1 - y2) * (x3 - x4));
            //std::cout << "t, u:" << t << " " << u << std::endl;
            if (t >= 0 && u >= 0 && u <= 1 &&
                ((abs(prod(direction, t)) < currentMinDistance) || (currentMinDistance == -1)) &&
                boundary.getCode() != reflectedBoundaryCode) {

                currentMinDistance = abs(prod(direction, t));
                currentClosestBoundary = boundary;

                line = line2P(p1, p1 + prod(direction, t));
                p2 = p1 + prod(direction, t);
                direction = prod(direction, t);
            }
        }
    }

    Ray computeBoundaryReflection() {
        Vector2f a = direction, l = currentClosestBoundary.direction;
        l = normalize(l);
        a = normalize(a);
        Vector2f n = normalVector(l);
        Vector2f refVector = a - prod(n, 2 * scalarProd(n, a));
        return Ray(p2, p2 + refVector, currentClosestBoundary.getCode());
    }


    Ray computePassThrough() {
        Vector2f a = direction;
        a = normalize(a);
        return Ray(p2, p2 + a, currentClosestBoundary.getCode());
    }
};

void drawLineSet(std::vector<Ray> rays, sf::RenderWindow &window) {
    for (auto &ray : rays) {
        ray.drawLine(window);
    }
}

void drawLineSet(std::vector<Boundary> rays, sf::RenderWindow &window) {
    for (auto &ray : rays) {
        ray.drawLine(window);
    }
}

class Scene {
protected:
    sf::RenderWindow *window;

public:
    std::vector<Boundary> boundaries;

    Scene(sf::RenderWindow &window) {
        sf::Vector2u windowSize = window.getSize();
        boundaries.emplace_back(Vector2f(0, 0), Vector2f(windowSize.x, 0), true);
        boundaries.emplace_back(Vector2f(0, 0), Vector2f(0, windowSize.y), true);
        boundaries.emplace_back(Vector2f(windowSize.x, 0), Vector2f(windowSize.x, windowSize.y), true);
        boundaries.emplace_back(Vector2f(0, windowSize.y), Vector2f(windowSize.x, windowSize.y), true);

        for (int i = 0; i < boundaries.size(); ++i) {
            boundaries[i].setCode(i);
        }

        this->window = &window;
    }

    void addBoundary(Boundary &boundary) {
        boundaries.push_back(boundary);
        updateLastBoundaryCode();
    }

    void setColor(sf::Color color) {
        for (auto &boundary : boundaries) {
            boundary.line[0].color = color;
            boundary.line[1].color = color;
        }
    }

    void drawScene() {
        drawLineSet(boundaries, *window);
    }


    void updateLastBoundaryCode() {
        boundaries[boundaries.size() - 1].setCode(boundaries.size() - 1);
    }
};

class LightSource {
protected:
    sf::RenderWindow *window;

public:
    Vector2f origin;

    std::vector<std::vector<Ray>> rays;

    LightSource(sf::RenderWindow &window, Vector2f origin, int n = 10) {
        this->window = &window;
        this->origin = origin;
        rays.resize(2);
        for (int i = 0; i < n; ++i) {
            rays[0].emplace_back(Ray(origin, origin + Vector2f(cos(i * 2 * 3.14 / n), sin(i * 2 * 3.14 / n + 1))));
        }
    }


    void drawSource() {
        sf::CircleShape originDot;
        originDot.setPosition(origin - Vector2f(10, 10));
        originDot.setFillColor(sf::Color::White);
        originDot.setOutlineColor(sf::Color::White);
        originDot.setRadius(10);
        window->draw(originDot);

        for (auto &generation : rays) {
            for (auto &ray : generation) {
                window->draw(ray.line);
            }
        }
    }

    void computeSceneInteraction(Scene &scene, int reflectionDepth = 0) {
        for (auto &ray : rays[0]) {
            for (auto &boundary : scene.boundaries) {
                ray.computeBoundaryIntersection(boundary);
            }
        }

        drawLineSet(rays[0], *window);

        for (int i = 1; i < reflectionDepth + 1; ++i) {
            for (auto &ray : rays[0]) {
                Ray reflection = ray.computeBoundaryReflection();
                for (auto &boundary : scene.boundaries) {
                    reflection.computeBoundaryIntersection(boundary);
                }
                rays[1].emplace_back(reflection);

                if (!ray.getCurrentClosestBoundary().getGlobalBoundaryQ()){
                    Ray passed = ray.computePassThrough();
                    for (auto &boundary : scene.boundaries) {
                        passed.computeBoundaryIntersection(boundary);
                    }
                    rays[1].emplace_back(passed);
                }
            }

            drawLineSet(rays[1], *window);

            rays[0].resize(rays[1].size());
            rays[0] = rays[1];
            rays[1].resize(0);
        }


    }
};

int main() {
    int windowWidth = 900;
    int windowHeight = 900;

    sf::ContextSettings settings;
    settings.majorVersion = 4;
    settings.antialiasingLevel = 8;

    sf::RenderWindow window(sf::VideoMode(windowWidth, windowHeight), "RayTracing",
                            sf::Style::Default, settings);

    window.clear(sf::Color::Black);

    //sf::View view;
    //view.reset(sf::FloatRect())

    //////////////////////////////////////////////////////////////////////////

    Boundary boundary1(Vector2f(100, 100), Vector2f(500, 300)),
            boundary2(Vector2f(200, 800), Vector2f(400, 50)),
            boundary3(Vector2f(500, 0), Vector2f(150, 400));


    Scene scene(window);
    scene.addBoundary(boundary1);
    scene.addBoundary(boundary2);
    scene.addBoundary(boundary3);
    scene.setColor(sf::Color::Red);

    LightSource source(window, Vector2f(700, 400), 1);
    source.computeSceneInteraction(scene, 4);

    source.drawSource();
    scene.drawScene();


    //////////////////////////////////////////////////////////////////////////
    window.display();
    keepWindowOpened(window);
    return 0;
}
