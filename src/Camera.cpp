#include "Camera.hpp"

Camera::Camera (unsigned int screenWidth, unsigned screenHeight,
                glm::vec2 const& origin, float zoomFactor):
            _screenWidth (screenWidth),
            _screenHeight(screenHeight),
            _origin (origin),
            _zoomFactor (zoomFactor)
{
    computeViewMatrix();
}

sf::Rect<int> Camera::getScreenWindow () const
{
    float pixelSize = 1.f / _zoomFactor;

    sf::Vector2i dim (static_cast<float>(_screenWidth) * pixelSize,
                      static_cast<float>(_screenHeight) * pixelSize);
    return sf::Rect<int>(_origin.x - dim.x/2, _origin.y - dim.y/2,
                         dim.x, dim.y);
}

sf::Vector2f Camera::pixelToCoords(sf::Vector2i pos) const
{
    //SFML inverts y axis
    pos.y = _screenHeight - pos.y;

    pos.x -= _screenWidth / 2;
    pos.y -= _screenHeight / 2;

    float pixelSize = 1.f / _zoomFactor;

    return sf::Vector2f(pos.x*pixelSize + _origin.x,
                        pos.y*pixelSize + _origin.y);
}

void Camera::setScreenSize(unsigned int screenWidth, unsigned int screenHeight)
{
    _screenWidth = screenWidth;
    _screenHeight = screenHeight;
    computeViewMatrix();
}

void Camera::setOrigin (glm::vec2 const& newOrigin)
{
    _origin = newOrigin;
    computeViewMatrix();
}

void Camera::setScaling (float newScaling)
{
    _zoomFactor = newScaling;
    computeViewMatrix();
}

void Camera::move (glm::vec2 movement)
{
    setOrigin (_origin + movement);
}

void Camera::moveInPixels (glm::vec2 movement)
{
    setOrigin (_origin + movement / _zoomFactor);
}

void Camera::zoom (float factor)
{
    setScaling (_zoomFactor * factor);
}

glm::mat3 const& Camera::getViewMatrix() const
{
    return _viewMatrix;
}

void Camera::computeViewMatrix()
{
    float pixelSize = 1.f / _zoomFactor;

    _viewMatrix[0][0] = 2.f / (static_cast<float>(_screenWidth) * pixelSize);
    _viewMatrix[1][0] = 0.f;
    _viewMatrix[2][0] = -2.f * _origin.x / (static_cast<float>(_screenWidth) * pixelSize);

    _viewMatrix[0][1] = 0.f;
    _viewMatrix[1][1] = 2.f / (static_cast<float>(_screenHeight) * pixelSize);
    _viewMatrix[2][1] = -2.f * _origin.y / (static_cast<float>(_screenHeight) * pixelSize);

    _viewMatrix[0][2] = 0.f;
    _viewMatrix[1][2] = 0.f;
    _viewMatrix[2][2] = 0.f;
}
