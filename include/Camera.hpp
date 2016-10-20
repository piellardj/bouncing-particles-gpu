#ifndef CAMERA_HPP_INCLUDED
#define CAMERA_HPP_INCLUDED

#include "glm.hpp"

#include <SFML/Graphics/Rect.hpp>
#include <SFML/System/Vector2.hpp>

/* Class for handling translation and scaling */
class Camera
{
    public:
        Camera (unsigned int screenWidth, unsigned screenHeight,
                glm::vec2 const& origin=glm::vec2(0.f,0.f),
                float zoomFactor=1.f);

 //       void manageEvent (sf::Event const& event, float timeInSeconds);

        sf::Rect<int> getScreenWindow () const;

        sf::Vector2f pixelToCoords(sf::Vector2i pos) const;

        void setScreenSize(unsigned int screenWidth, unsigned int screenHeight);

        void setOrigin (glm::vec2 const& newOrigin);
        void setScaling (float newScaling);

        /* Absolute movement */
        void move (glm::vec2 movement);

        /* Movement relative to zoom factor */
        void moveInPixels (glm::vec2 movement);

        void zoom (float factor);

        glm::mat3 const& getViewMatrix() const;

    private:
        void computeViewMatrix();

    private:
        unsigned int _screenWidth;
        unsigned int _screenHeight;

        glm::vec2 _origin; //absolute coordinates
        float _zoomFactor;

        glm::mat3 _viewMatrix;
};

#endif // CAMERA_HPP_INCLUDED
