#ifndef PYMATH_H
#define PYMATH_H

//#include<cstddef>
#include<glm/glm.hpp>

namespace pygame{
//    template<typename _Tp>
//    class vec2{
//        public:
//            _Tp x;
//            _Tp y;
//            vec2(_Tp a,_Tp b):x(a),y(b){}
//            vec2()=default;
//            _Tp operator[](int index) const{
//                if(index==0)return x;
//                else if(index==1)return y;
//                else throw std::out_of_range(std::to_string(index));
//            }
//            vec2<_Tp> operator-(vec2<_Tp> &other) const{
//                return vec2<_Tp>(x-other.x,y-other.y);
//            }
//            std::size_t length();
//    };
//    template<typename _Tp>
//    class vec3{
//        public:
//            _Tp x;
//            _Tp y;
//            _Tp z;
//            vec3(_Tp a,_Tp b,_Tp c):x(a),y(b),z(c){}
//            vec3()=default;
//            _Tp operator[](int index) const{
//                if(index==0)return x;
//                else if(index==1)return y;
//                else if(index==2)return z;
//                else throw std::out_of_range(std::to_string(index));
//            }
//            vec3<_Tp> operator-(vec3<_Tp> &other) const{
//                return vec3<_Tp>(x-other.x,y-other.y,z-other.z);
//            }
//    };
//    template<typename _Tp>
//    class vec4{
//        public:
//            _Tp x;
//            _Tp y;
//            _Tp z;
//            _Tp w;
//            vec4(_Tp a,_Tp b,_Tp c,_Tp d):x(a),y(b),z(c),w(d){}
//            vec4()=default;
//            _Tp operator[](int index) const{
//                if(index==0)return x;
//                else if(index==1)return y;
//                else if(index==2)return z;
//                else if(index==3)return w;
//                else throw std::out_of_range(std::to_string(index));
//            }
//            vec4<_Tp> operator-(vec4<_Tp> &other) const{
//                return vec4<_Tp>(x-other.x,y-other.y,z-other.z,w-other.w);
//            }
//    };
    typedef glm::vec2 vec2;
    typedef glm::vec3 vec3;
    typedef glm::vec4 vec4;
}
#endif//PYMATH_H
