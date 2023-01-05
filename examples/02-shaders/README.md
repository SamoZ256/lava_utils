Shaders are programs that run on a GPU and decide how should be pixels colored. In Vulkan, shaders can be written in almost any modern shading language(glsl, hlsl, msl) and then need to be compiled into SPIR-V(Standard Portable Intermediate Representation). We are going to use glsl for this tutorial, as that's the main shading language for Vulkan. Shaders contain different stages, and today we are going to learn about the two most basic ones: Vertex shader stage and Fragment shader stage. Vertex shader is run for every vertex (or point in 3D space) and decides where on the screen will the vertex end up. Fragment shader is run for every single fragment (or pixel in our case) and decides what will be the color of that fragment. Let's take a look at a very simple Vertex shader.

#version 460

vec2 positions[3] = {
    vec2(0.0,  1.0),
    vec2(0.0, -3.0),
    vec2(3.0,  1.0)
};

void main() {
    gl_Position = vec4(positions[gl_VertexIndex], 0.0, 1.0);
}
First, we specify the version we want to use. This is not that important, but older version lacks some functionality. Version 460 means version 4 major and version 6 minor. GLSL is very similar to C language. We also have the main function, but it has type void. In the main function, we assign value to the GLSL's built in variable gl_Position, which has type vec4. Vectors in GLSL are built-in data types and you can do various operations on them, for instance addition, multiplication. We use another GLSL's built-in variable called gl_VertexIndex, which is the index of the current vertex. We use it to index into our predefined array. It is not good to have all the vertices hardcoded into the vertex shader, so we will change that in the future tutorials. The coordinates range from -1 to 1 on both x and y axis, but we make them exceed to draw a big triangle covering the whole screen. 

Source: https://www.saschawillems.de/images/2016-08-13-vulkan-tip-for-rendering-a-fullscreen-quad-without-buffers/fullscreentriangle_clipped.png