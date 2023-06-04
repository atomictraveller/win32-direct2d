
#include <cmath>
#include <windows.h>
#include <stdio.h>
#include <GL/gl.h>

#pragma comment(lib, "opengl32.lib")


#include "framework.h"	//	default msvc win32 header
#include "ogl.h"	//	my opengl header
#include "easyland.h"	//	header for this project


#define MAX_LOADSTRING 100

HINSTANCE hInst;
WCHAR szTitle[MAX_LOADSTRING];
WCHAR szWindowClass[MAX_LOADSTRING];

ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_WIN32OPENGL, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);
    if (!InitInstance (hInstance, nCmdShow)) return FALSE;

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_WIN32OPENGL));
    MSG msg;
    while (GetMessage(&msg, nullptr, 0, 0)) {
        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg)) {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }
    return (int) msg.wParam;
}

ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style          = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc    = WndProc;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = hInstance;
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_WIN32OPENGL));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_WIN32OPENGL);
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}

BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
	hInst = hInstance;

	HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, nullptr);

	if (!hWnd) return FALSE;


//	begin opengl
	hOGLWnd = hWnd;
	HDC hDC = GetDC(hOGLWnd);

	DWORD pixelFormatFlags = PFD_SUPPORT_OPENGL | PFD_SUPPORT_COMPOSITION | PFD_GENERIC_ACCELERATED | PFD_DRAW_TO_WINDOW | PFD_DOUBLEBUFFER;
	PIXELFORMATDESCRIPTOR pfd = {
		sizeof(PIXELFORMATDESCRIPTOR),
		1, pixelFormatFlags, PFD_TYPE_RGBA, 32,	//Colordepth of the framebuffer.
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		24,                       //Number of bits for the depthbuffer
		8,                        //Number of bits for the stencilbuffer
		0,                        //Number of Aux buffers in the framebuffer.
		PFD_MAIN_PLANE,
		0, 0, 0, 0
	};
	int pixelFormat = ChoosePixelFormat(hDC, &pfd);
	SetPixelFormat(hDC, pixelFormat, &pfd);

	hrc0 = wglCreateContext(hDC);
	hrc1 = wglCreateContext(hDC);
	wglMakeCurrent(hDC, hrc1);

	HMODULE hOpenGL = LoadLibrary(L"opengl32.dll");	//	begin loading "modern" opengl API
	if (hOpenGL == NULL) return 0;
	//glCreateShader = (PFNGLCREATESHADERPROC)GetProcAddress(hOpenGL, "glCreateShader");
	glCreateShader = reinterpret_cast<PFNGLCREATESHADERPROC>(wglGetProcAddress("glCreateShader"));
	glShaderSource = reinterpret_cast<PFNGLSHADERSOURCEPROC>(wglGetProcAddress("glShaderSource"));
	glCompileShader = reinterpret_cast<PFNGLCOMPILESHADERPROC>(wglGetProcAddress("glCompileShader"));
	glGetShaderiv = reinterpret_cast<PFNGLGETSHADERIVPROC>(wglGetProcAddress("glGetShaderiv"));
	glGetShaderInfoLog = reinterpret_cast<PFNGLGETSHADERINFOLOGPROC>(wglGetProcAddress("glGetShaderInfoLog"));
	glCreateProgram = reinterpret_cast<PFNGLCREATEPROGRAMPROC>(wglGetProcAddress("glCreateProgram"));
	glAttachShader = reinterpret_cast<PFNGLATTACHSHADERPROC>(wglGetProcAddress("glAttachShader"));
	glLinkProgram = reinterpret_cast<PFNGLLINKPROGRAMPROC>(wglGetProcAddress("glLinkProgram"));
	glValidateProgram = reinterpret_cast<PFNGLVALIDATEPROGRAMPROC>(wglGetProcAddress("glValidateProgram"));
	glGetProgramiv = reinterpret_cast<PFNGLGETPROGRAMIVPROC>(wglGetProcAddress("glGetProgramiv"));
	glGetProgramInfoLog = reinterpret_cast<PFNGLGETPROGRAMINFOLOGPROC>(wglGetProcAddress("glGetProgramInfoLog"));////was missing
	glGenBuffers = reinterpret_cast<PFNGLGENBUFFERSPROC>(wglGetProcAddress("glGenBuffers"));
	glGenVertexArrays = reinterpret_cast<PFNGLGENVERTEXARRAYSPROC>(wglGetProcAddress("glGenVertexArrays"));
	glGetAttribLocation = reinterpret_cast<PFNGLGETATTRIBLOCATIONPROC>(wglGetProcAddress("glGetAttribLocation"));
	glBindVertexArray = reinterpret_cast<PFNGLBINDVERTEXARRAYPROC>(wglGetProcAddress("glBindVertexArray"));
	glEnableVertexAttribArray = reinterpret_cast<PFNGLENABLEVERTEXATTRIBARRAYPROC>(wglGetProcAddress("glEnableVertexAttribArray"));
	glVertexAttribPointer = reinterpret_cast<PFNGLVERTEXATTRIBPOINTERPROC>(wglGetProcAddress("glVertexAttribPointer"));
	glBindBuffer = reinterpret_cast<PFNGLBINDBUFFERPROC>(wglGetProcAddress("glBindBuffer"));
	glBufferData = reinterpret_cast<PFNGLBUFFERDATAPROC>(wglGetProcAddress("glBufferData"));
	glGetVertexAttribPointerv = reinterpret_cast<PFNGLGETVERTEXATTRIBPOINTERVPROC>(wglGetProcAddress("glGetVertexAttribPointerv"));
	glUseProgram = reinterpret_cast<PFNGLUSEPROGRAMPROC>(wglGetProcAddress("glUseProgram"));
	glDeleteVertexArrays = reinterpret_cast<PFNGLDELETEVERTEXARRAYSPROC>(wglGetProcAddress("glDeleteVertexArrays"));
	glDeleteBuffers = reinterpret_cast<PFNGLDELETEBUFFERSPROC>(wglGetProcAddress("glDeleteBuffers"));
	glDeleteProgram = reinterpret_cast<PFNGLDELETEPROGRAMPROC>(wglGetProcAddress("glDeleteProgram"));
	glDeleteShader = reinterpret_cast<PFNGLDELETESHADERPROC>(wglGetProcAddress("glDeleteShader"));

	glActiveTexture = reinterpret_cast<PFNGLACTIVETEXTUREPROC>(wglGetProcAddress("glActiveTexture"));

	glUniform1i = reinterpret_cast<PFNGLUNIFORM1IPROC>(wglGetProcAddress("glUniform1i"));
	glUniform1fv = reinterpret_cast<PFNGLUNIFORM1FVPROC>(wglGetProcAddress("glUniform1fv"));
	glUniform3f = reinterpret_cast<PFNGLUNIFORM3FPROC>(wglGetProcAddress("glUniform3f"));
	glUniform4f = reinterpret_cast<PFNGLUNIFORM4FPROC>(wglGetProcAddress("glUniform4f"));
	glUniform3fv = reinterpret_cast<PFNGLUNIFORM3FVPROC>(wglGetProcAddress("glUniform3fv"));
	glUniform4fv = reinterpret_cast<PFNGLUNIFORM4FVPROC>(wglGetProcAddress("glUniform4fv"));
	glUniformMatrix3fv = reinterpret_cast<PFNGLUNIFORMMATRIX3FVPROC>(wglGetProcAddress("glUniformMatrix3fv"));
	glUniformMatrix4fv = reinterpret_cast<PFNGLUNIFORMMATRIX4FVPROC>(wglGetProcAddress("glUniformMatrix4fv"));
	glGetUniformLocation = reinterpret_cast<PFNGLGETUNIFORMLOCATIONPROC>(wglGetProcAddress("glGetUniformLocation"));

	glCreateVertexArrays = reinterpret_cast<PFNGLCREATEVERTEXARRAYSPROC>(wglGetProcAddress("glCreateVertexArrays"));
	glGenFramebuffers = reinterpret_cast<PFNGLGENFRAMEBUFFERSPROC>(wglGetProcAddress("glGenFramebuffers"));

	glBindFramebuffer = reinterpret_cast<PFNGLBINDFRAMEBUFFERPROC>(wglGetProcAddress("glBindFramebuffer"));
	glFramebufferTexture1D = reinterpret_cast<PFNGLFRAMEBUFFERTEXTURE1DPROC>(wglGetProcAddress("glFramebufferTexture1D"));
	glFramebufferTexture2D = reinterpret_cast<PFNGLFRAMEBUFFERTEXTURE2DPROC>(wglGetProcAddress("glFramebufferTexture2D"));
	glFramebufferTexture3D = reinterpret_cast<PFNGLFRAMEBUFFERTEXTURE3DPROC>(wglGetProcAddress("glFramebufferTexture3D"));
	glFramebufferRenderbuffer = reinterpret_cast<PFNGLFRAMEBUFFERRENDERBUFFERPROC>(wglGetProcAddress("glFramebufferRenderbuffer"));

	FreeLibrary(hOpenGL);
	if (glCreateShader == NULL) return 0;
	if (glShaderSource == NULL) return 0;
	if (glCompileShader == NULL) return 0;
	if (glGetShaderiv == NULL) return 0;
	if (glGetShaderInfoLog == NULL) return 0;
	if (glCreateProgram == NULL) return 0;
	if (glAttachShader == NULL) return 0;
	if (glLinkProgram == NULL) return 0;
	if (glValidateProgram == NULL) return 0;
	if (glGetProgramiv == NULL) return 0;
	if (glGetProgramInfoLog == NULL) return 0;	//	
	if (glGenBuffers == NULL) return 0;
	if (glGenVertexArrays == NULL) return 0;
	if (glGetAttribLocation == NULL) return 0;
	if (glBindVertexArray == NULL) return 0;
	if (glEnableVertexAttribArray == NULL) return 0;
	if (glVertexAttribPointer == NULL) return 0;
	if (glBindBuffer == NULL) return 0;
	if (glBufferData == NULL) return 0;
	if (glGetVertexAttribPointerv == NULL) return 0;
	if (glUseProgram == NULL) return 0;
	if (glDeleteVertexArrays == NULL) return 0;
	if (glDeleteBuffers == NULL) return 0;
	if (glDeleteProgram == NULL) return 0;
	if (glDeleteShader == NULL) return 0;

	if (glActiveTexture == NULL) return 0;

	if (glUniform1i == NULL) return 0;
	if (glUniform1fv == NULL) return 0;
	if (glUniform3f == NULL) return 0;
	if (glUniform4f == NULL) return 0;
	if (glUniform3fv == NULL) return 0;
	if (glUniform4fv == NULL) return 0;
	if (glUniformMatrix3fv == NULL) return 0;
	if (glUniformMatrix4fv == NULL) return 0;
	if (glGetUniformLocation == NULL) return 0;

	if (glCreateVertexArrays == NULL) return 0;
	if (glGenFramebuffers == NULL) return 0;

	if (glBindFramebuffer == NULL) return 0;
	if (glFramebufferTexture1D == NULL) return 0;
	if (glFramebufferTexture2D == NULL) return 0;
	if (glFramebufferTexture3D == NULL) return 0;
	if (glFramebufferRenderbuffer == NULL) return 0;


	const char* simpleDepthShaderSource = R"(
		#version 330 core
		layout (location = 0) in vec3 pos;
		uniform mat4 lightSpaceMatrix;
		uniform mat4 model;
		void main() {
			gl_Position = lightSpaceMatrix * model * vec4(pos.x, pos.y, pos.z, 1.0);
		}
	)";
	const char* simpleFragmentShaderSource = R"(
		#version 330 core
		void main() {  
			// gl_FragDepth = gl_FragCoord.z;
		}   
	)";




	const char* vertexShaderSource = R"(
		#version 330 core
		layout (location = 0) in vec3 pos;
		layout (location = 1) in vec3 color;
		layout (location = 2) in vec3 normal;
		out vec4 vertexColor;
		out vec3 texCoord;
		out vec3 norma;
		out float depth;
out vec3 FragPos;
out vec4 FragPosLightSpace;

		uniform mat4 modelMatrix; 
		uniform mat4 viewMatrix;
		uniform mat4 projectionMatrix;
		uniform mat4 lightSpaceMatrix;

		void main() {
			FragPos = vec3(modelMatrix * vec4(pos.x, pos.y, pos.z, 1.0));
			FragPosLightSpace = lightSpaceMatrix * vec4(FragPos.x, max(FragPos.y, -14.1), FragPos.z, 1.0);

			gl_Position = projectionMatrix * viewMatrix * vec4(FragPos, 1.0);
			depth = (gl_Position.z - .001) * (gl_Position.z + .025);	//	for perlin octaves
			vertexColor = vec4(color, 1.0);
			texCoord = pos;
			norma = normal;
		}
	)";




	const char* fragmentShaderSource = R"(
		#version 330 core
		out vec4 FragColor;
		in vec4 vertexColor;
		in vec3 texCoord;
		in float depth;
in vec3 norma;
in vec3 FragPos;
in vec4 FragPosLightSpace;

		uniform float rv[49];
		uniform sampler2D deepMap;
uniform vec3 sun;
		//vec3 ambient = ambientStrength * lightColor;
		//vec3 diffuse = norm * lightColor;vec3 result = (ambient + diffuse) * objectColor;

float ShadowCalculation(vec4 fragPosLightSpace) {
    vec3 projCoords = fragPosLightSpace.xyz;// / fragPosLightSpace.w;

    projCoords = projCoords * 0.5 + 0.5;
	float closestDepth = texture(deepMap, projCoords.xy).r; 
    float currentDepth = projCoords.z;

float bias = max(0.04 * (1.0 - dot(norma, sun)), 0.003); //.004 
//float bias = max(0.1 * (1.0 - dot(norma, sun)), 0.005); //.004 
float shadow = currentDepth - bias > closestDepth  ? 0.0 : 0.6;

/*
//float shadow = 0.0;

vec2 ts = 2.5 / textureSize(deepMap, 0);

closestDepth = texture(deepMap, projCoords.xy + ts).r;	shadow += currentDepth - bias > closestDepth ? 0.0 : 0.4;    
closestDepth = texture(deepMap, projCoords.xy - ts).r;	shadow += currentDepth - bias > closestDepth ? 0.0 : 0.4;    
ts.x = -ts.x;
closestDepth = texture(deepMap, projCoords.xy + ts).r;	shadow += currentDepth - bias > closestDepth ? 0.0 : 0.4;    
closestDepth = texture(deepMap, projCoords.xy - ts).r;	shadow += currentDepth - bias > closestDepth ? 0.0 : 0.4;    
ts.y = 0;
closestDepth = texture(deepMap, projCoords.xy + ts).r;	shadow += currentDepth - bias > closestDepth ? 0.0 : 0.5;    
closestDepth = texture(deepMap, projCoords.xy - ts).r;	shadow += currentDepth - bias > closestDepth ? 0.0 : 0.5;     
ts.y = ts.x;	ts.x = 0.0;
closestDepth = texture(deepMap, projCoords.xy + ts).r;	shadow += currentDepth - bias > closestDepth ? 0.0 : 0.5;    
closestDepth = texture(deepMap, projCoords.xy - ts).r;	shadow += currentDepth - bias > closestDepth ? 0.0 : 0.5;   

shadow *= 0.1190476;//0.125;//135135;
*/


if(projCoords.z > 1.0) shadow = 0.5;
    return shadow;
}  




		void main() {
			uint ix, iy, iz;
			float scale = 0.01;//1472;//.003  // .001
			float gain = 0.9;
	float fdeep = 2.0 + .005 / max(depth * 0.04, .001); //*0.016
			int octaves = int(fdeep);
	float octd = fdeep - float(octaves);
			float hf = 0.0;
			float hfval = 0.0;
			for (int i = 0; i < octaves; i++) {
				hf += hfval * gain;
				float hfx = texCoord.x * scale;
				float hfy = texCoord.y * scale;
				float hfz = texCoord.z * scale;
				hfx = fract(hfx) * 25.0;	hfy = fract(hfy) * 25.0;	hfz = fract(hfz) * 25.0;
				ix = uint(hfx);	iy = uint(hfy);	iz = uint(hfz);
				float dx = hfx - ix;	float dy = hfy - iy;	float dz = hfz - iz;
				hfx = rv[ix];	hfy = rv[iy];	hfz = rv[iz];
				ix++;	iy++;	iz++;
				float vt0 = fract(hfx + hfy + hfz);	//	vertices of cube for interpolation
				float vt1 = fract(rv[ix] + hfy + hfz);
				float vt2 = fract(hfx + hfy + rv[iz]);
				float vt3 = fract(rv[ix] + hfy + rv[iz]);
				float vt4 = fract(hfx + rv[iy] + hfz);
				float vt5 = fract(rv[ix] + rv[iy] + hfz);
				float vt6 = fract(hfx + rv[iy] + rv[iz]);
				float vt7 = fract(rv[ix] + rv[iy] + rv[iz]);
				dx *= dx * (3.0 - dx - dx);
				dy *= dy * (3.0 - dy - dy);
				dz *= dz * (3.0 - dz - dz);
				float vi0 = vt0 + dx * (vt1 - vt0);
				float vi1 = vt2 + dx * (vt3 - vt2);
				float vi2 = vt4 + dx * (vt5 - vt4);
				float vi3 = vt6 + dx * (vt7 - vt6);
				vi0 += dz * (vi1 - vi0);
				vi2 += dz * (vi3 - vi2);
				vi0 += dy * (vi2 - vi0);
			//	vi0 = abs(1.0 - vi0 - vi0);
			//	hf += vi0 * gain
hfval = vi0;
				scale *= 2.1872;
				gain *= .72;//.6
			}
hf += octd * hfval * gain;
			hf *= 0.22;
//hf *= 0.25;
hf += .32;


			//float norm = max(-0.25, (norma.x * .7071 + norma.y * .3535 + norma.z * .6123 - 0.7) * 0.5);

float norm = max(-0.25, (dot(norma, sun) * .7 - 0.3));	//	DIFFUSE LIGHTING 
//		hf += norm + specular + .125f;


hf += norm;
float shadow = ShadowCalculation(FragPosLightSpace); 

hf = hf * 0.2 + shadow * (hf + norm);
//hf = shadow * -0.1 + 0.2;
//hf = texture(deepMap, texCoord.xz * 0.05).r; 
//FragColor = vec4(hf, hf, hf, 1.0);


						////float specularStrength = 0.5;
						////vec3 specular = specularStrength * spec;// * lightColor;  



			float water = -14.0;
			if (texCoord.y > water) FragColor = vec4(0.2 + hf, 0.12 + hf, 0.1 + hf, 1.0);
			else {
				float d = (texCoord.y - water) / texCoord.y;
				hf *= max(0.0, 0.7 - 0.2 * length(texCoord) * d);
				
			vec3 reflectDir = reflect(sun, vec3(0.0, 1.0, 0.0)); 
vec3 viewDir = normalize(FragPos);
//need to recalculate fragpos if water in vertex shader but good enuff for now
			float spec = pow(max(dot(viewDir, reflectDir), 0.0), 24);
			float specular = spec * spec * 0.7;
				hf += specular;

				//FragColor = vec4(hf, 0.1 + hf, 0.25 + hf, 1.0);
				FragColor = vec4(hf, 0.22 + hf, 0.25 + hf, 1.0);
			}



		} 
	)";


	int  success;
	char infoLog[512];
	vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
	glCompileShader(vertexShader);
	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);	//	fault checking luxury version
	if (!success) {
		glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);	//	do something with infolog
	}
	fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
	glCompileShader(fragmentShader);
	glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
	if (!success) {
		glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);	//	do something with infolog
	}
	shaderProgram = glCreateProgram();
	glAttachShader(shaderProgram, vertexShader);
	glAttachShader(shaderProgram, fragmentShader);
	glLinkProgram(shaderProgram);
	glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
	if (!success) {
		glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);	//	do something with infolog
	}
	glUseProgram(shaderProgram);
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);





	shadowvertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(shadowvertexShader, 1, &simpleDepthShaderSource, NULL);
	glCompileShader(shadowvertexShader);
	glGetShaderiv(shadowvertexShader, GL_COMPILE_STATUS, &success);	//	fault checking luxury version
	if (!success) {
		glGetShaderInfoLog(shadowvertexShader, 512, NULL, infoLog);	//	do something with infolog
	}
	shadowfragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(shadowfragmentShader, 1, &simpleFragmentShaderSource, NULL);
	glCompileShader(shadowfragmentShader);
	glGetShaderiv(shadowfragmentShader, GL_COMPILE_STATUS, &success);
	if (!success) {
		glGetShaderInfoLog(shadowfragmentShader, 512, NULL, infoLog);	//	do something with infolog
	}
	shadowshaderProgram = glCreateProgram();
	glAttachShader(shadowshaderProgram, shadowvertexShader);
	glAttachShader(shadowshaderProgram, shadowfragmentShader);
	glLinkProgram(shadowshaderProgram);
	glGetProgramiv(shadowshaderProgram, GL_LINK_STATUS, &success);
	if (!success) {
		glGetProgramInfoLog(shadowshaderProgram, 512, NULL, infoLog);	//	do something with infolog
	}
	//glUseProgram(shadowshaderProgram);
	glDeleteShader(shadowvertexShader);
	glDeleteShader(shadowfragmentShader);

glUseProgram(shaderProgram);

	float rvals[] = {
	0.28f, 0.76f, 0.12f, 0.48f, 0.84f, 0.2f, 0.92f, 0.64f, 0.04f, 0.32f,
	0.8f, 0.16f, 0.52f, 0.24f, 0.72f, 0.4f, 0.56f, 0.0f, 0.36f, 0.96f,
	0.08f, 0.68f, 0.88f, 0.44f, 0.6f,
	0.28f, 0.76f, 0.12f, 0.48f, 0.84f, 0.2f, 0.92f, 0.64f, 0.04f, 0.32f,
	0.8f, 0.16f, 0.52f, 0.24f, 0.72f, 0.4f, 0.56f, 0.0f, 0.36f, 0.96f,
	0.08f, 0.68f, 0.88f, 0.44f
	};

	int rvalsLocation = glGetUniformLocation(shaderProgram, "rv");
	glUniform1fv(rvalsLocation, 49, &rvals[0]);

	makeheightfield();

	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);	//	set as current vao

	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);	//	this has to be after vao binding
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_DYNAMIC_DRAW);

	glGenBuffers(1, &ebo);	//	element buffer object
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_DYNAMIC_DRAW);

	//index of uniform/attribute, vecn,,[0,1] normalise flag! :),vecn or 0 to let gl decide, offset
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)0);	// pointer to the vertex data
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)(3 * sizeof(float)));	//	offset for color
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)(6 * sizeof(float)));	//	offset for normal
	glEnableVertexAttribArray(2);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);








	
	glGenFramebuffers(1, &depthMapFBO);
	glGenTextures(1, &depthMap);
	glBindTexture(GL_TEXTURE_2D, depthMap);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT,
		SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	float borderColor[] = { 1.f, 1.f, 1.f, 1.f };
	glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
	
	glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMap, 0);
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);






	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
	//glEnable(GL_CULL_FACE);	//glDisable(GL_CULL_FACE);

	ShowWindow(hOGLWnd, SW_SHOWDEFAULT);	//ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);
	return TRUE;
}


LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_COMMAND:	{
        int wmId = LOWORD(wParam);
        switch (wmId)
        {
        case IDM_ABOUT:
            DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
            break;
        case IDM_EXIT:
            DestroyWindow(hWnd);
            break;
        default:
            return DefWindowProc(hWnd, message, wParam, lParam);
        }
    }	break;
	case WM_KEYDOWN: {
		if ((HIWORD(lParam) & KF_REPEAT) < 1) {
			switch (LOWORD(wParam)) {
			case 'H': {
				makeheightfield();

				//glGenBuffers(1, &vbo);
				glBindBuffer(GL_ARRAY_BUFFER, vbo);	//	this has to be after vao binding
				glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_DYNAMIC_DRAW);

				//glGenBuffers(1, &ebo);	//	element buffer object
				glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
				glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_DYNAMIC_DRAW);


				//index of uniform/attribute, vecn,,[0,1] normalise flag! :),vecn or 0 to let gl decide, offset
				glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)0);	// pointer to the vertex data
				glEnableVertexAttribArray(0);
				glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)(3 * sizeof(float)));	//	offset for color
				glEnableVertexAttribArray(1);
				glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)(6 * sizeof(float)));	//	offset for normal
				glEnableVertexAttribArray(2);

				//glBindBuffer(GL_ARRAY_BUFFER, 0);
				//glBindVertexArray(0);
			}	break;

			case 'Q':		sun[0] -= .1f;	break;
			case 'W':		sun[0] += .1f;	break;
			case 'S':		sun[2] -= .1f;	break;
			case 'X':		sun[2] += .1f;	break;
			case 'D':		sun[1] -= .1f;	break;
			case 'C':		sun[1] += .1f;	break;

			case 'A':		xrv += .005f;	if (xrv > 1.f) xrv = 1.f;	break;
			case 'Z':		xrv -= .005f;	if (xrv < -1.f) xrv = -1.f;	break;
			case VK_LEFT:	yrv += .005f;	if (yrv > 1.f) yrv = 1.f;	break;
			case VK_RIGHT:	yrv -= .005f;	if (yrv < -1.f) yrv = -1.f;	break;
			case VK_UP:		vel += .0001f;	if (vel > 1.f) vel = 1.f;	break;
			case VK_DOWN:	vel -= .0001f;	if (vel < -1.f) vel = -1.f;	break;
			case VK_ESCAPE:	PostMessage(hWnd, WM_CLOSE, 0, 0);	break;
			case VK_SPACE:  break;
			}
		}
	}   break;
	case WM_LBUTTONDOWN: {
		POINT mouloc;	GetCursorPos(&mouloc);	ScreenToClient(hWnd, &mouloc);
	}   break;
	case WM_CREATE: {
		RECT clientRect;	GetClientRect(hOGLWnd, &clientRect);
		dispx = clientRect.right - clientRect.left;	dispy = clientRect.bottom - clientRect.top;
		//glViewport(0, 0, dispx, dispy);
		
		SetTimer(hWnd, 1, 20, NULL);
	}	break;
    case WM_PAINT:	{
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hWnd, &ps);
		display();


		SetBkMode(hdc, TRANSPARENT);
		SetTextColor(hdc, RGB(10, 10, 10));

		char str[32];
		sprintf_s(str, "%.7f", sun[0]);    TextOutA(hdc, 10, 10, str, strlen(str));
		sprintf_s(str, "%.7f", sun[1]);    TextOutA(hdc, 10, 25, str, strlen(str));
		sprintf_s(str, "%.7f", sun[2]);    TextOutA(hdc, 10, 40, str, strlen(str));

        EndPaint(hWnd, &ps);
    }	break;
	case WM_TIMER: {
		InvalidateRgn(hWnd, 0, 0);	UpdateWindow(hWnd);
	}   break;
	case WM_SIZE: {
		RECT clientRect;	GetClientRect(hOGLWnd, &clientRect);
		dispx = clientRect.right - clientRect.left;	dispy = clientRect.bottom - clientRect.top;
		glViewport(0, 0, dispx, dispy);
	}	break;
    case WM_DESTROY:
		glUseProgram(0);
		glDeleteBuffers(1, &vao);
		glDeleteBuffers(1, &vbo);
		if (HDC currentDC = wglGetCurrentDC()) wglMakeCurrent(currentDC, NULL);
		DestroyWindow(hOGLWnd);
		wglDeleteContext(hrc0);
		wglDeleteContext(hrc1);
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
    case WM_INITDIALOG:
        return (INT_PTR)TRUE;

    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
        {
            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
        }
        break;
    }
    return (INT_PTR)FALSE;
}
