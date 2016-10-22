#include <cstdlib>
#include <cmath>
#include <iostream>

#include <SFML/System/Clock.hpp>
#include <SFML/System/Time.hpp>
#include <SFML/Graphics.hpp>

#include <GL/glew.h>

#include "ObstacleMap.hpp"
#include "Particles.hpp"
#include "Camera.hpp"


int main()
{
    /* Creation of the fixed dimensions window and the OpenGL 3+ context */
    sf::ContextSettings openGLContext(0, 0, 0, //no depth, no stencil, no antialiasing
                                      3, 0, //openGL 3.0 requested
                                      sf::ContextSettings::Default);
    sf::RenderWindow window(sf::VideoMode(400, 600), "Bouncing Particles",
                            sf::Style::Titlebar | sf::Style::Close,
                            openGLContext);
    window.setVerticalSyncEnabled(true);

    /* Checking if the requested OpenGL version is available */
    std::cout << "openGL version: " << window.getSettings().majorVersion << "." << window.getSettings().minorVersion << std::endl << std::endl;
    if (window.getSettings().majorVersion < 3) {
        std::cerr << "This program requires at least OpenGL 3.0" << std::endl << std::endl;
        return EXIT_FAILURE;
    }
    if (!sf::Shader::isAvailable()) {
        std::cerr << "This program requires support for OpenGL shaders." << std::endl << std::endl;
        return EXIT_FAILURE;
    }
    glewInit();

    /* Creation of a fixed camera */
    Camera camera(window.getSize().x, window.getSize().y);

    /* Creation of the obstacle map, initialized empty */
    ObstacleMap obstacleMap(window.getSize().x, window.getSize().y);

    Particles particles(window.getSize().x, window.getSize().y - 1,
                        128);

    /* for computing average frame per seconds */
    float total = 0.f;
    int loops = 0;
    sf::Clock clock;

    /* Main loop */
    while (window.isOpen()) {
        sf::Vector2i mousePos = sf::Mouse::getPosition(window);
        mousePos.y = window.getSize().y - mousePos.y;

        sf::Event event;
        while (window.pollEvent(event)) {
            switch (event.type) {
                case sf::Event::Closed:
                    window.close();
                break;
                case sf::Event::KeyReleased:
                    if (event.key.code == sf::Keyboard::R) {
                        particles.initialize();
                        obstacleMap.initialize();
                    }
                break;
                case sf::Event::MouseButtonPressed:
                    if (event.mouseButton.button == sf::Mouse::Left)
                        obstacleMap.addObstacle();
                break;
                case sf::Event::MouseMoved:
                    obstacleMap.mouseMoved(mousePos);

                    if (sf::Mouse::isButtonPressed(sf::Mouse::Left))
                        obstacleMap.addObstacle();
                break;
                case sf::Event::MouseWheelScrolled:
                    obstacleMap.changeRadius(event.mouseWheelScroll.delta);
                break;
                default:
                break;
            }
        }

        /* If a particle goes too fast, it may pass through a thin wall in one step.
           To avoid this, we set a maximum step time, and run several small steps if
           necessary */
        const sf::Time MAX_STEP = sf::seconds(0.005f);
        sf::Time elapsedTime = clock.getElapsedTime();

        while (elapsedTime > MAX_STEP) {
            particles.computeNewPositions(MAX_STEP, obstacleMap);
            elapsedTime -= MAX_STEP;
        }
        particles.computeNewPositions(elapsedTime, obstacleMap);


        ++loops;
        total += clock.getElapsedTime().asSeconds();
        clock.restart();


        window.clear();
        particles.draw(window, camera);
        window.draw(obstacleMap);
        window.display();
    }

    std::cout << "average fps: " << static_cast<float>(loops) / total << std::endl;

    return EXIT_SUCCESS;
}
