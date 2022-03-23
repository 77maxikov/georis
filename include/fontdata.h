#ifndef FONTDATA_H
#define FONTDATA_H
#include <vector>

namespace georis {
class FontData {
public:
    enum TextJustify{TJ_LEFT,TJ_CENTER,TJ_RIGHT};
    void init(const char* filename, unsigned pixheight);
    struct outlinepoint{
        long x;
        long y;
        int control;
    };
    typedef  std::vector<std::vector<outlinepoint> > symbolcontours;
    struct character_info {
        float ax; // advance.x
        float ay; // advance.y

        float bw; // bitmap.width;
        float bh; // bitmap.rows;

        float bl; // bitmap_left;
        float bt; // bitmap_top;

        symbolcontours contours;
    };
    std::vector<character_info> charinfo;

    std::vector<int> alphabet = {'0', '1','2','3','4','5','6','7','8','9','.',0x2300};
    static int getCharIndex(int c){
        if ( '0' <= c && c <= '9' ) return c - '0';
        if ( c == ',' || c == '.' ) return 10;
        if ( c == 0x2300 ) return 11;
        return -1;
    }
    void render_text(const char*, TextJustify tj = TJ_CENTER);
};
};

#endif // FONTDATA_H
