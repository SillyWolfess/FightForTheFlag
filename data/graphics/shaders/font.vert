#version 450 core

layout(location = 0) in vec3 vertexPosition_modelspace;
out vec3 fragmentColor;

uniform mat4 MVP;
uniform vec3 newColor = vec3(1.0f, 1.0f, 1.0f);
void main()
{
	gl_Position =  MVP * vec4(vertexPosition_modelspace, 1);
/*	gl_Position = vec4 (vertexPosition_modelspace.x, vertexPosition_modelspace.y, vertexPosition_modelspace.z, 1); */
	fragmentColor = newColor;
}

