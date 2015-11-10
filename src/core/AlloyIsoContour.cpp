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

#include "AlloyIsoContour.h"
#include <map>
#include <list>
namespace aly {
	bool IsoContour::orient(const Image1f& img, const EdgeSplit& split1, const EdgeSplit& split2, Edge& edge) {
		float2 pt1 = split1.pt2d;
		float2 pt2 = split2.pt2d;
		float1x2 norm = img.gradient(0.5f * (pt1.x + pt2.x), 0.5f * (pt2.y + pt2.y));
		if (norm.x * (pt2.y - pt1.y) + norm.y * (pt1.x - pt2.x) > 0.0f) {
			std::swap(edge.x, edge.y);
			return false;
		}
		return true;
	}

	void IsoContour::solve(const Image1f& levelset, float isoLevel, const TopologyRule2D& topoRule,const Winding& winding){
		rows = levelset.width;
		cols = levelset.height;
		isoLevel = isoLevel;
		rule = topoRule;
		img = &levelset;
		vertCount = 0;
		std::map<uint64_t, EdgeSplitPtr> splits;
		std::list<EdgePtr> edges;
		for (int i = 0; i < rows; i++) {
			for (int j = 0; j < cols; j++) {
				processSquare(i, j, splits, edges);
			}
		}
		std::vector<EdgeSplitPtr> pts(splits.size());
		for (const std::pair<uint64_t, EdgeSplitPtr>& split : splits) {
			pts[split.second->vid] = split.second;
		}
		// Generate iso-surface from list of triangles
		indexes.resize(edges.size() * 2);
		points.resize(splits.size());
		int index = 0;
		if (winding == Winding::Clockwise) {
			for (EdgePtr edge : edges) {
				indexes[index++] = edge->x;
				indexes[index++] = edge->y;
			}
		}
		else if (winding == Winding::CounterClockwise) {
			for (EdgePtr edge : edges) {
				indexes[index++] = edge->y;
				indexes[index++] = edge->x;
			}
		}
		index = 0;
		for (const std::pair<uint64_t, EdgeSplitPtr>& split : splits) {
			index = split.second->vid;
			points[index] = split.second->pt2d;
		}
		img = nullptr;
	}
	void IsoContour::processSquare2(int x, int y, std::map<uint64_t, EdgeSplitPtr>& splits, std::list<EdgePtr>& edges) {
		int iFlagIndex = 0;
		for (int iVertex = 0; iVertex < 4; iVertex++) {
			if (getValue(x + a2fVertex1Offset[iVertex][0], y + a2fVertex1Offset[iVertex][1]) > isoLevel) {
				iFlagIndex |= 1 << iVertex;
			}
		}
		const int* mask = nullptr;
		if (rule == TopologyRule2D::Connect4) {
			mask = &afSquareValue4[iFlagIndex][0];
		}
		else {
			mask = &afSquareValue8[iFlagIndex][0];
		}

		if (mask[0] < 4) {
			EdgeSplitPtr split1 = createSplit(splits, x
				+ a2fVertex1Offset[mask[0]][0], y
				+ a2fVertex1Offset[mask[0]][1], x
				+ a2fVertex2Offset[mask[0]][0], y
				+ a2fVertex2Offset[mask[0]][1]);

			EdgeSplitPtr split2 = createSplit(splits, x
				+ a2fVertex1Offset[mask[1]][0], y
				+ a2fVertex1Offset[mask[1]][1], x
				+ a2fVertex2Offset[mask[1]][0], y
				+ a2fVertex2Offset[mask[1]][1]);
			EdgePtr edge = EdgePtr(new Edge(split1->vid, split2->vid));

			if (split1->e1.get() == nullptr) {
				split1->e1 = edge;
			}
			else {
				split1->e2 = edge;
			}
			if (split2->e1.get() == nullptr) {
				split2->e1 = edge;
			}
			else {
				split2->e2 = edge;
			}
			edges.push_back(edge);

		}

		if (mask[2] < 4) {
			EdgeSplitPtr split1 = createSplit(splits, x
				+ a2fVertex1Offset[mask[2]][0], y
				+ a2fVertex1Offset[mask[2]][1], x
				+ a2fVertex2Offset[mask[2]][0], y
				+ a2fVertex2Offset[mask[2]][1]);

			EdgeSplitPtr split2 = createSplit(splits, x
				+ a2fVertex1Offset[mask[3]][0], y
				+ a2fVertex1Offset[mask[3]][1], x
				+ a2fVertex2Offset[mask[3]][0], y
				+ a2fVertex2Offset[mask[3]][1]);

			EdgePtr edge = EdgePtr(new Edge(split1->vid, split2->vid));
			// orient(*split1, *split2, edge);
			if (split1->e1.get() == nullptr) {
				split1->e1 = edge;
			}
			else {
				split1->e2 = edge;
			}
			if (split2->e1.get() == nullptr) {
				split2->e1 = edge;
			}
			else {
				split2->e2 = edge;
			}
			edges.push_back(edge);
		}

	}
	void IsoContour::processSquare(int x, int y, std::map<uint64_t, EdgeSplitPtr>& splits, std::list<EdgePtr>& edges) {
		if (rule == TopologyRule2D::Unconstrained) {
			processSquare1(x, y, splits, edges);
		}
		else {
			processSquare2(x, y, splits, edges);
		}
	}
	void IsoContour::processSquare1(int i, int j, std::map<uint64_t, EdgeSplitPtr>& splits, std::list<EdgePtr>& edges) {
		float iF00 = getValue(i, j);
		float iF10 = getValue(i + 1, j);
		float iF01 = getValue(i, j + 1);
		float iF11 = getValue(i + 1, j + 1);
		bool signFlip = false;
		if (iF00 != 0) {
			if (iF00 < 0) {
				iF00 = -iF00;
				iF10 = -iF10;
				iF11 = -iF11;
				iF01 = -iF01;
				signFlip = true;
			}

			if (iF10 > 0) {
				if (iF11 > 0) {
					if (iF01 > 0) {
						// ++++
						return;
					}
					else {
						// +++-
						addEdge(splits, edges, i, j + 1, i + 1, j + 1, i,
							j + 1, i, j);
					}
				}
				else if (iF11 < 0) {
					if (iF01 > 0) {
						// ++-+
						addEdge(splits, edges, i + 1, j, i + 1, j + 1, i + 1,
							j + 1, i, j + 1);
					}
					else if (iF01 < 0) {
						// ++--
						addEdge(splits, edges, i, j + 1, i, j, i + 1, j, i + 1,
							j + 1);
					}
					else {
						// ++-0
						addEdge(splits, edges, i, j + 1, i, j + 1, i + 1, j,
							i + 1, j + 1);
					}
				}
				else {
					if (iF01 > 0) {
						// ++0+
						return;
					}
					else if (iF01 < 0) {
						// ++0-
						addEdge(splits, edges, i + 1, j + 1, i + 1, j + 1, i,
							j, i, j + 1);
					}
					else {
						// ++00
						addEdge(splits, edges, i + 1, j + 1, i + 1, j + 1, i,
							j + 1, i, j + 1);
					}
				}
			}
			else if (iF10 < 0) {
				if (iF11 > 0) {
					if (iF01 > 0) {
						// +-++
						addEdge(splits, edges, i, j, i + 1, j, i + 1, j + 1,
							i + 1, j);
					}
					else if (iF01 < 0) {
						// +-+-
						// Ambiguous Case
						if (rule == TopologyRule2D::Unconstrained) {
							float iD0 = iF00 - iF10;
							float iXN0 = iF00 * (i + 1) - iF10 * i;
							float iD3 = iF11 - iF01;
							float iXN1 = iF11 * i - iF01 * (i + 1);
							float iDet = 0;
							if (iD0 * iD3 > 0) {
								iDet = iXN1 * iD0 - iXN0 * iD3;
							}
							else {
								iDet = iXN0 * iD3 - iXN1 * iD0;
							}
							if (iDet > 0) {
								addEdge(splits, edges, i + 1, j + 1, i, j + 1,
									i + 1, j + 1, i + 1, j);
								addEdge(splits, edges, i, j, i + 1, j, i, j, i,
									j + 1);
							}
							else {
								addEdge(splits, edges, i + 1, j + 1, i, j + 1,
									i, j, i, j + 1);
								addEdge(splits, edges, i, j, i + 1, j, i + 1,
									j + 1, i + 1, j);
							}
						}
						else if (rule == TopologyRule2D::Connect4) {
							if (signFlip) {
								addEdge(splits, edges, i + 1, j + 1, i, j + 1,
									i + 1, j + 1, i + 1, j);
								addEdge(splits, edges, i, j, i + 1, j, i, j, i,
									j + 1);
							}
							else {
								addEdge(splits, edges, i + 1, j + 1, i, j + 1,
									i, j, i, j + 1);
								addEdge(splits, edges, i, j, i + 1, j, i + 1,
									j + 1, i + 1, j);
							}
						}
						else if (rule == TopologyRule2D::Connect8) {
							if (signFlip) {
								addEdge(splits, edges, i + 1, j + 1, i, j + 1,
									i, j, i, j + 1);
								addEdge(splits, edges, i, j, i + 1, j, i + 1,
									j + 1, i + 1, j);
							}
							else {
								addEdge(splits, edges, i + 1, j + 1, i, j + 1,
									i + 1, j + 1, i + 1, j);
								addEdge(splits, edges, i, j, i + 1, j, i, j, i,
									j + 1);
							}
						}
					}
					else {
						// +-+0
						addEdge(splits, edges, i, j, i + 1, j, i + 1, j + 1,
							i + 1, j);
					}
				}
				else if (iF11 < 0) {
					if (iF01 > 0) {
						// +--+
						addEdge(splits, edges, i, j, i + 1, j, i + 1, j + 1, i,
							j + 1);
					}
					else if (iF01 < 0) {
						// +---
						addEdge(splits, edges, i, j + 1, i, j, i, j, i + 1, j);
					}
					else {
						// +--0
						addEdge(splits, edges, i, j + 1, i, j + 1, i, j, i + 1,
							j);
					}
				}
				else {
					if (iF01 > 0) {
						// +-0+
						addEdge(splits, edges, i + 1, j + 1, i + 1, j + 1, i,
							j, i + 1, j);
					}
					else if (iF01 < 0) {
						// +-0-
						addEdge(splits, edges, i, j + 1, i, j, i, j, i + 1, j);
					}
					else {
						// +-00
						addEdge(splits, edges, i + 1, j + 1, i + 1, j + 1, i,
							j + 1, i + 1, j + 1);
						addEdge(splits, edges, i, j + 1, i + 1, j + 1, i,
							j + 1, i, j + 1);
						addEdge(splits, edges, i, j + 1, i + 1, j + 1, i, j,
							i + 1, j);
					}
				}
			}
			else {
				if (iF11 > 0) {
					if (iF01 > 0) {
						// +0++
					}
					else if (iF01 < 0) {
						// +0+-
						addEdge(splits, edges, i, j + 1, i + 1, j + 1, i,
							j + 1, i, j);
					}
				}
				else if (iF11 < 0) {
					if (iF01 > 0) {
						// +0-+
						addEdge(splits, edges, i + 1, j, i + 1, j, i, j + 1,
							i + 1, j + 1);
					}
					else if (iF01 < 0) {
						// +0--
						addEdge(splits, edges, i + 1, j, i + 1, j, i, j, i,
							j + 1);
					}
					else {
						// +0-0
						addEdge(splits, edges, i + 1, j, i + 1, j, i, j + 1, i,
							j + 1);
					}
				}
				else {
					if (iF01 > 0) {
						// +00+
						addEdge(splits, edges, i + 1, j, i + 1, j, i + 1,
							j + 1, i + 1, j + 1);
					}
					else if (iF01 < 0) {
						// +00-
						addEdge(splits, edges, i + 1, j, i + 1, j, i + 1, j,
							i + 1, j + 1);
						addEdge(splits, edges, i + 1, j, i + 1, j + 1, i + 1,
							j + 1, i + 1, j + 1);
						addEdge(splits, edges, i + 1, j, i + 1, j + 1, i, j, i,
							j + 1);
					}
					else {
						// +000
						addEdge(splits, edges, i, j + 1, i, j + 1, i, j, i, j);
						addEdge(splits, edges, i, j, i, j, i + 1, j, i + 1, j);
					}
				}
			}
		}
		else if (iF10 != 0) {
			// convert to case 0+**
			if (iF10 < 0) {
				iF10 = -iF10;
				iF11 = -iF11;
				iF01 = -iF01;
			}

			if (iF11 > 0) {
				if (iF01 > 0) {
					// 0+++
				}
				else if (iF01 < 0) {
					// 0++-
					addEdge(splits, edges, i, j, i, j, i, j + 1, i + 1, j + 1);
				}
				else {
					// 0++0
					addEdge(splits, edges, i, j + 1, i, j + 1, i, j, i, j);
				}
			}
			else if (iF11 < 0) {
				if (iF01 > 0) {
					// 0+-+
					addEdge(splits, edges, i + 1, j, i + 1, j + 1, i + 1,
						j + 1, i, j + 1);
				}
				else if (iF01 < 0) {
					// 0+--
					addEdge(splits, edges, i, j, i, j, i + 1, j, i + 1, j + 1);
				}
				else {
					// 0+-0
					addEdge(splits, edges, i, j, i, j, i, j, i, j + 1);
					addEdge(splits, edges, i, j, i, j + 1, i, j + 1, i, j + 1);
					addEdge(splits, edges, i, j, i, j + 1, i + 1, j, i + 1,
						j + 1);
				}
			}
			else {
				if (iF01 > 0) {
					// 0+0+
				}
				else if (iF01 < 0) {
					// 0+0-
					addEdge(splits, edges, i, j, i, j, i + 1, j + 1, i + 1,
						j + 1);
				}
				else {
					// 0+00
					addEdge(splits, edges, i + 1, j + 1, i + 1, j + 1, i,
						j + 1, i, j + 1);
					addEdge(splits, edges, i, j + 1, i, j + 1, i, j, i, j);
				}
			}
		}
		else if (iF11 != 0) {
			// convert to case 00+*
			if (iF11 < 0) {
				iF11 = -iF11;
				iF01 = -iF01;
			}

			if (iF01 > 0) {
				// 00++
				addEdge(splits, edges, i, j, i, j, i + 1, j, i + 1, j);
			}
			else if (iF01 < 0) {
				// 00+-
				addEdge(splits, edges, i, j, i, j, i, j, i + 1, j);
				addEdge(splits, edges, i, j, i + 1, j, i + 1, j, i + 1, j);
				addEdge(splits, edges, i, j, i + 1, j, i, j + 1, i + 1, j + 1);
			}
			else {
				// 00+0
				addEdge(splits, edges, i + 1, j, i + 1, j, i + 1, j + 1, i + 1,
					j + 1);
				addEdge(splits, edges, i + 1, j + 1, i + 1, j + 1, i, j + 1, i,
					j + 1);
			}
		}
		else if (iF01 != 0) {
			// cases 000+ or 000-
			addEdge(splits, edges, i, j, i, j, i + 1, j, i + 1, j);
			addEdge(splits, edges, i + 1, j, i + 1, j, i + 1, j + 1, i + 1,
				j + 1);
		}
		else {
			// case 0000
			addEdge(splits, edges, i, j, i, j, i + 1, j, i + 1, j);
			addEdge(splits, edges, i + 1, j, i + 1, j, i + 1, j + 1, i + 1,
				j + 1);
			addEdge(splits, edges, i + 1, j + 1, i + 1, j + 1, i, j + 1, i,
				j + 1);
			addEdge(splits, edges, i, j + 1, i, j + 1, i, j, i, j);
		}
	}
	float IsoContour::getValue(int i, int j) {
		float val = (*img)(i,j) - isoLevel;
		if (nudgeLevelSet) {
			if (val < 0) {
				val = std::min(val, -LEVEL_SET_TOLERANCE);
			}
			else {
				val = std::max(val, LEVEL_SET_TOLERANCE);
			}
		}
		return val;
	}
	float IsoContour::fGetOffset(uint2 v1, uint2 v2) {
		float fValue1 = getValue(v1.x, v1.y);
		float fValue2 = getValue(v2.x, v2.y);
		double fDelta = fValue2 - fValue1;
		if (fDelta == 0.0) {
			return 0.5f;
		}
		return (float)(-fValue1 / fDelta);
	}

	void IsoContour::addEdge(std::map<uint64_t, EdgeSplitPtr>& splits, std::list<EdgePtr>& edges, int p1x, int p1y, int p2x, int p2y, int p3x, int p3y, int p4x, int p4y) {
		EdgeSplitPtr split1 = createSplit(splits, p1x, p1y, p2x, p2y);
		EdgeSplitPtr split2 = createSplit(splits, p3x, p3y, p4x, p4y);
		lastSplit1 = split1;
		lastSplit2 = split2;
		EdgePtr edge = EdgePtr(new Edge(split1->vid, split2->vid));
		if (split1->e1.get() == nullptr) {
			split1->e1 = edge;
		}
		else {
			split1->e2 = edge;
		}
		if (split2->e1.get() == nullptr) {
			split2->e1 = edge;
		}
		else {
			split2->e2 = edge;
		}
		edges.push_back(edge);
	}
	EdgeSplitPtr IsoContour::createSplit(std::map<uint64_t, EdgeSplitPtr>& splits, int p1x, int p1y, int p2x, int p2y) {
		EdgeSplitPtr split=EdgeSplitPtr(new EdgeSplit(uint2(p1x, p1y), uint2(p2x, p2y)));
		uint64_t hash = split->hashValue(rows, cols);
		if (splits.find(hash) != splits.end()) {
			EdgeSplitPtr foundSplit = splits[hash];
			return foundSplit;
		} else {
			split->vid = vertCount++;
			float2 pt2d(0.0f);
			float fOffset = fGetOffset(split->pt1, split->pt2);
			float fInvOffset = 1.0f - fOffset;
			pt2d.x = (fInvOffset * p1x + fOffset * p2x);
			pt2d.y = (fInvOffset * p1y + fOffset * p2y);
			split->pt2d = pt2d;
			splits[split->hashValue(rows,cols)]= split;
			return split;
		}
	}
}