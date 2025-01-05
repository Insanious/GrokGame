#include "Game.hpp"

Game::Game(u32 x, u32 y):
    window(sf::VideoMode({ x, y }), "Title"),
    view({ 0.f, 0.f }, { x / 2.f, y / 2.f }),
    mapSize(64, 64)
{
    std::srand(0); // TODO: rng
    window.setView(view);
    level.generate(mapSize);
}

void Game::draw()
{
    window.clear();

    window.draw(level);

    window.display();
}

void Game::run(int framesPerSeconds)
{
    sf::Clock clock;
    sf::Time timeSinceLastUpdate;
    sf::Time timePerFrame = sf::seconds(1.f/(float)framesPerSeconds);
    
    while (window.isOpen()) {
        bool repaint = false;

        sf::Time delta = clock.restart();
        timeSinceLastUpdate += delta;
        if(timeSinceLastUpdate > timePerFrame) {
            timeSinceLastUpdate -= timePerFrame;
            repaint = true;
            update(timePerFrame);
        }

        if(repaint)
            draw();
        else
            sf::sleep(sf::Time(std::chrono::milliseconds(1)));
    }
}

void Game::update(sf::Time dt)
{
    f32 panSpeed = 600.f * dt.asSeconds();
    f32 zoomSpeed = -0.1f;
    sf::Vector2f panDirection = { 0.f, 0.f };

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Scan::A)) panDirection.x = -1;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Scan::D)) panDirection.x = 1;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Scan::W)) panDirection.y = -1;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Scan::S)) panDirection.y = 1;
    if (panDirection != sf::Vector2f({ 0.f, 0.f })) {
        view.move(panDirection.normalized() * panSpeed);
        window.setView(view);
    }

    while (const std::optional event = window.pollEvent()) {
        if (event->is<sf::Event::Closed>()) {
            window.close();
        } else if (const auto* keyPressed = event->getIf<sf::Event::KeyPressed>()) {
            if (keyPressed->scancode == sf::Keyboard::Scancode::Escape) {
                window.close();
            } else if (keyPressed->scancode == sf::Keyboard::Scancode::R) {
                level.generate(mapSize);
            }
        } else if (const auto* scr = event->getIf<sf::Event::MouseWheelScrolled>()) {
            view.zoom(1.f + scr->delta * zoomSpeed);
            window.setView(view);
        }
    }
}