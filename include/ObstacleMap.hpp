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

        void addCircleObstacle (sf::Vector2i const& center, float radius);
        void removeCircleObstacle (sf::Vector2i const& center, float radius);

        /* Initializes the obstacle map to be empty */
        void initialize();

        sf::Texture const& getTexture () const;

    private:
        /* Changes a circle on the obstacle map to be full or empty */
        void setCircle (sf::Vector2i const& center, float radius, bool full);

        /* Inherited from sf::Drawable. */
        void draw(sf::RenderTarget &window, sf::RenderStates states) const;


    private:
        sf::Vector2f _bufferSize;

        unsigned int _currentIndex;
        std::array<sf::RenderTexture, 2> _buffers;

        sf::Shader _initShader;
        sf::Shader _addObstacleShader;
        mutable sf::Shader _displayObstacleShader;
};

#endif // OBSTACLEMAP_HPP_INCLUDED
