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

#ifndef GNARLY_PLATERAL_COLLIDABLE
#define GNARLY_PLATERAL_COLLIDABLE

#include "../geno/math/linear/GenoVector2.h"
#include "../geno/engine/GenoCamera2D.h"
#include "../geno/gl/GenoVao.h"

class Collidable {
	protected:
		static GenoVao * vao;

		GenoCamera2D * camera;
	public:
		GenoVector2f position;
		GenoVector2f dimensions;
		GenoVector2f velocity;

		Collidable(GenoCamera2D * camera, const GenoVector2f & position, const GenoVector2f & dimensions, const GenoVector2f & velocity);
		~Collidable();
};

#endif // GNARLY_PLATERAL_COLLIDABLE