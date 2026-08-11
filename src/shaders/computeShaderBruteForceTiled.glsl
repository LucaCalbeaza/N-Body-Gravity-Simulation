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

shared vec4 tile[256];

void main() {
    uint i = gl_GlobalInvocationID.x;
    uint tileID = gl_LocalInvocationID.x;

    vec3 positionI;
    vec3 accelerationI = vec3(0.0);

    if (i < uint(n)) {
    	positionI = bodies[i].position.xyz;
    } else {
	positionI = vec3(0.0);
    }

    uint numTiles = (uint(n) + 255u) / 256u;

    for (uint t = 0u; t < numTiles; t++) {
	uint idx = t * 256u + tileID;

	if (idx < uint(n)) {
		tile[tileID] = bodies[idx].position;
	} else {
		tile[tileID] = vec4(0.0);
	}

	barrier();
	memoryBarrierShared();

	for (uint j = 0u; j < 256u; j++) {
		vec3 positionJ = tile[j].xyz;
		float massJ = tile[j].w;
        	
		vec3 distance = positionI - positionJ; 
		float distanceSqr = dot(distance, distance) + rSoft * rSoft;
        float inverseDistance = inversesqrt(distanceSqr);
		float inverseDistanceCubed = inverseDistance * inverseDistance *inverseDistance;
        accelerationI += -G * massJ * distance * inverseDistanceCubed;
	}
	
	barrier();
    }

    if (i >= uint(n)) {
    	return;
    }

    // Update Body
    bodies[i].acceleration.xyz = accelerationI;
    bodies[i].velocity.xyz += accelerationI * dt;
    bodies[i].position.xyz += bodies[i].velocity.xyz *dt;
}

