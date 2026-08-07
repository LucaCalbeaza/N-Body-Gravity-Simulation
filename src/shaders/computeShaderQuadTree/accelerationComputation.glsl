#version 430 core
layout(local_size_x = 256, local_size_y = 1, local_size_z = 1) in;

uniform int n;
uniform int rootIndex;
uniform float theta;
uniform float G;
uniform float rSoft;
uniform float dt;

void main() {
    uint i = gl_GlobalInvocationID.x;
    if (i >= n) return;

    // Get body position and set acceleration to 0
    vec3 posI = bodies[i].position.xyz;
    vec3 accel = vec3(0.0);

    // Initialize a stack with size 64 and initialize 
    // with root and index 1
    int stack[64];
    int sp = 0;
    stack[sp++] = rootIndex;

    while (sp > 0) {
        // Pop node from stack
        int nodeIdx = stack[--sp];

        // If the node is a leaf perform a simple 
        // 1 to 1 acceleration computation
        if (nodes[nodeIdx].bodyIndex != -1) {
            int bodyJ = nodes[nodeIdx].bodyIndex;
            if (bodyJ == int(i)) continue;
            vec3 dist = posI - bodies[bodyJ].position.xyz;
            float distSqr = dot(dist, dist) + rSoft * rSoft;
            float invDistCube = 1.0 / sqrt(distSqr * distSqr * distSqr);
            accel += -G * bodies[bodyJ].position.w * dist * invDistCube;
            continue;
        }

        // If the node is an internal node compute 
        // the distance between body I and node, and 
        // get the distance of the node. 
        GpuNode node = nodes[nodeIdx];
        vec3 dist = posI - node.comAndMass.xyz;
        float size = node.centerAndSize.w;
        float distLen = length(dist);
        
        if ((size / distLen) < theta) {
            // Far enough: treat node as single point mass
            float distSqr = dot(dist, dist) + rSoft * rSoft;
            float invDistCube = 1.0 / sqrt(distSqr * distSqr * distSqr);
            accel -= G * node.comAndMass.w * dist * invDistCube;
        } else {
            // Too Close: push all non-empty quadrant children onto stack
            for (int q = 0; q < 4; q++) {
                if (node.children[q] != -1) stack[sp++] = node.children[q];
            }
        }
    }

    // Update Body State
    bodies[i].acceleration.xyz = accel;
    bodies[i].velocity.xyz += accel * dt;
    bodies[i].position.xyz += bodies[i].velocity.xyz * dt;
}