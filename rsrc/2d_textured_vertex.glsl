#version 460 core

layout(location = 0) in vec2 pos;
layout(location = 1) in vec2 texc;

layout(location = 0) out vec2 tc;

uniform vec2 position;
uniform float size;
uniform int rotation=0;
vec2 rotate(vec2 pos,int rott){
    int thep=0;
    vec2 ppos = pos;
    while(thep<rott){
        thep += 90;
        if(ppos.x>0.5){
            if(ppos.y>0.5){
                ppos.y = 1-ppos.y;
            }else{
                ppos.x = 1-ppos.x;
            }
        }else{
            if(ppos.y>0.5){
                ppos.x = 1-ppos.x;
            }else{
                ppos.y = 1-ppos.y;
            }
        }
    }
    return ppos;
}
void main(){
    vec2 poss = vec2(pos*size+position);
    gl_Position = vec4(poss.x/960.0-1.0,-poss.y/540.0+1.0,0.0,1.0);
    tc = rotate(texc,rotation);
}
