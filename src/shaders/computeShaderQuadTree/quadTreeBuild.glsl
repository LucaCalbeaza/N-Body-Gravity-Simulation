#version 430 core
layout(local_size_x = 64, local_size_y = 1, local_size_z = 1) in;

uniform int level;

// Return the quadrant the mortonCode belongs to 
// from the given level: 
// 0: SW, 1: NW, 2: SE, 3: NE
int getQuadrant(uint mortonCode, int level) {
    int shift = 30 - level * 2;
    return int((mortonCode >> shift) & 3u);
}

// Peform a binary search to find the the first index 
// within the given level that does not belong to the 
// given quadrant. 
int upperBoundQuadrant(int start, int end, int q, int level) {
    while (start < end) {
        int mid = (start + end) / 2;
        int digit = getQuadrant(mortonCode[sortedIdx[mid]], level);
        if (digit <= q) start = mid + 1; else end = mid;
    }
    return start;
}

// Return the center position (in the xyz) and the 
// size (in the w) of the child node in the given quadrant
vec4 childCenterAndSize(GpuNode parent, int q) {
    float childSize = parent.centerAndSize.w * 0.5; 
    float halfQuadrant = childSize * 0.5; 
    vec3 offset = parent.centerAndSize.xyz;
    
    float signX = float(q & 1) * 2.0 - 1.0; 
    float signY = float(q >> 1) * 2.0 - 1.0;  

    offset.x += signX * halfQuadrant;
    offset.y += signY * halfQuadrant;

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
        nodes[nodeIdx].children  = ivec4(-1);
        leafNode[start] = nodeIdx;
        return;
    }

    // Internal case: split [start, end) into up to 4 quadrant sub-ranges.
    // Note: I reset any data from previous frames here. 
    nodes[nodeIdx].bodyIndex = -1;
    nodes[nodeIdx].childCount = 0;
    nodes[nodeIdx].checkInCount = 0;
    nodes[nodeIdx].comAndMass = vec4(0.0);
    int prevBoundary = start;
    for (int q = 0; q < 4; q++) {
        int boundary = upperBoundQuadrant(start, end, q, level);
        if (boundary > prevBoundary) {
            int childIdx = int(atomicAdd(nextFreeNode, 1));
            nodes[childIdx].rangeStart = prevBoundary;
            nodes[childIdx].rangeEnd   = boundary;
            nodes[childIdx].centerAndSize = childCenterAndSize(nodes[nodeIdx], q);
            nodes[nodeIdx].children[q] = childIdx;
            nodes[nodeIdx].childCount++;
            nodeParent[childIdx] = nodeIdx;

            uint outSlot = atomicAdd(countOut, 1);
            activeB[outSlot] = childIdx;
        } else {
            nodes[nodeIdx].children[q] = -1;
        }
        prevBoundary = boundary;
    }
}

