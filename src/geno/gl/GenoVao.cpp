/*******************************************************************************
 *
 * Copyright (c) 2018 Gnarly Narwhal
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

#include "GenoVao.h"

GenoVao::GenoVao() {}

GenoVao::GenoVao(const GenoVao & vao) :
	vao(vao.vao),
	ibo(vao.ibo),
	count(vao.count),
	attribs(vao.attribs) {
	for (uint32 i = 0; i < attribs; ++i)
		vbos[i] = vao.vbos[i];
}

GenoVao & GenoVao::operator=(const GenoVao & vao) {
	this->vao = vao.vao;
	for (uint32 i = 0; i < attribs; ++i)
		vbos[i] = vao.vbos[i];
	this->ibo = vao.ibo;
	this->count = vao.count;
	this->attribs = vao.attribs;
	return *this;
}

void GenoVao::render() {
	glBindVertexArray(vao);
	glDrawElements(GL_TRIANGLES, count, GL_UNSIGNED_INT, 0);
}

GenoVao::~GenoVao() {
	glDeleteBuffers(attribs, vbos);
	glDeleteBuffers(1, &ibo);
	glDeleteVertexArrays(1, &vao);
}
