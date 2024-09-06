#version 330 core

layout(location = 0) in vec2 aPos;

out vec4 vertexColor;

uniform mat4 view;
uniform mat4 projection;

uniform vec4 aColor = vec4(1.0, 1.0, 1.0, 1.0);

void main()
{
	gl_Position = projection * view * vec4(aPos.xy, 0.0, 1.0);
	vertexColor = aColor;
}