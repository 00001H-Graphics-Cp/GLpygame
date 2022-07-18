#ifndef PYGAME_COLOR_HPP
#define PYGAME_COLOR_HPP
#include<pygame.hpp>
namespace pygame{
    namespace color{
        #define defcolor const constexpr pygame::Color
        defcolor WHITE(1.0,1.0,1.0,1.0);
        defcolor BLACK(0.0,0.0,0.0,1.0);
        defcolor RED(1.0,0.0,0.0,1.0);
        defcolor GREEN(0.0,1.0,0.0,1.0);
        defcolor BLUE(0.0,0.0,1.0,1.0);
        defcolor YELLOW(1.0,1.0,0.0,1.0);
        #undef defcolor
        namespace{
            vec3 constexpr rgb(vec4 v){
                return vec3(v.r,v.g,v.b);
            }
        }
        pygame::Color constexpr blend(pygame::Color src,pygame::Color dst){
            return pygame::Color(rgb(src)*src.a+rgb(dst)*dst.a*(1-src.a),1-((1-src.a)*(1-dst.a)));
        }
    }
}
#endif//PYGAME_COLOR_HPP