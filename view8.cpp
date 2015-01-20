#include "view8.h"

#include "Mesh.h"
#include "util.h"

#include <ft2build.h>
#include FT_FREETYPE_H

GLuint view8_program = 0;
GLint view8_attribute_coord = -1, view8_uniform_tex = -1, view8_uniform_color = -1;

struct point8 {
	GLfloat x;
	GLfloat y;
	GLfloat s;
	GLfloat t;
};

GLuint view8_vbo = 0;

FT_Library view8_ft;
FT_Face view8_face;

const char *view8_fontfilename = "data/FreeSans.ttf";

// TODO: something wrong here
/**
 * Render text using the currently loaded font and currently set font size.
 * Rendering starts at coordinates (x, y), z is always 0.
 * The pixel coordinates that the FreeType2 library uses are scaled by (sx, sy).
 */
void render_text(const char *text, float x, float y, float sx, float sy,
                 FT_Face face, GLint uniform_tex, GLint attribute_coord,
                 GLuint vbo)
{
	const char *p;
	FT_GlyphSlot g = face->glyph;

	/* Create a texture that will be used to hold one "glyph" */
	GLuint tex;

	glActiveTexture(GL_TEXTURE0);
	glGenTextures(1, &tex);
	glBindTexture(GL_TEXTURE_2D, tex);
	glUniform1i(uniform_tex, 0);

	/* We require 1 byte alignment when uploading texture data */
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

	/* Clamping to edges is important to prevent artifacts when scaling */
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	/* Linear filtering usually looks best for text */
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	/* Set up the VBO for our vertex data */
	glEnableVertexAttribArray(attribute_coord);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glVertexAttribPointer(attribute_coord, 4, GL_FLOAT, GL_FALSE, 0, 0);

	/* Loop through all characters */
	for (p = text; *p; p++) {
		/* Try to load and render the character */
		if (FT_Load_Char(face, *p, FT_LOAD_RENDER)){
			continue;
        }

		/* Upload the "bitmap", which contains an 8-bit grayscale image,
         * as an alpha texture */

		glTexImage2D(GL_TEXTURE_2D, 0, GL_ALPHA,
                     g->bitmap.width, g->bitmap.rows,
                     0, GL_ALPHA, GL_UNSIGNED_BYTE,
                     g->bitmap.buffer);

		/* Calculate the vertex and texture coordinates */
		float x2 = x + g->bitmap_left * sx;
		float y2 = -y - g->bitmap_top * sy;
		float w = g->bitmap.width * sx;
		float h = g->bitmap.rows * sy;

		point8 box[] = {
			{x2, -y2, 0, 0},
			{x2 + w, -y2, 1, 0},
			{x2, -y2 - h, 0, 1},
			{x2 + w, -y2 - h, 1, 1},

            // problem
			// {x2, -y2, 0, 0},
			// {x2 + w, -y2, 1, 0},
			// {x2, -y2 - h, 0, 1},
			// {x2 + w, -y2 - h, 1, 1},
		};

		// point8 box[4] = {
		// 	{x2, -y2, 0, 0},
		// 	{x2 + w, -y2, 0, 0},
		// 	{x2, -y2 - h, 0, 0},
		// 	{x2 + w, -y2 - h, 0, 0},
		// };

        // printf ("%f %f\n%f %f\n%f %f\n%f %f\n\n",
        //         x2,     -y2,
        //         x2 + w, -y2,
        //         x2,     -y2 - h,
        //         x2 + w, -y2 - h);

		/* Draw the character on the screen */
		glBufferData(GL_ARRAY_BUFFER, sizeof(box), box, GL_DYNAMIC_DRAW);
		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

		/* Advance the cursor to the start of the next character */
		x += (g->advance.x >> 6) * sx;
		y += (g->advance.y >> 6) * sy;
	}

    glDisableVertexAttribArray(attribute_coord);
	glDeleteTextures(1, &tex);
}

int view8_initResources()
{
    glClearColor(1.0, 1.0, 1.0, 0);

	/* Enable blending, necessary for our alpha texture */
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	/* Initialize the FreeType2 library */
	if (FT_Init_FreeType(&view8_ft)) {
		fprintf(stderr, "Could not init freetype library\n");
		return 1;
	}

	/* Load a font */
	if (FT_New_Face(view8_ft, view8_fontfilename, 0, &view8_face)) {
		fprintf(stderr, "Could not open font %s\n", view8_fontfilename);
		return 1;
	}

	view8_program = create_program("glsl/text.8.v.glsl",
                                          "glsl/text.8.f.glsl");

	view8_attribute_coord = get_attrib(view8_program, "coord");
	view8_uniform_tex = get_uniform(view8_program, "tex");
	view8_uniform_color = get_uniform(view8_program, "color");

	if(view8_attribute_coord == -1 || view8_uniform_tex == -1
       || view8_uniform_color == -1) {
		return 1;
    }

	// Create the vertex buffer object
	glGenBuffers(1, &view8_vbo);

    // test_initResources();

    return 0;
}

void view8Display()
{
	float sx = 2.0 / glutGet(GLUT_WINDOW_WIDTH);
	float sy = 2.0 / glutGet(GLUT_WINDOW_HEIGHT);

    // printf ("width: %d, height: %d\n",
    //         glutGet(GLUT_WINDOW_WIDTH), glutGet(GLUT_WINDOW_HEIGHT));

	glUseProgram(view8_program);

	/* White background */
	glClear(GL_COLOR_BUFFER_BIT);

	GLfloat black[4] = {0, 0, 0, 1};
	GLfloat blue[4] = {0, 0, 1, 1};
	GLfloat red[4] = {1, 0, 0, 1};
	GLfloat transparent_green[4] = {0, 1, 0, 0.5};

	/* Set font size to 48 pixels, color to black */
	FT_Set_Pixel_Sizes(view8_face, 0, 24);
	glUniform4fv(view8_uniform_color, 1, blue);

    char buffer[20] = {'\0'};
    sprintf(buffer, "fps: %.f", fps);
	render_text(buffer, -1 + 28 * sx, 1 - 50 * sy, sx, sy,
                view8_face, view8_uniform_tex,
                view8_attribute_coord, view8_vbo);

    // testOnDisplay();

	glutSwapBuffers();
}

void view8_freeResources()
{
	glDisable(GL_BLEND);
	glDeleteProgram(view8_program);
}

void view8_entry(Window *window)
{
    window->display = view8Display;
    window->entry = viewEntry;
    window->init = view8_initResources;
    window->free = view8_freeResources;
}

#ifdef TEST_ALONE
float fps = 10.0;
int main(int argc, char *argv[])
{
    Window window;
    resetWindow(&window);

    view8_entry(&window);

    if (mini_initWindow(argc, argv, &window) == 0){
        glutMainLoop();
    }

    return 0;
}
#endif /* TEST_ALONE */
