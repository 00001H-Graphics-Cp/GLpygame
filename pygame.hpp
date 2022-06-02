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
}
