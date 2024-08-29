#version 330 core
out vec4 FragColor;

in vec3 ourColor;
in vec2 UV;

// texture samplers
uniform sampler2D texture1;
uniform bool useTexture;

void main()
{
	if (useTexture)
	{
		FragColor = texture(texture1, UV);
	}
	else
	{
		FragColor = vec4(ourColor, 1.0f);
	}
}