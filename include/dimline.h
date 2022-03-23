#ifndef DIMLINE_H
#define DIMLINE_H
#include <vector>

#include "core.h"
#include "common_math.h"
#include "geommath.h"

namespace georis{
struct sketchDim{
    virtual double dist2point(double x,double y)const = 0;
    virtual bool inRect(double xtl,double ytl,double xbr,double ybr)const = 0;
    virtual void move(double dx,double dy) = 0;
    virtual std::vector<double> param()const = 0;
};

struct linDim :public sketchDim {
    UID idbeg;
    UID idend;
    double dimdx;
    double dimdy;
    Core *core;

    double dist2point(double x,double y) const {
        std::vector<double> v1;
        // get endpoints coords
        core->getObjParam(idbeg,v1);
        std::vector<double> v2;
        core->getObjParam(idend,v2);

        double p1x = v1[0] + dimdy*(v2[1]-v1[1]);
        double p1y = v1[1] + dimdy*(v1[0]-v2[0]);
        double p2x = v2[0] + dimdy*(v2[1]-v1[1]);
        double p2y = v2[1] + dimdy*(v1[0]-v2[0]);
        double mindist = psegdist2(x,y,v1[0],v1[1],p1x,p1y);
        mindist = std::min(mindist,psegdist2(x,y,p1x,p1y,p2x,p2y));
        mindist = std::min(mindist,psegdist2(x,y,p2x,p2y,v2[0],v2[1]));
        return mindist;
    }
    bool inRect(double xtl,double ytl,double xbr,double ybr)const{
        std::vector<double> v1;
        // get endpoints coords
        core->getObjParam(idbeg,v1);
        std::vector<double> v2;
        core->getObjParam(idend,v2);

        double p1x = v1[0] + dimdy*(v2[1]-v1[1]);
        double p1y = v1[1] + dimdy*(v1[0]-v2[0]);
        double p2x = v2[0] + dimdy*(v2[1]-v1[1]);
        double p2y = v2[1] + dimdy*(v1[0]-v2[0]);

        AABBr rec(xtl,ybr,xbr,ytl);
        return rec.contains({v1[0],v1[1]}) &&
                rec.contains({v2[0],v2[1]}) &&
                rec.contains({p1x,p1y}) &&
                rec.contains({p2x,p2y});
    }
    void move(double dx,double dy){

    }
    std::vector<double> param()const {
        std::vector<double> res;
        // get endpoints coords
        core->getObjParam(idbeg,res);
        std::vector<double> eppar;
        core->getObjParam(idend,eppar);
        std::move(eppar.begin(), eppar.end(), std::back_inserter(res));  // ##
        // dimension text pos
        res.push_back(dimdx);
        res.push_back(dimdy);
        return res;
    }

};
struct circDim:public sketchDim {
    UID idcenter;
    double dimdir;
    double dimr;
    double offset;
    Core *core;

    double dist2point(double x,double y) const {
        std::vector<double> centercoord;
        // get endpoints coords
        core->getObjParam(idcenter,centercoord);

        double p1x = centercoord[0] + (offset>1?offset:1.0)*dimr*cosd(dimdir);
        double p1y = centercoord[1] + (offset>1?offset:1.0)*dimr*sind(dimdir);
        return psegdist2(x,y,centercoord[0],centercoord[1],p1x,p1y);
    }
    bool inRect(double xtl,double ytl,double xbr,double ybr)const{
        std::vector<double> v1;
        // get centerpoint coords
        core->getObjParam(idcenter,v1);
        double p1x = v1[0] + offset*dimr*cosd(dimdir);
        double p1y = v1[1] + offset*dimr*sind(dimdir);

        AABBr rec(xtl,ybr,xbr,ytl);
        return rec.contains({v1[0],v1[1]}) &&
                rec.contains({p1x,p1y});
    }
    void move(double dx,double dy){

    }
    std::vector<double> param()const{
        std::vector<double> res;
        // get centerpoint coords
        core->getObjParam(idcenter,res);
        // dimension text pos
        res.push_back(dimdir);
        res.push_back(offset);
        return res;
    }
};

struct angDim:public sketchDim {
    UID idbeg1;
    UID idend1;
    UID idbeg2;
    UID idend2;
    double dimdir;
    double dimr;
    Core *core;

    double dist2point(double x,double y) const {
        std::vector<double> par = param();

        double phi = rad2deg(atan2(y-par[1],x-par[0]));

        if ( par[2] <= phi && phi <= par[3] )
            return ppdist2( par[0] + dimr*cosd(phi),par[1] + dimr*sind(phi),x,y);

        return std::min(ppdist2(par[0] + dimr*cosd(par[2]),par[1] + dimr*sind(par[2]),x,y),
                        ppdist2(par[0] + dimr*cosd(par[3]),par[1] + dimr*sind(par[3]),x,y));
    }
    bool inRect(double xtl,double ytl,double xbr,double ybr)const{
        std::vector<double> v1;
        // get endpoints coords
        core->getObjParam(idbeg1,v1);
        std::vector<double> v2;
        core->getObjParam(idend1,v2);

        AABBr rec(xtl,ybr,xbr,ytl);

        bool b1 = rec.contains({v1[0],v1[1]}) &&
                rec.contains({v2[0],v2[1]});
        core->getObjParam(idbeg2,v1);
        core->getObjParam(idend2,v2);


        return rec.contains({v1[0],v1[1]}) &&
                rec.contains({v2[0],v2[1]}) && b1;
    }
    void move(double dx,double dy){

    }
    std::vector<double> param()const{        
        // get endpoints coords
        std::vector<double> s1b;
        core->getObjParam(idbeg1,s1b);
        std::vector<double> s1e;
        core->getObjParam(idend1,s1e);
        std::vector<double> s2b;
        core->getObjParam(idbeg2,s2b);
        std::vector<double> s2e;
        core->getObjParam(idend2,s2e);

        std::vector<double> res(6);
        // res[0:1] - segments' crossing coords
        segcross(s1b[0],s1b[1],s1e[0],s1e[1],s2b[0],s2b[1],s2e[0],s2e[1],res[0],res[1]);
        // res[2] - arc begin angle
        if (std::abs(s1b[1] - res[1]) < epsi && std::abs(s1b[0] - res[0]) < epsi )
            res[2] = rad2deg(atan2(s1e[1] - res[1],s1e[0] - res[0]));
        else
            res[2] = rad2deg(atan2(s1b[1] - res[1],s1b[0] - res[0]));
        // res[3] - arc end angle
        if (std::abs(s2b[1] - res[1]) < epsi && std::abs(s2b[0] - res[0]) < epsi )
            res[3] = rad2deg(atan2(s2e[1] - res[1],s2e[0] - res[0]));
        else
            res[3] = rad2deg(atan2(s2b[1] - res[1],s2b[0] - res[0]));
        // res[4] - dim arc radii
        res[4] = dimr;
        // res[5] - dim text dir
        res[5] = dimdir;

        return res;
    }
};



};


#endif // DIMLINE_H
