/* Copyright (c) 2010 Fraunhofer IOSB
 *
 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files (the
 * "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish,
 * distribute, sublicense, and/or sell copies of the Software, and to
 * permit persons to whom the Software is furnished to do so, subject to
 * the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included
 * in all copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
 * CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
 * TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
 * SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

#ifndef LINE_AA_FILTERS
#define LINE_AA_FILTERS

#include <dvec.h>

#include "common/sse/sse.h"


//-----------------------------------------------------------------------------
// CubicPolynomial

// note: horizontal vectorization (using several lanes to shorten the
// Horner dependency chain) isn't a good fit for the SSE instruction set
// (horizontal operations have a high latency, e.g. 11 cycles for _mm_dp_ps,
// which isn't much faster than scalar computation).
// instead, we evaluate the polynomial for several values of x at a time.
// (pixel triplets only use 3 of 4 lanes, but computing distances for
// several adjacent pixels is infeasible.)

class CubicPolynomial_Horner
{
public:
	CubicPolynomial_Horner(float h0, float h1, float h2, float h3)
		: h(h3, h2, h1, h0)
	{
	}

	__forceinline F32vec4 operator()(const F32vec4& x) const
	{
		F32vec4 y = _mm_shuffle_ps(h, h, _MM_SHUFFLE(3,3,3,3));
		y *= x;
		y += _mm_shuffle_ps(h, h, _MM_SHUFFLE(2,2,2,2));
		y *= x;
		y += _mm_shuffle_ps(h, h, _MM_SHUFFLE(1,1,1,1));
		y *= x;
		y += _mm_shuffle_ps(h, h, _MM_SHUFFLE(0,0,0,0));
		return y;
	}

private:
	F32vec4 h;
};


// this form emphasizes FMA operations, so they can easily be updated
// when the AVX instruction set becomes available.
// NB: shuffling destroys its source operand, but there is no better
// alternative (_mm_movehdup_ps requires a subsequent combine step,
// which also clobbers the temporary)
class CubicPolynomial_FMA
{
public:
	CubicPolynomial_FMA(float h0, float h1, float h2, float h3)
		: h(h3, h2, h1, h0)
	{
	}

	__forceinline F32vec4 operator()(const F32vec4& x) const
	{
		F32vec4 h3x3_h2x2_h1x_h0;
		{
			F32vec4 h3x2_h2x_h1;
			{
				F32vec4 h3x_h2;
				{
					const F32vec4 h3 = _mm_shuffle_ps(h, h, _MM_SHUFFLE(3,3,3,3));
					const F32vec4 h2 = _mm_shuffle_ps(h, h, _MM_SHUFFLE(2,2,2,2));
					h3x_h2 = h3 * x + h2;
				}

				const F32vec4 h1 = _mm_shuffle_ps(h, h, _MM_SHUFFLE(1,1,1,1));
				h3x2_h2x_h1 = h3x_h2 * x + h1;
			}

			const F32vec4 h0 = _mm_shuffle_ps(h, h, _MM_SHUFFLE(0,0,0,0));
			h3x3_h2x2_h1x_h0 = h3x2_h2x_h1 * x + h0;
		}
		return h3x3_h2x2_h1x_h0;
	}

private:
	F32vec4 h;
};


//-----------------------------------------------------------------------------
// Filter

// linear cone (radius 1.5)
class Filter_GuptaSproull
{
public:
	F32vec4 operator()(const F32vec4& xxxx_) const
	{
		// the CubicPolynomial interface requires we return 3 alphas.
		// it computes them via SIMD, but we have to build a vector,
		// which is slower than the scalar version.
		// for fairness vs. the original GS formulation, the paper
		// references the previous implementation's (faster) results.
		F32vec4 xxxx(xxxx_);
		const float alpha0 = AlphaFromDistance(_mm_cvtss_f32(xxxx));
		xxxx = _mm_shuffle_ps(xxxx, xxxx, _MM_SHUFFLE(0,3,2,1));
		const float alpha1 = AlphaFromDistance(_mm_cvtss_f32(xxxx));
		xxxx = _mm_shuffle_ps(xxxx, xxxx, _MM_SHUFFLE(0,3,2,1));
		const float alpha2 = AlphaFromDistance(_mm_cvtss_f32(xxxx));
		xxxx = _mm_shuffle_ps(xxxx, xxxx, _MM_SHUFFLE(0,3,2,1));
		return F32vec4(0.0f, alpha2, alpha1, alpha0);
	}

private:
	float AlphaFromDistance(float dist) const
	{
		static __declspec(align(64)) const float table[25] =
		{
			0.780f * 255, 0.775f * 255, 0.760f * 255, 0.736f * 255, 0.703f * 255, 0.662f * 255, 0.613f * 255, 0.558f * 255,
			0.500f * 255, 0.441f * 255, 0.383f * 255, 0.328f * 255, 0.276f * 255, 0.228f * 255, 0.184f * 255, 0.145f * 255,
			0.110f * 255, 0.080f * 255, 0.056f * 255, 0.036f * 255, 0.021f * 255, 0.010f * 255, 0.004f * 255, 0.001f * 255, 0.000f
		};
		const int idx = dist*16 + 0.5f;
		const float alpha = table[idx];
		return alpha;
	}
};


// Baerentzen exp2
template<class CubicPolynomial>
struct Filter_Exp2 : public CubicPolynomial
{
	Filter_Exp2()
		: CubicPolynomial(1.57216f * 150, -0.708656f * 150, -0.953703f * 150, 0.491978f * 150)
	{
	}
};


// cubic polynomial with negative lobes
template<class CubicPolynomial>
struct Filter_MitchellNetravali : public CubicPolynomial
{
	Filter_MitchellNetravali()
		: CubicPolynomial(1.05671f * 210, -0.402299f * 210, -1.65744f * 210, 1.01315 * 210)
	{
	}
};


// optimal non-negative cubic polynomial (radius sqrt2, integral over 0..sqrt2 = 1/2)
template<class CubicPolynomial>
struct Filter_OptimalNonnegative : public CubicPolynomial
{
	Filter_OptimalNonnegative()
		// note: h0 > 1.0 => we can't multiply by 255 (otherwise,
		// the result at distance=0 would overflow the 8-bit pixel range)
		// reducing the multiplier is sufficient and beneficial anyway, since
		// overly bright lines are less appealing. (note that truncating h0
		// would impact the accuracy of the approximation.)
		: CubicPolynomial(1.0014f * 210, 0.00860678f * 210, -1.24862f * 210, 0.534384f * 210)
	{
	}
};

#endif	// #ifndef LINE_AA_FILTERS
