#version 330 core

layout(location = 0) in vec3 pPos;
layout(location = 1) in vec4 pColor;
layout(location = 2) in float pRadius;

uniform mat4 view;
uniform mat4 projection;

out vec4 PointColor;
out float PointRadius;

void main() {
    gl_Position = projection * view * vec4(pPos, 1.0);
    gl_PointSize = pRadius * 2.0;
    PointColor = pColor;
    PointRadius = pRadius;
}