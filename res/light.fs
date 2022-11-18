#version 460 core
out vec4 FragColor;

// in vec2 TexCoord;

// texture samplers
uniform sampler2D texture;

void main()
{
	// FragColor = texture(texture, TexCoord);
	FragColor = vec4(1.0, 0.0, 1.0, 1.0);
}