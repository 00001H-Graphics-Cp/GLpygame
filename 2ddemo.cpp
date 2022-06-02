#include<string>
#include<cstdio>
#include"pygame.hpp"
using std::wstring;
using namespace pygame;
using namespace pygame::event;
using pygame::display::Window;
using pygame::time::Clock;
int main(){
    init();
    glfwWindowHint(GLFW_OPENGL_PROFILE,GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR,4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR,6);
    glfwWindowHint(GLFW_RESIZABLE,GLFW_FALSE);
    
    Window win = Window(800,600,"this is a title");
    win.setAsOpenGLTarget();
    gsdlInit();
    Chlib charlib;
    glViewport(0,0,800,600);
    Font DEFAULT_FONT = charlib.getfont(L"Cnew","rsrc/courier_new.ttf");
    DEFAULT_FONT->set_dimensions(70,60);
    glDisable(GL_CULL_FACE);
    glDisable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
    glPixelStorei(GL_UNPACK_ALIGNMENT,1);
    Point texpos = {30.0,32.0};
    wstring tt;
    Clock clk;
    
    while(!win.pending_close_action()){
        glfwPollEvents();
        for(Event evt : win.eventqueue->get()){
            if(evt.type == MOUSEBUTTONDOWN){
                MouseButtonEvent mevt = any_cast<MouseButtonEvent>(evt.value);
                texpos = mevt.pos;
                texpos.y -= 30;
            }
        }
        glClearColor(0.0f,0.5f,0.75f,1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        tt = L"Current FPS:";
        wchar_t carr[150];
        swprintf(carr,L"%.5f",clk.get_fps());
        tt += carr;
        draw_text(DEFAULT_FONT,tt,texpos,2.0,{1.0,1.0,0.0,1.0},align::CENTER);
//        pygame::draw::cube();
        win.swapBuffers();
        clk.tick(60);
    }
    gsdlDeinit();
    quit();
    return 0;
}
