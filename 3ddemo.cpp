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
    Window win = Window(800,600,"3D Test");
    win.setAsOpenGLTarget();
    gsdlInit();
    glViewport(0,0,800,600);
    Chlib charlib;
    stbi_set_flip_vertically_on_load(true);
    Font DEFAULT_FONT = charlib.getfont(L"Cnew","rsrc/courier_new.ttf");
    DEFAULT_FONT->set_dimensions(0,35);
    glDisable(GL_CULL_FACE);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
    glPixelStorei(GL_UNPACK_ALIGNMENT,1);
    Clock clk;
    Texture tex = loadTexture("demorsrc/a.png");
    CubeTexture ctex = CubeTexture(tex);
    ctex.left = loadTexture("demorsrc/b.png");
    ctex.back = loadTexture("demorsrc/c.png");
    draw::pinf.aspc = 8.0/6.0;
    draw::set_camera(std::make_shared<pygame::three_d::Camera>(glm::vec3(2.0,4.0,5.0)));
    auto camera = draw::cam;
    camera->pitch = -5;
    camera->yaw = -90;
    glfwSetInputMode(win.glfwWindow(),GLFW_CURSOR,GLFW_CURSOR_DISABLED);
    bool grab=true;
    glm::dvec2 lastpos,cupos,dist;
    glfwGetCursorPos(win.glfwWindow(),&lastpos.x,&lastpos.y);
    float CAMSENSIT = 0.1f,CAMSPD=0.05f;
    while(!win.pending_close_action()){
        glEnable(GL_DEPTH_TEST);
        glfwPollEvents();
        for(Event evt : win.eventqueue->get()){
            if(evt.type == MOUSEBUTTONDOWN){
                MouseButtonEvent mevt = any_cast<MouseButtonEvent>(evt.value);
                if(mevt.btn==0){
                    glfwSetInputMode(win.glfwWindow(),GLFW_CURSOR,GLFW_CURSOR_DISABLED);
                    grab=true;
                }
            }
            if(evt.type == KEYUP){
                if(any_cast<KeyEvent>(evt.value).glfw_key == GLFW_KEY_ESCAPE){
                    glfwSetInputMode(win.glfwWindow(),GLFW_CURSOR,GLFW_CURSOR_NORMAL);
                    grab=false;
                }
            }
        }
        glfwGetCursorPos(win.glfwWindow(),&cupos.x,&cupos.y);
        if(grab){
            dist = cupos-lastpos;
            camera->pitch -= dist.y*CAMSENSIT;
            camera->yaw += dist.x*CAMSENSIT;
            while(camera->yaw>360)camera->yaw-=360;
            while(camera->yaw<0)camera->yaw+=360;
            camera->pitch = std::max(-89.9f,std::min(89.9f,camera->pitch));
            camera->pos += camera->direction()*CAMSPD*static_cast<float>(win.getKey(GLFW_KEY_W)-win.getKey(GLFW_KEY_S));
            camera->pos += camera->right()*CAMSPD*static_cast<float>(win.getKey(GLFW_KEY_D)-win.getKey(GLFW_KEY_A));
        }
        lastpos = cupos;
        glClearColor(0.0f,0.5f,0.75f,1.0f);
        glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
        pygame::draw::cube(Cube(1,1,1,1,1,1),ctex);
        glDisable(GL_DEPTH_TEST);
        pygame::draw_text(DEFAULT_FONT,L"GLpygame 3D demo(ver.0017a) all pasterights reserved",{10.0,10.0},1.0);
        win.swapBuffers();
        clk.tick(60);
    }
    gsdlDeinit();
    quit();
    return 0;
}
