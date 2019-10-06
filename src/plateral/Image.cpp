/*******************************************************************************
 *
 * Copyright (c) 2019 Gnarly Narwhal
 *
 * -----------------------------------------------------------------------------
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files(the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *
 *******************************************************************************/

#include "../geno/GenoMacros.h"
#include "../geno/math/linear/GenoMatrix4.h"

#include "Image.h"

GenoShader2t * Image::shader;
GenoVao * Image::vao;

void Image::init() {
	shader = new GenoShader2t();

	float vertices[] = {
		1, 0, 0, // Top left
		1, 1, 0, // Bottom left
		0, 1, 0, // Bottom right
		0, 0, 0  // Top right
	};
	uint32 indices[] = {
		0, 1, 3,
		1, 2, 3
	};
	float texCoords[] = {
		1, 0,
		1, 1,
		0, 1,
		0, 0
	};
	vao = new GenoVao(4, vertices, 6, indices);
	vao->addAttrib(4, 2, texCoords);
}

Image::Image(GenoCamera2D * camera, const GenoVector2f & position, const GenoVector2f & dimensions, const char * path) :
	camera(camera),
	position(position),
	dimensions(dimensions) {

	uint32 textureParams[] = {
		GL_TEXTURE_MIN_FILTER, GL_LINEAR,
		GL_TEXTURE_MAG_FILTER, GL_LINEAR,
		GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE,
		GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE
	};

	GenoTexture2DCreateInfo textureInfo = {};
	textureInfo.type      = GENO_TEXTURE2D_TYPE_PNG;
	textureInfo.texture   = path;
	textureInfo.numParams = GENO_ARRAY_SIZE(textureParams) / 2;
	textureInfo.params    = textureParams;

	texture = GenoTexture2D::create(textureInfo);
}

GenoTexture2D * Image::getTexture() {
	return texture;
}

void Image::render() {
	texture->bind();
	shader->enable();
	shader->setMvp(translate2D(camera->getVPMatrix(), position).scale2D(dimensions));
	vao->render();
}

Image::~Image() {}