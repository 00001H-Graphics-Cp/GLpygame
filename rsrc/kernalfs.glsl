#version 460 core

#extension GL_ARB_bindless_texture:require

layout(location = 0) in vec2 texcoords;

out vec4 fragcolor;

layout(bindless_sampler) uniform sampler2D img;

uniform float transparency=1.0;

void main(){
    float offset = 1.0f/300.0f;
    vec2 offsets[9] = vec2[](
        vec2(-offset, offset),// top-left
        vec2( 0.0f  , offset),// top-center
        vec2( offset, offset),// top-right
        vec2(-offset, 0.0f  ),// center-left
        vec2( 0.0f  , 0.0f  ),// center-center
        vec2( offset, 0.0f  ),// center-right
        vec2(-offset,-offset),// bottom-left
        vec2( 0.0f  ,-offset),// bottom-center
        vec2( offset,-offset) // bottom-right    
    );

    float kernel[9] = float[](
        -1,-1,-1,
        -1, 8,-1,
        -1,-1,-1
    );
    
    vec4 sampleTex[9];
    for(int i=0;i<9;i++){
        sampleTex[i] = texture(img,texcoords.xy + offsets[i]);
    }
    vec4 color = vec4(0.0);
    for(int i=0;i<9;i++)
        color += vec4(sampleTex[i].rgb*kernel[i],sampleTex[i].a);
    color.a *= transparency;
    fragcolor = color;
}
