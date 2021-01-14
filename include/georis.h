#pragma once

namespace georis{

enum InputMode{
    IM_NONE=0,
    IM_POINT,
    IM_LINE,
    IM_CIRCLE,
    IM_ARC,
    IM_RECT
    };

enum ObjectType{
        OT_NONE=0,
        OT_POINT,
        OT_SEGMENT,
        OT_PLANE,
        OT_CIRCLE,
        OT_ARC,
        OT_SPLINE,
        OT_LINDIM,
        OT_CIRCDIM
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
