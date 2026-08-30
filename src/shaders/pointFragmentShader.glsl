#version 430 core

in vec3 ourColor;
in float pointIntensity;
out vec4 FragColor;

void main() {
    // gl_PointCoord runs from (0,0) to (1,1) across the point sprite
    vec2 centered = gl_PointCoord - vec2(0.5);
    float dist = length(centered) * 2.0; 

    if (dist > 1.0) discard;

    // Bright core coloring
    float core = pow(clamp(1.0 - dist, 0.0, 1.0), 8.0);

    // Outer Halo colouring 
    float halo = pow(clamp(1.0 - dist, 0.0, 1.0), 2.0) * 0.35;

    float glow = core + halo;
    FragColor = vec4(ourColor * pointIntensity * glow, glow);
}