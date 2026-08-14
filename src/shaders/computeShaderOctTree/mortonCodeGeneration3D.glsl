#version 430 core
layout(local_size_x = 256, local_size_y = 1, local_size_z = 1) in;

uniform int n;
uniform uint totalSize;

// Interleave two 0s between each bit of the given unsigned int 
uint64_t spreadOutBits(uint v) {
    uint64_t x = uint64_t(v) & 0x1FFFFFul;              
    x = (x | (x << 32)) & 0x1f00000000ffffUL;
    x = (x | (x << 16)) & 0x1f0000ff0000ffUL;
    x = (x | (x << 8))  & 0x100f00f00f00f00fUL;
    x = (x | (x << 4))  & 0x10c30c30c30c30c3UL;
    x = (x | (x << 2))  & 0x1249249249249249UL;
    return x;
}

// Convert the normalized 3D vector into a morton value
uint64_t vectorToMorton3D(vec3 normalizedVec) {
    uint x = uint(clamp(normalizedVec.x, 0.0, 1.0) * 2097151.0);
    uint y = uint(clamp(normalizedVec.y, 0.0, 1.0) * 2097151.0);
    uint z = uint(clamp(normalizedVec.z, 0.0, 1.0) * 2097151.0);
    return spreadOutBits(x) | (spreadOutBits(y) << 1u) | (spreadOutBits(z) << 2u);
}

// Convert each body's position to a morton code and 
// fill the mortonCode vector with these values. Any 
// i not within the range of n is just placed as a 
// a padded filler value and does not have a morton
// code conversion. 
void main() {
    uint i = gl_GlobalInvocationID.x;
    if (i >= totalSize) {
        return;
    }

    sortedIdx[i] = i;

    if (i < uint(n)) {
        vec3 normalized = (bodies[i].position.xyz - sceneBoundsMin.xyz) / 
                          (sceneBoundsMax.xyz - sceneBoundsMin.xyz);
        mortonCode[i] = vectorToMorton3D(normalized);
    }
}