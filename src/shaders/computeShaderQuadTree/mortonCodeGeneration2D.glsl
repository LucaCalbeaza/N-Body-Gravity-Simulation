#version 430 core
layout(local_size_x = 256, local_size_y = 1, local_size_z = 1) in;

uniform int n;
uniform uint totalSize;

// Interleave 0s into the unsigned int 
uint spreadOutBits(uint v) {
    v = (v | (v << 8u)) & 0x00FF00FFu;
    v = (v | (v << 4u)) & 0x0F0F0F0Fu;
    v = (v | (v << 2u)) & 0x33333333u;
    v = (v | (v << 1u)) & 0x55555555u;
    return v; 
}

// Convert the normalized 2D vector into a morton value
uint vectorToMorton2D(vec2 normalizedVec) {
    uint x = uint(clamp(normalizedVec.x, 0.0, 1.0) * 65535.0);
    uint y = uint(clamp(normalizedVec.y, 0.0, 1.0) * 65535.0);
    return spreadOutBits(x) | (spreadOutBits(y) << 1u);
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
        vec2 normalized = (bodies[i].position.xy - sceneBounds.xy) / (sceneBounds.zw - sceneBounds.xy);
        mortonCode[i] = vectorToMorton2D(normalized);
    }
}