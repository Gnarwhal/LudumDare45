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
#include <fstream>

#include "../geno/math/linear/GenoVector2.h"
#include "../geno/engine/GenoEngine.h"
#include "../geno/engine/GenoInput.h"

#include "Map.h"

uint32 readUInt(std::istream & stream) {
	uint32 ret = 0;
	char c;
	while ((c = stream.get()) >= '0' && c <= '9') {
		ret *= 10;
		ret += c - '0';
	}
	return ret;
}

float readFloat(std::istream & stream) {
	float sign = 1;
	float ret = 0;
	char c = stream.get();
	if (c == '-') {
		sign = -1;
		c = stream.get();
	}
	while (c >= '0' && c <= '9') {
		ret *= 10;
		ret += c - '0';
		c = stream.get();
	}
	if (c == '.') {
		c = stream.get();
		float fract = 1;
		while (c >= '0' && c <= '9')
			ret += c * (fract *= 0.1);
	}
	return sign * ret;
}

Map::Map(GenoCamera2D * camera, const char * path) :
	camera(camera),
	thrown({
		true,
		Platform(camera, { 0.0f, 0.0f }, { 1, 1 })
	}),
	goal(camera, { 0.0f, 0.0f}),
	overlay(camera, { 0.0f, 0.0f }, camera->getDimensions(), {0, 0, 0, 1}, true),
	time(0),
	goalOverlay(camera, { 0.0f, 0.0f }, { 1, 2 }, {0, 0, 0, 0}, false) {
	player = new Player(camera);
	// ------------ LOADING ------------
	std::ifstream level(path);
	numBlocks = readUInt(level);
	killFloor = readFloat(level);
	player->position = { readFloat(level), readFloat(level) };
	goal.position = { readFloat(level), readFloat(level) };
	goalOverlay.position = goal.position;
	uint32 numPlatforms = readUInt(level);
	for (uint32 i = 0; i < numPlatforms; ++i) {
		GenoVector2f position   = GenoVector2f{ readFloat(level), readFloat(level) };
		GenoVector2f dimensions = GenoVector2f{ readFloat(level), readFloat(level) };
		constant.emplace_back(camera, position, dimensions);
	}
	// ----------------------------------
	camera->position = player->position + (player->dimensions - camera->getDimensions()) * 0.5f;
}

float absMin(float a, float b) {
	if (a * a < b * b)
		return a;
	else
		return b;
}

bool Map::checkPlayerCollision(const Collidable & platform, bool push) {
	GenoVector2f playerPosition   = player->getCollisionPosition();
	GenoVector2f playerDimensions = player->getCollisionDimensions();
	// AABB Collision
	if (playerPosition.x() < platform.position.x() + platform.dimensions.x() && playerPosition.x() + playerDimensions.x() > platform.position.x()
	 && playerPosition.y() < platform.position.y() + platform.dimensions.y() && playerPosition.y() + playerDimensions.y() > platform.position.y()) {
		if (push) {
			// Move the smallest amount needed to leave the platform
			float xPush = absMin(
				platform.position.x() - (playerPosition.x() + playerDimensions.x()),
				platform.position.x() + platform.dimensions.x() - playerPosition.x()
			);
			float yPush = absMin(
				platform.position.y() - (playerPosition.y() + playerDimensions.y()),
				platform.position.y() + platform.dimensions.y() - playerPosition.y()
			);
			if (fabs(xPush) < fabs(yPush)) {
				player->position.x() += xPush;
				player->velocity.x() = 0;
			}
			else {
				if (yPush < 0)
					player->ground();
				player->position.y() += yPush;
				player->velocity.y() = 0;
			}
		}
		return true;
	}
	return false;
}

void Map::checkThrownCollision(const Collidable & platform) {
	// AABB Collision
	if (thrown.data.position.x() < platform.position.x() + platform.dimensions.x() && thrown.data.position.x() + thrown.data.dimensions.x() > platform.position.x()
	 && thrown.data.position.y() < platform.position.y() + platform.dimensions.y() && thrown.data.position.y() + thrown.data.dimensions.y() > platform.position.y()) {
		// Move the smallest amount needed to leave the platform
		float xPush = absMin(
			platform.position.x() - (thrown.data.position.x() + thrown.data.dimensions.x()),
			platform.position.x() + platform.dimensions.x() - thrown.data.position.x()
		);
		float yPush = absMin(
			platform.position.y() - (thrown.data.position.y() + thrown.data.dimensions.y()),
			platform.position.y() + platform.dimensions.y() - thrown.data.position.y()
		);
		if (fabs(xPush) < fabs(yPush)) {
			thrown.data.position.x() += xPush;
			thrown.data.velocity.x() = 0;
		}
		else {
			thrown.data.position.y() += yPush;
			thrown.data.velocity.y() = 0;
		}
	}
}

constexpr float OPEN_TIME = 0.5;
constexpr float ENTER_TIME = 1;
constexpr float CLOSE_TIME = 1.5;

void Map::update() {
	constexpr float FADE_TIME = 0.5;
	float delta = GenoEngine::getLoop()->getDelta();
	if (substate == 0) {
		GenoInput::setEnabled(false);
		time += delta;
		overlay.color.w() = 1 - fmin(time / FADE_TIME, 1);
		if (time > FADE_TIME) {
			substate = 1;
			GenoInput::setEnabled(true);
		}
	}
	else if (substate == 3) {
		GenoInput::setEnabled(false);
		time += delta;
		overlay.color.w() = fmin(time / FADE_TIME, 1);
		if (time > FADE_TIME)
			substate = 4;
	}
	if (substate != 2) {
		/////////////////////////////////////////////////////////
		/////////////////////////////////////////////////////////
		GenoVector2f oldPosition = player->position;
	
		player->update();

		// Player Collision
		for (auto & platform : constant)
			checkPlayerCollision(platform, true);
		for (auto & platform : spawned)
			checkPlayerCollision(platform, true);

		if (!thrown.null) {
			thrown.data.update();
			// Thrown Collision
			for (auto & platform : constant)
				checkThrownCollision(platform);
			for (auto & platform : spawned)
				checkThrownCollision(platform);
			if (GenoInput::getMouseButtonState(GLFW_MOUSE_BUTTON_1) == GENO_INPUT_PRESSED) {
				spawned.push_back(thrown.data.finalize());
				thrown.null = true;
			}
		}
		else if (GenoInput::getMouseButtonState(GLFW_MOUSE_BUTTON_1) == GENO_INPUT_PRESSED) {
			constexpr float MIN_LAUNCH_SPEED = 5;
			constexpr float MAX_LAUNCH_SPEED = 15;

			constexpr float MAX_MOUSE_DIST = 5;

			GenoVector2f launchVelocity = GenoInput::getMouseCoords(camera) - (player->position + player->dimensions * 0.5f);
			float length = launchVelocity.getLength();
			if (length > 0)
				launchVelocity.setLength(MAX_LAUNCH_SPEED); // MIN_LAUNCH_SPEED + (MAX_LAUNCH_SPEED - MIN_LAUNCH_SPEED) * min(1, length / MAX_MOUSE_DIST));
			launchVelocity += player->velocity;

			thrown.null = false;
			thrown.data.reset(player->position + (player->dimensions - thrown.data.dimensions) * 0.5f, launchVelocity);
		}
		if (GenoInput::getMouseButtonState(GLFW_MOUSE_BUTTON_2) == GENO_INPUT_PRESSED) {
			constexpr float MAX_FORCE = 20;
			constexpr float MAX_DISTANCE = 4;
			
			GenoVector2f mouse = GenoInput::getMouseCoords(camera);
			for (uint32 i = 0; i < spawned.size(); ++i) {
				auto & platform = spawned[i];
				if (mouse.x() > platform.position.x() && mouse.x() < platform.position.x() + platform.dimensions.x()
					&& mouse.y() > platform.position.y() && mouse.y() < platform.position.y() + platform.dimensions.y()) {
					GenoVector2f direction = (player->position - platform.position + (player->dimensions - platform.dimensions) * 0.5f);
					float speed = (fmax(0, MAX_DISTANCE - direction.getLength()) / MAX_DISTANCE) * MAX_FORCE;
					player->velocity += direction.setLength(speed);

					if (!thrown.null) {
						GenoVector2f direction = (thrown.data.position - platform.position + (thrown.data.dimensions - platform.dimensions) * 0.5f);
						float speed = (fmax(0, MAX_DISTANCE - direction.getLength()) / MAX_DISTANCE) * MAX_FORCE;
						thrown.data.velocity += direction.setLength(speed);
					}

					detonations.push_back(platform.detonate());
					spawned.erase(spawned.begin() + i);

					break;
				}
			}
			if (mouse.x() > thrown.data.position.x() && mouse.x() < thrown.data.position.x() + thrown.data.dimensions.x()
				&& mouse.y() > thrown.data.position.y() && mouse.y() < thrown.data.position.y() + thrown.data.dimensions.y() && !thrown.null) {
				GenoVector2f direction = (player->position - thrown.data.position + (player->dimensions - thrown.data.dimensions) * 0.5f);
				float speed = (fmax(0, MAX_DISTANCE - direction.getLength()) / MAX_DISTANCE) * MAX_FORCE;
				player->velocity += direction.setLength(speed);

				detonations.push_back(thrown.data.detonate());
				thrown.null = true;
			}
		}

		for (uint32 i = 0; i < detonations.size(); ++i) {
			detonations[i].update();
			if (detonations[i].isComplete()) {
				detonations.erase(detonations.begin() + i);
				--i;
			}
		}

		GenoVector2f bounds[] = {
			camera->position + camera->getDimensions() * 0.3f,
			camera->position + camera->getDimensions() * 0.4f,
			camera->position + camera->getDimensions() * 0.6f,
			camera->position + camera->getDimensions() * 0.7f,
		};
		// camera - x
		if (player->position.x() < bounds[0].x())
			camera->position.x() += player->position.x() - bounds[0].x();
		else if (player->position.x() > bounds[3].x())
			camera->position.x() += player->position.x() - bounds[3].x();
		// camera - y
		if (player->position.y() < bounds[0].y())
			camera->position.y() += player->position.y() - bounds[0].y();
		else if (player->position.y() > bounds[3].y())
			camera->position.y() += player->position.y() - bounds[3].y();

		if (camera->position.y() > killFloor - camera->getDimensions().y() * 0.7f)
			camera->position.y() = killFloor - camera->getDimensions().y() * 0.7f;

		if (player->position.y() > killFloor && substate == 1) {
			time = 0;
			state = 1;
			substate = 3;
		}
		else if (substate == 1) {
			if (checkPlayerCollision(goal, false) && fabs(player->position.y() - goal.position.y()) < 0.0001) {
				state = 2;
				substate = 2;
				time = 0;
				startX = player->position.x();
				saveDims = goal.dimensions.x();
			}
		}
		/////////////////////////////////////////////////////////
		/////////////////////////////////////////////////////////
	}
	else {
		constexpr float CLOSE_AMOUNT = 0.9;

		time += delta;

		player->position.y() = goal.position.y();
		if (time < OPEN_TIME) {
			player->position.x() = (goal.position.x() + (saveDims - player->dimensions.x()) * 0.5f - startX) * (time / OPEN_TIME) + startX;
			goal.dimensions.x() = 1 - (time / OPEN_TIME) * CLOSE_AMOUNT;
		}
		else if (time < ENTER_TIME) {
			constexpr float ENTER_LTIME = ENTER_TIME - OPEN_TIME;
			float ltime = time - OPEN_TIME;
			goalOverlay.color.w() = (ltime / ENTER_LTIME);
		}
		else if (time < CLOSE_TIME) {
			constexpr float CLOSE_LTIME = CLOSE_TIME - ENTER_TIME;
			float ltime = time - ENTER_TIME;
			goal.dimensions.x() = (ltime / CLOSE_LTIME) * CLOSE_AMOUNT;
		}
		else if (time < FADE_TIME + CLOSE_TIME)
			overlay.color.w() = (time - FADE_TIME) / CLOSE_TIME;
		else
			substate = 4;
	}
}

void Map::render() {
	if (substate == 2) {
		if (time < OPEN_TIME) {
			goal.render();
			player->render();
		}
		else {
			player->render();
			goalOverlay.render();
			goal.render();
		}
	}
	else {
		goal.render();
		player->render();
	}
	for (auto & platform : constant)
		platform.render();
	for (auto & platform : spawned)
		platform.render();
	for (auto & detonated : detonations)
		detonated.render();
	if (!thrown.null)
		thrown.data.render();
	if (substate != 1)
		overlay.render();
}

uint32 Map::getState() {
	return state * (substate == 4);
}

Map::~Map() {
	delete player;
}
