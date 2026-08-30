#version 430 core
layout(local_size_x = 256, local_size_y = 1, local_size_z = 1) in;

uniform int n;

shared vec2 sMin[256];
shared vec2 sMax[256];


void main() {
    // Assing global and local invocation ID
    uint i = gl_GlobalInvocationID.x;
    uint local = gl_LocalInvocationID.x;

    // Add the body xy position to the min and max 
    // arrays. If the global ID is past n, add filer
    // values that won't interfer with the min and 
    // max computations to the arrays. 
    if (i < uint(n)) {
        sMin[local] = bodies[i].position.xy;
        sMax[local] = bodies[i].position.xy;
    } else {
        sMin[local] = vec2(3.402823e38);
        sMax[local] = vec2(-3.402823e38);
    }
    barrier();

    // Find the min x,y values and max x,y values using parallel reduction
    for (uint stride = 128u; stride > 0u; stride >>= 1u) {
        if (local < stride && i + stride < n) {
            sMin[local] = min(sMin[local], sMin[local + stride]);
            sMax[local] = max(sMax[local], sMax[local + stride]);
        }
        barrier();
    }

    // Assing the min and max of the workgroup to the partialBounds buffer
    if (local == 0u) {
        partialBounds[gl_WorkGroupID.x] = vec4(sMin[0], sMax[0]);
    }
}