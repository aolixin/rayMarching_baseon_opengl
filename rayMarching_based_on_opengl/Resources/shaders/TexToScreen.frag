#version 330 core

float WIDTH = 1920;
float HEIGHT = 1080;
vec3 pix = vec3((gl_FragCoord.xy /vec2(WIDTH,HEIGHT) ) * 2 - vec2(1),gl_FragCoord.z*2-1);

uniform sampler2D TexBackground;
uniform sampler2D TexCloud;


void main() {

    vec4 bgColor = texture2D(TexBackground, pix.xy*0.5+0.5);
    vec4 cloudColor = texture2D(TexCloud, pix.xy*0.5+0.5);

    //根据透明度混合颜色
    gl_FragData[0] = vec4(bgColor.rgb * (1 - cloudColor.a) + cloudColor.rgb,1.0);

}