#include "precompiled.h"
#include "LineAA/LineAA.h"

#include "lib/timer.h"
#include "lib/allocators/shared_ptr.h"
#include "lib/sysdep/cpu.h"

#include "bindings/ipp.h"

namespace LineAA {

struct NonzeroPixel : public std::unary_function<u8, bool>
{
	bool operator()(u8 pixel) const
	{
		return pixel != pixelBackground;
	}
};

void BenchmarkLines(LineAlgorithm lineAlgorithm)
{
	const int x0 = 10;
	const int y0 = 10;
	const int x1 = 8192-10;
	const int y1 = 1024-10;
	const size_t numLines = 16384;
	zci::PImage img = zci::Allocate(x1 + 10, y1 + numLines*10 + 10, 1, 1);
	for(size_t y = 0; y < img->Height(); y++)
		memset(zci::Row<u8>(img, y), pixelBackground, img->Width()*sizeof(u8));
	std::vector<Point2i> points;
	points.reserve(numLines*2);
	for(size_t i = 0; i < numLines; i++)
	{
		points.push_back(Point2i(x0, y0 + i*10));
		points.push_back(Point2i(x1, y1 + i*10));
		points.push_back(Point2i(x1, y1 + i*10 + 5));
		points.push_back(Point2i(x0, y0 + i*10 + 5));
	}
	COMPILER_FENCE;
	const volatile double t0 = timer_Time();
	const i64 c0 = x86_x64_rdtsc();
	COMPILER_FENCE;
	LineAA::DrawLineStrip(&points[0], points.size(), img, lineAlgorithm);
	COMPILER_FENCE;
	const volatile double t1 = timer_Time();
	const i64 c1 = x86_x64_rdtsc();
	COMPILER_FENCE;
	size_t numNonzero = 0;
	for(size_t y = 0; y < img->Height(); y++)
	{
		crpcU8 row = zci::Row<u8>(img, y);
		numNonzero += std::count_if(row, row+img->Width(), NonzeroPixel());
	}
	debug_printf(L"drawn(nonzero): %I64d (%.2f Mpixel/s, %I64d clk)\n", numNonzero, numNonzero / (t1-t0) * 1e-6, c1-c0);

	//zci::Store(img, L"d:\\benchmark.img");
}


// for printing purposes
static void ConvertToWhiteBackground(const zci::PImage& img)
{
	for(size_t y = 0; y < img->Height(); y++)
	{
		crpU8 row = zci::Row<u8>(img, y);
		for(size_t x = 0; x < img->Width(); x++)
			row[x] = (u8)(0xFF - row[x]);
	}
}


zci::PImage DrawTestPattern(LineAlgorithm lineAlgorithm)
{
	zci::PImage img = zci::Allocate(160+1, 160+1, 1, 1);
	for(size_t y = 0; y < img->Height(); y++)
		memset(zci::Row<u8>(img, y), pixelBackground, img->Width()*sizeof(u8));

	// bounds checks
	if(0)	// disabled for printing purposes
	{
		std::vector<Point2i> points;
		points.push_back(Point2i(0, 0));
		points.push_back(Point2i(160, 0));
		points.push_back(Point2i(160, 160));
		points.push_back(Point2i(0, 160));
		points.push_back(Point2i(0, 0));
		LineAA::DrawLineStrip(&points[0], points.size(), img, lineAlgorithm);
	}

	// x major
	{
		std::vector<Point2i> points;
		points.push_back(Point2i(80,55));
		points.push_back(Point2i(5, 30));
		points.push_back(Point2i(80, 5));
		points.push_back(Point2i(155, 30));
		points.push_back(Point2i(80, 55));
		LineAA::DrawLineStrip(&points[0], points.size(), img, lineAlgorithm);
	}

	// y major
	{
		std::vector<Point2i> points;
		points.push_back(Point2i(90, 80));
		points.push_back(Point2i(115, 5));
		points.push_back(Point2i(140, 80));
		points.push_back(Point2i(115, 155));
		points.push_back(Point2i(90, 80));
		LineAA::DrawLineStrip(&points[0], points.size(), img, lineAlgorithm);
	}

	// house
	{
		std::vector<Point2i> points;
		points.push_back(Point2i(20, 140));
		points.push_back(Point2i(70, 90));
		points.push_back(Point2i(70, 140));
		points.push_back(Point2i(20, 90));
		points.push_back(Point2i(70, 90));
		points.push_back(Point2i(45, 65));
		points.push_back(Point2i(20, 90));
		points.push_back(Point2i(20, 140));
		points.push_back(Point2i(70, 140));
		LineAA::DrawLineStrip(&points[0], points.size(), img, lineAlgorithm);
	}

	ConvertToWhiteBackground(img);
	return img;
}


zci::PImage DrawZoomed(LineAlgorithm lineAlgorithm, double zoomFactor)
{
	zci::PImage img = zci::Allocate(40, 10, 1, 1);
	for(size_t y = 0; y < img->Height(); y++)
		memset(zci::Row<u8>(img, y), pixelBackground, img->Width()*sizeof(u8));

	// x major
	{
		std::vector<Point2i> points;
		points.push_back(Point2i(1, 1));
		points.push_back(Point2i(38, 8));
		LineAA::DrawLineStrip(&points[0], points.size(), img, lineAlgorithm);
	}
	ConvertToWhiteBackground(img);

	const int interp = IPPI_INTER_NN;
	zci::PImage zoomed = zci::Allocate(img->Width()*zoomFactor, img->Height()*zoomFactor, img->NumComponents(), img->ComponentType());
	const IppiSize inSize = { int(img->Width()), int(img->Height()) };
	const IppiRect inROI = { 0, 0, int(img->Width()), int(img->Height()) };
	const IppiRect outROI = { 0, 0, int(zoomed->Width()), int(zoomed->Height()) };

	shared_ptr<u8> buf;
	{
		int size;
		const IppStatus ret = ippiResizeGetBufSize(inROI, outROI, 1, interp, &size);
		debug_assert(ret == ippStsNoErr);
		buf = AllocateAligned<u8>(size);
	}
	const IppStatus ret = ippiResizeSqrPixel_8u_C1R((const u8*)img->Pixels(), inSize, int(img->Step()), inROI, (u8*)zoomed->Pixels(), int(zoomed->Step()), outROI, zoomFactor, zoomFactor, 0.0, 0.0, interp, buf.get());
	debug_assert(ret == ippStsNoErr);

	return zoomed;
}

}	// namespace LineAA
