#include "stdafx.h"

GLuint Hitable::drawID = 0;
GLuint Hitable::bufferBaseVertex = 0;
GLuint Hitable::bufferBaseIndex = 0;
std::vector<Hitable::BindlessTexture> Hitable::_allBindlessTextures;