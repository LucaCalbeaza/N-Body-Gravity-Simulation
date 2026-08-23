#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aColor;
layout (location = 2) in vec3 instancePos;
layout (location = 3) in vec3 instanceVel;

out vec3 ourColor;

uniform mat4 view;
uniform mat4 projection;
uniform float maxSpeedThreshold;
uniform vec3 minColor;
uniform vec3 maxColor;


void main() {
    gl_Position = projection * view * vec4(aPos + instancePos, 1.0);
    float speed = length(instanceVel);
    float normalizedSpeed = clamp(speed / maxSpeedThreshold, 0.0, 1.0);
    ourColor = mix(minColor, maxColor, normalizedSpeed);
}
