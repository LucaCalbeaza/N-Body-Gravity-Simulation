#version 430 core

struct Body {
    vec4 position;     // xyz = position, w = mass
    vec4 velocity;     
    vec4 acceleration;
};

layout(std430, binding = 0) readonly buffer BodiesBuffer {
    Body bodies[];
};

uniform mat4 view;
uniform mat4 projection;
uniform float maxSpeedThreshold;
uniform vec3 minColor;
uniform vec3 maxColor;
uniform float bodyRadius;   
uniform float fovY;       
uniform float viewportHeight;

out vec3 ourColor;
out float pointIntensity;

void main() {
    Body body = bodies[gl_VertexID];

    vec4 viewPos = view * vec4(body.position.xyz, 1.0);
    gl_Position = projection * viewPos;

    // Mix color across gradient dependant on speed
    float speed = length(body.velocity.xyz);
    float normalizedSpeed = clamp(speed / maxSpeedThreshold, 0.0, 1.0);
    ourColor = mix(minColor, maxColor, normalizedSpeed);

    // Perspective-correct point size: bigger when close, smaller when far,
    // clamped so points never vanish or blow up.
    float dist = max(-viewPos.z, 0.001);
    float pixelDiameter = (bodyRadius * viewportHeight) / (dist * tan(fovY * 0.5));
    gl_PointSize = clamp(pixelDiameter, 1.0, 64.0) * 2.0f;
    pointIntensity = clamp(0.5 + 0.5 * normalizedSpeed, 0.0, 1.0);
}