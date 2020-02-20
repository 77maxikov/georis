#include "ConstrL2L.h"
#include <stdexcept>

georis::ConstrL2L::ConstrL2L(const georis::line2r &l1, const georis::line2r &l2) {
    if (l1.beg.x == 0) throw std::invalid_argument("ConstrL2L::invalid line 1");
    l11x = l1.beg.x;
    if (l1.beg.y == 0) throw std::invalid_argument("ConstrL2L::invalid line 1");
    l11y = l1.beg.y;
    if (l1.end.x == 0) throw std::invalid_argument("ConstrL2L::invalid line 1");
    l12x = l1.end.x;
    if (l1.end.y == 0) throw std::invalid_argument("ConstrL2L::invalid line 1");
    l12y = l1.end.y;
    if (l2.beg.x == 0) throw std::invalid_argument("ConstrL2L::invalid line 2");
    l21x = l2.beg.x;
    if (l2.beg.y == 0) throw std::invalid_argument("ConstrL2L::invalid line 2");
    l21y = l2.beg.y;
    if (l2.end.x == 0) throw std::invalid_argument("ConstrL2L::invalid line 2");
    l22x = l2.end.x;
    if (l2.end.y == 0) throw std::invalid_argument("ConstrL2L::invalid line 2");
    l22y = l2.end.y;
}
georis::ConstrL2L::ConstrL2L(const georis::point2r &beg,const georis::point2r &end, const georis::line2r &l2) {
    if (beg.x == 0) throw std::invalid_argument("ConstrL2L::invalid beg");
    l11x = beg.x;
    if (beg.y == 0) throw std::invalid_argument("ConstrL2L::invalid line 1");
    l11y = beg.y;
    if (end.x == 0) throw std::invalid_argument("ConstrL2L::invalid line 1");
    l12x = end.x;
    if (end.y == 0) throw std::invalid_argument("ConstrL2L::invalid line 1");
    l12y = end.y;
    if (l2.beg.x == 0) throw std::invalid_argument("ConstrL2L::invalid line 2");
    l21x = l2.beg.x;
    if (l2.beg.y == 0) throw std::invalid_argument("ConstrL2L::invalid line 2");
    l21y = l2.beg.y;
    if (l2.end.x == 0) throw std::invalid_argument("ConstrL2L::invalid line 2");
    l22x = l2.end.x;
    if (l2.end.y == 0) throw std::invalid_argument("ConstrL2L::invalid line 2");
    l22y = l2.end.y;
}
