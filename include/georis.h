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
        OT_CIRCLE,
        OT_ARC,
        OT_SPLINE,
        OT_LINDIM,
        OT_CIRCDIM
        };

enum ConstraintType{
	CT_FIX = 0,
    CT_EQUAL,      //1
    CT_VERTICAL,   //2
    CT_HORIZONTAL, //3
    CT_DISTANCE,   //4
    CT_ANGLE,      //5
    CT_PARALLEL,   //6
    CT_ORTHO,      //7
    CT_TANGENT,    //8
    CT_COINCIDENT, //9
    CT_MIDPOINT,   //10
    CT_COLLINEAR,  //11
    CT_DIMENSION,  //12
    CT_SYMMETRIC,  //13
    CT_CONCENTRIC  //14
	};
}
