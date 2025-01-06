#include "PlayerManager.hpp"

PlayerManager::PlayerManager()
{

}

void PlayerManager::draw(sf::RenderTarget& target, sf::RenderStates states) const {
    for (auto player: players)
        target.draw(player->sprite, states);
}

void PlayerManager::update() {
    for (auto player: players) {
        if (player->animation.getElapsedTime() > player->animationTime) {
            sf::IntRect rect = player->sprite.getTextureRect();
            rect.position.x = (rect.position.x + rect.size.x) % (rect.size.x * player->animations);
            player->sprite.setTextureRect(rect);

            player->animation.restart();
        }
    }
}

void PlayerManager::addPlayer(sf::Texture& texture, sf::Vector2f pos, sf::Vector2i size) {
    Player* player = new Player(texture, sf::IntRect({ 0, 0 }, size));
    player->sprite.setPosition(pos);
    players.push_back(player);
}
