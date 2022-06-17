#ifndef GSDL_H
#define GSDL_H

#include<algorithm>
#include<string>
#include<stdexcept>
#include<memory>

#include<glad/glad.h>
#include<glm/glm.hpp>
#include<glm/gtc/type_ptr.hpp>
#include<glm/gtc/matrix_transform.hpp>
#define STB_IMAGE_IMPLEMENTATION
#include"stb_image.h"
#undef STB_IMAGE_IMPLEMENTATION

#include"chlibs.hpp"
#include"math.hpp"
#include"fileutils.hpp"

namespace pygame{
    #define inherits : public
    class error inherits std::logic_error{
        using std::logic_error::logic_error;
    };
    class shader_error inherits error{
        using error::error;
    };
    class vshad_compilation_failed inherits shader_error{
        using shader_error::shader_error;
    };
    class fshad_compilation_failed inherits shader_error{
        using shader_error::shader_error;
    };
    class program_linking_failed inherits shader_error{
        using shader_error::shader_error;
    };
    #undef inherits


    typedef vec2 Point;
    typedef vec4 Color;
    struct Rect{
        float x;
        float y;
        float w;
        float h;
        Rect() noexcept = default;
        Rect(float x,float y,float w,float h) : x(x),y(y),w(w),h(h) {}
        bool inline colliderect(Rect &other) const{
            return ((x<(other.y+other.w))//left is lefter than other right
                  &&((x+w)>(other.x))//right is righter than other left
                  &&(y<(other.y+other.h))//top is topper than other bottom//note:bigger y = lower
                  &&((y+h)>(other.y)));//bottom is bottomer than other top
        }
        bool inline collidepoint(Point point) const{
            return (((point.x)<(x+w))//lefter than right
                  &&((point.x)>x)//righter than left
                  &&((point.y)<(y+h))//topper than bottom
                  &&((point.y)>y));//bottomer than top
        }
    };
    struct Cube{//Note: 2d is screen space but 3d is world space
        glm::vec3 pos;
        float w;//x-wise
        float h;//y-wise
        float l;//z-wise
        float xrot=0.0;
        float yrot=0.0;
        float zrot=0.0;
        Cube() = default;
        Cube(float x,float y,float z,float w,float h,float l)
        : pos(x,y,z),w(w),h(h),l(l) {}
        glm::vec3 centerD() const{
            return glm::vec3(w/2.0,h/2.0,l/2.0);
        }
        glm::vec3 inline center() const{
            return pos+centerD();
        }
        void inline set_center(glm::vec3 center){
            pos = center-centerD();
        }
        
    };
    struct Texture{
        int w;
        int h;
        int color_channels;
        GLuint texturei;
        GLuint64 texturehandle;
        private:bool initialized = false;
        public:
        Texture(GLuint64 handle) : texturehandle(handle) {
            glMakeTextureHandleResidentARB(handle);
            initialized = true;
        }
        Texture(){
            texturehandle = -1;
        }
        ~Texture(){
            if(initialized)glMakeTextureHandleNonResidentARB(texturehandle);
        }
    };
    struct CubeTexture{
        public:
            Texture front;
            Texture back;
            Texture top;
            Texture bottom;
            Texture left;
            Texture right;
            CubeTexture(Texture face){
                front=back=top=bottom=left=right=face;
            }
    };
    std::unordered_map<std::string,Texture> texturecache;
    Texture loadTexture(const char* filename){
        if(texturecache.count((std::string)filename)){
            return texturecache[(std::string)filename];
        }

        int w,h,color_chnls;
        unsigned char *data = stbi_load(filename,&w,&h,&color_chnls,0);
        if(data==nullptr){
            throw pygame::error((std::string)"Unable to load texture: "+filename);
        }

        GLuint texture;
        glGenTextures(1,&texture);
        glBindTexture(GL_TEXTURE_2D,texture);
        glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_CLAMP_TO_BORDER);
        glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_CLAMP_TO_BORDER);
        float bordercolor[]={0.0,0.0,0.0,1.0};
        glTexParameterfv(GL_TEXTURE_2D,GL_TEXTURE_BORDER_COLOR,bordercolor);
        glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);

        glTexImage2D(GL_TEXTURE_2D,0,GL_RGBA16,w,h,0,((color_chnls==3)?(GL_RGB):(GL_RGBA)),GL_UNSIGNED_BYTE,data);
        glGenerateMipmap(GL_TEXTURE_2D);

        stbi_image_free(data);

        GLuint64 handle = glGetTextureHandleARB(texture);
        Texture tex = Texture(handle);
        tex.texturei = texture;
        tex.color_channels = color_chnls;
        tex.w = w;
        tex.h = h;
        texturecache.emplace((std::string)filename,tex);
        return tex;
    }
    GLuint loadprogram(const wchar_t*vs,const wchar_t*fs){
        using namespace std;
        GLint compiled;

        char infolog[8192];

        std::wstring vsrcutf16 = loadStringFile(vs);
        std::string vtxsrc(vsrcutf16.begin(),vsrcutf16.end());
        const char *vtxsrc_cstr = vtxsrc.c_str();
        GLuint vshad = glCreateShader(GL_VERTEX_SHADER);
        glShaderSource(vshad,1,&vtxsrc_cstr,NULL);
        glCompileShader(vshad);
        glGetShaderiv(vshad,GL_COMPILE_STATUS,&compiled);
        if(!compiled){
            glGetShaderInfoLog(vshad,8000,nullptr,infolog);
            throw vshad_compilation_failed(infolog);
        }

        std::wstring fsrcutf16 = loadStringFile(fs);
        std::string frgsrc(fsrcutf16.begin(),fsrcutf16.end());
        const char *frgsrc_cstr = frgsrc.c_str();
        GLuint fshad = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(fshad,1,&frgsrc_cstr,NULL);
        glCompileShader(fshad);
        glGetShaderiv(fshad,GL_COMPILE_STATUS,&compiled);
        if(!compiled){
            glGetShaderInfoLog(fshad,8000,nullptr,infolog);
            throw fshad_compilation_failed(infolog);
        }

        GLuint program = glCreateProgram();
        glAttachShader(program,vshad);
        glAttachShader(program,fshad);
        glLinkProgram(program);

        GLint linked;
        glGetProgramiv(program,GL_LINK_STATUS,&linked);
        if(!linked){
            glGetProgramInfoLog(program,8000,nullptr,infolog);
            throw program_linking_failed(infolog);
        }
        glDeleteShader(vshad);
        glDeleteShader(fshad);
        return program;
    }
    struct Shader{
        GLuint program=-1;
        std::unordered_map<std::string,GLint> locations;
        GLint inline getLocation(const char* location){
            if(locations.count(location)==0){
                locations.emplace(location,glGetUniformLocation(program,location));
            }
            return locations[location];
        }
    };
    GLuint square_vao,square_vbo,fill_vao,fill_vbo,texture_vao,texture_vbo;
    GLuint colored_polygon_vao,colored_polygon_vbo;
    GLuint texture_3d_vao,texture_3d_vbo;
    Shader texture_shader,text_shader,fill_shader,single_color_shader;
    Shader texture_3d_shader;

    GLfloat simple_quad[]={//GL_TRIANGLE_FAN
        0.0,0.0,0.0,0.0,
        0.0,1.0,0.0,1.0,
        1.0,1.0,1.0,1.0,
        1.0,0.0,1.0,0.0
    };
    void gsdlInit(){
        if(!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)){
            throw error("GLAD load failed! Did your forget to bind an OpenGL context?");
        }
        text_shader.program = loadprogram(L"rsrc/2d_textured_vertex.glsl",L"rsrc/text_rendering_fragment.glsl");
        texture_shader.program = loadprogram(L"rsrc/2d_textured_vertex.glsl",L"rsrc/texture_sampling_fragment.glsl");
        fill_shader.program = loadprogram(L"rsrc/2d_colored_vertex.glsl",L"rsrc/fill_fragment.glsl");
        single_color_shader.program = loadprogram(L"rsrc/2d_vertex.glsl",L"rsrc/single_color_fragment.glsl");
        texture_3d_shader.program = loadprogram(L"rsrc/3d_textured_vertex.glsl",L"rsrc/3d_textured_fragment.glsl");

        glGenVertexArrays(1,&square_vao);
        glGenBuffers(1,&square_vbo);
        glBindVertexArray(square_vao);
        glBindBuffer(GL_ARRAY_BUFFER,square_vbo);
        glBufferData(GL_ARRAY_BUFFER,sizeof(simple_quad),simple_quad,GL_STATIC_DRAW);
        glVertexAttribPointer(0,2,GL_FLOAT,GL_FALSE,4*sizeof(GLfloat),(void*)0);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(1,2,GL_FLOAT,GL_FALSE,4*sizeof(GLfloat),(void*)(2*sizeof(GLfloat)));
        glEnableVertexAttribArray(1);


        glGenVertexArrays(1,&texture_vao);
        glGenBuffers(1,&texture_vbo);
        glBindVertexArray(texture_vao);
        glBindBuffer(GL_ARRAY_BUFFER,texture_vbo);
        //2(xy)+2(st) = 4/vertex
        //let's support 20 vtx
        //that's 80*float
        glBufferData(GL_ARRAY_BUFFER,sizeof(GLfloat)*80,nullptr,GL_DYNAMIC_DRAW);
        glVertexAttribPointer(0,2,GL_FLOAT,GL_FALSE,4*sizeof(GLfloat),(void*)0);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(1,2,GL_FLOAT,GL_FALSE,4*sizeof(GLfloat),(void*)(2*sizeof(GLfloat)));
        glEnableVertexAttribArray(1);

        glGenVertexArrays(1,&fill_vao);
        glGenBuffers(1,&fill_vbo);
        glBindVertexArray(fill_vao);
        glBindBuffer(GL_ARRAY_BUFFER,fill_vbo);
        //2(xy)+3(rgb) = 5/vertex
        //let's support 20 vtx as well
        //that's 100*float
        glBufferData(GL_ARRAY_BUFFER,sizeof(GLfloat)*100,nullptr,GL_DYNAMIC_DRAW);
        glVertexAttribPointer(0,2,GL_FLOAT,GL_FALSE,5*sizeof(GLfloat),(void*)0);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(1,3,GL_FLOAT,GL_FALSE,5*sizeof(GLfloat),(void*)(2*sizeof(GLfloat)));
        glEnableVertexAttribArray(1);

        glGenVertexArrays(1,&colored_polygon_vao);
        glGenBuffers(1,&colored_polygon_vbo);
        glBindVertexArray(colored_polygon_vao);
        glBindBuffer(GL_ARRAY_BUFFER,colored_polygon_vbo);
        glBufferData(GL_ARRAY_BUFFER,sizeof(GLfloat)*64,nullptr,GL_DYNAMIC_DRAW);
        glVertexAttribPointer(0,2,GL_FLOAT,GL_FALSE,2*sizeof(GLfloat),(void*)0);
        glEnableVertexAttribArray(0);

        glGenVertexArrays(1,&texture_3d_vao);
        glGenBuffers(1,&texture_3d_vbo);
        glBindVertexArray(texture_3d_vao);
        glBindBuffer(GL_ARRAY_BUFFER,texture_3d_vbo);
        //xyz(3)+st(2)=5/vertex
        //Let's support 20 vtx
        //100 * glfloat
        glBufferData(GL_ARRAY_BUFFER,sizeof(GLfloat)*100,nullptr,GL_DYNAMIC_DRAW);
        glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,5*sizeof(GLfloat),(void*)0);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(1,2,GL_FLOAT,GL_FALSE,5*sizeof(GLfloat),(void*)(3*sizeof(GLfloat)));
        glEnableVertexAttribArray(1);

    }
    void gsdlDeinit(){
        glDeleteProgram(text_shader.program);
        glDeleteProgram(texture_shader.program);
        glDeleteProgram(fill_shader.program);
        glDeleteProgram(single_color_shader.program);
        glDeleteProgram(texture_3d_shader.program);
        glDeleteVertexArrays(1,&texture_vao);
        glDeleteVertexArrays(1,&colored_polygon_vao);
        glDeleteVertexArrays(1,&fill_vao);
        glDeleteVertexArrays(1,&texture_3d_vao);
        glDeleteBuffers(1,&texture_vbo);
        glDeleteBuffers(1,&colored_polygon_vbo);
        glDeleteBuffers(1,&fill_vbo);
        glDeleteBuffers(1,&texture_3d_vbo);
    }
    void blit(GLuint64 image,Point location,double size,int rotation=0){
        glBindVertexArray(square_vao);
        glBindBuffer(GL_ARRAY_BUFFER,square_vbo);
        GLint imglocation = texture_shader.getLocation("img");
        GLint poslocation = texture_shader.getLocation("position");
        GLint sizelocation = texture_shader.getLocation("size");
        GLint rotationlocation = texture_shader.getLocation("rotation");
        glUseProgram(texture_shader.program);
        glUniformHandleui64ARB(imglocation,image);
        glUniform2f(poslocation,location.x,location.y);
        glUniform1f(sizelocation,size);
        glUniform1i(rotationlocation,rotation);
        glDrawArrays(GL_TRIANGLE_FAN,0,4);
    }
    enum class align{
        LEFT,CENTER,RIGHT,
        TOP,BOTTOM
    };
    Rect get_text_rect(Font font,std::wstring text,Point position,float size,
                   align algn=align::LEFT){
        float textwidth=0.0;
        float topchrs=0.0;
        float bottomchrs=0.0;
        float xdelta=0.0;
        Ch_Texture ch;
        for(wchar_t chr : text){
            ch = font->loadChar(chr);
            textwidth += (ch.distance/64.0)*size;
            topchrs = std::max(topchrs,size*ch.yoffset);
            bottomchrs = std::min(bottomchrs,size*(ch.yoffset-ch.height));
        }
        if(algn==align::CENTER){
            xdelta -= textwidth/2.0;
        }else if(algn==align::RIGHT){
            xdelta -= textwidth;
        }else if(algn!=align::LEFT){
            assert(false);
        }
        return Rect(position.x+xdelta,position.y,textwidth,topchrs-bottomchrs);
    }
    Rect draw_text(Font font,std::wstring text,Point position,float size,
                   Color color={1.0,1.0,1.0,1.0},align algn=align::LEFT){
        float topchrs=0.0;
        Ch_Texture ch;
        for(wchar_t chr : text){
            ch = font->loadChar(chr);
            topchrs = std::max(topchrs,size*ch.yoffset);
        }
        glUseProgram(text_shader.program);
        glUniform4f(text_shader.getLocation("color"),color.x,color.y,color.z,color.w);
        glUniform1f(text_shader.getLocation("size"),size);
        glUniform1i(text_shader.getLocation("rotation"),0);
        glBindVertexArray(texture_vao);
        glBindBuffer(GL_ARRAY_BUFFER,texture_vbo);
        GLuint imgloc = text_shader.getLocation("img");
        Point charpos,posytion = position;
        Rect tr = get_text_rect(font,text,position,size,algn);
        posytion.x = tr.x;
        Point sz;
        GLint poslocation = text_shader.getLocation("position");
        for(wchar_t chr:text){
            ch = font->loadChar(chr);
            glUniformHandleui64ARB(imgloc,ch.handle);
            sz = {(float)ch.width,(float)ch.height};
            float vtx[16] = {
                0.0   ,-sz.y,0.0,0.0,
                0.0   , 0.0 ,0.0,1.0,
                sz.x  ,-sz.y,1.0,0.0,
                sz.x  , 0.0 ,1.0,1.0
            };
            glBufferSubData(GL_ARRAY_BUFFER,0,sizeof(vtx),vtx);
            charpos = posytion;
            charpos.x -= size*ch.xoffset;
            charpos.y += size*(ch.height-ch.yoffset);
            charpos.y += topchrs;
            glUniform2f(poslocation,charpos.x,charpos.y);
            glDrawArrays(GL_TRIANGLE_STRIP,0,16);
            posytion.x += (ch.distance/64.0)*size;
        }
        return tr;
    }
    namespace three_d{
        class Camera{
            public:
                glm::vec3 pos;
                float yaw,pitch,roll;
                Camera() = default;
                Camera(glm::vec3 pos,float yaw=0.0,float pitch=0.0,float roll=0.0) : pos(pos),yaw(yaw),pitch(pitch),roll(roll)
                {}
                glm::vec3 inline worldup() const{
                    float rroll = glm::radians(roll);
                    return glm::vec3(sin(rroll),cos(rroll),0);
                }
                glm::mat4 inline viewmatrix() const{
                    glm::mat4 trans = glm::mat4(1.0);
                    trans = glm::rotate(trans,-glm::radians(pitch),glm::vec3(1.0,0.0,0.0));
                    trans = glm::rotate(trans,glm::radians(yaw+90),glm::vec3(0.0,1.0,0.0));
                    trans = glm::rotate(trans,-glm::radians(roll),direction());
                    trans = glm::translate(trans,-pos);
                    return trans;
                }
                glm::vec3 inline direction() const{
                    float ryaw = glm::radians(yaw);
                    float rpitch = glm::radians(pitch);
                    return glm::normalize(glm::vec3(cos(ryaw)*cos(rpitch),sin(rpitch),sin(ryaw)*cos(rpitch)));
                }
                glm::vec3 inline xzfront() const{
                    float ryaw = glm::radians(yaw);
                    return glm::vec3(cos(ryaw),0,sin(ryaw));
                }
                glm::vec3 inline xzright() const{
                    float ryaw = glm::radians(yaw);
                    return glm::vec3(-sin(ryaw),0,cos(ryaw));
                }
                glm::vec3 inline right() const{
                    return glm::cross(direction(),up());
                }
                glm::vec3 inline up() const{
                    return glm::mat3(glm::rotate(glm::mat4(1.0),glm::radians(pitch),xzright()))*worldup();
                }
        };
    }//namespace three_d;
    using three_d::Camera;
    namespace draw{
        Rect rect(Rect in,Color color){
            glUseProgram(single_color_shader.program);
            glBindVertexArray(colored_polygon_vao);
            glBindBuffer(GL_ARRAY_BUFFER,colored_polygon_vbo);
            float vtx[8] = {
                0.0 ,0.0,
                0.0 ,in.h,
                in.w,in.h,
                in.w,0.0
            };
            glBufferSubData(GL_ARRAY_BUFFER,0,sizeof(vtx),vtx);
            glUniform2f(single_color_shader.getLocation("position"),in.x,in.y);
            glUniform1f(single_color_shader.getLocation("size"),1.0);
            glUniform4f(single_color_shader.getLocation("color"),color.x,color.y,color.z,color.w);
            glDrawArrays(GL_TRIANGLE_FAN,0,8);
            return in;
        }
        std::shared_ptr<three_d::Camera> cam = nullptr;
        struct PerspectiveInfo{
            float fov_degs=70;
            float near_clip=0.1;
            float far_clip=1000;
            float aspc=1;
        };
        PerspectiveInfo pinf;
        void set_camera(std::shared_ptr<three_d::Camera> camer){
            cam = camer;
        }
        Cube cube(Cube in,CubeTexture textures){
            if(cam==nullptr){
                throw error("No camera set!");
            }
            glUseProgram(texture_3d_shader.program);
            glBindVertexArray(texture_3d_vao);
            glBindBuffer(GL_ARRAY_BUFFER,texture_3d_vbo);
            glUniformMatrix4fv(texture_3d_shader.getLocation("view"),1,GL_FALSE,glm::value_ptr(cam->viewmatrix()));
            glm::mat4 projmat = glm::perspective(glm::radians(pinf.fov_degs),pinf.aspc,
                                pinf.near_clip,pinf.far_clip);
            glUniformMatrix4fv(texture_3d_shader.getLocation("projection"),1,GL_FALSE,glm::value_ptr(projmat));
            glm::mat4 model = glm::mat4(1.0);
            model = glm::translate(model,in.center());
            model = glm::rotate(model,-glm::radians(in.zrot),glm::vec3(0.0,0.0,-1.0));
            model = glm::rotate(model,-glm::radians(in.xrot),glm::vec3(1.0,0.0,0.0));
            model = glm::rotate(model,glm::radians(in.yrot),glm::vec3(0.0,1.0,0.0));
            model = glm::translate(model,-in.centerD());
            glUniformMatrix4fv(texture_3d_shader.getLocation("model"),1,GL_FALSE,glm::value_ptr(model));
            {
                float vtx[20] = {
                    0.0 ,in.h,0.0,1.0,1.0,
                    in.w,in.h,0.0,0.0,1.0,
                    0.0 ,0.0 ,0.0,1.0,0.0,
                    in.w,0.0 ,0.0,0.0,0.0
                };
                glBufferSubData(GL_ARRAY_BUFFER,0,sizeof(vtx),vtx);
                glUniformHandleui64ARB(texture_3d_shader.getLocation("tex"),textures.back.texturehandle);
                glDrawArrays(GL_TRIANGLE_STRIP,0,4);
            }
            {
                float vtx[20] = {
                    0.0 ,in.h,in.l,0.0,1.0,
                    0.0 ,0.0 ,in.l,0.0,0.0,
                    in.w,in.h,in.l,1.0,1.0,
                    in.w,0.0 ,in.l,1.0,0.0
                };
                glBufferSubData(GL_ARRAY_BUFFER,0,sizeof(vtx),vtx);
                glUniformHandleui64ARB(texture_3d_shader.getLocation("tex"),textures.front.texturehandle);
                glDrawArrays(GL_TRIANGLE_STRIP,0,4);
            }
            {
                float vtx[20] = {
                    0.0,0.0 ,0.0 ,0.0,0.0,
                    0.0,0.0 ,in.l,1.0,0.0,
                    0.0,in.h,0.0 ,0.0,1.0,
                    0.0,in.h,in.l,1.0,1.0
                };
                glBufferSubData(GL_ARRAY_BUFFER,0,sizeof(vtx),vtx);
                glUniformHandleui64ARB(texture_3d_shader.getLocation("tex"),textures.left.texturehandle);
                glDrawArrays(GL_TRIANGLE_STRIP,0,4);
            }
            {
                float vtx[20] = {
                    in.w,0.0 ,0.0 ,1.0,0.0,
                    in.w,in.h,0.0 ,1.0,1.0,
                    in.w,0.0 ,in.l,0.0,0.0,
                    in.w,in.h,in.l,0.0,1.0
                };
                glBufferSubData(GL_ARRAY_BUFFER,0,sizeof(vtx),vtx);
                glUniformHandleui64ARB(texture_3d_shader.getLocation("tex"),textures.right.texturehandle);
                glDrawArrays(GL_TRIANGLE_STRIP,0,4);
            }
            {
                float vtx[20] = {
                    0.0 ,in.h,in.l,0.0,0.0,
                    in.w,in.h,in.l,1.0,0.0,
                    0.0 ,in.h,0.0 ,0.0,1.0,
                    in.w,in.h,0.0 ,1.0,1.0
                };
                glBufferSubData(GL_ARRAY_BUFFER,0,sizeof(vtx),vtx);
                glUniformHandleui64ARB(texture_3d_shader.getLocation("tex"),textures.top.texturehandle);
                glDrawArrays(GL_TRIANGLE_STRIP,0,4);
            }
            {
                float vtx[20] = {
                    0.0 ,0.0,in.l,0.0,0.0,
                    0.0 ,0.0,0.0 ,0.0,1.0,
                    in.w,0.0,in.l,1.0,0.0,
                    in.w,0.0,0.0 ,1.0,1.0
                };
                glBufferSubData(GL_ARRAY_BUFFER,0,sizeof(vtx),vtx);
                glUniformHandleui64ARB(texture_3d_shader.getLocation("tex"),textures.bottom.texturehandle);
                glDrawArrays(GL_TRIANGLE_STRIP,0,4);
            }
            return in;
        }
    }//namespace draw;
    namespace time{
        class Clock{
            private:
                double tick_before_last_tick;
                double last_tick;
            public:
                Clock(){
                    last_tick = tick_before_last_tick = glfwGetTime();
                }
                void tick(double fps){
                    while((glfwGetTime()-last_tick)<(1.0/fps));
                    tick_before_last_tick = last_tick;
                    last_tick = glfwGetTime();
                }
                double get_fps() const{
                    if(last_tick==tick_before_last_tick)return 0;
                    return 1.00/(last_tick-tick_before_last_tick);
                }
        };
        void delay(long ms){
            float mss = static_cast<double>(ms);
            double now = glfwGetTime();
            while(((glfwGetTime()-now)*1000.0)<mss);
        }
    }//namespace time;
}//namespace pygame;
namespace std{
    std::string inline to_string(pygame::Rect rct){
        return (string)"pygame.Rect("+to_string(rct.x)+","+to_string(rct.y)+","+to_string(rct.w)+","+to_string(rct.h)+")";
    }
    std::string inline to_string(pygame::Point pt){
        return (string)"pygame.Point("+to_string(pt.x)+","+to_string(pt.y)+")";
    }
}//namespace std;
#endif//GSDL_H
