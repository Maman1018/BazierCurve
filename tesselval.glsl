#version 410 core

layout(isolines, equal_spacing, cw) in;

vec4 bezier(vec4 p0, vec4 p1, vec4 p2, vec4 p3, float t) {
    float u = 1.0 - t;
    return u * u * u * p0 +
           3.0 * u * u * t * p1 +
           3.0 * u * t * t * p2 +
           t * t * t * p3;
}

void main() {
    float t = gl_TessCoord.x;
    gl_Position = bezier(gl_in[0].gl_Position, gl_in[1].gl_Position,
                         gl_in[2].gl_Position, gl_in[3].gl_Position, t);
}

