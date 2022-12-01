#version 330 core
layout (points) in;
layout (triangle_strip, max_vertices = 3) out;

in VS_OUT {
    vec3 color;
    vec3 normal;
    vec3 fragPos;
} gs_in[];

out vec3 fColor;
out vec3 normal;
out vec3 FragPos;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

vec3 plano(vec3 normal, vec3 position, float dx, float dy) {
    float d = dot(normal, position.xyz);
    float a = normal.x;
    float b = normal.y;
    float c = normal.z;
    float x = position.x + dx;
    float y = position.y + dy;
    float z = (-d + a*x + b*y) / c;
    return vec3(x,y,z);
}

void qsplat(vec4 position) {
    fColor = gs_in[0].color; // gs_in[0] since there's only one input vertex
    normal = gs_in[0].normal;
    //gl_Position = position + vec4( 0.2, 0.2, 0.0, 0.0);
    vec3 fragPos = gs_in[0].fragPos;
    float factor = 0.1;
    FragPos = plano(normal, fragPos, factor, factor);
    gl_Position = projection * view * vec4(FragPos, 1.0);
    EmitVertex();

    FragPos = plano(normal, fragPos, factor, -factor);
    gl_Position = projection * view * vec4(FragPos, 1.0);
    EmitVertex();

    FragPos = plano(normal, fragPos, -factor, factor);
    gl_Position = projection * view * vec4(FragPos, 1.0);
    EmitVertex();
    EndPrimitive();
}

void main() {    
    qsplat(gl_in[0].gl_Position);
}