
varying vec3 v_position;
varying vec3 v_world_position;
varying vec3 v_normal;
varying vec2 v_uv;
varying vec4 v_color;

uniform vec4 u_color;
uniform sampler2D u_texture;
uniform float u_time;
uniform float u_scale;

void main()
{
	vec2 uv = v_uv * u_scale;
	gl_FragColor = u_color * texture2D( u_texture, uv );
}
