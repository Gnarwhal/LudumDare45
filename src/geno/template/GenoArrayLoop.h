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

#ifndef GNARLY_GENOME_ARRAY_LOOP
#define GNARLY_GENOME_ARRAY_LOOP

#include <initializer_list>

template <typename T>
class GenoArrayLoop {
	private:
		uint32 length;
		T * array;
		
		void clean() noexcept {
			delete [] array;
		}

	public:
		GenoArrayLoop(uint32 length, T * array) :
			length(length),
			array(array) {
		}
		
		GenoArrayLoop(uint32 length) :
			length(length),
			array(new T[length]) {
		}
		
		GenoArrayLoop(std::initializer_list<T> list) :
			length(list.size()),
			array(new T[length]) {
			T * init = list.begin();
			for (uint32 i = 0; i < length; ++i)
				array[i] = init[i];
		}
		
		GenoArrayLoop(const GenoArrayLoop<T> & array) :
			length(array.length),
			array(new T[length]) {
			for (uint32 i = 0; i < length; ++i)
				array[i] = array.array[i];
		}

		GenoArrayLoop(GenoArrayLoop<T> && array) :
			length(array.length),
			array(array.array) {
			array.array = 0;
		}

		GenoArrayLoop & operator=(const GenoArrayLoop<T> & array) {
			clean();
			length = array.length;
			this->array = new T[length];
			for (uint32 i = 0; i < length; ++i)
				this->array[i] = array.array[i];
			return *this;
		}

		GenoArrayLoop & operator=(GenoArrayLoop<T> && array) {
			clean();
			length = array.length;
			this->array = array.array;
			array.array = 0;
			return *this;
		}

		T & operator[](int32 index) {
			if (index < 0)
				return array[((index + 1) % length) + length - 1];
			else
				return array[index % length];
		}

		const T & operator[](int32 index) const {
			if (index < 0)
				return array[((index + 1) % length) + length - 1];
			else
				return array[index % length];
		}

		uint32 getLength() {
			return length;
		}

		~GenoArrayLoop() {
			clean();
		}
};

#define GNARLY_GENOME_ARRAY_LOOP_FORWARD
#endif // GNARLY_GENOME_ARRAY_LOOP