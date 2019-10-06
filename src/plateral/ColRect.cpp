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

#include "ColRect.h"

GenoShader2c * ColRect::shader = 0;

ColRect::ColRect(GenoCamera2D * camera, const GenoVector2f & position, const GenoVector2f & dimensions, const GenoVector4f & color, bool gui) :
	Collidable(camera, position, dimensions, { 0.0f , 0.0f }),
	color(color),
	gui(gui) {
	if (shader == 0)
		shader = new GenoShader2c();
}

void ColRect::render() {
	shader->enable();
	shader->setColor(color);
	if (gui)
		shader->setMvp(translate2D(camera->getProjection(), position).scale2D(dimensions));
	else
		shader->setMvp(translate2D(camera->getVPMatrix(), position).scale2D(dimensions));
	vao->render();
}

ColRect::~ColRect() {}