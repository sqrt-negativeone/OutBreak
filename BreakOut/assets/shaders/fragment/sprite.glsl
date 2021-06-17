#version 330

in vec2 texCoordinates;
uniform sampler2D image;
uniform vec4 color;

out vec4 fragColor;

void main()
{
	fragColor = color * texture(image, texCoordinates);
}
