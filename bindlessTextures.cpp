#pragma once
#include "stdafx.h"

/* BINDLESS TEXTURES

Instead of binding and unbinding textures at runtime, register all textures to the GPU
so they can be accessed quickly without binding.
*/

// pintable.cpp - line 2380 'play()' function where textures may be easily accessed for being made resident

// setup data structure for making textures resident
// create data structure (fixed size?)
struct
{
	GLuint      transformBuffer;
	GLuint      textureHandleBuffer;
} buffers;

enum
{
    NUM_TEXTURES = 384,
    TEXTURE_LEVELS = 5,
    TEXTURE_SIZE = (1 << (TEXTURE_LEVELS - 1))
};

GLuint      program;

struct
{
    GLint   mv_matrix;
    GLint   vp_matrix;
} uniforms;

struct
{
    GLuint      name;
    GLuint64    handle;
} textures[1024];

struct MATRICES
{
	vmath::mat4     view;
	vmath::mat4     projection;
	vmath::mat4     model[NUM_TEXTURES];
};

// this should exist just before runtime starts
glGenBuffers(1, &buffers.transformBuffer);
glBindBuffer(GL_UNIFORM_BUFFER, buffers.transformBuffer);

glBufferStorage(GL_UNIFORM_BUFFER,
	sizeof(MATRICES),
	nullptr,
	GL_MAP_WRITE_BIT);    

glGenBuffers(1, &buffers.textureHandleBuffer);
glBindBuffer(GL_UNIFORM_BUFFER, buffers.textureHandleBuffer);
glBufferStorage(GL_UNIFORM_BUFFER,
				NUM_TEXTURES * sizeof(GLuint64) * 2,	// why *2?
				nullptr,
				GL_MAP_WRITE_BIT); // |

GLuint64* pHandles =
(GLuint64*)glMapBufferRange(GL_UNIFORM_BUFFER,
							0,
							NUM_TEXTURES * sizeof(GLuint64) * 2,
							GL_MAP_WRITE_BIT |
							GL_MAP_INVALIDATE_BUFFER_BIT);

// for all textures

{
	glGenTextures(1, &textures[i].name)
}






// function for making each texture resident
void makeTexResident() {
	//create texture handle


	//add handle to data structure
	
	
	return;
}




// basicShader.glfx CHANGES
//
// add bindless texture extension (OpenGL 4.0+)
// #extension GL_ARB_bindless_texture : require
//
// add uniform block of texture samplers to be accessed in fragment shader stage
// in vertex shader sstage there will need to be the corresponding transform matrices (so another uniform block, same size)
// these will both be accessed through an invocationID (or similar?)



// multiDraw changes
//
// + input struct for all input vars to be accessed through buffer

