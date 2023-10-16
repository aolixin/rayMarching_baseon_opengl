#version 330 core
out vec4 FragColor;

in vec3 Normal;  
in vec3 FragPos;  


uniform sampler2D TexBgColor;

uniform vec3 viewPos; 
uniform vec3 lightPos; 

uniform float cloudSize;
uniform float cloudHeight;

uniform float cloudThickness;

float bottom = cloudHeight - cloudThickness;
float top = cloudHeight + cloudThickness;

#define baseBright  vec3(1.26,1.25,1.29)    // 基础颜色 -- 亮部
#define baseDark    vec3(0.31,0.31,0.32)    // 基础颜色 -- 暗部
#define lightBright vec3(1.29, 1.17, 1.05)  // 光照颜色 -- 亮部
#define lightDark   vec3(0.7,0.75,0.8)      // 光照颜色 -- 暗部

//#define bottom 13   // 云层底部
//#define top 20      // 云层顶部
#define cellSize 64   // worleyNoise 密度
float noiseMapSize = 512;

vec2 hash(vec2 p)
{
    float random = sin(666 + p.x *5678 + p.y *1234)*4321;
    return vec2(p.x + sin(random) /2 + 0.5f ,p.y + cos(random) / 2 +0.5f);
}
float noise( vec2 para)
{
    float x = para.x;
    float y = para.y;
    float dis = 10000;
    for(int Y = -1;Y<=1;Y++)
    {
        for(int X = -1;X<=1;X++)
        {
            vec2 cellPoint = hash(vec2( int(x) + X ,int(y) + Y) );
            dis = min(dis , distance(cellPoint,vec2(x,y)));
        }
    }
    return dis;
}

// 生成 worleyNoise
// pos -- 世界坐标
float worleyNoise(vec3 pos)
{ 
    float noiseSum = 0;
    vec2 coord = pos.xz / vec2(cloudSize * 2 ,cloudSize * 2) + vec2(0.5,0.5);
    vec2 xy = coord * noiseMapSize;

    vec2 para = vec2(xy.x/cellSize,xy.y/cellSize );
    noiseSum = noise(para);
    noiseSum += noise(para * 3.5)/3.5;
    noiseSum += noise(para * 12.25)/12.25;
    noiseSum += noise(para * 42.87)/42.87;
    return noiseSum;
}

float random(float x)
{
    float y = fract(sin(x)*100000.0);
    return y;
}

vec3 whiteNoise(vec3 pos)
{
    return vec3( random(pos.x),random(pos.y),random(pos.z));
}


// 计算 pos 点的云密度
// pos -- 世界坐标
float getDensity( vec3 pos) {
            
    float noise = worleyNoise(pos);

    noise = 1-noise;

     // 高度衰减
    float mid = (bottom + top) / 2.0;
    float h = top - bottom;
    float weight = 1.0 - 2.0 * abs(mid - pos.y) / h;
    weight = pow(weight, 0.5);

    
    noise *= weight;

    //if(noise >0.3)noise = 1;
    //return (1-noise);
    if(noise < 0.3)noise = 0;
    return noise;
}


// 获取体积云颜色
vec4 getCloud(vec3 worldPos, vec3 cameraPos) {
            
    vec3 direction = normalize(worldPos - cameraPos);   // 视线射线方向
    vec3 step = direction * 0.25;   // 步长
    vec4 colorSum = vec4(0);        // 积累的颜色
    vec3 point = cameraPos;         // 从相机出发开始测试
    // 如果相机在云层下，将测试起始点移动到云层底部 bottom
    
    // 如果相机在云层下，将测试起始点移动到云层底部 bottom
    if(point.y<bottom) {
            
        point += direction * (abs(bottom - cameraPos.y) / abs(direction.y));
    }
    // 如果相机在云层上，将测试起始点移动到云层顶部 top
    if(top<point.y) {
            
        point += direction * (abs(cameraPos.y - top) / abs(direction.y)) + 1;
    }

    // ray marching
    for(int i=0; i<100; i++) {
            
        point += step;

        //防止分层,添加白噪音
        point += whiteNoise(point) * 0.1;

        if(bottom>point.y || point.y>top || -cloudSize>point.x || point.x>cloudSize || -cloudSize>point.z || point.z>cloudSize) {
            
            continue;
        }
        
        //float density = 0.1;
        float density = getDensity(point);

        vec3 L = normalize(lightPos - point);
        float lightDensity = getDensity(point + L);
        float delta = clamp(density - lightDensity ,0.0,1.0);

        density *= 0.5;

        vec3 base = mix(baseBright,baseDark,density) * density;
        vec3 light = mix(lightDark,lightBright,delta);

        vec4 color = vec4(base * light,density);

        //vec4 color = vec4(0.9, 0.8, 0.7, 1.0) * density * 0.3;    // 当前点的颜色
        //vec4 color = vec4(0.9, 0.8, 0.7, 1.0) * 0.1;    // 当前点的颜色
        colorSum = colorSum + color * (1.0 - colorSum.a);   // 与累积的颜色混合
    }

    return colorSum;
}



void main()
{
    vec3 pix = vec3((gl_FragCoord.xy /vec2(1920,1080) ) * 2 - vec2(1),gl_FragCoord.z*2-1);

    vec3 bgColor = texture2D(TexBgColor,pix.xy * 0.5 + 0.5).rgb;
    vec4 cloud = getCloud(FragPos, viewPos); // 云颜色

    FragColor.rgb = bgColor.rgb*(1.0 - cloud.a) + cloud.rgb;    // 混色
    FragColor.a = 1.0;
} 