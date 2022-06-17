#include<string>
#include<cstdio>
#include<vector>
#include<utility>
#include<any>
#include"pygame.hpp"
using std::wstring;
using std::vector;
using namespace pygame;
using namespace pygame::event;
using pygame::display::Window;
using pygame::time::Clock;
class GameObject{
    public:
        virtual void draw() const = 0;
        virtual ~GameObject(){
        }
        float yaccel = 0;
};
class CubeObject : public GameObject{
    public:
        Cube cbe;
        CubeTexture tex;
        CubeObject(Cube c,CubeTexture t) : cbe(c),tex(t) {}
        virtual void draw() const{
            pygame::draw::cube(cbe,tex);
        }
};
const float PLAYER_REACH = 3.0;
const float GRAVITY=-0.003;
const float PLAYER_HEIGHT=1.75;
const float JUMP_YACCEL=0.09;
const float CAMSENSIT = 0.1f,CAMSPD=0.06f;
float inline constexpr modulo(float x,float y){
    while(x>y)x-=y;
    while(x<y)x+=y;
    return x;
}
float inline constexpr modudist(float x,float y,float mod){
    using glm::abs;
    using glm::min;
    x = modulo(x,mod);
    y = modulo(y,mod);
    return min(abs(x-y),min(abs(x+mod-y),abs(x-mod-y)));
}
static_assert(modudist(0,10,11)==1);
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
    Font RECT_FONT = charlib.getfont(L"Crosshair","rsrc/courier_new.ttf");
    DEFAULT_FONT->set_dimensions(0,45);
    RECT_FONT->set_dimensions(25,20);
    glEnable(GL_CULL_FACE);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
    glPixelStorei(GL_UNPACK_ALIGNMENT,1);
    Clock clk;
    Texture fteximg = loadTexture("demorsrc/grid.png");
    Texture tex = loadTexture("demorsrc/a.png");
    CubeTexture ctex = CubeTexture(tex);
    ctex.left = loadTexture("demorsrc/b.png");
    ctex.back = loadTexture("demorsrc/c.png");
    CubeTexture floortex = CubeTexture(fteximg);
    Cube floorcube = Cube(-25,-1,-25,50,1,50);
    draw::pinf.aspc = 8.0/6.0;
    draw::set_camera(std::make_shared<pygame::Camera>(glm::vec3(2.0,4.0,7.0)));
    std::shared_ptr camera = draw::cam;
    camera->pitch = -5;
    camera->yaw = -90;
    glfwSetInputMode(win.glfwWindow(),GLFW_CURSOR,GLFW_CURSOR_DISABLED);
    bool Curgrab=true;
    glm::dvec2 lastpos,cupos,dist;
    glfwGetCursorPos(win.glfwWindow(),&lastpos.x,&lastpos.y);
    glClearColor(0.0f,0.5f,0.75f,1.0f);
    vector<CubeObject> objects;
    CubeObject cube1(Cube(0.0,0.0,0.0,1.0,1.0,1.0),ctex);
    CubeObject cube2(Cube(0.0,0.0,6.0,1.0,1.0,1.0),ctex);
    CubeObject *grabbing=nullptr;
    objects.push_back(cube1);
    objects.push_back(cube2);
    float player_yaccel=0.0;
    int mind=0;
    int rota=0;
    while(!win.pending_close_action()){
        glEnable(GL_DEPTH_TEST);
        glfwPollEvents();
        for(Event evt : win.eventqueue->get()){
            if(evt.type == MOUSEBUTTONDOWN){
                MouseButtonEvent mevt = any_cast<MouseButtonEvent>(evt.value);
                if(mevt.btn==0){
                    glfwSetInputMode(win.glfwWindow(),GLFW_CURSOR,GLFW_CURSOR_DISABLED);
                    Curgrab=true;
                }
            }
            if(evt.type == KEYUP){
                if(any_cast<KeyEvent>(evt.value).glfw_key == GLFW_KEY_ESCAPE){
                    glfwSetInputMode(win.glfwWindow(),GLFW_CURSOR,GLFW_CURSOR_NORMAL);
                    Curgrab=false;
                }
            }
            if(evt.type == KEYDOWN){
                int key = any_cast<KeyEvent>(evt.value).glfw_key;
                if(key == GLFW_KEY_E){
                    if(!Curgrab);
                    else if(grabbing)grabbing=nullptr;
                    else for(CubeObject& obj : objects){
                        float ln = glm::length(obj.cbe.center()-camera->pos);
                        if(ln>PLAYER_REACH)continue;
                        glm::vec3 dest = camera->pos+camera->direction()*ln;
                        ln = glm::length(obj.cbe.center()-dest);
                        if(ln>0.5)continue;
                        grabbing = &obj;
                        int minimangle=0;
                        int mindlta=999;
                        int dlta;
                        for(int angle=0;angle<=270;angle+=90){
                            dlta = modudist(grabbing->cbe.yrot,angle-camera->yaw,360);
                            if(dlta<mindlta){
                                mindlta = dlta;
                                minimangle = angle;
                            }
                        }
                        mind = minimangle;
                        break;
                    }
                }else if(key == GLFW_KEY_SPACE){
                    if(Curgrab&&camera->pos.y-PLAYER_HEIGHT==0){
                        player_yaccel = std::max(player_yaccel,JUMP_YACCEL);
                    }
                }else if(key == GLFW_KEY_R){
                    if(Curgrab)rota += 90;
                }else if(key == GLFW_KEY_Q){
                    if(Curgrab)rota-=90;
                }
            }
        }
        glfwGetCursorPos(win.glfwWindow(),&cupos.x,&cupos.y);
        if(grabbing==nullptr)rota=0;
        rota = modulo(rota,360);
        if(Curgrab){
            dist = cupos-lastpos;
            camera->pitch -= dist.y*CAMSENSIT;
            camera->yaw += dist.x*CAMSENSIT;
            while(camera->yaw>360)camera->yaw-=360;
            while(camera->yaw<0)camera->yaw+=360;
            camera->pitch = std::max(-89.9f,std::min(89.9f,camera->pitch));
            camera->pos += camera->xzfront()*CAMSPD*static_cast<float>(win.getKey(GLFW_KEY_W)-win.getKey(GLFW_KEY_S));
            camera->pos += camera->xzright()*CAMSPD*static_cast<float>(win.getKey(GLFW_KEY_D)-win.getKey(GLFW_KEY_A));
            if(grabbing!=nullptr){
                grabbing->cbe.set_center(camera->pos+camera->direction()*PLAYER_REACH);
                grabbing->cbe.pos.y = std::max(grabbing->cbe.pos.y,0.0f);
                grabbing->cbe.yrot = modulo(mind-camera->yaw,360);
            }
        }
        lastpos = cupos;
        glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
        pygame::draw::cube(floorcube,floortex);
        for(CubeObject& gobj : objects){
            gobj.draw();
            if(&gobj!=grabbing){
                if(gobj.cbe.pos.y>0){
                    gobj.yaccel += GRAVITY;
                }else{
                    gobj.cbe.pos.y = 0;
                    gobj.yaccel = std::max<float>(gobj.yaccel,0);
                }
                gobj.cbe.pos.y += gobj.yaccel;
            }
        }
        if(camera->pos.y-PLAYER_HEIGHT>0){
            player_yaccel += GRAVITY;
        }else{
            camera->pos.y = PLAYER_HEIGHT;
            player_yaccel = std::max<float>(player_yaccel,0);
        }
        camera->pos.y += player_yaccel;
        glDisable(GL_DEPTH_TEST);
        pygame::draw_text(DEFAULT_FONT,L"GLpygame 3D demo(ver.0017a) all pasterights reserved",{10.0,10.0},1.0);
        pygame::draw_text(RECT_FONT,L"+",{1920.0/2.0,1080.0/2.0-12.5},3.0,{1.0,1.0,1.0,0.7},align::CENTER);
        if(grabbing != nullptr){
            pygame::draw_text(DEFAULT_FONT,L"GRAB",{10.0,75.0},1.0);
        }
        wchar_t strm[500];
        memset(strm,'\0',sizeof(strm));
        static_assert('\0'==0);
        swprintf(strm,L"Pos:(%.2f,%.2f,%.2f) Y-accel: %.2f",camera->pos.x,camera->pos.y,camera->pos.z,player_yaccel,mind);
        pygame::draw_text(DEFAULT_FONT,strm,{120.0,75.0},1.0);
        win.swapBuffers();
        clk.tick(60);
    }
    gsdlDeinit();
    quit();
    return 0;
}
