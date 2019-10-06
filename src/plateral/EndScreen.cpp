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

#include <iostream>

#include "../geno/GenoMacros.h"
#include "../geno/engine/GenoEngine.h"
#include "../geno/engine/GenoMonitor.h"

#include "EndScreen.h"

GenoShader2t  * EndScreen::shader     = 0;
GenoTexture2D * EndScreen::endScreen1 = 0;
GenoTexture2D * EndScreen::endScreen2 = 0;

EndScreen::EndScreen(GenoCamera2D * camera) :
	Collidable(camera, { 0.0f, 0 }, camera->getDimensions(), { 0.0f, 0 }),
	overlay(camera, { 0.0f, 0.0f }, camera->getDimensions(), { 0, 0, 0, 1 }, true),
	two(false),
	complete(false),
	time(0) {

	if (shader == 0) {
		shader = new GenoShader2t();

		uint32 textureParams[] = {
			GL_TEXTURE_MIN_FILTER, GL_LINEAR,
			GL_TEXTURE_MAG_FILTER, GL_LINEAR,
			GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE,
			GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE
		};
	
		float width = GenoMonitors::getPrimaryMonitor()->getDefaultVideoMode()->getWidth();

		const char * path1 = 0;
		if (width >= (2560 + 3840) * 0.5f)
			path1 = "res/img/EndScreen4k.png";
		else if (width >= (1080 + 2560) * 0.5f)
			path1 = "res/img/EndScreen1440p.png";
		else
			path1 = "res/img/EndScreen1080p.png";

		const char * path2 = 0;
		if (width >= (2560 + 3840) * 0.5f)
			path2 = "res/img/EndScreen24k.png";
		else if (width >= (1080 + 2560) * 0.5f)
			path2 = "res/img/EndScreen21440p.png";
		else
			path2 = "res/img/EndScreen21080p.png";

		GenoTexture2DCreateInfo textureInfo = {};
		textureInfo.type        = GENO_TEXTURE2D_TYPE_PNG;
		textureInfo.numParams   = GENO_ARRAY_SIZE(textureParams) / 2;
		textureInfo.params      = textureParams;
		textureInfo.texture     = path1;

		endScreen1 = GenoTexture2D::create(textureInfo);

		textureInfo.texture = path2;

		endScreen2 = GenoTexture2D::create(textureInfo);
	}
}

void EndScreen::render() {
	time += GenoEngine::getLoop()->getDelta();
	if (time < 0.5)
		overlay.color.w() = 1 - time * 2;
	else if (time >= 2.5 && time < 3)
		overlay.color.w() = (time - 2.5) * 2;
	else if (time >= 3 && time < 3.5) {
		two = true;
		overlay.color.w() = 1 - (time - 3) * 2;
	}
	else if (time >= 5.5 && time < 6)
		overlay.color.w() = (time - 5.5) * 2;
	else if (time >= 6)
		complete = true;

	if (two)
		endScreen2->bind();
	else
		endScreen1->bind();
	shader->enable();
	shader->setMvp(scale2D(camera->getProjection(), camera->getDimensions()));
	vao->render();
	overlay.render();
}

bool EndScreen::done() {
	return complete;
}

EndScreen::~EndScreen() {}