#version 410 core
layout(vertices = 4) out; // Output 1 patch with variable size

void main() {
    // this pass through all control points, 
    gl_out[gl_InvocationID].gl_Position = gl_in[gl_InvocationID].gl_Position;
    
    if (gl_InvocationID == 0) {
        // Set tessellation levels based on number of control points
        gl_TessLevelOuter[0] = float(gl_PatchVerticesIn) * 5.0; 
        gl_TessLevelOuter[1] = float(gl_PatchVerticesIn) * 5.0;
    }
}