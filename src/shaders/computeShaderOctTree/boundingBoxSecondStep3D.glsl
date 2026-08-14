#version 430 core
layout(local_size_x = 256, local_size_y = 1, local_size_z = 1) in;

uniform int numGroups; // number of partial bounding boxes

shared vec3 sMin[256];
shared vec3 sMax[256];

void main() {
    uint local = gl_LocalInvocationID.x;

    // Initial Values
    vec3 localMin = vec3(3.402823e38);
    vec3 localMax = vec3(-3.402823e38);

    // Reduce local min and max into 256 values
    for (uint idx = local; idx < uint(numGroups); idx += 256u) {
        PartialBoundsEntry partial = partialBounds[idx];
        localMin = min(localMin, partial.minB.xyz);
        localMax = max(localMax, partial.maxB.xyz);
    }
    sMin[local] = localMin;
    sMax[local] = localMax;
    barrier();

    // Find the min x,y values and max x,y values using parallel reduction
    for (uint stride = 128u; stride > 0u; stride >>= 1u) {
        if (local < stride) {
            sMin[local] = min(sMin[local], sMin[local + stride]);
            sMax[local] = max(sMax[local], sMax[local + stride]);
        }
        barrier();
    }

    if (local == 0u) {
        sceneBoundsMin = vec4(sMin[0], 0.0);
        sceneBoundsMax = vec4(sMax[0], 0.0);
    }
}
