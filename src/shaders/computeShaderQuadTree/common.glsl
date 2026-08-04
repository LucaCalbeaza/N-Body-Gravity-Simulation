struct shaderBody {
    vec4 position;      // position.w = mass
    vec4 velocity;
    vec4 acceleration;
};

struct GpuNode {
    vec4 comAndMass;        // xyz = center of mass, w = node total mass
    vec4 centerAndSize;     // xyz = node center position, w = node size
    ivec4 children;         // 4 quadrant child node indices; -1 = empty
    int bodyIndex;          // -1 for internal nodes; body index for leaves
    int rangeStart;         // minimum index of node range
    int rangeEnd;           // maximum index of node range
    int checkInCount;       // used during COM reduction
};

layout(std430, binding = 0) buffer Bodies      { shaderBody bodies[]; };
layout(std430, binding = 1) buffer SortedIndex { uint sortedIdx[]; };     
layout(std430, binding = 2) buffer MortonCodes { uint mortonCode[]; };
layout(std430, binding = 3) buffer Nodes       { GpuNode nodes[]; };
layout(std430, binding = 4) buffer NodeCounter { uint nextFreeNode; };    
layout(std430, binding = 5) buffer BoundsBuf   { vec4 sceneBounds; };

// Ping-pong "active node" lists — one holds the nodes to process this level,
// the other collects the children created for the next level.
layout(std430, binding = 6) buffer ActiveA { int activeA[]; };
layout(std430, binding = 7) buffer ActiveB { int activeB[]; };
layout(std430, binding = 8) buffer ActiveCounts { uint countIn; uint countOut; };