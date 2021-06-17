#version 330

in vec2 texCoord;
uniform vec4 color;
uniform sampler2D sprite;
out vec4 fragColor;

void main()
{
	fragColor = texture(sprite, texCoord) * color;
}
