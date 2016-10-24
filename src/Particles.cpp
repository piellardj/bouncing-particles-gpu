#include "Particles.hpp"

#include <cmath>
#include <algorithm>
#include <exception>
#include <cstdlib>
#include <iostream>

#include <SFML/Graphics/VertexArray.hpp>
#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Graphics/Image.hpp>
#include <SFML/Graphics/RectangleShape.hpp>

#include "GLCheck.hpp"
#include "Utilities.hpp"


/* random float between 0 and 1.
   Doesn't matter if not initialized. */
float randomFloat ()
{
    return std::rand() / static_cast<float>(RAND_MAX);
}

Particles::Particles(unsigned int width, unsigned int height, unsigned int nb):
            _worldSize(width, height),
            _bufferSize(nb, nb),
            _force(0.f, -0.1f),
            _currentBufferIndex (0),
            _texCoordBufferID(-1)
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
    if (!_initialPositionsShader.loadFromMemory(fragmentShader, sf::Shader::Fragment))
        throw std::runtime_error("unable to load shader shaders/computeInitialPositions.frag");

    loadFile("shaders/computeInitialVelocities.frag", fragmentShader);
    searchAndReplace("__UTILS.GLSL__", utils, fragmentShader);
    if (!_initialVelocitiesShader.loadFromMemory(fragmentShader, sf::Shader::Fragment))
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


    /* Create VBO for particles' position on buffer */
    std::vector< glm::vec2 > texCoords (getNbParticles());

    for (unsigned int i = 0 ; i < bufferSize.x * bufferSize.y ; ++i) {
        sf::Vector2i pos (i % bufferSize.x, i / bufferSize.x);

        texCoords[i] = glm::vec2(static_cast<float>(pos.x) / _bufferSize.x,
                                 static_cast<float>(pos.y) / _bufferSize.y);
    }

    /* Activate buffer and send data to the graphics card */
    GLCHECK(glGenBuffers(1, &_texCoordBufferID)); //corresponding texture pixel coordinates
    GLCHECK(glBindBuffer(GL_ARRAY_BUFFER, _texCoordBufferID));
    GLCHECK(glBufferData(GL_ARRAY_BUFFER, texCoords.size()*sizeof(glm::vec2), texCoords.data(), GL_STATIC_DRAW));

    initialize();
}

Particles::~Particles()
{
    /* Don't forget to free the buffers */
    if (_texCoordBufferID != (GLuint)(-1))
        GLCHECK(glDeleteBuffers(1, &_texCoordBufferID));
}

unsigned int Particles::getNbParticles() const
{
    return _bufferSize.x * _bufferSize.y;
}

void Particles::initialize()
{
    /* Blending disabled, all four components replaced */
    sf::RenderStates noBlending(sf::BlendMode(sf::BlendMode::One, sf::BlendMode::Zero));
    sf::RectangleShape square(_bufferSize);

    /* Positions */
    _initialPositionsShader.setParameter("worldSize", _worldSize);
    _initialPositionsShader.setParameter("seed1", randomFloat() * 10.f);
    _initialPositionsShader.setParameter("seed2", randomFloat() * 10.f);
    noBlending.shader = &_initialPositionsShader;
    for (sf::RenderTexture &texture : _positions)
        texture.draw (square, noBlending);

    /* Velocities */
    _initialVelocitiesShader.setParameter("maxInitialSpeed", 1.f);
    _initialVelocitiesShader.setParameter("seed1", randomFloat() * 10.f);
    _initialVelocitiesShader.setParameter("seed2", randomFloat() * 10.f);
    noBlending.shader = &_initialVelocitiesShader;
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

    /* Updates velocities */
    _updateVelocityShader.setParameter("positions", _positions[_currentBufferIndex].getTexture());
    _updateVelocityShader.setParameter("oldVelocities", _velocities[_currentBufferIndex].getTexture());
    _updateVelocityShader.setParameter("obstacleMap", obstacleMap.getTexture());
    _updateVelocityShader.setParameter("worldSize", _worldSize);
    _updateVelocityShader.setParameter("bufferSize", _bufferSize);
    _updateVelocityShader.setParameter("acceleration", _force);//_force);
    _updateVelocityShader.setParameter("dt", dt);
    renderStates.shader = &_updateVelocityShader;
    _velocities[nextBufferIndex].draw (square, renderStates);

    /* Updates positions based on the newly computed velocities */
    _updatePositionShader.setParameter("oldPositions", _positions[_currentBufferIndex].getTexture());
    _updatePositionShader.setParameter("velocities", _velocities[nextBufferIndex].getTexture());
    _updatePositionShader.setParameter("obstacleMap", obstacleMap.getTexture());
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
    _displayVerticesShader.setParameter("velocities", _velocities[_currentBufferIndex].getTexture());
    sf::Shader::bind(&_displayVerticesShader);

    /* First we retrieve the shader program's, Attributes' and Uniforms' ID */
    GLuint displayShaderID = -1;
    GLCHECK(displayShaderID = _displayVerticesShader.getNativeHandle());
    if (displayShaderID == (GLuint)(-1)) {
        std::cerr << "Unable to find displayShaderID" << std::endl;
        return;
    }

    GLuint texCoordAttributeID = -1, viewMatrixUniformID = -1;
    GLCHECK(texCoordAttributeID = glGetAttribLocation(displayShaderID, "coordsOnBuffer"));
    if (texCoordAttributeID == (GLuint)(-1)) {
        std::cerr << "Unable to find texCoordAttributeID" << std::endl;
        return;
    }
    GLCHECK(viewMatrixUniformID = glGetUniformLocation(displayShaderID, "viewMatrix"));
    if (viewMatrixUniformID == (GLuint)(-1)) {
        std::cerr << "Unable to find viewMatrixUniformID" << std::endl;
        return;
    }

//    std::cout << "shaderID : " << displayShaderID << std::endl;
//    std::cout << "texCoord ; color ; viewMatrix  ->  " << texCoordAttributeID << " ; " << colorAttributeID << " ; " << viewMatrixUniformID << std::endl;

    /* Sending the view matrix */
    GLCHECK(glUniformMatrix3fv(viewMatrixUniformID, 1, GL_FALSE, &camera.getViewMatrix()[0][0]));

    /* Enabling texture coordinates buffer */
    GLCHECK(glBindBuffer(GL_ARRAY_BUFFER, _texCoordBufferID));
    GLCHECK(glEnableVertexAttribArray(texCoordAttributeID));
    GLCHECK(glVertexAttribPointer(texCoordAttributeID, 2, GL_FLOAT, GL_FALSE, 0, (void*)0));

    GLCHECK(glPointSize(2.f));

    /* Actual drawing */
    GLCHECK(glDrawArrays(GL_POINTS, 0, getNbParticles()));

    /* Don't forget to unbind buffers */
    GLCHECK(glBindBuffer(GL_ARRAY_BUFFER, 0));
    GLCHECK(glDisableVertexAttribArray(texCoordAttributeID));

    /* Displays the positions texture buffer, for debugging purposes */
//    window.setActive(true);
//    window.pushGLStates();
//
//    sf::Sprite sprite;
//    sprite.setTexture(_positions[_currentBufferIndex].getTexture());
//    sprite.setPosition(10,10);
//    window.draw(sprite);
//
//    window.popGLStates();
}
