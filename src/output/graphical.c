#include "graphical.h"
#include "../config.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

void calculate_win_pos(int *winX, int *winY, int winW, int winH, int scrW, int scrH, char *winPos) {
	if(!strcmp(winPos, "top")){
		(*winX) = (scrW - winW) / 2 + (*winX);
		(*winY) -= p.shdw;
	}else if(!strcmp(winPos, "bottom")){
		(*winX) = (scrW - winW) / 2 + (*winX);
		(*winY) = (scrH - winH) - (*winY) + p.shdw;
	}else if(!strcmp(winPos, "top_left")){
		(*winY) -= p.shdw;
	}else if(!strcmp(winPos, "top_right")){
		(*winX) = (scrW - winW) - (*winX);
		(*winY) -= p.shdw;
	}else if(!strcmp(winPos, "left")){
		(*winY) = (scrH - winH) / 2;
	}else if(!strcmp(winPos, "right")){
		(*winX) = (scrW - winW) - (*winX);
		(*winY) = (scrH - winH) / 2 + (*winY);
	}else if(!strcmp(winPos, "bottom_left")){
		(*winY) = (scrH - winH) - (*winY) + p.shdw;
	}else if(!strcmp(winPos, "bottom_right")){
		(*winX) = (scrW - winW) - (*winX);
		(*winY) = (scrH - winH) - (*winY) + p.shdw;
	}else if(!strcmp(winPos, "center")){
		(*winX) = (scrW - winW) / 2 + (*winX);
		(*winY) = (scrH - winH) / 2 + (*winY);
	}
	// Some error checking
	#ifdef DEBUG
		if(winX > scrW - winW) printf("Warning: Screen out of bounds (X axis)!");
		if(winY > scrH - winH) printf("Warning: Screen out of bounds (Y axis)!");
	#endif
}

#ifdef GL
unsigned int VBObuffer;
float glBars[1600]; // 2*4*200

static unsigned int CompileShader(unsigned int type, char *source) {
	unsigned int id = glCreateShader(type);
	glShaderSource(id, 1, &source, NULL);
	glCompileShader(id);

	int result;
	glGetShaderiv(id, GL_COMPILE_STATUS, &result);
	if(result == GL_FALSE) {
		int length;
		glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length);
		char *message = (char*)alloca(length*sizeof(char));
		glGetShaderInfoLog(id, length, &length, message);
		fprintf(stderr, "Failed to compile %s shader: %s\n", type==GL_VERTEX_SHADER? "vertex" : "fragment", message);

		glDeleteShader(id);
		return 0;
	}

	return id;
}

static unsigned int CreateShader(const char* vertexShader,
								 const char* fragmentShader) {
	unsigned int program = glCreateProgram();
	unsigned int vs = CompileShader(GL_VERTEX_SHADER, vertexShader);
	unsigned int fs = CompileShader(GL_FRAGMENT_SHADER, fragmentShader);

	glAttachShader(program, vs);
	glAttachShader(program, fs);
	glLinkProgram(program);
	glValidateProgram(program);

	glDeleteShader(vs);
	glDeleteShader(fs);

	return program;
}

void VBOGLsetup() {
	glewInit();

	for(int i=0; i<200; i++) glBars[i] = 0.0;
	glGenBuffers(1, &VBObuffer);
	glBindBuffer(GL_ARRAY_BUFFER, VBObuffer);
	glBufferData(GL_ARRAY_BUFFER, 1600*sizeof(float), glBars, GL_DYNAMIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(float)*2, 0);

	char *vertex = "void main(){ \n"
				    "    gl_Position = gl_ProjectionMatrix * gl_ModelViewMatrix * gl_Vertex;\n"
	               "}\n";
	char *fragment = "void main(){\n"
			         "   gl_FragColor = vec4(1.0, 1.0, 1.0, 1.0);\n"
			         "}\n";

	unsigned int shader = CreateShader(vertex, fragment);
	glUseProgram(shader);
}

void VBOGLdestroy() {
	//free(glBars);
}

int drawGLBars(int rest, int bars, double colors[8], double gradColors[24], int *f) {
	for(int i = 0; i < bars; i++) {
		glBars[i*8]   = rest+(p.bw+p.bs)*i;
		glBars[i*8+6] = glBars[i*8];
		glBars[i*8+2] = rest+(p.bw+p.bs)*i+p.bw;
		glBars[i*8+4] = glBars[i*8+2];
		glBars[i*8+1] = (unsigned int)f[i]+p.shdw> (unsigned int)p.h-p.shdw ? (unsigned int)p.h-p.shdw : (unsigned int)f[i]+p.shdw;
		glBars[i*8+3] = glBars[i*8+1];
		glBars[i*8+5] = p.shdw;
		glBars[i*8+7] = glBars[i*8+5];
	}
	glBufferSubData(GL_ARRAY_BUFFER, 0, 8*bars*sizeof(float), glBars);
	glDrawArrays(GL_QUADS, 0, 4*bars);
	return 0;
}
#endif
