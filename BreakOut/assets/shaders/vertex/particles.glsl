#version 330
layout (location = 0) in vec4 vertex;

out vec2 texCoord;

uniform mat4 projection;
uniform vec2 offset;
uniform float scale;

void main()
{
	texCoord = vec2(vertex.yz);
	gl_Position = projection * vec4(offset + vertex.xy * scale, 0, 1);
}
