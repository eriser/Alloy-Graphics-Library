/*
* Copyright(C) 2015, Blake C. Lucas, Ph.D. (img.science@gmail.com)
*
* Permission is hereby granted, free of charge, to any person obtaining a copy
* of this software and associated documentation files (the "Software"), to deal
* in the Software without restriction, including without limitation the rights
* to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
* copies of the Software, and to permit persons to whom the Software is
* furnished to do so, subject to the following conditions:
* The above copyright notice and this permission notice shall be included in
* all copies or substantial portions of the Software.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
* AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
* OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
* THE SOFTWARE.
*/

#include "AlloyDistanceField.h"
#include "BinaryMinHeap.h"
using namespace std;
namespace aly {
	const ubyte1 DistanceField3f::ALIVE = ubyte1((uint8_t)1);
	const ubyte1 DistanceField3f::NARROW_BAND = ubyte1((uint8_t)2);
	const ubyte1 DistanceField3f::FAR_AWAY = ubyte1((uint8_t)3);
	const float DistanceField3f::DISTANCE_UNDEFINED = std::numeric_limits<float>::max();
	/*
		Sethian, J. A. (1999). Level set methods and fast marching methods: evolving interfaces
		in computational geometry, fluid mechanics, computer vision, and materials science (Vol. 3).
		Cambridge university press.
	*/
	float DistanceField3f::march(float Nv, float Sv, float Ev, float Wv,
		float Fv, float Bv, int Nl, int Sl, int El, int Wl, int Fl, int Bl) {
		float s, s2; /* s = a + b +c; s2 = a*a + b*b +c*c */
		float tmp;
		int count;
		s = 0;
		s2 = 0;
		count = 0;
		if (Nl == ALIVE && Sl == ALIVE) {
			tmp = std::min(Nv, Sv);
			s += tmp;
			s2 += tmp * tmp;
			count++;
		}
		else if (Nl == ALIVE) {
			s += Nv;
			s2 += Nv * Nv;
			count++;
		}
		else if (Sl == ALIVE) {
			s += Sv;
			s2 += Sv * Sv;
			count++;
		}
		if (El == ALIVE && Wl == ALIVE) {
			tmp = std::min(Ev, Wv);
			s += tmp;
			s2 += tmp * tmp;
			count++;
		}
		else if (El == ALIVE) {
			s += Ev;
			s2 += Ev * Ev;
			count++;
		}
		else if (Wl == ALIVE) {
			s += Wv;
			s2 += Wv * Wv;
			count++;
		}
		if (Fl == ALIVE && Bl == ALIVE) {
			tmp = std::min(Fv, Bv);
			s += tmp;
			s2 += tmp * tmp;
			count++;
		}
		else if (Fl == ALIVE) {
			s += Fv;
			s2 += Fv * Fv;
			count++;
		}
		else if (Bl == ALIVE) {
			s += Bv;
			s2 += Bv * Bv;
			count++;
		}
		tmp = (s + std::sqrt((s * s - count * (s2 - 1.0f)))) / count;
		return tmp;
	}
	void DistanceField3f::solve(const Volume1f& vol, Volume1f& distVol, float maxDistance) {
		const int rows = vol.rows;
		const int cols = vol.cols;
		const int slices = vol.slices;
		BinaryMinHeap<float, 3> heap(vol.dimensions());
		distVol.resize(rows, cols, slices);
		distVol.set(float1(DISTANCE_UNDEFINED));

		static const int neighborsX[6] = { 1, 0, -1, 0, 0, 0 };
		static const int neighborsY[6] = { 0, 1, 0, -1, 0, 0 };
		static const int neighborsZ[6] = { 0, 0, 0, 0, 1, -1 };
		std::list<VoxelIndex> voxelList;
		VoxelIndex* he = nullptr;

		Volume1ub labelVol(rows, cols, slices);
		Volume1b signVol(rows, cols, slices);
		labelVol.set(FAR_AWAY);
		size_t countAlive = 0;
#pragma omp parallel for
		for (int k = 0;k < slices;k++) {
			int LX, HX, LY, HY, LZ, HZ;
			short NSFlag, WEFlag, FBFlag;
			float s = 0, t = 0, w = 0;
			float Nv = 0, Sv = 0, Wv = 0, Ev = 0, Fv = 0, Bv = 0, Cv = 0;
			int8_t Ns = 0, Ss = 0, Ws = 0, Es = 0, Fs = 0, Bs = 0, Cs = 0;
			ubyte1 Nl = 0;
			ubyte1 Sl = 0;
			ubyte1 Wl = 0;
			ubyte1 El = 0;
			ubyte1 Fl = 0;
			ubyte1 Bl = 0;
			ubyte1 Cl = 0;
			for (int j = 0;j < cols;j++) {
				for (int i = 0;i < rows;i++) {
					Cv = vol(i, j, k).x;
					if (Cv == 0) {
						signVol(i, j, k).x = 0;
						distVol(i, j, k).x = 0;
						labelVol(i, j, k) = ALIVE;
#pragma omp atomic
						countAlive++;
					}
					else {
						if (Cv != DISTANCE_UNDEFINED) {
							signVol(i, j, k).x = (int8_t)aly::sign(Cv);
							LX = (i == 0) ? 1 : 0;
							HX = (i == (rows - 1)) ? 1 : 0;

							LY = (j == 0) ? 1 : 0;
							HY = (j == (cols - 1)) ? 1 : 0;

							LZ = (k == 0) ? 1 : 0;
							HZ = (k == (slices - 1)) ? 1 : 0;

							NSFlag = 0;
							WEFlag = 0;
							FBFlag = 0;

							Nv = vol(i, j - 1 + LY, k).x;
							Sv = vol(i, j + 1 - HY, k).x;
							Wv = vol(i - 1 + LX, j, k).x;
							Ev = vol(i + 1 - HX, j, k).x;
							Fv = vol(i, j, k + 1 - HZ).x;
							Bv = vol(i, j, k - 1 + LZ).x;
							if (Nv * Cv < 0 && Nv != DISTANCE_UNDEFINED) {
								NSFlag = 1;
								s = Nv;
							}
							if (Sv * Cv < 0 && Sv != DISTANCE_UNDEFINED) {
								if (NSFlag == 0) {
									NSFlag = 1;
									s = Sv;
								}
								else {
									s = (std::abs(Nv) > std::abs(Sv)) ? Nv : Sv;
								}
							}
							if (Wv * Cv < 0 && Wv != DISTANCE_UNDEFINED) {
								WEFlag = 1;
								t = Wv;
							}
							if (Ev * Cv < 0 && Ev != DISTANCE_UNDEFINED) {
								if (WEFlag == 0) {
									WEFlag = 1;
									t = Ev;
								}
								else {
									t = (std::abs(Ev) > std::abs(Wv)) ? Ev : Wv;
								}
							}
							if (Fv * Cv < 0 && Fv != DISTANCE_UNDEFINED) {
								FBFlag = 1;
								w = Fv;
							}
							if (Bv * Cv < 0 && Bv != DISTANCE_UNDEFINED) {
								if (FBFlag == 0) {
									FBFlag = 1;
									w = Bv;
								}
								else {
									w = (std::abs(Fv) > std::abs(Bv)) ? Fv : Bv;
								}
							}
							float result = 0;
							if (NSFlag != 0) {
								s = Cv / (Cv - s);
								result += 1.0f / (s * s);
							}
							if (WEFlag != 0) {
								t = Cv / (Cv - t);
								result += 1.0f / (t * t);
							}
							if (FBFlag != 0) {
								w = Cv / (Cv - w);
								result += 1.0f / (w * w);
							}
							if (result == 0) {
								distVol(i, j, k).x = 0;
							}
							else {
#pragma omp atomic
								countAlive++;
								labelVol(i, j, k) = ALIVE;
								result = std::sqrt(result);
								distVol(i, j, k).x = (float)(1.0 / result);
							}
						}
						else {
							signVol(i, j, k).x = 0;
						}
					}
				}
			}
		}
		heap.reserve(countAlive);
		{
			int koff;
			int nj, nk, ni;
			float newvalue;
			float s = 0, t = 0, w = 0;
			float Nv = 0, Sv = 0, Wv = 0, Ev = 0, Fv = 0, Bv = 0, Cv = 0;
			int8_t Ns = 0, Ss = 0, Ws = 0, Es = 0, Fs = 0, Bs = 0, Cs = 0;
			ubyte1 Nl = 0;
			ubyte1 Sl = 0;
			ubyte1 Wl = 0;
			ubyte1 El = 0;
			ubyte1 Fl = 0;
			ubyte1 Bl = 0;
			ubyte1 Cl = 0;
			/* Initialize NarrowBand Heap */
			for (int k = 0;k < slices;k++) {
				for (int j = 0;j < cols;j++) {
					for (int i = 0;i < rows;i++) {
						if (labelVol(i, j, k) != ALIVE) {
							continue;
						}
						for (koff = 0; koff < 6; koff++) {
							ni = i + neighborsX[koff];
							nj = j + neighborsY[koff];
							nk = k + neighborsZ[koff];
							if (nj < 0 || nj >= cols || nk < 0 || nk >= slices || ni < 0
								|| ni >= rows) {
								continue; /* Out of computational Boundary */
							}
							if (labelVol(ni, nj, nk) != FAR_AWAY) {
								continue;
							}
							labelVol(ni, nj, nk) = NARROW_BAND;
							if (nj > 0) {
								Nv = distVol(ni, nj - 1, nk).x;
								Ns = signVol(ni, nj - 1, nk).x;
								Nl = labelVol(ni, nj - 1, nk);
							}
							else {
								Ns = 0;
								Nl = 0;
							}
							/* Neighbour to the south */
							if (nj < cols - 1) {
								Sv = distVol(ni, nj + 1, nk).x;
								Ss = signVol(ni, nj + 1, nk).x;
								Sl = labelVol(ni, nj + 1, nk);
							}
							else {
								Ss = 0;
								Sl = 0;
							}
							/* Neighbour to the east */
							if (nk < slices - 1) {
								Ev = distVol(ni, nj, nk + 1).x;
								Es = signVol(ni, nj, nk + 1).x;
								El = labelVol(ni, nj, nk + 1);
							}
							else {
								Es = 0;
								El = 0;
							}
							/* Neighbour to the west */
							if (nk > 0) {
								Wv = distVol(ni, nj, nk - 1).x;
								Ws = signVol(ni, nj, nk - 1).x;
								Wl = labelVol(ni, nj, nk - 1);
							}
							else {
								Ws = 0;
								Wl = 0;
							}
							/* Neighbour to the front */
							if (ni < rows - 1) {
								Fv = distVol(ni + 1, nj, nk).x;
								Es = signVol(ni + 1, nj, nk).x;
								Fl = labelVol(ni + 1, nj, nk);
							}
							else {
								Fs = 0;
								Fl = 0;
							}
							/* Neighbour to the back */
							if (ni > 0) {
								Bv = distVol(ni - 1, nj, nk).x;
								Bs = signVol(ni - 1, nj, nk).x;
								Bl = labelVol(ni - 1, nj, nk);
							}
							else {
								Bs = 0;
								Bl = 0;
							}
							/*
							 * Update the value of this to-be-updated NarrowBand
							 * point
							 */
							signVol(ni, nj, nk).x = aly::sign(Ns + Ss + Bs + Fs + Es + Ws);
							newvalue = march(Nv, Sv, Ev, Wv, Fv, Bv, Nl, Sl, El, Wl, Fl, Bl);
							distVol(ni, nj, nk).x = (float)(newvalue);
							voxelList.push_back(VoxelIndex(Coord(ni, nj, nk), (float)newvalue));
							heap.add(&voxelList.back());
						}
					};
				}
			}

			/*
			 * Begin Fast Marching to get the unsigned distance function inwords and
			 * outwards simultaneously since points inside and outside the contour
			 * won't interfere with each other
			 */
			while (!heap.isEmpty()) { /* There are still points not yet accepted */
				int i, j, k;
				he = heap.remove();
				i = he->index[0];
				j = he->index[1];
				k = he->index[2];
				if (he->value > maxDistance) {
					break;
				}
				distVol(i, j, k).x = (he->value);
				labelVol(i, j, k) = ALIVE;
				for (koff = 0; koff < 6; koff++) {
					ni = i + neighborsX[koff];
					nj = j + neighborsY[koff];
					nk = k + neighborsZ[koff];
					if (nj < 0 || nj >= cols || nk < 0 || nk >= slices || ni < 0
						|| ni >= rows) {
						continue; /* Out of boundary */
					}
					if (labelVol(ni, nj, nk) == ALIVE) {
						continue; /* Don't change ALIVE neighbour */
					}
					if (nj > 0) {
						Nv = distVol(ni, nj - 1, nk).x;
						Ns = signVol(ni, nj - 1, nk).x;
						Nl = labelVol(ni, nj - 1, nk);
					}
					else {
						Ns = 0;
						Nl = 0;
					}

					/* Neighbour to the south */
					if (nj < cols - 1) {
						Sv = distVol(ni, nj + 1, nk).x;
						Ss = signVol(ni, nj + 1, nk).x;
						Sl = labelVol(ni, nj + 1, nk);
					}
					else {
						Ss = 0;
						Sl = 0;
					}

					/* Neighbour to the east */
					if (nk < slices - 1) {
						Ev = distVol(ni, nj, nk + 1).x;
						Es = signVol(ni, nj, nk + 1).x;
						El = labelVol(ni, nj, nk + 1);
					}
					else {
						Es = 0;
						El = 0;
					}

					/* Neighbour to the west */
					if (nk > 0) {
						Wv = distVol(ni, nj, nk - 1).x;
						Ws = signVol(ni, nj, nk - 1).x;
						Wl = labelVol(ni, nj, nk - 1);
					}
					else {
						Ws = 0;
						Wl = 0;
					}

					/* Neighbour to the front */
					if (ni < rows - 1) {
						Fv = distVol(ni + 1, nj, nk).x;
						Fs = signVol(ni + 1, nj, nk).x;
						Fl = labelVol(ni + 1, nj, nk);
					}
					else {
						Fs = 0;
						Fl = 0;
					}

					/* Neighbour to the back */
					if (ni > 0) {
						Bv = distVol(ni - 1, nj, nk).x;
						Bs = signVol(ni - 1, nj, nk).x;
						Bl = labelVol(ni - 1, nj, nk);
					}
					else {
						Bs = 0;
						Bl = 0;
					}
					signVol(ni, nj, nk).x = aly::sign(Ns + Ss + Bs + Fs + Es + Ws);
					newvalue = march(Nv, Sv, Ev, Wv, Fv, Bv, Nl, Sl, El, Wl, Fl, Bl);
					voxelList.push_back(VoxelIndex(Coord(ni, nj, nk), (float)newvalue));
					VoxelIndex* vox = &voxelList.back();
					if (labelVol(ni, nj, nk) == NARROW_BAND) {
						heap.change(Coord(ni, nj, nk), vox);
					}
					else {
						heap.add(vox);
						labelVol(ni, nj, nk) = NARROW_BAND;
					}
				}
			}
		}
#pragma omp parallel for
		for (int k = 0;k < slices;k++) {
			for (int j = 0;j < cols;j++) {
				for (int i = 0;i < rows;i++) {
					int8_t s = signVol(i, j, k).x;
					if (labelVol(i, j, k) != ALIVE) {
						distVol(i, j, k).x = maxDistance*aly::sign(vol(i, j, k).x);
					}
					else {
						distVol(i, j, k).x *= s;
					}

				}
			}
		}
		heap.clear();
	}
}
