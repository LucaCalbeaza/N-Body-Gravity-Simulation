#version 430 core
layout(local_size_x = 256, local_size_y = 1, local_size_z = 1) in;

uniform int n;
uniform uint blockSize; 
uniform uint stepSize;
uniform uint totalSize; 


// Return the morton value of body[i]
uint keyAt(uint i) {
    uint body = sortedIdx[i];
    return (body < uint(n)) ? mortonCode[body] : 0xFFFFFFFFu;
}


void main() {
    uint i = gl_GlobalInvocationID.x;
    if (i >= totalSize) return;

    // Ensure that each pair is only handled once
    uint partner = i ^ stepSize;
    if (partner <= i) return; 

    // Determines whether current block is ascending or descending 
    bool ascending = ((i & blockSize) == 0u);

    // Determine wether pair needs to be swapped
    bool swap = ascending ? (keyAt(i) > keyAt(partner)) : (keyAt(i) < keyAt(partner));

    // Swap pairs
    if (swap) {
        uint tmp = sortedIdx[i];
        sortedIdx[i] = sortedIdx[partner];
        sortedIdx[partner] = tmp;
    }
}