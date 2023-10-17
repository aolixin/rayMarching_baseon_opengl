#version 330 core

float WIDTH = 1920;
float HEIGHT = 1080;
vec3 pix = vec3((gl_FragCoord.xy /vec2(WIDTH,HEIGHT) ) * 2 - vec2(1),gl_FragCoord.z*2-1);

uniform sampler2D TexInput;


uniform float cloudSize;
#define kernelSize 5

float sigmaSpace = 5; // 空间域标准差
float sigmaColor = 5; // 色彩域标准差



float gaussian(float x, float sigma) {
    return exp(-0.5 * x * x / (sigma * sigma));
}



vec4 BilateralFiltering(sampler2D Tex, vec2 TexCoord)
{
    ivec2 texelSize = ivec2(textureSize(Tex, 0));
    vec2 texelStep = 1.0 / vec2(texelSize);
    
    vec4 centerColor = texture(Tex, TexCoord);
    vec4 outputColor = vec4(0.0);
    float totalWeight = 0.0;

    for (int x = -kernelSize; x <= kernelSize; x++) {
        for (int y = -kernelSize; y <= kernelSize; y++) {
            vec2 offset = vec2(x, y) * texelStep;
            vec4 sampleColor = texture(Tex, TexCoord + offset);
            
            float spatialWeight = gaussian(length(offset), sigmaSpace);
            float colorWeight = gaussian(length(sampleColor - centerColor), sigmaColor);
            float weight = spatialWeight * colorWeight;
            
            outputColor += sampleColor * weight;
            totalWeight += weight;
        }
    }
    
    return outputColor / totalWeight;
}


void main() {

    gl_FragData[0] =  BilateralFiltering(TexInput,pix.xy*0.5+0.5);
}