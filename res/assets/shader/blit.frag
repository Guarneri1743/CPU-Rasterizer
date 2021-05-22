#version 460 core

out vec4 FragColor;

in vec3 out_color;
in vec2 out_uv;

uniform sampler2D framebuffer;

void main()
{
	FragColor = texture(framebuffer, vec2(out_uv.x, out_uv.y));
}