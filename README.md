# bouncing-particles
Particle collisions system running on GPU using OpenGL shaders.


# Description
Particle system running on the GPU. The only force applying to particles is gravity.
You can draw with left mouse button obstacles on which the particles rebound. Right mouse button erases obstacles. R key resets the simulation.

Particles' positions and velocities are stored in textures. Each component is stored in 2 channels of a pixel (making it possible to store 65534 values).

There is an additional buffer containing the obstacles' normals. These normals are used for computing the reflection of particles.

On my integrated Intel chip, I can run a simulation of 1 million particles in 60 fps.


# Screenshots
![alt text](screenshots/screen_1.png "Screenshot of a simulation")

![alt text](screenshots/screen_2.png "Screenshot of a simulation")

![alt text](screenshots/screen_3.png "Screenshot of a simulation")


# Compiling
Developped under Linux using SFML 2.3.2 for OpenGL context creation and shaders management. Requires at least OpenGL 3.0.

A Makefile is placed at the root of the repository, expecting SFML, OpenGL and Glew to be installed on the machine.
