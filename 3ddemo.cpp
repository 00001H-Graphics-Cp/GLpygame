#include<string>
#include<cstdio>
#include<vector>
#include<utility>
#include<any>
#include<iostream>
#include"pygame.hpp"
using std::wstring;
using std::vector;
using std::any_cast;
using namespace pygame;
using namespace pygame::event;
using pygame::display::Window;
using pygame::time::Clock;
using std::cout;
using std::cin;
using std::endl;
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
void APIENTRY glDebugOutput(GLenum source, 
                            GLenum type, 
                            unsigned int id, 
                            GLenum severity, 
                            GLsizei length, 
                            const char *message, 
                            const void *userParam)
{
    // ignore non-significant error/warning codes
    if(id == 131169 || id == 131185 || id == 131218 || id == 131204) return; 

    std::cout << "---------------" << std::endl;
    std::cout << "Debug message (" << id << "): " <<  message << std::endl;

    switch (source)
    {
        case GL_DEBUG_SOURCE_API:             std::cout << "Source: API"; break;
        case GL_DEBUG_SOURCE_WINDOW_SYSTEM:   std::cout << "Source: Window System"; break;
        case GL_DEBUG_SOURCE_SHADER_COMPILER: std::cout << "Source: Shader Compiler"; break;
        case GL_DEBUG_SOURCE_THIRD_PARTY:     std::cout << "Source: Third Party"; break;
        case GL_DEBUG_SOURCE_APPLICATION:     std::cout << "Source: Application"; break;
        case GL_DEBUG_SOURCE_OTHER:           std::cout << "Source: Other"; break;
    } std::cout << std::endl;

    switch (type)
    {
        case GL_DEBUG_TYPE_ERROR:               std::cout << "Type: Error"; break;
        case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR: std::cout << "Type: Deprecated Behaviour"; break;
        case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:  std::cout << "Type: Undefined Behaviour"; break; 
        case GL_DEBUG_TYPE_PORTABILITY:         std::cout << "Type: Portability"; break;
        case GL_DEBUG_TYPE_PERFORMANCE:         std::cout << "Type: Performance"; break;
        case GL_DEBUG_TYPE_MARKER:              std::cout << "Type: Marker"; break;
        case GL_DEBUG_TYPE_PUSH_GROUP:          std::cout << "Type: Push Group"; break;
        case GL_DEBUG_TYPE_POP_GROUP:           std::cout << "Type: Pop Group"; break;
        case GL_DEBUG_TYPE_OTHER:               std::cout << "Type: Other"; break;
    } std::cout << std::endl;
    
    switch (severity)
    {
        case GL_DEBUG_SEVERITY_HIGH:         std::cout << "Severity: high"; break;
        case GL_DEBUG_SEVERITY_MEDIUM:       std::cout << "Severity: medium"; break;
        case GL_DEBUG_SEVERITY_LOW:          std::cout << "Severity: low"; break;
        case GL_DEBUG_SEVERITY_NOTIFICATION: std::cout << "Severity: notification"; break;
    } std::cout << std::endl;
    std::cout << std::endl;
}
int SW = 800;
int SH = 600;
#define GL_DEBUG_CONTEXT false
int main(){
    std::shared_ptr<Chlib> pcharlib=nullptr;
    try{
        pcharlib = std::make_shared<Chlib>();
    }catch(FTError &e){
        std::cout << e.what() << std::endl;
        int som;
        std::cin >> som;
    }
    Chlib &charlib = *pcharlib;
    init();
    glfwWindowHint(GLFW_OPENGL_PROFILE,GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR,4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR,6);
    glfwWindowHint(GLFW_RESIZABLE,GLFW_FALSE);
    glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT,GL_DEBUG_CONTEXT);  
    Window win = Window(SW,SH,"3D Test");
    win.setAsOpenGLTarget();
    gsdlInit();
    #if GL_DEBUG_CONTEXT==true
    glEnable(GL_DEBUG_OUTPUT);
    glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS); 
    glDebugMessageCallback(glDebugOutput, nullptr);
    glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, nullptr, GL_TRUE);
    #endif
    glClearColor(0.7,0.7,0.7,1.0);
    
    pygame::draw_made_with_glpy(win);
    stbi_set_flip_vertically_on_load(true);
    Font DEFAULT_FONT = charlib.getfont(L"Cnew","rsrc/courier_new.ttf");
    DEFAULT_FONT->set_dimensions(0,45);
    glEnable(GL_CULL_FACE);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
    glPixelStorei(GL_UNPACK_ALIGNMENT,1);
    Clock clk;
    pTexture fteximg = loadTexture2D("demorsrc/grid.png");
    pTexture tex = loadTexture2D("demorsrc/a.png");
    CubeTexture ctex = CubeTexture(tex);
    ctex.left = loadTexture2D("demorsrc/b.png");
    ctex.back = loadTexture2D("demorsrc/c.png");
    cout << "ImageLoad:: Complete" << endl;
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
    cout << "GameSetup:: Complete" << endl;
    pTexture scene = std::make_shared<Texture>(nullptr,1920,1080,GL_RGB,GL_RGB,GL_NEAREST,GL_NEAREST,false);
    Framebuffer fbo;
    Renderbuffer rb(GL_DEPTH24_STENCIL8,1920,1080);
    fbo.attachTexture(scene);
    fbo.attachRenderbuf(GL_DEPTH_STENCIL_ATTACHMENT,rb);
    assert(fbo.isComplete());
    cout << "SceneSetup:: Complete" << endl;
    Shader NEWSHADER;
    NEWSHADER.program = loadprogram(L"rsrc/2d_textured_vertex.glsl",L"rsrc/kernalfs.glsl");
    while(!win.shouldClose()){
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

        fbo.bind();
        glViewport(0,0,1920,1080);
        glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
        pygame::draw::cube(floorcube,floortex);
        for(CubeObject& p : objects){
            p.draw();
        }
        Framebuffer::unbind();
        glViewport(0,0,SW,SH);

        if(Curgrab){
            for(CubeObject& gobj : objects){
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
        }
        glDisable(GL_DEPTH_TEST);
        pygame::blit(scene,{0.0,0.0},1.0,0,1.0F,&NEWSHADER);
        pygame::draw_text(DEFAULT_FONT,L"GLpygame 3D demo(ver.0226rc1) all pasterights reserved",{10.0,10.0},1.0);
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
    pygame::quit();
    return 0;
}
