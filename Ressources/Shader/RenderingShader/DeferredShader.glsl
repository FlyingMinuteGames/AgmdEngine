#version 330
#include "global_uniform.glsl"

#ifdef _VERTEX_

in vec3 in_Vertex;
in vec3 in_Normal;
in vec4 in_Color;
in vec2 in_TexCoord0;

out vec4 color;
out vec2 texCoord0;
out vec3 normal;
out vec4 pos;

void main()
{
	vec3 vertex = in_Vertex;

	
	color = in_Color;
	texCoord0 = in_TexCoord0;
	normal = mat3(u_matModel) * in_Normal;
	pos = u_matModel * vec4(in_Vertex, 1.0f);

	gl_Position = u_matViewProjection *  u_matModel * vec4(vertex,1.0f);
}
#endif


#ifdef _FRAGMENT_

in vec4 color;
in vec2 texCoord0;
in vec3 normal;
in vec4 pos;

layout(location = 0) out vec4 out_Color;
layout(location = 1) out vec3 out_Normal;
layout(location = 2) out vec3 out_Position;
uniform sampler2D texture0;

void main()
{
	out_Color = texture(texture0,texCoord0);
	out_Normal = normal;
	out_Position = pos.xyz;
}



#endif