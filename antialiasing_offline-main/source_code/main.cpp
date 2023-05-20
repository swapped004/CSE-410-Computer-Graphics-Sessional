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

#include "lib/timer.h"

#include "common/app_util.h"

#include "LineAA/LineAA.h"


static void StoreTestImage(LineAA::LineAlgorithm lineAlgorithm, double zoomFactor)
{
	zci::PImage img = LineAA::DrawTestPattern(lineAlgorithm);
	wchar_t pathname[PATH_MAX];
	swprintf_s(pathname, ARRAY_SIZE(pathname), L"e:\\test-%ws.bmp", LineAA::LineAlgorithmName(lineAlgorithm));
	zci::Store(img, pathname);

	img = LineAA::DrawZoomed(lineAlgorithm, zoomFactor);
	swprintf_s(pathname, ARRAY_SIZE(pathname), L"e:\\lines_x%.2g-%ws.bmp", zoomFactor, LineAA::LineAlgorithmName(lineAlgorithm));
	zci::Store(img, pathname);
}

static void StoreTestImages(double zoomFactor)
{
	StoreTestImage(LineAA::OPTIMAL_NONNEGATIVE, zoomFactor);
	StoreTestImage(LineAA::GUPTA_SPROULL, zoomFactor);
	StoreTestImage(LineAA::EXP2, zoomFactor);
	StoreTestImage(LineAA::WU, zoomFactor);
}

static void StoreDifferenceImage()
{
	zci::PImage img1 = zci::Load(L"e:\\lines_x4-Optimal_Nonnegative.bmp");
	zci::PImage img2 = zci::Load(L"e:\\lines_x4-Gupta-Sproull.bmp");
	zci::PImage imgD = zci::Allocate(img1->Width(), img1->Height(), 3, sizeof(u8));
	int min = std::numeric_limits<int>::max();
	int max = std::numeric_limits<int>::min();
	for(size_t y = 0; y < img1->Height(); y++)
	{
		crpU8 row1 = zci::Row<u8>(img1, y);
		crpU8 row2 = zci::Row<u8>(img2, y);
		for(size_t x = 0; x < img1->Width(); x++)
		{
			const int d = row1[x] - row2[x];
			min = std::min(min, d);
			max = std::max(max, d);
		}
	}

	for(size_t y = 0; y < img1->Height(); y++)
	{
		crpU8 row1 = zci::Row<u8>(img1, y);
		crpU8 row2 = zci::Row<u8>(img2, y);
		crpU8 rowD = zci::Row<u8>(imgD, y);
		for(size_t x = 0; x < img1->Width(); x++)
		{
			int r, g, b;
			const int d = row1[x] - row2[x];
			if(d > 0 && max > 0)
			{
				r = g = b = 255 - d * (255.0f / max);
			}
			else if(d < 0 && min < 0)
			{
				r = g = b = 255 - d * (255.0f / min);
			}
			else
			{
				r = g = b = 255;
			}

			debug_assert(0 <= r && r <= 255);
			debug_assert(0 <= g && g <= 255);
			debug_assert(0 <= b && b <= 255);
			rowD[3*x+0] = (u8)r;
			rowD[3*x+1] = (u8)g;
			rowD[3*x+2] = (u8)b;
		}
	}

	debug_printf(L"min/max difference between GS and new: %d %d\n", min, max);
	zci::Store(imgD, L"e:\\Diff.bmp");
}

int main(int argc, wchar_t* argv[])
{
	app_util_PrepareForBenchmark();
	debug_filter_add(L"TIMER");
	//_CrtSetBreakAlloc(577);

	StoreTestImages(1.0);
	StoreTestImages(2.0);
	StoreTestImages(4.0);
	StoreTestImages(8.0);
	StoreDifferenceImage();

#if 1
	LineAA::BenchmarkLines(LineAA::OPTIMAL_NONNEGATIVE);
	//LineAA::BenchmarkLines(LineAA::GUPTA_SPROULL);
	LineAA::BenchmarkLines(LineAA::OPTIMAL_NONNEGATIVE);
	//LineAA::BenchmarkLines(LineAA::GUPTA_SPROULL);
	//LineAA::BenchmarkLines(LineAA::OPTIMAL_NONNEGATIVE);
	//LineAA::BenchmarkLines(LineAA::GUPTA_SPROULL);
#endif

	timer_DisplayClientTotals();
	vm_DumpStatistics();

	return 0;
}
