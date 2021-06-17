#version 330

in vec2 texPosition;
out vec4 color;

uniform vec3 textColor;
uniform sampler2D character;

void main()
{
	vec4 sampled = vec4(1.0, 1.0,1.0,texture(character, texPosition).r);
	color = vec4(textColor, 1.0) * sampled;
}
