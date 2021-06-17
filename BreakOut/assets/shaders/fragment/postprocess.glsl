#version 330

in vec2 texCoordinates;
uniform sampler2D scene;

out vec4 fragColor;
uniform bool shake;

uniform vec2 offsets[9];
uniform float blur_kernel[9];

void main()
{
	fragColor = vec4(0.0);
	if (shake)
	{
		for (int i =0; i < 9; ++i)
		{
			vec3 samples = vec3(texture(scene, texCoordinates + offsets[i]));
			fragColor += vec4(samples * blur_kernel[i], 0.0);
			fragColor.a = 1;
		}
	}
	else
		fragColor = texture(scene, texCoordinates);
}
