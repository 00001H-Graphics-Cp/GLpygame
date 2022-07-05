#version 460 core

#extension GL_ARB_bindless_texture:require

layout(location = 0) in vec2 texcoords;

out vec4 fragcolor;

layout(bindless_sampler) uniform sampler2D img;

uniform float transparency=1.0;

void main(){
    vec4 color = texture(img,texcoords);
    color.a *= transparency;
    fragcolor = color;
}
