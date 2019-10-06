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
#include "../geno/thread/GenoTime.h"
#include "../geno/engine/GenoEngine.h"
#include "Map.h"

#include "Platform.h"

GenoShader2c * Platform::shader = 0;

Platform::Platform(GenoCamera2D * camera, const GenoVector2f & position, const GenoVector2f & dimensions) :
	Collidable(camera, position, dimensions, { 0.0f, 0.0f }),
	color(1, 1, 1, 1),
	state(STATE_STATIC),
	scale(0),
	time(0) {
	if (shader == 0)
		shader = new GenoShader2c();
}

Platform::Platform(GenoCamera2D * camera, const GenoVector2f & position, const GenoVector2f & dimensions, const GenoVector2f & velocity) :
	Collidable(camera, position, dimensions, velocity),
	color(1, 1, 1, 1),
	state(STATE_MOBILE),
	scale(0),
	time(0) {
	if (shader == 0)
		shader = new GenoShader2c();
}

void Platform::update() {
	if (state == STATE_MOBILE) {
		float delta = GenoEngine::getLoop()->getDelta();
		velocity.y() += Map::GRAVITY * delta;
		position += velocity * delta;
	}
	else if (state == STATE_EXPLODING) {
		constexpr float COMPLETE = 0.25f;
		constexpr float SCALE = 2;
		time += GenoEngine::getLoop()->getDelta();
		if (time > COMPLETE) {
			state = STATE_COMPLETE;
			time = COMPLETE;
		}
		color.w() = (sin(time * 8 * 3.1415926535) * 0.5 + 0.5) * (1 - time / COMPLETE);
		scale = (-1.0 / (2.1 * (time / COMPLETE) + 0.4) + 2.5) / 2.1;
	}
}

void Platform::render() {
	shader->enable();
	shader->setColor(color);
	shader->setMvp(translate2D(camera->getVPMatrix(), position - GenoVector2f{ scale, scale }).scale2D(dimensions + GenoVector2f{ scale, scale } * 2.0f));
	vao->render();
	shader->setColor({ 0, 0, 0, 1 });
	shader->setMvp(translate2D(camera->getVPMatrix(), position + GenoVector2f{ 0.0625f, 0.0625f } - GenoVector2f{ scale, scale }).scale2D(dimensions - (GenoVector2f{ 0.0625f, 0.0625f } - GenoVector2f{ scale, scale }) * 2.0f));
	vao->render();
}

Platform & Platform::finalize() {
	state = STATE_STATIC;
	velocity = { 0.0f, 0.0f };
	return *this;
}

Platform & Platform::detonate() {
	state = STATE_EXPLODING;
	velocity = { 0.0f, 0.0f };
	return *this;
}

GenoVector4f hsvToRgb(int H, double S, double V) {
	double C = S * V;
	double X = C * (1 - abs(fmod(H / 60.0, 2) - 1));
	double m = V - C;
	double Rs, Gs, Bs;

	if(H >= 0 && H < 60) {
		Rs = C;
		Gs = X;
		Bs = 0;	
	}
	else if(H >= 60 && H < 120) {	
		Rs = X;
		Gs = C;
		Bs = 0;	
	}
	else if(H >= 120 && H < 180) {
		Rs = 0;
		Gs = C;
		Bs = X;	
	}
	else if(H >= 180 && H < 240) {
		Rs = 0;
		Gs = X;
		Bs = C;	
	}
	else if(H >= 240 && H < 300) {
		Rs = X;
		Gs = 0;
		Bs = C;	
	}
	else {
		Rs = C;
		Gs = 0;
		Bs = X;	
	}
	
	return {
		(float) (Rs + m),
		(float) (Gs + m),
		(float) (Bs + m),
		1
	};
}

void Platform::reset(const GenoVector2f & position, const GenoVector2f & velocity) {
	constexpr uint32 RANGE = 10;
	state = STATE_MOBILE;
	color = hsvToRgb(rand() % 361, 1, 1);
	this->position = position;
	this->velocity = velocity;
}

bool Platform::isComplete() {
	return state == STATE_COMPLETE;
}

Platform::~Platform() {}