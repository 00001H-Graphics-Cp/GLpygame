#ifndef CHLIBS_H
#define CHLIBS_H

#include<iostream>
#include<string>
#include<stdexcept>
#include<unordered_map>
#include<glad/glad.h>
#include<ft2build.h>
#include FT_FREETYPE_H
class FTError:public std::logic_error{
    using std::logic_error::logic_error;
};
struct Ch_Texture{
    GLuint texture;
    GLuint64 handle;
    int width;
    int height;
    int xoffset;
    int yoffset;
    int distance;
};
class _Chlib_Font{
    public:
        FT_Face face;
        int _id;
        std::unordered_map<wchar_t,Ch_Texture> charmap;
        void set_dimensions(int w,int h){
            FT_Set_Pixel_Sizes(face,w,h);
        }
        Ch_Texture loadChar(wchar_t ch){
            if(charmap.count(ch)){
                return charmap[ch];
            }
            int x;
            if((x = FT_Load_Char(face,ch,FT_LOAD_RENDER|FT_LOAD_TARGET_LIGHT))){
                std::cerr << "Loadchar failed:";
                std::wcerr << ch;
                std::cerr << " " << x << std::endl;
                throw FTError("Unable to load chararcter.");
            }
            GLuint texture;
            glGenTextures(1,&texture);
            glBindTexture(GL_TEXTURE_2D,texture);
            glTexImage2D(GL_TEXTURE_2D,0,GL_RED,face->glyph->bitmap.width,face->glyph->bitmap.rows,0,GL_RED,GL_UNSIGNED_BYTE,face->glyph->bitmap.buffer);
            glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
            GLuint64 handle = glGetTextureHandleARB(texture);
            glMakeTextureHandleResidentARB(handle);
            Ch_Texture chtx;
            chtx.texture = texture;
            chtx.handle = handle;
            chtx.width = face->glyph->bitmap.width;
            chtx.height = face->glyph->bitmap.rows;
            chtx.xoffset = face->glyph->bitmap_left;
            chtx.yoffset = face->glyph->bitmap_top;
            chtx.distance = face->glyph->advance.x;
            charmap.emplace(ch,chtx);
            return chtx;
        }
        void done(){
            FT_Done_Face(face);
        }
};
#define Font _Chlib_Font*
#define NewFont() (new _Chlib_Font())
namespace std{
    template<>
    class hash<Font>{
        std::size_t operator()(const _Chlib_Font &fnt) const noexcept{
            return std::hash<int>{}(fnt._id);
        }
    };
}
class Chlib{
    private:
        int _font_id=0;
    public:
        FT_Library ftlib;
        std::unordered_map<std::wstring,Font> fonts;
        Chlib(){
            if(FT_Init_FreeType(&ftlib)){
                throw FTError("FreeType initialization failed!");
            }
        }
        Font getfont(std::wstring name,const char *orfile=nullptr){
            if(fonts.count(name)){
                return fonts[name];
            }else if(orfile==nullptr){
                throw FTError("Requested for unknown font without file!");
            }else{
                FT_Face face;
                if(FT_New_Face(ftlib,orfile,0,&face)){//Doesn't support wchar_t
                    throw FTError("Cannot load face!");
                }
                Font font = NewFont();
                font->_id = _font_id++;
                font->face = face;
                fonts.emplace(name,font);
                return font;
            }
        }
};
#endif//CHLIBS_H
