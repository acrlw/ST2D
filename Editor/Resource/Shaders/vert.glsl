#version 330 core

layout(location = 0) in vec3 aPos;
layout(location = 1) in vec4 aColor;

out vec4 vertexColor;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

uniform int isFillMode = 0;
uniform vec4 fillColor = vec4(1.0, 1.0, 1.0, 1.0);

void main()
{
	gl_Position = projection * view * model * vec4(aPos, 1.0);
	if (isFillMode == 1)
		vertexColor = fillColor;
	else
		vertexColor = aColor;
}