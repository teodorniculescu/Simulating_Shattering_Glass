# Simulating_Shattering_Glass

<img src="/back_mirror.gif?raw=true">

<img src="/front_mirror.gif?raw=true">

This project is a particle simulation which imitates the physical and optical properties of glass shards. Each particle has an initial position, rotation and velocity. The particle parameters are modified upon impact. The shards spread acording to a [planar Gaussian distribtion](https://homepages.inf.ed.ac.uk/rbf/HIPR2/gsmooth.htm). A random factor is applied to the shards in order to provide a different look after each simulation. The shards are extruded triangles. In order to implement the mirror effect, a reflexion and a refraction effect must be implemented for each face of the mirror. They are combined with a certain percentage (for example 40%) in the Fragment Shader. The image which displays the cubemap and the boat is a rendering from the position of the mirror which is applied as a texture to each individual shard.

0 – resets the simulation
9 – toggle wireframe mode
8 – toggle shard normals
7 – speeds up time
6 – slows down time
R – T – Y – F – G – H – modifies the mirror rendering position

In order to run the code on your machine, you must download the [Graphical Processing Systems Course Framework](https://github.com/UPB-Graphics/SPG-Framework) and append this repository to the Visual Studio project.

