#include <stdlib.h>
#include <time.h>

#include <SFML/Graphics.hpp>
#include <algorithm>
#include <array>
#include <chrono>
#include <cmath>
#include <iostream>
#include <optional>
#include <thread>
#include <vector>

const sf::Vector2f viewportSize = {1300, 840};
const sf::FloatRect simulationBounds = {400, 160, 740, 665};
const sf::FloatRect grabberBounds = {
    simulationBounds.left + 20, simulationBounds.top + 20,
    simulationBounds.width - 40, simulationBounds.height - 40};

class CollidingObject;

struct MovingResult {
    CollidingObject* collidedWith = nullptr;

    bool verticalCollision = false;
    bool horizontalCollision = false;

    bool anyCollision() {
        return verticalCollision || horizontalCollision ||
               collidedWith != nullptr;
    }

    // Combines two moving results
    MovingResult combine(MovingResult result) {
        verticalCollision = verticalCollision || result.verticalCollision;
        horizontalCollision = horizontalCollision || result.horizontalCollision;
        collidedWith =
            collidedWith == nullptr ? result.collidedWith : collidedWith;

        return *this;
    }
};

class CollidingObject : public sf::Drawable {
private:
    sf::Vector2f velocity = {0, 0};
    sf::Vector2f position = {0, 0};

    const double maxSpeed = 200;
    sf::Color color;

    MovingResult applyVelocity(double factor) {
        sf::Vector2f factoredVector(velocity.x * factor, velocity.y * factor);

        return moveTo(getPosition() + factoredVector);
    }

public:
    virtual bool contains(sf::Vector2f) const = 0;
    virtual std::vector<sf::Vector2f> getBoundPoints() const = 0;
    virtual bool canBePicked() const = 0;

    void setColor(sf::Color color) { this->color = color; }

    sf::Color getColor() const { return color; }

    MovingResult moveTo(const sf::Vector2f& newPos) {
        auto currentPositionCopy = getPosition();

        setPosition(newPos);

        for (CollidingObject* object : CollidingObject::All) {
            if (object == this) continue;

            if (this->testCollisionWith(*object)) {
                setPosition(currentPositionCopy);
                MovingResult result;
                result.collidedWith = object;
                return result;
            }
        }

        bool XRest = false, YRest = false;

        for (sf::Vector2f point : getBoundPoints()) {
            if (XRest && YRest) break;

            if (!XRest) {
                if (point.x < simulationBounds.left ||
                    (point.x >
                     (simulationBounds.left + simulationBounds.width))) {
                    XRest = true;
                    velocity.x = 0;
                }
            }
            if (!YRest) {
                if (point.y < simulationBounds.top ||
                    ((point.y) >
                     (simulationBounds.top + simulationBounds.height))) {
                    YRest = true;
                    velocity.y = 0;
                }
            }
        }

        setPosition(sf::Vector2f((XRest) ? currentPositionCopy.x : newPos.x,
                                 (YRest) ? currentPositionCopy.y : newPos.y));

        MovingResult result;
        result.horizontalCollision = XRest;
        result.verticalCollision = YRest;

        return result;
    }

    void tick() {
        MovingResult result;

        for (int i = 1; i <= 10; i++) {
            result.combine(applyVelocity(0.1));
            if (result.horizontalCollision) velocity.x = 0;
            if (result.verticalCollision) velocity.y = 0;

            if (velocity.y == 0 && velocity.x == 0) break;
        }

        if (result.verticalCollision && !result.horizontalCollision)
            velocity.x *= 0.93;

        velocity = {velocity.x * .985f, velocity.y * .985f};
    }

    sf::Vector2f getVelocity() { return velocity; }

    void setVelocity(sf::Vector2f vel) { velocity = std::move(vel); }

    void addVelocity(sf::Vector2f translation) {
        velocity += translation;
        if (std::abs(velocity.x) > maxSpeed)
            velocity.x = ((velocity.x < 0) ? -1 : 1) * maxSpeed;
        else if (std::abs(velocity.y) > maxSpeed)
            velocity.y = ((velocity.y < 0) ? -1 : 1) * maxSpeed;
    }

    bool testCollisionWith(CollidingObject& obj) {
        for (auto point : getBoundPoints())
            if (obj.contains(point)) return true;

        return false;
    }

    void setPosition(sf::Vector2f newPos) { position = std::move(newPos); }

    sf::Vector2f getPosition() const { return position; }

    static std::vector<CollidingObject*> All;
};

std::vector<CollidingObject*> CollidingObject::All = {};

class LiftGrabber : public sf::Drawable {
private:
    sf::Vector2f position = {grabberBounds.left, grabberBounds.top};
    sf::Vector2f velocity = {0, 0};
    const double maxSpeed = 2;

    CollidingObject* grabbedObject = nullptr;

    bool applyVelocity(double factor) {
        sf::Vector2f factoredVector(velocity.x * factor, velocity.y * factor);

        if (grabbedObject == nullptr) {
            return setPosition(getPosition() + factoredVector);
        } else {
            auto result = grabbedObject->moveTo(grabbedObject->getPosition() +
                                                factoredVector);

            if (result.collidedWith != nullptr) factoredVector = {0, 0};
            if (result.horizontalCollision) factoredVector.x = 0;
            if (result.verticalCollision) factoredVector.y = 0;

            return setPosition(getPosition() + factoredVector);
        }

        return true;
    }

public:
    LiftGrabber() {}

    virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const {
        sf::CircleShape grabber;
        grabber.setOrigin({5, 5});
        grabber.setPosition(position);
        grabber.setRadius(5);
        grabber.setOutlineColor(sf::Color::Red);
        grabber.setOutlineThickness(3);

        sf::Vertex lines[4] = {
            sf::Vertex({position.x - 10, simulationBounds.top},
                       sf::Color::Black),
            sf::Vertex({position.x, position.y - 5}, sf::Color::Black),
            sf::Vertex({position.x + 10, simulationBounds.top},
                       sf::Color::Black),
            sf::Vertex({position.x, position.y - 5}, sf::Color::Black)};

        target.draw(grabber);
        target.draw(lines, 4, sf::Lines);
    }

    sf::Vector2f getPosition() const { return position; }

    void tick() {
        for (int i = 1; i <= 10; i++) {
            if (!applyVelocity(0.1)) break;
        }

        velocity = {velocity.x * .9f, velocity.y * .9f};
    }

    sf::Vector2f getVelocity() { return velocity; }

    void setVelocity(sf::Vector2f vel) { velocity = std::move(vel); }

    void addVelocity(sf::Vector2f translation) {
        velocity += translation;
        if (std::abs(velocity.x) > maxSpeed)
            velocity.x = ((velocity.x < 0) ? -1 : 1) * maxSpeed;
        else if (std::abs(velocity.y) > maxSpeed)
            velocity.y = ((velocity.y < 0) ? -1 : 1) * maxSpeed;
    }

    bool setPosition(sf::Vector2f newPos) {
        if (newPos.x > (grabberBounds.left + grabberBounds.width))
            newPos.x = grabberBounds.left + grabberBounds.width;
        if (newPos.y > (grabberBounds.top + grabberBounds.height))
            newPos.y = grabberBounds.top + grabberBounds.height;
        if (newPos.x < grabberBounds.left) newPos.x = grabberBounds.left;
        if (newPos.y < grabberBounds.top) newPos.y = grabberBounds.top;

        if (position.x == newPos.x && position.y == newPos.y) return false;

        position = std::move(newPos);
        return true;
    }

    void grab(CollidingObject* object) { grabbedObject = object; }

    void letGo() {
        if (grabbedObject == nullptr) return;

        grabbedObject->addVelocity(velocity);
        grabbedObject = nullptr;
    }

    CollidingObject* currentlyGrabbed() { return grabbedObject; }
};

class RectangularObject : public CollidingObject {
private:
    sf::Vector2f size;

public:
    RectangularObject() {}

    sf::Vector2f getSize() const { return size; }

    void setSize(sf::Vector2f newSize) { size = std::move(newSize); }

    virtual bool contains(sf::Vector2f point) const {
        return ((point.x >= getPosition().x)) &&
               ((point.x <= getPosition().x + getSize().x)) &&
               ((point.y >= getPosition().y)) &&
               ((point.y <= getPosition().y + getSize().y));
    }

    virtual std::vector<sf::Vector2f> getBoundPoints() const {
        auto position = getPosition();
        std::vector<sf::Vector2f> points;

        std::array<sf::Vector2f, 4> corners = {
            position + sf::Vector2f(getSize().x, 0),
            position + sf::Vector2f(getSize().x, getSize().y),
            position + sf::Vector2f(0, getSize().y), position};

        std::array<sf::Vector2f, 4> directions = {
            sf::Vector2f(getSize().x / 5, 0), sf::Vector2f(0, getSize().y / 5),
            sf::Vector2f(-getSize().x / 5, 0),
            sf::Vector2f(0, -getSize().y / 5)};

        int i = 0;
        sf::Vector2f p = position;
        while (i < 4) {
            if (std::abs(p.x - corners[i].x) < 2 &&
                std::abs(p.y - corners[i].y) < 2) {
                p = corners[i];
                i++;
            } else {
                p += directions[i];
            }

            points.push_back(p);
        }

        return points;
    }

    virtual inline bool canBePicked() const { return true; }

    virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const {
        sf::RectangleShape rect;
        rect.setPosition(getPosition());
        rect.setSize(getSize());
        rect.setFillColor(getColor());

        target.draw(rect);
    }
};

class CircularObject : public CollidingObject {
private:
    double radius = 0;

public:
    CircularObject() {}

    double getRadius() const { return radius; }

    void setRadius(double newRadius) { radius = newRadius; }

    virtual bool contains(sf::Vector2f point) const {
        auto newVec = getPosition() - point;
        return ((newVec.x * newVec.x) + (newVec.y * newVec.y)) <=
               radius * radius;
    }

    virtual std::vector<sf::Vector2f> getBoundPoints() const {
        std::vector<sf::Vector2f> points;
        for (double angle = 0; angle < 2 * 3.14;
             angle += 2 * 3.14 / (2 * 3.14 * radius / 2)) {
            points.push_back(
                sf::Vector2f(getPosition().x + radius * std::cos(angle),
                             getPosition().y + radius * std::sin(angle)));
        }

        return points;
    }

    virtual inline bool canBePicked() const { return false; }

    virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const {
        sf::CircleShape circle;
        circle.setPosition(getPosition());
        circle.setOrigin(sf::Vector2f(radius, radius));
        circle.setRadius(getRadius());
        circle.setFillColor(getColor());

        target.draw(circle);
    }
};

const sf::Color ObjectsColors[] = {sf::Color(0x264653ff), sf::Color(0x2a9d8fff),
                                   sf::Color(0xe9c46aff)};

CollidingObject* randomObjectAt(sf::Vector2f& position) {
    switch (rand() % 2) {
        case 0: {
            CircularObject* obj = new CircularObject;
            obj->setRadius(rand() % 10 + 20);
            if (obj->moveTo(position).anyCollision()) {
                delete obj;
                return nullptr;
            }

            return obj;
        }
        case 1: {
            RectangularObject* obj = new RectangularObject;
            const int size = rand() % 20 + 30;
            obj->setSize(sf::Vector2f(size, size));
            if (obj->moveTo(position).anyCollision()) {
                delete obj;
                return nullptr;
            }
            return obj;
        }
        default:
            return nullptr;
    }
}

void addObject(sf::Vector2f position) {
    CollidingObject* obj = randomObjectAt(position);
    if (obj == nullptr) return;
    obj->setColor(
        ObjectsColors[rand() % (sizeof(ObjectsColors) / sizeof(sf::Color))]);
    CollidingObject::All.push_back(obj);
}

int main() {
    srand(time(NULL));

    sf::RenderWindow window(sf::VideoMode::getDesktopMode(), "Projekt 4",
                            sf::Style::Default);

    window.setFramerateLimit(60);
    window.setView(sf::View(sf::FloatRect(sf::Vector2f(0, 0), viewportSize)));

    sf::RectangleShape background;
    background.setFillColor(sf::Color::White);
    background.setPosition({0, 0});
    background.setSize(viewportSize);

    sf::Texture tex;
    tex.loadFromFile("dzwig.png");
    sf::Sprite lift(tex);

    lift.setPosition({10, 10});
    lift.scale({(viewportSize.x - 20) / tex.getSize().x,
                (viewportSize.y - 20) / tex.getSize().y});

    LiftGrabber liftGrabber;

    auto prevTime = 0.0;
    double timeSum = 0.0;
    unsigned int frameCount = 0;

    while (window.isOpen()) {
        auto begin = std::chrono::system_clock::now();

        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
                for (CollidingObject* object : CollidingObject::All) {
                    delete object;
                }
                return 0;
            } else if (event.type == sf::Event::MouseButtonPressed) {
                if (event.mouseButton.button == sf::Mouse::Left) {
                    auto windowSize = window.getSize();

                    addObject(sf::Vector2f(
                        (viewportSize.x / windowSize.x) * event.mouseButton.x,
                        (viewportSize.y / windowSize.y) * event.mouseButton.y));
                }
            }
        }

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right)) {
            liftGrabber.addVelocity(sf::Vector2f(10 * prevTime, 0));
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left)) {
            liftGrabber.addVelocity(sf::Vector2f(-10 * prevTime, 0));
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up)) {
            liftGrabber.addVelocity(sf::Vector2f(0, -10 * prevTime));
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down)) {
            liftGrabber.addVelocity(sf::Vector2f(0, 10 * prevTime));
        }

        for (CollidingObject* object : CollidingObject::All) {
            if (object != liftGrabber.currentlyGrabbed()) {
                object->addVelocity(sf::Vector2f(0, 5 * prevTime));
            }
        }

        liftGrabber.tick();

        static bool grabbed = false;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space)) {
            if (!grabbed) {
                if (liftGrabber.currentlyGrabbed() != nullptr && !grabbed)
                    liftGrabber.letGo();
                else {
                    for (CollidingObject* object : CollidingObject::All) {
                        if (object->contains(liftGrabber.getPosition()) &&
                            object->canBePicked()) {
                            liftGrabber.grab(object);
                            object->setVelocity({0, 0});
                        }
                    }
                }
            }

            grabbed = true;
        } else
            grabbed = false;

        for (CollidingObject* object : CollidingObject::All) {
            if (object != liftGrabber.currentlyGrabbed()) {
                object->tick();
            }
        }

        window.clear();

        window.draw(background);
        window.draw(lift);
        window.draw(liftGrabber);

        for (CollidingObject* object : CollidingObject::All) {
            window.draw(*object);
        }

        window.display();

        prevTime = std::chrono::duration_cast<std::chrono::microseconds>(
                       std::chrono::system_clock::now() - begin)
                       .count() /
                   1000000.0;
        timeSum += prevTime;
        frameCount++;
        if (timeSum >= 1) {
            std::cout << "FPS:" << frameCount << std::endl;
            timeSum = 0;
            frameCount = 0;
        }
    }

    return 0;
}
