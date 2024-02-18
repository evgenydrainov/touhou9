#version 330 core

layout(location = 0) in vec3 in_Position;
layout(location = 1) in vec4 in_Color;
layout(location = 2) in vec2 in_TexCoord;

out vec4 v_Color;
out vec3 v_Position;

uniform mat4 u_MVP;

void main() {
	gl_Position = u_MVP * vec4(in_Position, 1.0);

	v_Color = in_Color;
	v_Position = in_Position;
}
