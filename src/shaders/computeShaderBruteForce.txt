#version 430 core
layout(local_size_x = 256, local_size_y = 1, local_size_z = 1) in;

struct shaderBody {
    vec4 position;
    vec4 velocity;
    vec4 acceleration;
};

layout(std430, binding = 0) buffer Bodies {
    shaderBody bodies[];
};

uniform int n;
uniform float G;
uniform float rSoft;
uniform float dt;

void main() {
    uint i = gl_GlobalInvocationID.x;
    
    if (i >= uint(n)) {
        return;
    }

    vec3 positionI = bodies[i].position.xyz;
    vec3 accelerationI = vec3(0.0);

    // Calculate Acceleration
    for (int j = 0; j < uint(n); j++) {
        if (i == j) {
            continue;
        }

        vec3 positionJ = bodies[j].position.xyz;
        vec3 distance = positionI - positionJ; 

        float distanceSqr = dot(distance, distance);
        float length = sqrt(distanceSqr);
        accelerationI += ((-G * bodies[j].position.w * distance / length) / ((length + rSoft) * (length + rSoft)));
    }

    // Update Body
    bodies[i].acceleration.xyz = accelerationI;
    bodies[i].velocity.xyz += accelerationI * dt;
    bodies[i].position.xyz += bodies[i].velocity.xyz *dt;
}

