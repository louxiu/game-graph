attribute vec3 coord2d;
attribute vec3 pic_color;
varying vec4 f_color;
uniform float offset_x;
uniform float offset_y;
uniform lowp float sprite;
uniform mat4 mvp;

void main(void) {
	gl_Position = mvp * vec4((coord2d.x + offset_x),
							 (coord2d.y + offset_y), coord2d.z, 1);

	f_color = vec4(pic_color, 1);
	// 0 for water
	gl_PointSize = max(1.0, sprite);
}
