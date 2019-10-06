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
#include <string>
#include <sstream>

#include "Scene.h"

extern uint32 readUInt(std::istream & stream);

Scene::Scene(GenoCamera2D * camera) :
	camera(camera),
	curLevel(0) {
	std::ifstream data("res/levels/count.txt");
	numLevels = readUInt(data);
	levels = new std::string[numLevels];
	for (uint32 i = 0; i < numLevels; ++i) {
		std::stringstream stream;
		stream << "res/levels/level" << i << ".txt";
		levels[i] = stream.str();
	}
	map = new Map(camera, levels[curLevel].c_str());
}

void Scene::update() {
	if (endScreen == 0) {
		map->update();
		if (map->getState() > 0) {
			if (map->getState() == 2)
				++curLevel;
			if (curLevel == numLevels)
				endScreen = new EndScreen(camera);
			else {
				delete map;
				map = new Map(camera, levels[curLevel].c_str());
			}
		}
	}
	else {
		if (endScreen->done()) {
			delete endScreen;
			endScreen = 0;
			delete map;
			curLevel = 0;
			map = new Map(camera, levels[curLevel].c_str());
		}
	}
}

void Scene::render() {
	if (curLevel < numLevels)
		map->render();
	else
		endScreen->render();
}

Scene::~Scene() {
	delete [] levels;
	delete map;
}