#version 330 core
layout(location = 0) in vec4 vertex; // <vec2 postion, vec2 texture>

out vec4 color;

uniform mat4 model;
//we're omitting the view matrix because view never changes (its value is identity always)
uniform mat4 projection;

void main()
{
	TexCoords = vertex.zw;
	gl_Position = projection * model * vec4(vertex.xy, 0.0, 1.0);
}