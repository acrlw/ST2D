#version 330 core
layout(location = 0) in vec4 vertex;
layout(location = 1) in vec4 color;

out vec2 TexCoords;
out vec4 TextColor;

void main()
{
    gl_Position = vec4(vertex.xy, 0.0, 1.0);
    TexCoords = vertex.zw;
    TextColor = color;
}