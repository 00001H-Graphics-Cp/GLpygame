#version 460 core

layout(location = 0) in vec4 color;

out vec4 fragcolor;

void main(){
    fragcolor = color;
}
