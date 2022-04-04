#include "fontdata.h"
#include <stdexcept>
#include <cassert>

#include <ft2build.h>
#include FT_FREETYPE_H
#include <freetype/ftglyph.h>
#include <FL/gl.h>


void georis::FontData::init(const char* filename, unsigned pixheight){
    int error = 0;
     // Инициализация библиотеки FreeType.
     FT_Library library;
     if (error = FT_Init_FreeType( &library ))
         throw std::runtime_error("FT_Init_FreeType failed");

     // Объект для хранения шрифта.
     FT_Face face;

     // Загрузим шрифт из файла. Если файла шрифта не существует или шрифт битый,
     // то программа может умереть.
     if (error = FT_New_Face( library, filename, 0, &face ))
         throw std::runtime_error("FT_New_Face failed (there is probably a problem with your font file)");

     // По некоторым причинам FreeType измеряет размер шрифта в терминах 1/64 пикселя.
     // Таким образом, для того чтобы сделать шрифт выстой h пикселей, мы запрашиваем размер h*64.
     // (h << 6 тоже самое что и h*64)

     FT_Set_Char_Size( face, 0, pixheight << 6, 96, 96);

      charinfo.resize(alphabet.size());

     for (auto alpha : alphabet){
         if(FT_Load_Glyph( face, FT_Get_Char_Index( face, alpha ), FT_LOAD_DEFAULT ))
             throw std::runtime_error("FT_Load_Glyph failed");

         int charindex = getCharIndex(alpha);
         if ( charindex < 0 ) throw std::runtime_error("No index for alpha");

         // Fill in glyph dimensions
         charinfo[charindex].ax = face->glyph->advance.x;
         charinfo[charindex].ay = face->glyph->advance.y;
         charinfo[charindex].bt = face->glyph->bitmap_top;
         charinfo[charindex].bl = face->glyph->bitmap_left;
         charinfo[charindex].bw = face->glyph->bitmap.width;
         charinfo[charindex].bh = face->glyph->bitmap.rows;

         charinfo[charindex].contours.resize(face->glyph->outline.n_contours );
         short ncont = 0;
         for (short p = 0; p< face->glyph->outline.n_points ;++ p){
             charinfo[charindex].contours[ncont].push_back({face->glyph->outline.points[p].x,
                                         face->glyph->outline.points[p].y,
                                         (face->glyph->outline.tags[p] & 1)==0 });

             if ( face->glyph->outline.contours[ncont] == p)
                 ++ncont;

         }
     }

     // Уничтожим шрифт.
     FT_Done_Face(face);

     // Не нужна и библиотека.
     FT_Done_FreeType(library);
}
void genBezierPoints(float pts[8],unsigned order, int numpoints = 30){
    assert (order == 3 || order == 4);

    static float coords[4][3];

    coords[0][0] = pts[0];
    coords[0][1] = pts[1];
    coords[0][2] = 0;

    coords[1][0] = pts[2];
    coords[1][1] = pts[3];
    coords[1][2] = 0;

    coords[2][0] = pts[4];
    coords[2][1] = pts[5];
    coords[2][2] = 0;
    if ( order == 4 ){
        coords[3][0] = pts[6];
        coords[3][1] = pts[7];
        coords[3][2] = 0;
    }

    glMap1f(GL_MAP1_VERTEX_3, 0.0f, 1.0f, 3, order, (const float*)coords);
    glEnable(GL_MAP1_VERTEX_3);

    glBegin(GL_LINE_STRIP);
    for(int i=0; i<numpoints; i++)
        glEvalCoord1f((float)i/numpoints);

    if ( order == 3 )
        glVertex3f(pts[4],pts[5],0);
    else
        glVertex3f(pts[6],pts[7],0);
    glEnd();

    glDisable(GL_MAP1_VERTEX_3);
}

void georis::FontData::render_text(const char* text,TextJustify tj) {

     glMatrixMode(GL_MODELVIEW);
     glPushMatrix();

     const float scaleFactor = 1.0f/4096/12;

     // Calculate bbox width for rendered text
     const char *tmp = text;
     float bboxWidth = 0;
     while ( *tmp != '\0' ){
         int alpha = 0;
         if ( ('0' <= *text && *text <= '9') || '.' == *text ) alpha = *text;
         if ( 'D' == *text ) alpha = 0x2300;
         int charindex = getCharIndex(alpha);
         if ( charindex < 0 ) {
             //throw std::runtime_error("No index for alpha");
             ++tmp;
             continue;
         }
         bboxWidth += charinfo[charindex].ax*scaleFactor;
         ++tmp;
     }
     // Apply text justification
     switch (tj){
     case TJ_CENTER:
         glTranslatef(-bboxWidth/2,0,0);
         break;
     case TJ_RIGHT:
         glTranslatef(-bboxWidth,0,0);
         break;
     }

     glLineWidth(1);
     while (*text){
         int alpha = 0;
         if ( ('0' <= *text && *text <= '9') || '.' == *text ) alpha = *text;
         if ( 'D' == *text ) alpha = 0x2300;

         int charindex = getCharIndex(alpha);
         if ( charindex < 0 ) {
             ++text;
             continue;
             //throw std::runtime_error("No index for alpha");
         }
         // Draw outline contours
         for (auto &cont :charinfo[charindex].contours ){
             bool need2close = true;
             size_t k = 0;
             while ( k < cont.size() ){
                 glBegin(GL_LINE_STRIP);

                 while ( k < cont.size() ){
                     glVertex2f(static_cast<float>(cont[k].x)*scaleFactor,static_cast<float>(cont[k].y)*scaleFactor);
                     if ( k+1 >= cont.size() ) {++k; break;}
                     else if ( cont[k+1].control ) break;
                     ++k;
                 }
                 glEnd();
                 if ( k >= cont.size() ) break;
                 else{
                     //glVertex2f(static_cast<float>(pointdata[k].x)*scaleFactor,static_cast<float>(pointdata[k].y)*scaleFactor);
                     float pts[8];
                     pts[0] = static_cast<float>(cont[k].x)*scaleFactor;
                     pts[1] = static_cast<float>(cont[k].y)*scaleFactor;
                     pts[2] = static_cast<float>(cont[k+1].x)*scaleFactor;
                     pts[3] = static_cast<float>(cont[k+1].y)*scaleFactor;

                     if ( k+2 < cont.size() ){
                         pts[4] = static_cast<float>(cont[k+2].x)*scaleFactor;
                         pts[5] = static_cast<float>(cont[k+2].y)*scaleFactor;
                         if ( !cont[k+2].control ){
                             genBezierPoints(pts,3);
                             k += 2;
                         }
                         else {
                             if ( k+3 < cont.size() ){
                                 pts[6] = static_cast<float>(cont[k+3].x)*scaleFactor;
                                 pts[7] = static_cast<float>(cont[k+3].y)*scaleFactor;
                             }
                             else {
                                 pts[6] = static_cast<float>(cont.front().x)*scaleFactor;
                                 pts[7] = static_cast<float>(cont.front().y)*scaleFactor;
                                 need2close = false;
                             }
                             genBezierPoints(pts,4);
                             k+=3;
                         }
                     }
                     else {
                         pts[4] = static_cast<float>(cont.front().x)*scaleFactor;
                         pts[5] = static_cast<float>(cont.front().y)*scaleFactor;
                         need2close = false;
                         genBezierPoints(pts,3);
                         k += 2;
                     }
                 }
             }
             if (need2close){
                 glBegin(GL_LINES);
                 glVertex2f(static_cast<float>(cont.front().x)*scaleFactor,static_cast<float>(cont.front().y)*scaleFactor);
                 glVertex2f(static_cast<float>(cont.back().x)*scaleFactor,static_cast<float>(cont.back().y)*scaleFactor);
                 glEnd();
             }
         }
         glTranslatef(charinfo[charindex].ax*scaleFactor,0,0);
         ++text;
     }
     glPopMatrix();
}

