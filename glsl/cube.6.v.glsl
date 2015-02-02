attribute vec4 coord3d;
attribute vec4 texcoord;
varying vec2 f_texcoord;
uniform mat4 mvp;

void main(void) {
  gl_Position = mvp * coord3d;
  f_texcoord = vec2(texcoord.x, texcoord.y);
}
