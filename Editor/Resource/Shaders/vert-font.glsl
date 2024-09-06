#version 330 core
layout(location = 0) in vec4 vertex;
layout(location = 1) in vec4 color;

out vec2 TexCoords;
out vec4 TextColor;

uniform mat4 view;
uniform mat4 projection;

void main()
{
    gl_Position = projection * vec4(vertex.xy, 0.0, 1.0);
    TexCoords = vertex.zw;
    TextColor = color;
}