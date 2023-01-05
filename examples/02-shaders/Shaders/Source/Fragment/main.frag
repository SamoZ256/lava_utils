#version 460

layout (location = 0) out vec4 FragColor;

layout (location = 0) in vec2 inTexCoord;

void main() {
    FragColor = vec4(inTexCoord, 0.0, 1.0);
}
