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

#include "precompiled.h"
#include "LineAA/LineAA.h"

//#define OMP_ENABLED 0	// uncomment to disable parallelization

#include "common/sse/sse.h"	// SSE3 _mm_addsub_ps, SSE4 controlled by ENABLE_SSE4
#include "common/parallelization.h"
#include "common/pointer_typedefs.h"

#include "zci_image.h"

#include "filters.h"


namespace LineAA {

const wchar_t* LineAlgorithmName(LineAlgorithm lineAlgorithm)
{
	switch(lineAlgorithm)
	{
	case WU:
		return L"Wu";
	case GUPTA_SPROULL:
		return L"Gupta-Sproull";
	case EXP2:
		return L"Exp2";
	case MITCHELL_NETRAVALI:
		return L"Mitchell-Netravali";
	case OPTIMAL_NONNEGATIVE:
		return L"Optimal_Nonnegative";
	default:
		debug_assert(0);
		return L"";
	}
}


//-----------------------------------------------------------------------------
// PixelExtractor

struct PixelExtractor_SSE2
{
	// instantiations must be in increasing order of idx since we
	// successively extract the lower 32 bits.
	// (rationale for template argument: see PixelExtractor_SSE4)
	template<int idx>
	u8 Get(Is32vec4& pixels) const
	{
		const u8 pixel = (u8)_mm_cvtsi128_si32(pixels);
		pixels = Is32vec4(_mm_srli_si128(pixels, sizeof(u32)));
		return pixel;
	}
};

#if ENABLE_SSE4
struct PixelExtractor_SSE4
{
	// (a function parameter isn't a compile-time constant in ICC11 debug builds)
	template<int idx>
	u8 Get(Is32vec4& pixels) const
	{
		return (u8)_mm_extract_epi32(pixels, idx);
	}

	template<>
	u8 Get<0>(Is32vec4& pixels) const
	{
		return (u8)_mm_cvtsi128_si32(pixels);
	}
};
typedef PixelExtractor_SSE4 PixelExtractor;
#else
typedef PixelExtractor_SSE2 PixelExtractor;
#endif


//-----------------------------------------------------------------------------
// LineDrawer

class LineDrawer
{
public:
	LineDrawer(const zci::PImage& img, const Point2i& start, const Point2i& end)
	{
		// handle all 8 octants by transposing/mirroring to u,v
		lu = abs(end.x - start.x);
		lv = abs(end.y - start.y);
		const ssize_t step = (ssize_t)img->Step();
		du = (end.x > start.x)? +1 : -1;	// loop counter
		dv = (end.y > start.y)? +step : -step;	// dependent
		if(lu < lv)
		{
			std::swap(lu, lv);
			std::swap(du, dv);
		}

		// the middle (WRT v axis) of the three affected pixels
		pos = zci::Row<u8>(img, ssize_t(start.y))+start.x;
	}

	template<class Filter>
	void Run()
	{
		// special-case horizontal and vertical lines
		// (avoids the need for bounds checks in the main loop)
		if(lv == 0)
		{
			for(size_t i = 0; i <= lu; i++)
				pos[i*du] = pixelForeground;
			return;
		}

		// NB: the following XMM constants/variables aren't enregistered when
		// passed by const reference or defined as class members!
		const Filter filter;

		// signed perpendicular distance from the line (absolute value <= sqrt2)
		F32vec4 signedPerpDist(0.0f);

		// the original definition calls for a mask: (d > 0)? FF..FF : 0.
		// however, inverting it allows computing via single right-shift.
		// NB: this is needed by both vector and integer code,
		// but XMM register pressure is lower than that of the GPRs.
		Is32vec4 negDiscriminator;

		// slope, used to update signedPerpDist.
		// (ordering chosen such that 0,+,- can be added via _mm_addsub_ps
		// and extracted starting with the least-significant element)
		__m128 sin_cos_cos_0;

		// for updating negDiscriminator and taking absolute value
		// (a second 0x7FFFFFFF allows broadcasting via _mm_unpack_epi32,
		// which is smaller than _mm_shuffle_epi32)
		// (constants are stored together to ease register pressure)
		__m128i lu2_lv2_7F_7F;

		{
			const Is32vec4 lu_epi32 = _mm_shuffle_epi32(epi32_from_size_t(lu), _MM_SHUFFLE(0,0,0,0));
			const Is32vec4 lv_epi32 = _mm_shuffle_epi32(epi32_from_size_t(lv), _MM_SHUFFLE(0,0,0,0));

			negDiscriminator = lu_epi32 - (lv_epi32+lv_epi32);

			// sin_cos_cos_0
			{
				VERIFY(lu != 0 || lv != 0);	// else recipLength = infinity
				const F32vec4 lu_ps = _mm_cvtepi32_ps(lu_epi32);
				const F32vec4 lv_ps = _mm_cvtepi32_ps(lv_epi32);
				const F32vec4 recipLength = rsqrt(lu_ps*lu_ps + lv_ps*lv_ps);
#if ENABLE_SSE4
				sin_cos_cos_0 = _mm_insert_ps(lu_ps, lv_ps, 0x31)*recipLength;
#else
				sin_cos_cos_0 = _mm_set_ps((float)lv, (float)lu, (float)lu, 0.0f)*recipLength;	// ICC11 doesn't generate _mm_insert_ps
#endif
			}

			// lu2_lv2_7F_7F
			{
				const Is32vec4 lu_lv_lu_lv = _mm_unpacklo_epi32(lv_epi32, lu_epi32);
				const Iu32vec4 clearSignBits = Iu32vec4(_mm_cmpeq_epi32(lu_epi32, lu_epi32)) >> 1;
				lu2_lv2_7F_7F = _mm_unpackhi_epi64(clearSignBits, lu_lv_lu_lv+lu_lv_lu_lv);
			}
		}

		for(size_t i = 0; i <= lu; i++)
		{
			DrawTriplet(pos, signedPerpDist, sin_cos_cos_0, lu2_lv2_7F_7F, filter);

			const __m128i mask_epi32 = _mm_srai_epi32(negDiscriminator, 31);	// (negDiscriminator < 0)? -1 : 0
			const ssize_t mask = (ssize_t)size_t_from_si128(mask_epi32);	// for updating pos (GPR)

			// common subexpressions for both straight and diagonal movement:
			{
				const F32vec4 sin_a = _mm_shuffle_ps(sin_cos_cos_0, sin_cos_cos_0, _MM_SHUFFLE(3,3,3,3));
				const Is32vec4 lv2 = _mm_shuffle_epi32(lu2_lv2_7F_7F, _MM_SHUFFLE(2,2,2,2));
				pos += du;
				signedPerpDist += sin_a;
				negDiscriminator -= lv2;
			}

			// if moving diagonally (i.e. additionally incrementing v):
			{
				const F32vec4 cos_a = _mm_shuffle_ps(sin_cos_cos_0, sin_cos_cos_0, _MM_SHUFFLE(2,2,2,2));
				const Is32vec4 lu2 = _mm_shuffle_epi32(lu2_lv2_7F_7F, _MM_SHUFFLE(3,3,3,3));
				pos += dv & mask;
				signedPerpDist -= cos_a & _mm_castsi128_ps(mask_epi32);
				negDiscriminator += lu2 & mask_epi32;
			}
		}
	}

private:
	template<class Filter>
	__forceinline void DrawTriplet(rpU8 pos, const F32vec4& signedPerpDist, const __m128& sin_cos_cos_0, const __m128i& lu2_lv2_7F_7F, const Filter& filter)
	{
		const __m128 clearSignBits = _mm_castsi128_ps(_mm_unpacklo_epi32(lu2_lv2_7F_7F, lu2_lv2_7F_7F));

		// perpDists = unused | pd-cos | pd+cos | pd+0
		const F32vec4 perpDists = _mm_and_ps(_mm_addsub_ps(signedPerpDist, sin_cos_cos_0), clearSignBits);
		const F32vec4 alphas = filter(perpDists);	// pre-scaled to 0,255
		Is32vec4 pixels = _mm_cvtps_epi32(alphas);	// (pixelExtractor might modify this)

		PixelExtractor pixelExtractor;
		pos[0] = pixelExtractor.Get<0>(pixels);
		pos[-dv] = pixelExtractor.Get<1>(pixels);
		pos[+dv] = pixelExtractor.Get<2>(pixels);
	}

	size_t lu, lv;	// length
	ssize_t du, dv;	// increment

	rpU8 pos;
};


//-----------------------------------------------------------------------------

static void DrawLine_Wu(const zci::PImage& UNUSED(img), const Point2i& UNUSED(start), const Point2i& UNUSED(end))
{
	// implementation removed due to GPL license.
	// see http://www.codeproject.com/kb/gdi/antialias.aspx or
	// (equivalently) http://code.google.com/p/wu-antialiasing/
}


// drawing non-overlapping (but randomly shuffled) lines only
// takes 1/4 longer than parallel lines sorted by y, so we
// currently don't bother sorting/binning.

void DrawLineStrip(crpcPoint2i points, size_t numPoints, const zci::PImage& img, LineAlgorithm lineAlgorithm)
{
	typedef CubicPolynomial_Horner CubicPolynomial;
	const int chunkSize = 100;

	if(lineAlgorithm == OPTIMAL_NONNEGATIVE)
	{
		OMP(parallel for schedule(dynamic, chunkSize))
		for(ssize_t i = 1; i < ssize_t(numPoints); i++)
			LineDrawer(img, points[i-1], points[i]).Run<Filter_OptimalNonnegative<CubicPolynomial> >();
	}
	else if(lineAlgorithm == WU)
	{
		OMP(parallel for schedule(dynamic, chunkSize))
		for(ssize_t i = 1; i < ssize_t(numPoints); i++)
			DrawLine_Wu(img, points[i-1], points[i]);
	}
	else if(lineAlgorithm == MITCHELL_NETRAVALI)
	{
		OMP(parallel for schedule(dynamic, chunkSize))
		for(ssize_t i = 1; i < ssize_t(numPoints); i++)
			LineDrawer(img, points[i-1], points[i]).Run<Filter_MitchellNetravali<CubicPolynomial> >();
	}
	else if(lineAlgorithm == GUPTA_SPROULL)
	{
		OMP(parallel for schedule(dynamic, chunkSize))
		for(ssize_t i = 1; i < ssize_t(numPoints); i++)
			LineDrawer(img, points[i-1], points[i]).Run<Filter_GuptaSproull>();
	}
	else if(lineAlgorithm == EXP2)
	{
		OMP(parallel for schedule(dynamic, chunkSize))
			for(ssize_t i = 1; i < ssize_t(numPoints); i++)
				LineDrawer(img, points[i-1], points[i]).Run<Filter_Exp2<CubicPolynomial> >();
	}
}

}	// namespace LineAA
