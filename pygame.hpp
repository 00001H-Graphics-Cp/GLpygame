#include<glad/glad.h>
#include"glfwPygame.hpp"
#include"gsdl.hpp"
namespace pygame{
    namespace{
        bool _is_init = false;
    }
    void init(){
        if(!_is_init)
            display::init();
        _is_init = true;
    }
    void quit(){
        if(_is_init)
            display::quit();
        _is_init = false;
    }
    void draw_made_with_glpy(display::Window win,float insecs=1.0,float staysecs=2.5,float outsecs=1.0){
        //WARNING: Overwrites some OpenGL parameters!
        //WARNING: Requires glClearColor to be set!
        const float FPS=60.00;
        #define sec2frm(sec) (glm::round((sec)*FPS))
        Texture tex = loadTexture("rsrc/glpy.png");
        float inframes = sec2frm(insecs);
        float stayframes = sec2frm(staysecs);
        float outframes = sec2frm(outsecs);
        float frame=0;
        glDisable(GL_DEPTH_TEST);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
        float visibility;
        time::Clock clok;
        const float size = 3.5;
        Point middle((1920.0-tex.w*size)/2.0,(1080.0-tex.h*size)/2.0);
        while(!win.pending_close_action()){
            glfwPollEvents();
            win.eventqueue.get();
            glClear(GL_COLOR_BUFFER_BIT);
            if(0<=frame&&frame<=inframes){
                visibility = frame/inframes;
            }else if(inframes<frame&&frame<=(inframes+stayframes)){
                visibility = 1;
            }else{
                visibility = 1-((frame-stayframes-inframes)/outframes);
            }
            pygame::blit(tex,middle,size,0,visibility);
            win.swapBuffers();
            if(frame>(inframes+stayframes+outframes))break;
            clok.tick(FPS);
            frame++;
        }
    }
}
