/*
The MIT License (MIT)

Copyright (c) 2014 Marcel

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/
#include "tinysplinecpp.h"
#include <stdexcept>

TsDeBoorNet::TsDeBoorNet()
{
    ts_deboornet_default(&deBoorNet);
}

TsDeBoorNet::TsDeBoorNet(const TsDeBoorNet &other) {
    const tsError err = ts_deboornet_copy(&other.deBoorNet, &deBoorNet);
    if (err < 0)
        throw std::runtime_error(ts_enum_str(err));
}

TsDeBoorNet::~TsDeBoorNet()
{
    ts_deboornet_free(&deBoorNet);
}

TsDeBoorNet &TsDeBoorNet::operator=(const TsDeBoorNet &other) {
    if (this != &other) {
        const tsError err = ts_deboornet_copy(&other.deBoorNet, &deBoorNet);
        if (err < 0)
            throw std::runtime_error(ts_enum_str(err));
    }
    return *this;
}

float TsDeBoorNet::u() const
{
    return deBoorNet.u;
}

size_t TsDeBoorNet::k() const
{
    return deBoorNet.k;
}

size_t TsDeBoorNet::s() const
{
    return deBoorNet.s;
}

size_t TsDeBoorNet::h() const
{
    return deBoorNet.h;
}

size_t TsDeBoorNet::dim() const
{
    return deBoorNet.dim;
}

size_t TsDeBoorNet::nPoints() const
{
    return deBoorNet.n_points;
}

float* TsDeBoorNet::points()
{
    return deBoorNet.points;
}

float* TsDeBoorNet::result()
{
    return deBoorNet.result;
}

tsDeBoorNet* TsDeBoorNet::data()
{
    return &deBoorNet;
}




TsBSpline::TsBSpline() 
{
    ts_bspline_default(&bspline);
}

TsBSpline::TsBSpline(const TsBSpline& other)
{
    const tsError err = ts_bspline_copy(&other.bspline, &bspline);
    if (err < 0)
        throw std::runtime_error(ts_enum_str(err));
}

TsBSpline::TsBSpline(const size_t deg, const size_t dim, const size_t nCtrlp,
                     const tsBSplineType type)
{
    const tsError err = ts_bspline_new(deg, dim, nCtrlp, type, &bspline);
    if (err < 0)
        throw std::runtime_error(ts_enum_str(err));
}

TsBSpline::TsBSpline(const float *points, const size_t nPoints, const size_t dim)
{
    const tsError err = ts_bspline_interpolate(points, nPoints, dim, &bspline);
    if (err < 0)
        throw std::runtime_error(ts_enum_str(err));
}

TsBSpline::~TsBSpline()
{
    ts_bspline_free(&bspline);
}

TsBSpline &TsBSpline::operator=(const TsBSpline &other) {
    if (this != &other) {
        const tsError err = ts_bspline_copy(&other.bspline, &bspline);
        if (err < 0)
            throw std::runtime_error(ts_enum_str(err));
    }
    return *this;
}

TsDeBoorNet TsBSpline::operator()(const float u) const
{
    return evaluate(u);
}

size_t TsBSpline::deg() const
{
    return bspline.deg;
}

size_t TsBSpline::order() const
{
    return bspline.order;
}

size_t TsBSpline::dim() const
{
    return bspline.dim;
}

size_t TsBSpline::nCtrlp() const
{
    return bspline.n_ctrlp;
}

size_t TsBSpline::nKnots() const
{
    return bspline.n_knots;
}

float* TsBSpline::ctrlp()
{
    return bspline.ctrlp;
}

float* TsBSpline::knots()
{
    return bspline.knots;
}
const float* TsBSpline::ctrlp() const
{
	return bspline.ctrlp;
}

const float* TsBSpline::knots() const 
{
	return bspline.knots;
}
tsBSpline* TsBSpline::data()
{
    return &bspline;
}

void TsBSpline::setDeg(const size_t deg)
{
    setOrder(deg+1);
}

void TsBSpline::setOrder(const size_t order)
{
    const size_t deg = order-1;
    if (deg >= bspline.n_ctrlp)
        throw std::runtime_error(ts_enum_str(TS_DEG_GE_NCTRLP));
    bspline.order = order;
    bspline.deg = deg;
}

void TsBSpline::setupKnots(const tsBSplineType type)
{
    const tsError err = ts_bspline_setup_knots(&bspline, type, &bspline);
    if (err < 0)
        throw std::runtime_error(ts_enum_str(err));
}

size_t TsBSpline::insertKnot(const float u, const size_t n)
{
    size_t k;
    const tsError err = ts_bspline_insert_knot(&bspline, u, n, &bspline, &k);
    if (err < 0)
        throw std::runtime_error(ts_enum_str(err));
    return k;
}

void TsBSpline::resize(const int n, const int back)
{
    const tsError err = ts_bspline_resize(&bspline, n, back, &bspline);
    if (err < 0)
        throw std::runtime_error(ts_enum_str(err));
}

size_t TsBSpline::split(const float u)
{
    size_t k;
    const tsError err = ts_bspline_split(&bspline, u, &bspline, &k);
    if (err < 0)
        throw std::runtime_error(ts_enum_str(err));
    return k;
}

void TsBSpline::buckle(const float b)
{
    const tsError err = ts_bspline_buckle(&bspline, b, &bspline);
    if (err < 0)
        throw std::runtime_error(ts_enum_str(err));
}

void TsBSpline::toBeziers()
{
    const tsError err = ts_bspline_to_beziers(&bspline, &bspline);
    if (err < 0)
        throw std::runtime_error(ts_enum_str(err));
}

TsDeBoorNet TsBSpline::evaluate(const float u) const
{
    TsDeBoorNet deBoorNet;
    const tsError err = ts_bspline_evaluate(&bspline, u, deBoorNet.data());
    if (err < 0)
        throw std::runtime_error(ts_enum_str(err));
    return deBoorNet;
}