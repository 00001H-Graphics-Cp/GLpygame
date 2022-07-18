#ifndef CHLIBS_H
#define CHLIBS_H

#include<iostream>
#include<string>
#include<stdexcept>
#include<unordered_map>
#include<glad/glad.h>
#include<ft2build.h>
#include<texture.hpp>
#include FT_FREETYPE_H

namespace pygame{
    class FTError:public std::logic_error{
        using std::logic_error::logic_error;
    };
    struct Ch_Texture{
        pTexture tex;
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
                auto glyf = face->glyph;
                auto bmap = glyf->bitmap;
                pTexture t;
                unsigned char colorbyte = 0;
                if(bmap.width==0 && bmap.rows==0){
                    t = std::make_shared<Texture>(&colorbyte,1,1,GL_RED,GL_RED,GL_NEAREST,GL_NEAREST,false);
                }else{
                    t = std::make_shared<Texture>(bmap.buffer,bmap.width,bmap.rows,GL_RED,GL_RED,GL_LINEAR,GL_LINEAR,false);
                }
                Ch_Texture chtx;
                chtx.tex = t;
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
}
namespace std{
    template<>
    class hash<pygame::Font>{
        std::size_t operator()(const pygame::_Chlib_Font &fnt) const noexcept{
            return std::hash<int>{}(fnt._id);
        }
    };
}
#endif//CHLIBS_H
