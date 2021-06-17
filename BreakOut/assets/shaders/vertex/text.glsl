#version 330
layout (location = 0) in vec4 vertex;

uniform mat4 projection;
out vec2 texPosition;

void main()
{
	texPosition = vertex.zw;
	gl_Position = projection * vec4(vertex.xy, 0, 1.0);
}
