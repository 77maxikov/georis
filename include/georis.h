#pragma once

namespace georis{

enum ObjectType{
        OT_NONE=0,
        OT_POINT,
        OT_SEGMENT,
        OT_PLANE,
        OT_CIRCLE,
        OT_ARC,
        OT_SPLINE
        };

enum ConstraintType{
	CT_FIX = 0,
	CT_EQUAL,
	CT_VERTICAL,
	CT_HORIZONTAL,
	CT_DISTANCE,
	CT_ANGLE,
	CT_PARALLEL,
	CT_ORTHO,
	CT_TANGENT,
	CT_COINCIDENT,
    CT_MIDPOINT,
	CT_COLLINEAR,
	CT_DIMENSION,
	CT_SYMMETRIC,
	CT_CONCENTRIC
	};
}
