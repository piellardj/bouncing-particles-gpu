#include "ObstacleMap.hpp"

#include "Utilities.hpp"

#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Graphics/RectangleShape.hpp>


ObstacleMap::ObstacleMap(unsigned int width, unsigned int height):
            MIN_RADIUS(3.f),
            MAX_RADIUS(100.f),
            DEFAULT_RADIUS(15.f),
            _mousePosition(10000, 10000), //offscreen
            _currentRadius(DEFAULT_RADIUS),
            _bufferSize(width, height)
{
    /* Allocation of buffers */
    if (!_fixedBuffer.create(width, height))
        throw std::runtime_error("unable to create obstacle map buffer");
    if (!_dynamicBuffer.create(width, height))
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

void ObstacleMap::mouseMoved (sf::Vector2i const& position)
{
    _mousePosition.x = position.x;
    _mousePosition.y = position.y;

    updateDynamicBuffer();
}

void ObstacleMap::changeRadius (float diff)
{
    _currentRadius += diff;
    _currentRadius = std::max(MIN_RADIUS, _currentRadius);
    _currentRadius = std::min(MAX_RADIUS, _currentRadius);

    updateDynamicBuffer();
}

void ObstacleMap::addObstacle ()
{
    sf::RenderStates noBlending(sf::BlendMode(sf::BlendMode::One, sf::BlendMode::Zero));

    sf::Sprite dynamicSprite(_dynamicBuffer.getTexture());
    dynamicSprite.setScale(1.f, -1.f);
    dynamicSprite.setPosition(0.f, _dynamicBuffer.getSize().y);
    _fixedBuffer.draw (dynamicSprite, noBlending);
}

void ObstacleMap::initialize()
{
    _currentRadius = DEFAULT_RADIUS;

    /* Blending disabled, all four components replaced */
    sf::RenderStates noBlending(sf::BlendMode(sf::BlendMode::One, sf::BlendMode::Zero));
    sf::RectangleShape square(_bufferSize);

    /* Clear fixed obstacles buffer */
    noBlending.shader = &_initShader;
    _fixedBuffer.draw (square, noBlending);

    updateDynamicBuffer();
}

sf::Texture const& ObstacleMap::getTexture () const
{
    return _dynamicBuffer.getTexture();
}

void ObstacleMap::updateDynamicBuffer()
{
    sf::RenderStates noBlending(sf::BlendMode(sf::BlendMode::One, sf::BlendMode::Zero));

    /* Add mouse obstacle to fixed obstacles buffer */
    sf::RectangleShape square(_bufferSize);

    _addObstacleShader.setParameter("oldObstacleMap", _fixedBuffer.getTexture());
    _addObstacleShader.setParameter("bufferSize", _bufferSize);
    _addObstacleShader.setParameter("newObstacleCenter", _mousePosition);
    _addObstacleShader.setParameter("radius", _currentRadius);
    noBlending.shader = &_addObstacleShader;
    _dynamicBuffer.draw (square, noBlending);
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
