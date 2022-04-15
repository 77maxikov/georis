#pragma once

namespace georis{

enum InputMode{
    IM_NONE=0,
    IM_POINT,
    IM_SEGMENT,
    IM_LINE,
    IM_RAY,
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
    OT_LINE,
    OT_RAY,
};

enum CompositeType{
    COT_RECT,
    COT_RECT3P
};

enum ConstraintType{
    CT_FIX = 0,
    // Равные отрезки, окружности равного диаметра
    CT_EQUAL,      //1
    CT_VERTICAL,   //2
    CT_HORIZONTAL, //3
    // Расстояние между точками, от точки до прямой
    CT_DISTANCE,   //4
    // Угол между прямыми
    CT_ANGLE,      //5
    CT_PARALLEL,   //6
    CT_ORTHO,      //7
    CT_TANGENT,    //8
    CT_COINCIDENT, //9
    CT_MIDPOINT,   //10
    CT_COLLINEAR,  //11
    CT_DIMENSION,  //12
    CT_SYMMETRIC,  //13
    CT_CONCENTRIC, //14
    CT_ARCBASIC,   //15
    CT_VDIST,      //16
    CT_HDIST       //17
};
enum ErrorType{
    ET_EQ_PARAM = 0,
    ET_CONST,
    ET_EQ_SS_LEN,
    ET_DIST_P2P,
    ET_DIST_P2L,
    ET_DIST_P2PL,
    ET_ANGL_L2L,
    ET_PARL_L2L,
    ET_ORTO_L2L,
    ET_P_ON_L,
    ET_MIDVAL,
    ET_SYMM_PP2L
};

enum DCType{
    DCT_NONE = 0,
    DCT_LINDIM,
    DCT_HDIM,
    DCT_VDIM,
    DCT_CIRCDIM,
    DCT_ANGLEDIM
};

#define MODE_NORMAL        0x0
#define MODE_SELECTED      0x1
#define MODE_FIXED         0x2
#define MODE_HIGHLIGHTED   0x4
#define MODE_CONSTRUCTI    0x8
#define MODE_CONSTRAINED   0x10
#define MODE_DIMLINE       0x20

}
