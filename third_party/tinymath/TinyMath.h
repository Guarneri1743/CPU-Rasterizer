////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Introduction:																							  //
// A math lib with minimal functionality that graphical works require, mainly used by my personal projects.   //																	  //
// 																											  //
// References:																							      //
// https://github.com/polymonster/maths																		  //
// https://en.wikipedia.org/wiki/Swizzling_(computer_graphics)												  //
// https://stackoverflow.com/questions/51641131/how-to-achieve-vector-swizzling-in-c                          //
// https://github.com/g-truc/glm																			  //
////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include "Vector2.h"
#include "Vector3.h"
#include "Vector4.h"
#include "Vector2Int.h"
#include "Vector3Int.h"
#include "Vector4Int.h"
#include "Matrix3x3.h"
#include "Matrix4x4.h"
#include "Computation.h"

#include "primitives/AxisAlignedBoundingBox.h"
#include "primitives/Capsule.h"
#include "primitives/Cylinder.h"
#include "primitives/Line.h"
#include "primitives/Plane.h"
#include "primitives/Ray.h"
#include "primitives/Rect.h"
#include "primitives/Segment.h"
#include "primitives/Sphere.h"

#include "color/Color.h"
#include "color/ColorEncoding.h"