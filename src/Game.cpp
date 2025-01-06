#include "Game.hpp"

Game::Game(u32 x, u32 y):
    window(sf::VideoMode({ x, y }), "Title"),
    view({ 0.f, 0.f }, { x / 2.f, y / 2.f }),
    level({ 64, 64 }, { 32, 16 }, { 32, 32 }),
    highlighted(nullptr)
{
    std::srand(0); // TODO: rng
    window.setView(view);

    if (!tileset.loadFromFile("resources/tileset_isometric_pack_1bit_white.png")) {
        printf("failed to load texture\n");
        exit(0);
    }
    level.generate(&tileset);

    pointer.setSize(level.tilesetSize);
    pointer.setTexture(&tileset);
    sf::Vector2i size(level.tilesetSize);
    pointer.setTextureRect(sf::IntRect({ size.x * 3, size.y * 20 }, size));
}

void Game::draw()
{
    window.clear();

    window.draw(level);
    window.draw(pointer);

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
    if (panDirection != sf::Vector2f()) {
        view.move(panDirection.normalized() * panSpeed);
        window.setView(view);
    }

    sf::Vector2f mousePos = window.mapPixelToCoords(sf::Mouse::getPosition(window));
    sf::Vector2f halfTileSize = level.tilesetSize.componentWiseDiv({ 2, 2 });
    sf::Vector2i gridPos = level.screenToMap(mousePos - halfTileSize);
    pointer.setPosition(level.mapToScreen(gridPos));

    while (const std::optional event = window.pollEvent()) {
        if (event->is<sf::Event::Closed>()) {
            window.close();
        } else if (const auto* keyPressed = event->getIf<sf::Event::KeyPressed>()) {
            if (keyPressed->scancode == sf::Keyboard::Scancode::Escape) {
                window.close();
            } else if (keyPressed->scancode == sf::Keyboard::Scancode::R) {
                level.generate(&tileset);
                highlighted = nullptr;
            }
        } else if (const auto* scroll = event->getIf<sf::Event::MouseWheelScrolled>()) {
            view.zoom(1.f + scroll->delta * zoomSpeed);
            window.setView(view);
        } else if (const auto* mouseButtonPressed = event->getIf<sf::Event::MouseButtonPressed>()) {
            if (mouseButtonPressed->button == sf::Mouse::Button::Left) {
                if (highlighted)
                        highlighted->setFillColor(sf::Color::White);

                if ((highlighted = level.getRect(gridPos)))
                    highlighted->setFillColor(sf::Color::Red);
            }
        }
    }
}
