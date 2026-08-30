#version 430 core
layout(local_size_x = 256, local_size_y = 1, local_size_z = 1) in;

uniform int numGroups; // number of partial bounding boxes

shared vec2 sMin[256];
shared vec2 sMax[256];

void main() {
    uint local = gl_LocalInvocationID.x;

    // Initial Values
    vec2 localMin = vec2(3.402823e38);
    vec2 localMax = vec2(-3.402823e38);

    // Reduce local min and max into 256 values
    for (uint idx = local; idx < uint(numGroups); idx += 256u) {
        vec4 partial = partialBounds[idx];
        localMin = min(localMin, partial.xy);
        localMax = max(localMax, partial.zw);
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
        sceneBounds = vec4(sMin[0], sMax[0]);
    }
}
