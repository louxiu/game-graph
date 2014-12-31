attribute vec4 coord2d;
attribute vec4 v_color;
varying vec3 f_color;

void main()
{
    gl_Position = vec4(coord2d);
    f_color = vec3(v_color);
}
