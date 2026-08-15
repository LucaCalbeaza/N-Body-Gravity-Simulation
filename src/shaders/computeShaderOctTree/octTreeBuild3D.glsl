#version 430 core
layout(local_size_x = 64, local_size_y = 1, local_size_z = 1) in;

uniform int level;

// Return the octant the mortonCode belongs to 
// from the given level, assuming origin at (0,0,0): 
// 0: (-x, -y, -z), 1 : (x, -y, -z), 2: (-x, y, -z), 
// 3: (x, y, -z),   4: (-x, -y, z),  5: (x, -y, z)
// 6: (-x, y, z)    7: (x, y, z) 
int getOctant(uint64_t mortonCode, int level) {
    int shift = 60 - level * 3;
    return int((mortonCode >> shift) & 7ul);
}

// Peform a binary search to find the the first index 
// within the given level that does not belong to the 
// given quadrant. 
int upperBoundOctant(int start, int end, int q, int level) {
    while (start < end) {
        int mid = (start + end) / 2;
        int digit = getOctant(mortonCode[sortedIdx[mid]], level);
        if (digit <= q) start = mid + 1; else end = mid;
    }
    return start;
}

// Return the center position (in the xyz) and the 
// size (in the w) of the child node in the given quadrant
vec4 childCenterAndSize(GpuNode parent, int q) {
    float childSize = parent.centerAndSize.w * 0.5; 
    float halfOctant = childSize * 0.5; 
    vec3 offset = parent.centerAndSize.xyz;
    
    float signX = float(q & 1) * 2.0 - 1.0;
    float signY = float((q >> 1) & 1) * 2.0 - 1.0;
    float signZ = float((q >> 2) & 1) * 2.0 - 1.0;  

    offset.x += signX * halfOctant;
    offset.y += signY * halfOctant;
    offset.z += signZ * halfOctant;

    return vec4(offset, childSize);
}

void main() {
    uint idx = gl_GlobalInvocationID.x;
    if (idx >= countIn) return;

    // Find the range [start, end) of node indexes 
    // for the current node index. Note: The end of 
    // the range is non inclusive. 
    int nodeIdx = activeA[idx];
    int start = nodes[nodeIdx].rangeStart;
    int end = nodes[nodeIdx].rangeEnd;

    // Leaf case:
    if (end - start <= 1) {
        nodes[nodeIdx].bodyIndex = int(sortedIdx[start]);
        nodes[nodeIdx].children[0] = ivec4(-1);
        nodes[nodeIdx].children[1] = ivec4(-1);
        nodes[nodeIdx].comAndMass   = vec4(0.0);
        nodes[nodeIdx].childCount   = 1; 
        nodes[nodeIdx].checkInCount = 0;
        leafNode[start] = nodeIdx;
        return;
    }

    // Bottom of the tree non-leaf node case:
    if (level >= 15) {
        nodes[nodeIdx].bodyIndex    = -1;
        nodes[nodeIdx].children[0]  = ivec4(-1);
        nodes[nodeIdx].children[1]  = ivec4(-1);
        nodes[nodeIdx].comAndMass   = vec4(0.0);
        nodes[nodeIdx].childCount   = end - start;
        nodes[nodeIdx].checkInCount = 0;
        for (int k = start; k < end; k++) {
            leafNode[k] = nodeIdx;
        }
        return;
    }

    // Internal case: split [start, end) into up to 4 quadrant sub-ranges.
    // Note: Reset any data from previous frames here. 
    nodes[nodeIdx].bodyIndex = -1;
    nodes[nodeIdx].childCount = 0;
    nodes[nodeIdx].checkInCount = 0;
    nodes[nodeIdx].comAndMass = vec4(0.0);
    int prevBoundary = start;
    for (int q = 0; q < 8; q++) {
        int boundary = upperBoundOctant(start, end, q, level);
        int group = q / 4;  // children[0] = octants 0-3, children[1] = octants 4-7
        int comp  = q % 4;
        if (boundary > prevBoundary) {
            int childIdx = int(atomicAdd(nextFreeNode, 1));
            nodes[childIdx].rangeStart = prevBoundary;
            nodes[childIdx].rangeEnd   = boundary;
            nodes[childIdx].centerAndSize = childCenterAndSize(nodes[nodeIdx], q);
            nodes[nodeIdx].children[group][comp] = childIdx;
            nodes[nodeIdx].childCount++;
            nodeParent[childIdx] = nodeIdx;

            uint outSlot = atomicAdd(countOut, 1);
            activeB[outSlot] = childIdx;
        } else {
            nodes[nodeIdx].children[group][comp] = -1;
        }
        prevBoundary = boundary;
    }
}

