#ifndef OBSTACLEMAP_HPP_INCLUDED
#define OBSTACLEMAP_HPP_INCLUDED

#include <array>

#include <SFML/Graphics/Texture.hpp>
#include <SFML/Graphics/RenderTexture.hpp>
#include <SFML/Graphics/Shader.hpp>
#include <SFML/Graphics/Drawable.hpp>
#include <SFML/System/Vector2.hpp>

/* Class for managing an obstacle map.
 * Obstacles are stored in a texture buffer.
 * In in pixel is written the local normal to the obstacle, (0,0) if no obstacle.
 */
class ObstacleMap: public sf::Drawable
{
    public:
        ObstacleMap(unsigned int width, unsigned int height);

        /* Moves or changes the radius of the mouse linked obstacle */
        void mouseMoved (sf::Vector2i const& position);
        void changeRadius (float diff);

        /* Adds from the fixed obstacles map the mouse obstacle */
        void addObstacle ();

        /* Initializes the obstacle map to be empty */
        void initialize();

        sf::Texture const& getTexture () const;

    private:
        /* Recomputes the dynamic buffer */
        void updateDynamicBuffer ();

        /* Inherited from sf::Drawable. */
        void draw(sf::RenderTarget &window, sf::RenderStates states) const;


    private:
        const float MIN_RADIUS;
        const float MAX_RADIUS;
        const float DEFAULT_RADIUS;

        sf::Vector2f _mousePosition;
        float _currentRadius;

        sf::Vector2f _bufferSize;

        sf::RenderTexture _fixedBuffer; //fixed obstacles
        sf::RenderTexture _dynamicBuffer; //fixed obstacles + mouse obstacle

        sf::Shader _initShader;
        sf::Shader _addObstacleShader;
        mutable sf::Shader _displayObstacleShader;
};

#endif // OBSTACLEMAP_HPP_INCLUDED
