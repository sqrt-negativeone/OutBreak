#version 330
layout (location = 0) in vec4 vertex;

out vec2 texCoordinates;
uniform bool shake;
uniform float time;

void main()
{
	texCoordinates = vertex.zw;
	gl_Position = vec4(vertex.xy, 0.0, 1.0);
	if (shake)
	{
		float strenght = 0.01;
		gl_Position.x += cos(time * 10) * strenght;
		gl_Position.y += cos(time * 15) * strenght;
	}
}
