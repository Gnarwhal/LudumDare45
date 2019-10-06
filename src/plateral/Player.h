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

#ifndef GNARLY_PLATERAL_PLAYER
#define GNARLY_PLATERAL_PLAYER

#include "../geno/shaders/GenoShader2ss.h"
#include "../geno/gl/GenoSpritesheet.h"

#include "Collidable.h"

class Player : public Collidable {
	private:
		static GenoShader2ss   * shader;
		static GenoSpritesheet * texture;

		bool grounded;
		GenoVector2f hitboxScale;
		GenoVector2i lastState;
		GenoVector2i state;
		float direction = 1;

		float time;

		constexpr static float IDLE_TIMES[] = {
			1 / 2.0f,
			1 / 10.0f,
			1 / 10.0f,
			1 / 10.0f
		};

		constexpr static float WALK_TIME = 1 / 12.0f;
		constexpr static float RUN_TIME  = 1 / 18.0f;

	public:
		Player(GenoCamera2D * camera);
		void update();
		void render();
		void ground();
		GenoVector2f getCollisionPosition();
		GenoVector2f getCollisionDimensions();
		~Player();
};

#endif // GNARLY_PLATERAL_PLAYER