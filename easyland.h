#pragma once

#include "resource.h"

int dispx;	//	i use to store display size
int dispy;

HWND hOGLWnd = NULL;
HGLRC hrc0 = NULL;	//	two render contexts to flip
HGLRC hrc1 = NULL;

unsigned int vertexShader;
unsigned int fragmentShader;
unsigned int shaderProgram;

unsigned int shadowvertexShader;
unsigned int shadowfragmentShader;
unsigned int shadowshaderProgram;


unsigned int vao, vbo, ebo;


unsigned int depthMapFBO;
const unsigned int SHADOW_WIDTH  = 1024;
const unsigned int SHADOW_HEIGHT = 1024;
unsigned int depthMap;


float vertices[1170450];//x,y,z,r,g,b, normal x,y,z 255 strips of 510 triangles
unsigned int indices[390150];
float cam[3];	//	player location, axis rotations
float yr, xr;	//	round and round/up an down
float yrv, xrv;
float vel;
unsigned int rnd = 100;
int fc = 0;

float normx[256][256];
float normy[256][256];
float normz[256][256];



//float sun[] = { .3535f, .7071f, .6123f };
float sun[] = { -10.f, 1.f, -1.f};
//float sun[] = { -.5f, -.5f, -.3f };



void makeheightfield() {

	float grid[256][256];	//	y,x
	float perlin0[16][16];
	float perlin1[16][16];
	for (int j = 0; j < 16; j++) for (int i = 0; i < 16; i++) {
		rnd = 196314165 * rnd + 907633515;
		float temp = (float)rnd * 1.462918e-9f;	// 2.3283e-10f;
		perlin0[j][i] = cos(temp);	perlin1[j][i] = sin(temp);
	}

	for (unsigned int j = 0; j < 256; j++) for (unsigned int i = 0; i < 256; i++) {
		float scale = .005f;//.0085f;//.0077f;//.017f;//.009f;
		float gain = 1.f;
		int octaves = 8;
		float hfsum = 0.f;
		for (int c = 0; c < octaves; c++) {
			float px = (float)(i + 277) * scale;
			float py = (float)(j + 77) * scale;
			unsigned char x0, y0, x1, y1;
			px = fmod(px, 255.f);	py = fmod(py, 255.f);
			x0 = (unsigned char)px;	y0 = (unsigned char)py;
			x1 = (x0 + 1) & 255;	y1 = (y0 + 1) & 255;
			float dx = px - (float)x0;
			float dy = py - (float)y0;
			x0 &= 15;	y0 &= 15;	x1 &= 15;	y1 &= 15;
/*			dx += .6f * (dx * dx * (3.f - dx - dx) - dx);	dy += .6f * (dy * dy * (3.f - dy - dy) - dy);
			float i0 = perlin[x0][y0] + dx * (perlin[x1][y0] - perlin[x0][y0]);
			float i1 = perlin[x0][y1] + dx * (perlin[x1][y1] - perlin[x0][y1]);
			i0 += dy * (i1 - i0);
*/
			float sx = dx * dx * (3.f - dx - dx);
			float sy = dy * dy * (3.f - dy - dy);
			float xm = dx - 1.f;
			float ym = dy - 1.f;
			float i0 = perlin0[x0][y0] * dx + perlin1[x0][y0] * dy;
			float i1 = perlin0[x1][y0] * xm + perlin1[x1][y0] * dy;
			i0 += sx * (i1 - i0);
			float i2 = perlin0[x0][y1] * dx + perlin1[x0][y1] * ym;
			float i3 = perlin0[x1][y1] * xm + perlin1[x1][y1] * ym;
			i2 += sx * (i3 - i2);
			i0 += sy * (i2 - i0);
			//i0 += i0;
			i0 += .5f;
			//i0 = fmin(fmax(i0, 0.f), 1.f);
			//i0 = fmax(i0, 0.f);
			//i0 = (i0 + i0 * i0) * .5f;
			i0 = 1.f - abs(1.f - i0 - i0);
			hfsum += i0 * i0 * gain;
			//c < 5 ? hfsum += i0 * i0 * gain : hfsum += i0 * i0 * gain * hfsum * .5f;
			gain *= .55f;//.5f;//.62
			scale *= 1.77f;//1.5f;//1.5
		}
		grid[j][i] = (hfsum * hfsum) * 5.5f - 20.f;
	}

	for (int j = 0; j < 256; j++) {
		normx[j][0] = grid[j][0] - grid[j][1];	//	calculate normals
		normy[0][j] = grid[0][j] - grid[1][j];
		for (int i = 1; i < 255; i++) {
			normx[j][i] = ((grid[j][i - 1] - grid[j][i]) + (grid[j][i] - grid[j][i + 1])) * 0.5f;
			normy[i][j] = ((grid[i - 1][j] - grid[i][j]) + (grid[i][j] - grid[i + 1][j])) * 0.5f;
		}
		normx[j][255] = grid[j][254] - grid[j][255];
		normy[255][j] = grid[254][j] - grid[255][j];
	}
	for (int j = 0; j < 256; j++) for (int i = 0; i < 256; i++) {
		float hyp = 1.f / sqrt(normx[j][i] * normx[j][i] + normy[j][i] * normy[j][i] + 1.f);
		normx[j][i] *= hyp;	normy[j][i] *= hyp;	normz[j][i] = hyp;//bcs this is from 2d hf, z = vertical
	}

	//1 3 5..
	//0 2 4  two to begin, +1 per tri
	for (int j = 0; j < 255; j++) {
		int jx = j * 4590;	int jm = j + 1;//3060
		for (int i = 0; i < 510; i += 2) {
			register int ix = jx + i * 9;	int tx = i >> 1;
			vertices[ix] = tx - 127;	vertices[ix + 1] = grid[j][tx];	vertices[ix + 2] = j - 127;
			vertices[ix + 3] = vertices[ix + 4] = vertices[ix + 5] = .5f;
			vertices[ix + 6] = normx[j][tx];	vertices[ix + 7] = normz[j][tx];	vertices[ix + 8] = normy[j][tx];
			vertices[ix + 9] = tx - 127;	vertices[ix + 10] = grid[jm][tx];	vertices[ix + 11] = j - 126;
			vertices[ix + 12] = vertices[ix + 13] = vertices[ix + 14] = .5f;
			vertices[ix + 15] = normx[jm][tx];	vertices[ix + 16] = normz[jm][tx];	vertices[ix + 17] = normy[jm][tx];
		}
	}
	for (int k = 0; k < 255; k++) {
		unsigned long int tj = k * 510;
		for (int i = 0; i < 510; i += 2) {
			unsigned long int j = tj + i;
			indices[j] = j;		indices[j + 1] = j + 1;
		}
	}
}





void renderscene() {
	glBindVertexArray(vao);
	for (int i = 0; i < 255; i++)
		glDrawElements(GL_TRIANGLE_STRIP, 510, GL_UNSIGNED_INT, (void*)(sizeof(unsigned int) * i * 510));	//	# vertices (tris * 3)

}

void display(void) {

	norm(&sun[0], &sun[1], &sun[2]);


	float scale = .005f;

	int sd = SHADOW_HEIGHT * 8;
	//float c0 = (int)(cam[0] * sd) / (float)sd;
	//float c1 = (int)(cam[1] * sd) / (float)sd;
	//float c2 = (int)(cam[2] * sd) / (float)sd;
	//float c0 = (int)(cam[0] / scale * sd) / (float)sd * scale;
	//float c1 = (int)(cam[1] / scale * sd) / (float)sd * scale;
	//float c2 = (int)(cam[2] / scale * sd) / (float)sd * scale;
	float c0 = cam[0];
	float c1 = cam[1];
	float c2 = cam[2];

	
	float model[] = {
		scale, 0.f, 0.f, 0.f,
		0.f, scale, 0.f, 0.f,
		0.f, 0.f, scale, 0.f,
		c0, c1, c2, 1.f };

//	begin shadow map
	float look[] = { 0.f, 0.f, 0.f };	//	look at point
	float sh[16] = { 0 };

	sh[15] = 1.f;
	sh[2] = look[0] - sun[0];	//	forward
	sh[6] = look[1] - sun[1];
	sh[10] = look[2] - sun[2];
	norm(&sh[2], &sh[6], &sh[10]);
	sh[0] = -sh[10];	//	90 degrees right
	sh[4] = 0.f;
	sh[8] = sh[2];
	norm(&sh[0], &sh[4], &sh[8]);
	sh[1] = sh[4] * sh[10] - sh[8] * sh[6];	//	"up" = rt x fwd
	sh[5] = sh[8] * sh[2] - sh[0] * sh[10];
	sh[9] = sh[0] * sh[6] - sh[4] * sh[2];
	norm(&sh[1], &sh[5], &sh[9]);

	//sh[12] = -(sh[0] * sun[0] + sh[4] * sun[1] + sh[8] * sun[2]);
	//sh[13] = -(sh[1] * sun[0] + sh[5] * sun[1] + sh[9] * sun[2]);
	//sh[14] = -(sh[2] * sun[0] + sh[6] * sun[1] + sh[10] * sun[2]);

	for (int i = 0; i < 12; i++) sh[i] *= 1.4f;

	glUseProgram(shadowshaderProgram);
	int shadowviewMatrixLocation = glGetUniformLocation(shadowshaderProgram, "lightSpaceMatrix");
	glUniformMatrix4fv(shadowviewMatrixLocation, 1, GL_FALSE, &sh[0]);

	int shadowmodelMatrixLocation = glGetUniformLocation(shadowshaderProgram, "model");
	glUniformMatrix4fv(shadowmodelMatrixLocation, 1, GL_FALSE, &model[0]);

	glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
	glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
	glClear(GL_DEPTH_BUFFER_BIT);
	renderscene();
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
//	end shadow map




	model[12] = cam[0];
	model[13] = cam[1];
	model[14] = cam[2];

	float aspect = (float)dispy / (float)dispx;		// projection matrix for perspective
	float nc = .0001f; // custom near plane value
	float fc = 2.f; // custom far plane value
	float range = 1.f / (nc - fc);
	//float fov2 = .78539816339f;	//	y fov in radians /2
	float ctf = 1.8f;	//cos(fov2) / sin(fov2);	//	cotangent.. for fov90 this is 1, for 45 this is 2.414213562373
	float projection[] = {
		ctf * aspect, 0.f, 0.f, 0.f,
		0.f, ctf, 0.f, 0.f,
		0.f, 0.f, (nc + fc) * range, -1.f,
		0.f, 0.f, 2.f * nc * fc * range, 0.f };

	float cy = cos(yr);	//	really cy.. fix when finished
	float sy = sin(yr);
	float cx = cos(xr);	//	really cx
	float sx = sin(xr);

	float view[] = {
		cy, sy * sx, sy * cx, 0.f,
		0.f, cx / aspect, -sx / aspect, 0.f,
		-sy, cy * sx, cy * cx, 0.f,
		0.f, 0.f, 0.f, 1.f };

glUseProgram(shaderProgram);
	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);//omg
	glViewport(0, 0, dispx, dispy);
	//glClearColor(0.7f, 0.74f, 1.f, 1.0f);
	glClearColor(0.4f, .62f, .9f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	int modelMatrixLocation = glGetUniformLocation(shaderProgram, "modelMatrix");
	glUniformMatrix4fv(modelMatrixLocation, 1, GL_FALSE, &model[0]);

	int viewMatrixLocation = glGetUniformLocation(shaderProgram, "viewMatrix");
	glUniformMatrix4fv(viewMatrixLocation, 1, GL_FALSE, &view[0]);
	//glGetFloatv(GL_PROJECTION_MATRIX, projection);
	int projectionMatrixLocation = glGetUniformLocation(shaderProgram, "projectionMatrix");
	glUniformMatrix4fv(projectionMatrixLocation, 1, GL_FALSE, &projection[0]);

	int lightSpaceMatrixLocation = glGetUniformLocation(shaderProgram, "lightSpaceMatrix");
	glUniformMatrix4fv(lightSpaceMatrixLocation, 1, GL_FALSE, &sh[0]);

	int sunLocation = glGetUniformLocation(shaderProgram, "sun");
	glUniform3fv(sunLocation, 1, &sun[0]);


	GLuint depthMapTextureUnit = 0;  // Assuming texture unit 0 is available
	glActiveTexture(GL_TEXTURE0 + depthMapTextureUnit);
	glBindTexture(GL_TEXTURE_2D, depthMap);

	GLint depthMapLocation = glGetUniformLocation(shaderProgram, "deepMap");
	glUniform1i(depthMapLocation, depthMapTextureUnit);


renderscene();

//	glBindVertexArray(vao);	glGenVertexArrays(1, &vao);	glDrawArrays(GL_TRIANGLES, 0, 3);	//one vbo/one triangle

	HDC hDC = GetDC(hOGLWnd);	//	classic opengl display
	SwapBuffers(hDC);
	ReleaseDC(hOGLWnd, hDC);


	yr += yrv;	xr += xrv;	//	move player
	if (xr > 1.5f) xr = 1.5f;
	else if (xr < -1.5f) xr = -1.5f;
	cam[0] += sy * vel;	cam[2] += cy * vel;
	//cam[1] = -0.15f;
	yrv *= .99f;	xrv *= .99f;	vel *= .99f;
	if (vel > -.000001f && vel < .000001f) vel = 0.f;
	if (yrv > -.000001f && yrv < .000001f) yrv = 0.f;
	if (xrv > -.000001f && xrv < .000001f) xrv = 0.f;

}
