#include "Particles.hpp"

#include <cmath>
#include <algorithm>
#include <exception>

#include <iostream>

#include <SFML/Graphics/VertexArray.hpp>
#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Graphics/Image.hpp>
#include <SFML/Graphics/RectangleShape.hpp>

#include "GLCheck.hpp"
#include "Utilities.hpp"


Particles::Particles(unsigned int width, unsigned int height):
            _worldSize(width, height),
            _bufferSize(128.f, 128.f),
            _force(0.f, -0.1f),
            _currentBufferIndex (0),
            _colorBufferID(0),
            _texCoordBufferID(0)
{
    sf::Vector2u bufferSize(_bufferSize.x, _bufferSize.y );

    /* Allocation of buffers */
    for (sf::RenderTexture &positionBuffer : _positions) {
        if (!positionBuffer.create(bufferSize.x, bufferSize.y))
            throw std::runtime_error("unable to create positions buffer");
    }
    for (sf::RenderTexture &velocityBuffer : _velocities) {
        if (!velocityBuffer.create(bufferSize.x, bufferSize.y))
            throw std::runtime_error("unable to create velocities buffer");
    }


    /* Loading of the shaders */
    std::string fragmentShader, vertexShader, utils;
    loadFile("shaders/utils.glsl", utils);

    loadFile("shaders/computeInitialPositions.frag", fragmentShader);
    searchAndReplace("__UTILS.GLSL__", utils, fragmentShader);
    if (!_computeInitialPositionsShader.loadFromMemory(fragmentShader, sf::Shader::Fragment))
        throw std::runtime_error("unable to load shader shaders/computeInitialPositions.frag");

    loadFile("shaders/computeInitialVelocities.frag", fragmentShader);
    searchAndReplace("__UTILS.GLSL__", utils, fragmentShader);
    if (!_computeInitialVelocitiesShader.loadFromMemory(fragmentShader, sf::Shader::Fragment))
        throw std::runtime_error("unable to load shader shaders/computeInitialVelocities.frag");

    loadFile("shaders/updateVelocity.frag", fragmentShader);
    searchAndReplace("__UTILS.GLSL__", utils, fragmentShader);
    if (!_updateVelocityShader.loadFromMemory(fragmentShader, sf::Shader::Fragment))
        throw std::runtime_error("unable to load shader shaders/updateVelocity.frag");

    loadFile("shaders/updatePosition.frag", fragmentShader);
    searchAndReplace("__UTILS.GLSL__", utils, fragmentShader);
    if (!_updatePositionShader.loadFromMemory(fragmentShader, sf::Shader::Fragment))
        throw std::runtime_error("unable to load shader shaders/updatePosition.frag");

    loadFile("shaders/displayParticles.vert", vertexShader);
    searchAndReplace("__UTILS.GLSL__", utils, vertexShader);
    loadFile("shaders/displayParticles.frag", fragmentShader);
    searchAndReplace("__UTILS.GLSL__", utils, fragmentShader);

    if (!_displayVerticesShader.loadFromMemory(vertexShader, fragmentShader))
        throw std::runtime_error("unable to load shader shaders/displayParticles.frag or shaders/displayParticles.vert");


    /* Create VBO */
    std::vector< glm::vec4 > colors (getNbParticles());
    std::vector< glm::vec2 > texCoords (getNbParticles());

    for (unsigned int i = 0 ; i < bufferSize.x * bufferSize.y ; ++i) {
        sf::Vector2i pos (i % bufferSize.x, i / bufferSize.x);

        colors[i] = glm::vec4(1.f, 0.f, 0.f, 1.f);
        texCoords[i] = glm::vec2(static_cast<float>(pos.x) / _bufferSize.x,
                              static_cast<float>(pos.y) / _bufferSize.y);
    }

    /* Activate buffer and send data to the graphics card */
    GLCHECK(glGenBuffers(1, &_colorBufferID)); //colors
    GLCHECK(glBindBuffer(GL_ARRAY_BUFFER,_colorBufferID));
    GLCHECK(glBufferData(GL_ARRAY_BUFFER, colors.size()*sizeof(glm::vec4), colors.data(), GL_STATIC_DRAW));

    GLCHECK(glGenBuffers(1, &_texCoordBufferID)); //corresponding texture pixel coordinates
    GLCHECK(glBindBuffer(GL_ARRAY_BUFFER, _texCoordBufferID));
    GLCHECK(glBufferData(GL_ARRAY_BUFFER, texCoords.size()*sizeof(glm::vec2), texCoords.data(), GL_STATIC_DRAW));

    initialize();
}

Particles::~Particles()
{
    /* Don't forget to free the buffers */
    if (_colorBufferID != 0)
        GLCHECK(glDeleteBuffers(1, &_colorBufferID));
    if (_texCoordBufferID != 0)
        GLCHECK(glDeleteBuffers(1, &_texCoordBufferID));
}

unsigned int Particles::getNbParticles() const
{
    return static_cast<unsigned int>(_bufferSize.x * _bufferSize.y);
}

void Particles::initialize()
{
    /* Blending disabled, all four components replaced */
    sf::RenderStates noBlending(sf::BlendMode(sf::BlendMode::One, sf::BlendMode::Zero));
    sf::RectangleShape square(_bufferSize);

    /* Positions */
    _computeInitialPositionsShader.setParameter("worldSize", _worldSize);
    _computeInitialPositionsShader.setParameter("bufferSize", _bufferSize);
    noBlending.shader = &_computeInitialPositionsShader;
    for (sf::RenderTexture &texture : _positions)
        texture.draw (square, noBlending);

    /* Velocities */
    noBlending.shader = &_computeInitialVelocitiesShader;
    for (sf::RenderTexture &texture : _velocities)
        texture.draw (square, noBlending);
}

void Particles::setForce (sf::Vector2f const& force)
{
    _force = force;
}

void Particles::computeNewPositions(sf::Time const& dtime, ObstacleMap const& obstacleMap)
{
    /* Blending disabled, all four components replaced */
    sf::RenderStates renderStates(sf::BlendMode(sf::BlendMode::One, sf::BlendMode::Zero));

    int nextBufferIndex = (_currentBufferIndex + 1) % 2;

    sf::RectangleShape square(_bufferSize);
    float dt = 30.f * dtime.asSeconds(); //30 is arbitrary

    _updateVelocityShader.setParameter("positions", _positions[_currentBufferIndex].getTexture());
    _updateVelocityShader.setParameter("oldVelocities", _velocities[_currentBufferIndex].getTexture());
    _updateVelocityShader.setParameter("obstacleMap", obstacleMap.getTexture());
    _updateVelocityShader.setParameter("worldSize", _worldSize);
    _updateVelocityShader.setParameter("bufferSize", _bufferSize);
    _updateVelocityShader.setParameter("acceleration", _force);//_force);
    _updateVelocityShader.setParameter("dt", dt);
    renderStates.shader = &_updateVelocityShader;
    _velocities[nextBufferIndex].draw (square, renderStates);

    _updatePositionShader.setParameter("oldPositions", _positions[_currentBufferIndex].getTexture());
    _updatePositionShader.setParameter("velocities", _velocities[nextBufferIndex].getTexture());
    _updatePositionShader.setParameter("worldSize", _worldSize);
    _updatePositionShader.setParameter("bufferSize", _bufferSize);
    _updatePositionShader.setParameter("dt", dt);
    renderStates.shader = &_updatePositionShader;
    _positions[nextBufferIndex].draw (square, renderStates);

    _currentBufferIndex = nextBufferIndex;
}

void Particles::draw(sf::RenderWindow &window, Camera const& camera) const
{
    window.setActive(true);

    GLCHECK(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));

    _displayVerticesShader.setParameter("positions", _positions[_currentBufferIndex].getTexture());
    sf::Shader::bind(&_displayVerticesShader);

    /* First we retrieve the shader program's, Attributes' and Uniforms' ID */
    GLuint displayShaderID = 0;
    GLCHECK(displayShaderID = _displayVerticesShader.getNativeHandle());

    GLuint texCoordAttributeID = 0, colorAttributeID = 0, viewMatrixUniformID = 0;
    GLCHECK(texCoordAttributeID = glGetAttribLocation(displayShaderID, "coordsOnBuffer"));
    GLCHECK(colorAttributeID = glGetAttribLocation(displayShaderID, "color"));
    GLCHECK(viewMatrixUniformID = glGetUniformLocation(displayShaderID, "viewMatrix"));

//    std::cout << "shaderID : " << displayShaderID << std::endl;
//    std::cout << "texCoord ; color ; viewMatrix  ->  " << texCoordAttributeID << " ; " << colorAttributeID << " ; " << viewMatrixUniformID << std::endl;

    /* Sending the view matrix */
    GLCHECK(glUniformMatrix3fv(viewMatrixUniformID, 1, GL_FALSE, &camera.getViewMatrix()[0][0]));

    /* Enabling texture coordinates buffer */
    GLCHECK(glBindBuffer(GL_ARRAY_BUFFER, _texCoordBufferID));
    GLCHECK(glEnableVertexAttribArray(texCoordAttributeID));
    GLCHECK(glVertexAttribPointer(texCoordAttributeID, 2, GL_FLOAT, GL_FALSE, 0, (void*)0));

    /* Enabling color buffer */
    GLCHECK(glBindBuffer(GL_ARRAY_BUFFER, _colorBufferID));
    GLCHECK(glEnableVertexAttribArray(colorAttributeID));
    GLCHECK(glVertexAttribPointer(colorAttributeID, 4, GL_FLOAT, GL_FALSE, 0, (void*)0));

    GLCHECK(glPointSize(2.f));

    /* Actual drawing */
    GLCHECK(glDrawArrays(GL_POINTS, 0, getNbParticles()));

    /* Don't forget to unbind buffers */
    GLCHECK(glBindBuffer(GL_ARRAY_BUFFER, 0));


//    window.setActive(true);
//    window.pushGLStates();
//
//    sf::Sprite sprite;
//    sprite.setTexture(_positions[_currentBufferIndex].getTexture());
//    sprite.setPosition(10,10);
//    window.draw(sprite);
//
//    window.display();
//    window.popGLStates();
}
