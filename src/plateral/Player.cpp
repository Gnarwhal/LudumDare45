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

#include "../geno/engine/GenoMonitor.h"
#include "../geno/engine/GenoEngine.h"
#include "../geno/GenoMacros.h"
#include "../geno/engine/GenoInput.h"
#include "Map.h"

#include "Player.h"

GenoShader2ss   * Player::shader  = 0;
GenoSpritesheet * Player::texture = 0;

Player::Player(GenoCamera2D * camera) :
	Collidable(camera, { 0.0f, 0.0f }, { 2.0f, 2.0f }, { 0.0f, 0.0f }),
	hitboxScale(0.5f, 1) {
	if (shader == 0) {
		shader = new GenoShader2ss();

		uint32 textureParams[] = {
			GL_TEXTURE_MIN_FILTER, GL_LINEAR,
			GL_TEXTURE_MAG_FILTER, GL_LINEAR,
			GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE,
			GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE
		};

		const char * path = 0;
		float width = GenoMonitors::getPrimaryMonitor()->getDefaultVideoMode()->getWidth();
		if (width >= (2560 + 3840) * 0.5f)
			path = "res/img/Player4k.png";
		else if (width >= (1080 + 2560) * 0.5f)
			path = "res/img/Player1440p.png";
		else
			path = "res/img/Player1080p.png";

		GenoSpritesheetCreateInfo textureInfo = {};
		textureInfo.type        = GENO_SPRITESHEET_TYPE_PNG;
		textureInfo.numParams   = GENO_ARRAY_SIZE(textureParams) / 2;
		textureInfo.params      = textureParams;
		textureInfo.texture     = path;
		textureInfo.numSpritesX = 8;
		textureInfo.numSpritesY = 4;
		textureInfo.addPadding  = true;

		texture = GenoSpritesheet::create(textureInfo);
	}
}

constexpr float WALK_SPEED = 5;
constexpr float RUN_SPEED = 8;
constexpr float MAX_SPEED = 30;
constexpr float JUMP_SPEED = 12;
constexpr float DECELERATION = 20;
constexpr float HIGH_SPEED_DECELERATION = 10;

void Player::update() {

	float delta = (float) GenoEngine::getLoop()->getDelta();

	float friction = 0;
	float floor = 0;
	float speed = (GenoInput::getKeyState(GLFW_KEY_LEFT_SHIFT) >= GENO_INPUT_PRESSED && grounded) ? RUN_SPEED : WALK_SPEED;
	GenoVector2f acceleration = { 0, Map::GRAVITY };

	if (GenoInput::getKeyState(GLFW_KEY_A) >= GENO_INPUT_PRESSED) {
		floor = -speed;
		if (velocity.x() > -speed)
			velocity.x() = -speed;
		else
			friction = HIGH_SPEED_DECELERATION;

	}
	else if (GenoInput::getKeyState(GLFW_KEY_D) >= GENO_INPUT_PRESSED) {
		floor = speed;
		if (velocity.x() < speed)
			velocity.x() = speed;
		else
			friction = -HIGH_SPEED_DECELERATION;
	}
	else if (velocity.x() != 0) {
		float absVelX = fabs(velocity.x());
		if (absVelX > speed)
			friction = -HIGH_SPEED_DECELERATION * (velocity.x() / absVelX);
		else
			friction = -DECELERATION * (velocity.x() / absVelX);
	}

	float absVelocityY = fabs(velocity.y());
	if (absVelocityY > MAX_SPEED)
		velocity.y() = (velocity.y() / absVelocityY) * MAX_SPEED;

	if ((GenoInput::getKeyState(GLFW_KEY_W)     >= GENO_INPUT_PRESSED
	  || GenoInput::getKeyState(GLFW_KEY_SPACE) >= GENO_INPUT_PRESSED) && grounded)
		velocity.y() = -JUMP_SPEED;
	
	grounded = false;
	velocity += acceleration * delta;
	float degradedVelocity = velocity.x() + friction * delta;
	// If the acceleration due to friction would accelerate it past the speed floor set it to the speed floor
	if ((degradedVelocity - floor) * (velocity.x() - floor) < 0)
		velocity.x() = floor;
	// otherwise just apply the friction
	else
		velocity.x() = degradedVelocity;
	
	position += velocity * delta;

	// Set the direction of the player
	if (velocity.x() < 0)
		direction = 0;
	else if (velocity.x() > 0)
		direction = 1;
}

void Player::render() {
	float delta = GenoEngine::getLoop()->getDelta();
	// Jumping Animation Control
	if (!grounded) {
		state.y() = 2;
		if (velocity.y() < -1)
			state.x() = 0;
		else if (velocity.y() > 1)
			state.x() = 2;
		else
			state.x() = 1;
	}
	// Idle Animation Control
	else if (velocity.x() == 0) {
		state.y() = 3;
		if (lastState.y() != 3) {
			time = 0;
			state.x() = 0;
		}
		else
			time += delta;
		while (time >= IDLE_TIMES[state.x()]) {
			time -= IDLE_TIMES[state.x()];
			state.x() = (state.x() + 1) % 1;
		}
	}
	// Walk Animation Control
	else if (fabs(velocity.x()) < WALK_SPEED) {
		state.y() = 1;
		if (lastState.y() != 1) {
			time = 0;
			state.x() = 0;
		}
		else
			time += delta;
		while (time >= WALK_TIME) {
			time -= WALK_TIME;
			state.x() = (state.x() + 1) % 6;
		}
	}
	// Run Animation Control
	else {
		state.y() = 1;
		if (lastState.y() != 1) {
			time = 0;
			state.x() = 0;
		}
		else
			time += delta;
		while (time >= RUN_TIME) {
			time -= RUN_TIME;
			state.x() = (state.x() + 1) % 6;
		}
	}
	lastState = state;

	texture->bind();
	shader->enable();
	shader->setTextureTransform(texture->getTransform(state));
	shader->setMvp(translate2D(camera->getVPMatrix(), position + scaleXY(dimensions, 1 - direction, 0.0f)).scale2D(scaleX(dimensions, direction * 2 - 1)));
	vao->render();
}

void Player::ground() {
	grounded = true;
}

GenoVector2f Player::getCollisionPosition() {
	return position + (dimensions * (GenoVector2f{ 1, 1 } - hitboxScale)) * 0.5f;
}

GenoVector2f Player::getCollisionDimensions() {
	return dimensions * hitboxScale;
}


Player::~Player() {}