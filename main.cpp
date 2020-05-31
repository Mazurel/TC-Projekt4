#include <vector>
#include <chrono>
#include <thread>
#include <optional>
#include <algorithm>
#include <cmath>

#include <SFML/Graphics.hpp>

#include <iostream>

const sf::Vector2f viewportSize = {1300, 840};
const sf::FloatRect simulationBounds = {400, 160, 740, 665};
const sf::FloatRect grabberBounds =
    {
        simulationBounds.left + 20,
        simulationBounds.top + 20,
        simulationBounds.width - 40,
        simulationBounds.height - 40};

class CollidingObject : public sf::Drawable, public sf::Transformable
{
private:
    bool collided = false;
    sf::Vector2f velocity = { 0, 0 };
    const double maxSpeed = 100;

    bool applyVelocity(double factor)
    {
        sf::Vector2f factoredVector(velocity.x * factor, velocity.y * factor);

        return setPosition(getPosition() + factoredVector);
    }

public:
    virtual bool contains(sf::Vector2f) const = 0;
    virtual std::vector<sf::Vector2f> getBoundPoints() const = 0;
    virtual bool canBePicked() const = 0;
    virtual bool setPosition(const sf::Vector2f &newPos) = 0;

    void tick()
    {
        for (int i = 1; i <= 10; i++)
        {
            if (!applyVelocity(0.1))
                {
                    velocity = {0 , 0};
                    break;
                }
        }

        velocity = { velocity.x * .99f, velocity.y * .99f }; 
    }

    sf::Vector2f getVelocity()
    {
        return velocity;
    }

    void setVelocity(sf::Vector2f vel)
    {
        velocity = std::move(vel);
    }

    void addVelocity(sf::Vector2f translation)
    {
        velocity += translation;
        if (std::abs(velocity.x) > maxSpeed) velocity.x = ((velocity.x < 0) ? -1 : 1) * maxSpeed;
        else if (std::abs(velocity.y) > maxSpeed) velocity.y = ((velocity.y < 0) ? -1 : 1) * maxSpeed;
    }

    void collision()
    {
        collided = true;
    }

    bool hasCollided()
    {
        return collided;
    }

    void resetCollision()
    {
        collided = false;
    }

    bool testCollisionWith(CollidingObject& obj)
    {
        for (auto point: getBoundPoints())
            if (obj.contains(point))
            {
                return true;
            }
        
        return false;
    }

    static std::vector<CollidingObject *> All;
};

std::vector<CollidingObject *> CollidingObject::All = {};

class LiftGrabber : public sf::Drawable
{
private:
    sf::Vector2f position = {grabberBounds.left, grabberBounds.top};
    sf::Vector2f velocity = { 0, 0 };
    const double maxSpeed = 2;

    CollidingObject *grabbedObject = nullptr;

    bool applyVelocity(double factor)
    {
        sf::Vector2f factoredVector(velocity.x * factor, velocity.y * factor);

        if (grabbedObject == nullptr)
        {
            return setPosition(getPosition() + factoredVector);
        }
        else
        {
            grabbedObject->setPosition(grabbedObject->getPosition() + factoredVector);
            if (!grabbedObject->hasCollided())
            {
                return setPosition(getPosition() + factoredVector);
            }
        }

        return true;
    }

public:
    LiftGrabber()
    {
    }

    virtual void draw(sf::RenderTarget &target, sf::RenderStates states) const
    {
        sf::CircleShape grabber;
        grabber.setOrigin({5, 5});
        grabber.setPosition(position);
        grabber.setRadius(5);
        grabber.setOutlineColor(sf::Color::Red);
        grabber.setOutlineThickness(3);

        sf::Vertex lines[4] =
            {
                sf::Vertex({position.x - 10, simulationBounds.top}, sf::Color::Black),
                sf::Vertex({position.x, position.y - 5}, sf::Color::Black),
                sf::Vertex({position.x + 10, simulationBounds.top}, sf::Color::Black),
                sf::Vertex({position.x, position.y - 5}, sf::Color::Black)};

        target.draw(grabber);
        target.draw(lines, 4, sf::Lines);
    }

    sf::Vector2f getPosition() const
    {
        return position;
    }

    void tick()
    {
        for (int i = 1; i <= 10; i++)
        {
            if (!applyVelocity(0.1))
                break;
        }

        velocity = { velocity.x * .9f, velocity.y * .9f }; 
    }

    sf::Vector2f getVelocity()
    {
        return velocity;
    }

    void setVelocity(sf::Vector2f vel)
    {
        velocity = std::move(vel);
    }

    void addVelocity(sf::Vector2f translation)
    {
        velocity += translation;
        if (std::abs(velocity.x) > maxSpeed) velocity.x = ((velocity.x < 0) ? -1 : 1) * maxSpeed;
        else if (std::abs(velocity.y) > maxSpeed) velocity.y = ((velocity.y < 0) ? -1 : 1) * maxSpeed;
    }

    bool setPosition(sf::Vector2f newPos)
    {
        if (newPos.x > (grabberBounds.left + grabberBounds.width))
            newPos.x = grabberBounds.left + grabberBounds.width;
        if (newPos.y > (grabberBounds.top + grabberBounds.height))
            newPos.y = grabberBounds.top + grabberBounds.height;
        if (newPos.x < grabberBounds.left)
            newPos.x = grabberBounds.left;
        if (newPos.y < grabberBounds.top)
            newPos.y = grabberBounds.top;

        if (position.x == newPos.x && position.y == newPos.y) return false;

        position = std::move(newPos);
        return true;
    }

    void grab(CollidingObject *object)
    {
        grabbedObject = object;
    }

    void letGo()
    {
        if (grabbedObject == nullptr)
            return;

        grabbedObject->addVelocity(velocity);
        grabbedObject = nullptr;
    }

    CollidingObject *currentlyGrabbed()
    {
        return grabbedObject;
    }
};

class RectangularObject : public CollidingObject
{
private:
    sf::Vector2f size;

public:
    RectangularObject()
    {
    }

    bool setPosition(const sf::Vector2f &newPos) override
    {
        auto currentPositionCopy = getPosition();

        Transformable::setPosition(newPos);

        for (CollidingObject *object : CollidingObject::All)
        {
            if (object == this)
                continue;
            auto points = object->getBoundPoints();

            if (this->testCollisionWith(*object))
            {
                this->collision();
                object->collision();

                Transformable::setPosition(currentPositionCopy);
                return false;
            }
        }

        Transformable::setPosition(currentPositionCopy);

        auto position = newPos;

        if (position.x + getSize().x > (simulationBounds.left + simulationBounds.width))
            position.x = simulationBounds.left + simulationBounds.width;
        if ((position.y + getSize().y) > (simulationBounds.top + simulationBounds.height))
        {
            position.y = simulationBounds.top + simulationBounds.height - getSize().y;
            setVelocity({getVelocity().x * .5f, 0});
        }
        if (position.x < simulationBounds.left)
            position.x = simulationBounds.left;
        if (position.y < simulationBounds.top)
            position.y = simulationBounds.top;

        Transformable::setPosition(position);

        return true;
    }

    sf::Vector2f getSize() const
    {
        return size;
    }

    void setSize(sf::Vector2f newSize)
    {
        size = std::move(newSize);
    }

    virtual bool contains(sf::Vector2f point) const
    {
        return sf::FloatRect(getPosition(), getSize()).contains(point);
    }

    virtual std::vector<sf::Vector2f> getBoundPoints() const
    {
        return {
            getPosition(),
            getPosition() + sf::Vector2f(getSize().x, 0),
            getPosition() + sf::Vector2f(0, getSize().y),
            getPosition() + getSize()};
    }

    virtual inline bool canBePicked() const
    {
        return true;
    }

    virtual void draw(sf::RenderTarget &target, sf::RenderStates states) const
    {
        states.transform *= getTransform();

        sf::RectangleShape rect;
        rect.setPosition({0, 0});
        rect.setSize(getSize());
        rect.setFillColor(sf::Color::Red);

        target.draw(rect, states);
    }
};

class CircularObject : public CollidingObject
{
private:
    double radius = 0;

public:
    CircularObject()
    {
    }

    bool setPosition(const sf::Vector2f &newPos) override
    {
        auto currentPositionCopy = getPosition();

        Transformable::setPosition(newPos);

        for (CollidingObject *object : CollidingObject::All)
        {
            if (object == this)
                continue;
            auto points = object->getBoundPoints();

            if (this->testCollisionWith(*object))
            {
                this->collision();
                object->collision();

                Transformable::setPosition(currentPositionCopy);
                return false;
            }
        }

        Transformable::setPosition(currentPositionCopy);

        auto position = newPos;

        if (position.x + getRadius() > (simulationBounds.left + simulationBounds.width))
            position.x = simulationBounds.left + simulationBounds.width;
        if ((position.y + getRadius()) > (simulationBounds.top + simulationBounds.height))
        {
            position.y = simulationBounds.top + simulationBounds.height - getRadius();
            setVelocity({getVelocity().x * .5f, 0});
        }
        if (position.x < simulationBounds.left)
            position.x = simulationBounds.left;
        if (position.y < simulationBounds.top)
            position.y = simulationBounds.top;

        Transformable::setPosition(position);

        return true;
    }

    double getRadius() const
    {
        return radius;
    }

    void setRadius(double newRadius)
    {
        radius = newRadius;
    }

    virtual bool contains(sf::Vector2f point) const
    {
        return std::abs(point.x - getPosition().x) < radius && std::abs(getPosition().y - point.y) < radius;
    }

    virtual std::vector<sf::Vector2f> getBoundPoints() const
    {
        std::vector<sf::Vector2f> points;
        for (double angle = 0; angle < 2 * 3.14; angle += 2 * 3.14 / 20)
        {
            points.push_back(sf::Vector2f(radius * std::cos(angle), radius * std::sin(radius)) + getPosition());
        }

        return points;
    }

    virtual inline bool canBePicked() const
    {
        return false;
    }

    virtual void draw(sf::RenderTarget &target, sf::RenderStates states) const
    {
        states.transform *= getTransform();

        sf::CircleShape circle;
        circle.setPosition({0, 0});
        circle.setRadius(getRadius());
        circle.setFillColor(sf::Color::Red);

        target.draw(circle, states);
    }
};

void addObject(sf::Vector2f position)
{
    if (simulationBounds.contains(position))
    {
        CircularObject* obj = new CircularObject;
        obj->setPosition(position);
        obj->setRadius(20);
        CollidingObject::All.push_back(obj);
    }
}

int main()
{
    sf::RenderWindow window(sf::VideoMode(viewportSize.x, viewportSize.y), "Projekt 4", sf::Style::Default, sf::ContextSettings(0, 0, 3U, 1, 1, 0, false));
    window.setFramerateLimit(60);

    sf::RectangleShape background;
    background.setFillColor(sf::Color::White);
    background.setPosition({0, 0});
    background.setSize(viewportSize);

    sf::Texture tex;
    tex.loadFromFile("dzwig.png");
    sf::Sprite lift(tex);

    lift.setPosition({10, 10});
    lift.scale({(viewportSize.x - 20) / tex.getSize().x, (viewportSize.y - 20) / tex.getSize().y});

    LiftGrabber liftGrabber;

    auto prevTime = 0.0;

    while (window.isOpen())
    {
        auto begin = std::chrono::system_clock::now();

        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
            {
                window.close();
                for (CollidingObject *object : CollidingObject::All)
                {
                    delete object;
                }
            }
            else if (event.type == sf::Event::MouseButtonPressed)
            {
                if (event.mouseButton.button == sf::Mouse::Left)
                {
                    addObject(sf::Vector2f(event.mouseButton.x, event.mouseButton.y));
                }
            }
        }

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right))
        {
            liftGrabber.addVelocity(sf::Vector2f(100 * prevTime, 0));
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left))
        {
            liftGrabber.addVelocity(sf::Vector2f(-100 * prevTime, 0));
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up))
        {
            liftGrabber.addVelocity(sf::Vector2f(0, -100 * prevTime));
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down))
        {
            liftGrabber.addVelocity(sf::Vector2f(0, 100 * prevTime));
        }
        
        for (CollidingObject *object : CollidingObject::All)
        {
            object->resetCollision();
            if (object != liftGrabber.currentlyGrabbed())
            {
                object->addVelocity(sf::Vector2f(0, 5 * prevTime));
            }
        }

        liftGrabber.tick();

        static bool grabbed = false;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space))
        {
            if (!grabbed)
            {
                if (liftGrabber.currentlyGrabbed() != nullptr && !grabbed)
                    liftGrabber.letGo();
                else
                {
                    for (CollidingObject *object : CollidingObject::All)
                    {
                        if (object->contains(liftGrabber.getPosition()) && object->canBePicked())
                        {
                            liftGrabber.grab(object);
                            object->setVelocity({0 , 0});
                        }
                    }
                }
            }

            grabbed = true;
        }
        else grabbed = false;
        
        for (CollidingObject *object : CollidingObject::All)
        {
            object->resetCollision();
            if (object != liftGrabber.currentlyGrabbed())
            {
                object->tick();
            }
        }

        window.clear();

        window.draw(background);
        window.draw(lift);
        window.draw(liftGrabber);

        for (CollidingObject *object : CollidingObject::All)
        {
            window.draw(*object);
        }

        window.display();

        prevTime = (std::chrono::system_clock::now() - begin).count() / 1000000000.0;
        std::cout << prevTime << std::endl;
    }

    return 0;
}
