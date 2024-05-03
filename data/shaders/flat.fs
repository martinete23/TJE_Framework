
uniform vec4 u_color;
uniform vec3 u_Kd;

void main()
{
	gl_FragColor = vec4(u_Kd, 1.0) * u_color;
}
