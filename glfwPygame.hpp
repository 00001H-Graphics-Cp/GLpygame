#ifndef GLFWPYGAME_H
#define GLFWPYGAME_H

#include<vector>
#include<memory>
#include<any>
#include<GLFW/glfw3.h>
#include<unordered_map>
#include<stdexcept>
#include"gsdl.hpp"
namespace pygame{
    namespace event{
        enum EventType{
            MOUSEMOTION=0xfa01,MOUSEBUTTONDOWN=0xfa02,MOUSEBUTTONUP=0xfa03,
            KEYDOWN=0xfb01,KEYUP=0xfb02
        };
        struct Event{
            int type;
            std::any value;
            Event(int etype,std::any thing) : type(etype),value(thing){}
        };
        class Events{
            private:
                std::vector<Event> eventlist;
            public:
                void add(Event event){
                    eventlist.push_back(event);
                }
                std::vector<Event> get(){
                    std::vector<Event> retval = eventlist;
                    eventlist.clear();
                    return retval;
                }
                void put(Event event){
                    eventlist.push_back(event);
                }
        };
        struct MouseButtonEvent{
            Point pos;
            int btn;
            MouseButtonEvent(Point pos,int btn) : pos(pos),btn(btn) {}
        };
        struct KeyEvent{
            int glfw_key;
            int scancode;
            int mods;
            KeyEvent(int key,int scan,int mods) : glfw_key(key),scancode(scan),mods(mods) {}
            bool inline is_key(int key,int _orscan=-1) const{return (((key==GLFW_KEY_UNKNOWN)||(glfw_key==GLFW_KEY_UNKNOWN))?(scancode==_orscan):(key==glfw_key));}
        };
    }
    namespace display{
        class window_creation_failed : public std::logic_error{
            using std::logic_error::logic_error;
        };
        class Window;
        std::unordered_map<GLFWwindow*,Window*> winmaps;
        class Window{
            private:
                static Point inline toPygameCoords(glm::dvec2 in,double sw,double sh){
                    return {(float)(in.x/sw*1920.0),(float)(in.y/sh*1080.0)};
                }
                static Point inline getMousePos(GLFWwindow *win){
                    double x,y;
                    glfwGetCursorPos(win,&x,&y);
                    Window *self = winmaps[win];
                    return toPygameCoords(vec2{x,y},self->sw,self->sh);
                }
                static void inline _Handle_MouseMotion(GLFWwindow* win,double x,double y){
                    Window *self = winmaps[win];
                    self->eventqueue->put(event::Event(event::MOUSEMOTION,toPygameCoords(vec2{x,y},self->sw,self->sh)));
                }
                static void inline _Handle_MouseButton(GLFWwindow* win,int btn,int action,int mods){
                    winmaps[win]->eventqueue->put(event::Event(((action==GLFW_PRESS) ? event::MOUSEBUTTONDOWN : event::MOUSEBUTTONUP),event::MouseButtonEvent(getMousePos(win),btn)));
                }
                static void inline _Handle_KeyPress(GLFWwindow* win,int key,int scan,int action,int mods){
                    winmaps[win]->eventqueue->put(event::Event(((action==GLFW_PRESS) ? event::KEYDOWN : event::KEYUP),event::KeyEvent(key,scan,mods)));
                }
                GLFWwindow* win = nullptr;
                GLFWmonitor* fullscreen_monitor = nullptr;
                GLFWmonitor* current_monitor = nullptr;
                int fullscreen_fps = 0;
                double sw,sh;
            public:
                std::shared_ptr<event::Events> eventqueue;
                Window(int width,int height,const char *title,GLFWmonitor *monitor=nullptr,GLFWwindow *share=NULL){
                    win = glfwCreateWindow(width,height,title,monitor,share);
                    if(win==NULL){
                        throw window_creation_failed("Window creation failed!");
                    }
                    eventqueue = std::make_shared<event::Events>();
                    winmaps.emplace(win,this);
                    glfwSetCursorPosCallback(win,_Handle_MouseMotion);
                    glfwSetMouseButtonCallback(win,_Handle_MouseButton);
                    glfwSetKeyCallback(win,_Handle_KeyPress);
                    sw = width;
                    sh = height;
                    current_monitor = monitor;
                    fullscreen_monitor = ((monitor==nullptr)?glfwGetPrimaryMonitor():monitor);
                }
                bool inline isWindowed() const{
                    return current_monitor==nullptr;
                }
                bool inline isFullscreen() const{
                    return !isWindowed();
                }
                void inline setFullscreenMonitor(GLFWmonitor *mon){
                    fullscreen_monitor = mon;
                }
                void inline setFullscreenFPS(int fps){//0 for automatic
                    fullscreen_fps = fps;
                }
                void inline toggleFullscreen(){
                    if(isWindowed()){
                        current_monitor = fullscreen_monitor;
                    }else{
                        current_monitor = nullptr;
                    }
                    glfwSetWindowMonitor(win,current_monitor,0,0,sw,sh,
                    ((fullscreen_fps==0)?GLFW_DONT_CARE:fullscreen_fps));
                }
                Point inline getMousePos() const{
                    return getMousePos(win);
                }
                GLFWwindow *glfwWindow() const{
                    return win;
                }
                bool inline shouldClose() const{
                    return glfwWindowShouldClose(win);
                }
                void inline setAsOpenGLTarget() const{
                    glfwMakeContextCurrent(win);
                }
                double inline getWidth() const{
                    return sw;
                }
                double inline getHeight() const{
                    return sh;
                }
                void inline onresize(GLFWframebuffersizefun func) const{
                    glfwSetFramebufferSizeCallback(win,func);
                }
                void inline swapBuffers() const{
                    glfwSwapBuffers(win);
                }
                void inline close() const{
                    glfwDestroyWindow(win);
                }
                bool inline getKey(int key) const{
                    return glfwGetKey(win,key);
                }
        };
        void init(){
            glfwInit();
        }
        void quit(){
            glfwTerminate();
        }
    }//namespace display;
}//namespace pygame;
#endif//GLFWPYGAME_H
