#include "ObstacleMap.hpp"

#include "Utilities.hpp"

#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Graphics/RectangleShape.hpp>


ObstacleMap::ObstacleMap(unsigned int width, unsigned int height):
            _bufferSize(width, height),
            _currentIndex(0)
{
    /* Allocation of buffers */
    for (sf::RenderTexture& buffer : _buffers)
        if (!buffer.create(width, height))
            throw std::runtime_error("unable to create obstacle map buffer");

    /* Loading of the shaders */
    std::string fragmentShader, utils;
    loadFile("shaders/utils.glsl", utils);

    loadFile("shaders/computeInitialObstacleMap.frag", fragmentShader);
    searchAndReplace("__UTILS.GLSL__", utils, fragmentShader);
    if (!_initShader.loadFromMemory(fragmentShader, sf::Shader::Fragment))
        throw std::runtime_error("unable to load shader shaders/computeObstacleMap.frag");

    loadFile("shaders/updateObstacleMap.frag", fragmentShader);
    searchAndReplace("__UTILS.GLSL__", utils, fragmentShader);
    if (!_addObstacleShader.loadFromMemory(fragmentShader, sf::Shader::Fragment))
        throw std::runtime_error("unable to load shader shaders/updateObstacleMap.frag");

    loadFile("shaders/displayObstacleMap.frag", fragmentShader);
    searchAndReplace("__UTILS.GLSL__", utils, fragmentShader);
    if (!_displayObstacleShader.loadFromMemory(fragmentShader, sf::Shader::Fragment))
        throw std::runtime_error("unable to load shader shaders/displayObstacleMap.frag");

    initialize();
}


void ObstacleMap::addCircleObstacle (sf::Vector2i const& center, float radius)
{
    setCircle(center, radius, true);
}

void ObstacleMap::removeCircleObstacle (sf::Vector2i const& center, float radius)
{
    setCircle(center, radius, false);
}

void ObstacleMap::initialize()
{
    /* Blending disabled, all four components replaced */
    sf::RenderStates noBlending(sf::BlendMode(sf::BlendMode::One, sf::BlendMode::Zero));
    sf::RectangleShape square(_bufferSize);

    noBlending.shader = &_initShader;
    _buffers[_currentIndex].draw (square, noBlending);
}

sf::Texture const& ObstacleMap::getTexture () const
{
    return _buffers[_currentIndex].getTexture();
}

void ObstacleMap::setCircle(sf::Vector2i const& center, float radius, bool full)
{
    unsigned int nextIndex = (_currentIndex + 1) % 2;

    /* Blending disabled, all four components replaced */
    sf::RenderStates noBlending(sf::BlendMode(sf::BlendMode::One, sf::BlendMode::Zero));
    sf::RectangleShape square(_bufferSize);

    float hardness = (full) ? 1.f : 0.f;
    _addObstacleShader.setParameter("oldObstacleMap", _buffers[_currentIndex].getTexture());
    _addObstacleShader.setParameter("bufferSize", _bufferSize);
    _addObstacleShader.setParameter("hardness", hardness);
    _addObstacleShader.setParameter("newObstacleCenter", sf::Vector2f(center.x, center.y));
    _addObstacleShader.setParameter("radius", radius);

    noBlending.shader = &_addObstacleShader;
    _buffers[nextIndex].draw (square, noBlending);

    _currentIndex = nextIndex;
}

void ObstacleMap::draw(sf::RenderTarget &window, sf::RenderStates states) const
{
    _displayObstacleShader.setParameter("obstacleMap", getTexture());
    _displayObstacleShader.setParameter("bufferSize", _bufferSize);

    window.pushGLStates();

    sf::RectangleShape square(_bufferSize);
    window.draw(square, sf::RenderStates(&_displayObstacleShader));

    window.popGLStates();
}
