#version 430 core
#extension GL_NV_shader_atomic_float : require
layout(local_size_x = 256, local_size_y = 1, local_size_z = 1) in;


uniform int n;

void atomicAddCOM(int parentIndex, vec3 com, float mass) {
    atomicAdd(nodes[parentIndex].comAndMass.x, com.x * mass);
    atomicAdd(nodes[parentIndex].comAndMass.y, com.y * mass);
    atomicAdd(nodes[parentIndex].comAndMass.z, com.z * mass);
    atomicAdd(nodes[parentIndex].comAndMass.w, mass);
}

void main() {
    int leafIdx = int(gl_GlobalInvocationID.x);
    if (leafIdx >= int(n)) return;

    // Get body index, alongside it's com and mass
    int body = int(sortedIdx[leafIdx]);
    vec3 com = bodies[body].position.xyz;
    float mass = bodies[body].position.w;

    // Get leaf node index and parent index
    int node = leafNode[leafIdx];        
    int parent = nodeParent[node];    

    // Set Leaf node comAndMass vector
    nodes[node].comAndMass = vec4(com, mass);
   

    while (parent != -1) {
        // Add this child's contribution onto the parent
        atomicAddCOM(parent, com, mass);  

        // Ensure only 1 child thread progresses up into the tree
        memoryBarrierBuffer();
        int arrived = atomicAdd(nodes[parent].checkInCount, 1);
        if (arrived + 1 < nodes[parent].childCount) {
            return;
        }
        memoryBarrierBuffer();

        // Last child to arrive: finalize this node's COM, then progress up
        com  = nodes[parent].comAndMass.xyz / nodes[parent].comAndMass.w;
        nodes[parent].comAndMass.xyz = com;
        mass = nodes[parent].comAndMass.w;
        node = parent;
        parent = nodeParent[node];
    }
}