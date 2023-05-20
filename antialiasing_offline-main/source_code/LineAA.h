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

#ifndef INCLUDED_LINE_AA
#define INCLUDED_LINE_AA

#include "LineAA_api.h"

#include "common/arguments.h"
#include "common/point.h"

#include "zci_image.h"

namespace LineAA {

static const u8 pixelBackground = 0;
static const u8 pixelForeground = 0xFF;


enum LineAlgorithm
{
	WU,
	MITCHELL_NETRAVALI,
	GUPTA_SPROULL,
	EXP2,
	OPTIMAL_NONNEGATIVE	// -sqrt2..sqrt2
};

LINE_AA_API const wchar_t* LINE_AA_CALL LineAlgorithmName(LineAlgorithm lineAlgorithm);

// drawing always starts at the first point, but there is no constraint
// on the relative position WRT end (the draw time for an X-major line
// isn't affected by the scan direction, which is plausible because
// hardware prefetchers also handle backward streams)
//
// no point may lie on the border of the image unless it is part of a
// vertical or horizontal line.
//
// fill rate: 2.6 GPixel/s
// (W5580 CPU, 64k parallel lines with slope ~-1/8 and length ~8k, sorted by increasing y)
// 1.35x speedup vs. Wu despite shading 3 pixels instead of 2.
LINE_AA_API void LINE_AA_CALL DrawLineStrip(crpcPoint2i points, size_t numPoints, const zci::PImage& img, LineAlgorithm lineAlgorithm = OPTIMAL_NONNEGATIVE);

LINE_AA_API void LINE_AA_CALL BenchmarkLines(LineAlgorithm lineAlgorithm);
LINE_AA_API zci::PImage LINE_AA_CALL DrawTestPattern(LineAlgorithm lineAlgorithm);
LINE_AA_API zci::PImage LINE_AA_CALL DrawZoomed(LineAlgorithm lineAlgorithm, double zoomFactor);

}	// namespace LineAA

#endif	// #ifndef INCLUDED_LINE_AA
