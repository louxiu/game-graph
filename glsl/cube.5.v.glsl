attribute vec4 coord3d;
attribute vec4 v_color;
varying vec4 f_color;

uniform mat4 mvp;

void main()
{
    gl_Position = mvp * coord3d;
    f_color = v_color;
}
