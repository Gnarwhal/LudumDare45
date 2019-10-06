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

#ifndef GNARLY_PLATERAL_PLATFORM
#define GNARLY_PLATERAL_PLATFORM

#include "../geno/math/linear/GenoVector4.h"
#include "../geno/shaders/GenoShader2c.h"

#include "Collidable.h"

class Platform : public Collidable {
	private:
		constexpr static uint32
			STATE_STATIC    = 0,
			STATE_MOBILE    = 1,
			STATE_EXPLODING = 2,
			STATE_COMPLETE  = 3;

		static GenoShader2c * shader;

		GenoVector4f color;
		int32 state;
		float time;
		float scale;

	public:
		Platform(GenoCamera2D * camera, const GenoVector2f & position, const GenoVector2f & dimensions);
		Platform(GenoCamera2D * camera, const GenoVector2f & position, const GenoVector2f & dimensions, const GenoVector2f & velocity);
		void update();
		void render();
		Platform & finalize();
		Platform & detonate();
		void reset(const GenoVector2f & position, const GenoVector2f & velocity);
		bool isComplete();
		~Platform();
};

#endif // GNARLY_PLATERAL_PLATFORM