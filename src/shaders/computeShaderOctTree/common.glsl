#extension GL_ARB_gpu_shader_int64 : require

struct shaderBody {
    vec4 position;      // position.w = mass
    vec4 velocity;
    vec4 acceleration;
};

struct GpuNode {
    vec4 comAndMass;        // xyz = center of mass, w = node total mass
    vec4 centerAndSize;     // xyz = node center position, w = node size
    ivec4 children[2];      // 8 Octant child node indices; -1 = empty
    int bodyIndex;          // -1 for internal nodes; body index for leaves
    int rangeStart;         // minimum index of node range
    int rangeEnd;           // maximum index of node range
    int checkInCount;       // used during COM reduction
    int childCount;          // number of children
};

// Level constant for the 3D Tree
const int MAX_OCTREE_LEVEL = 21;

layout(std430, binding = 0) buffer Bodies      { shaderBody bodies[]; };                            // shaderBodies Buffer
layout(std430, binding = 1) buffer SortedIndex { uint sortedIdx[]; };                               // Z-Order curve sorted mortonCode Buffer
layout(std430, binding = 2) buffer MortonCodes { uint64_t mortonCode[]; };                          // Unsorted mortonCode Buffer generated from shaderBodies
layout(std430, binding = 3) buffer Nodes       { GpuNode nodes[]; };                                // GpuNodes Buffer
layout(std430, binding = 4) buffer NodeCounter { uint nextFreeNode; };                              // Next available node index from Nodes buffer 

layout(std430, binding = 5) buffer BoundsBuf   { vec4 sceneBoundsMin; vec4 sceneBoundsMax; };         // Final min/max scene bounds from step 2
struct PartialBoundsEntry { vec4 minB; vec4 maxB; };
layout(std430, binding = 6) buffer PartialBounds { PartialBoundsEntry partialBounds[]; };           // List of min/max bounds from step 1

layout(std430, binding = 7) buffer ActiveA { int activeA[]; };                                      // ActiveA and ActiveB alternate between each buffer holding the
layout(std430, binding = 8) buffer ActiveB { int activeB[]; };                                      // indexes of the current level and the next level
layout(std430, binding = 9) buffer ActiveCounts { uint countIn; uint countOut; };                   // CountIn: number of nodes on current level; CountOut: number of nodes on next level

layout(std430, binding = 10) buffer LeafNode { int leafNode[]; };                                   // List of indexes to leaf nodes
layout(std430, binding = 11) buffer NodeParent { int nodeParent[]; };                               // Maps indexes from Nodes to their parent index within Nodes