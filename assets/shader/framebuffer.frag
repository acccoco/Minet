#version 330 core
out vec4 FragColor;
in vec2 TexCoords;
uniform sampler2D texture1;

// 后期处理：反相
vec3 pp_inverse();
// 后期处理：灰度
vec3 pp_gray();
// 使用卷积核进行处理
vec3 convolution(float kernel[9]);
// 后期处理：锐化
vec3 pp_sharpe1();
vec3 pp_sharpe2();
// 后期处理：模糊
vec3 pp_blur();
// 后期处理：边缘检测
vec3 pp_edge_detect();
// 后期处理：边缘检测 + 灰度
vec3 pp_edge_gray();

void main()
{
    vec3 final_color=pp_edge_gray();
    FragColor=vec4(final_color,1.);
}

vec3 pp_inverse(){
    vec3 tex_color=texture(texture1,TexCoords).rgb;
    return 1.-tex_color;
}

vec3 pp_gray(){
    vec3 tex_color=texture(texture1,TexCoords).rgb;
    float gray=dot(vec3(.2126,.7152,.0722),tex_color);
    return vec3(gray,gray,gray);
}

vec3 pp_sharpe1(){
    // 锐化核
    float kernel[9]=float[](
        2,2,2,
        2,-15,2,
        2,2,2
    );
    
    return convolution(kernel);
}

vec3 pp_sharpe2(){
    // 另一种锐化核
    float kernel[9]=float[](
        -1,-1,-1,
        -1,9,-1,
        -1,-1,-1
    );
    
    return convolution(kernel);
}

vec3 convolution(float kernel[9]){
    // 采样矩阵
    const float offset=1./256.;
    vec2 samplers[]=vec2[](
        vec2(-offset,offset),vec2(0.f,offset),vec2(offset,offset),
        vec2(-offset,0.f),vec2(0.f,0.f),vec2(offset,0.f),
        vec2(-offset,-offset),vec2(0.f,-offset),vec2(offset,-offset)
    );
    
    // 进行采样
    vec3 sample_res[9];
    for(int i=0;i<9;++i){
        sample_res[i]=vec3(texture(texture1,TexCoords+samplers[i]));
    }
    vec3 res=vec3(0);
    for(int i=0;i<9;++i){
        res+=sample_res[i]*kernel[i];
    }
    
    return res;
}

vec3 pp_blur(){
    // 模糊核
    float kernel[9]=float[](
        1,2,1,
        2,4,2,
        1,2,1
    );
    for(int i=0;i<9;++i)
    kernel[i]/=16.;
    return convolution(kernel);
}

vec3 pp_edge_detect(){
    // 边缘检测的核
    float kernel[9]=float[](
        1,1,1,
        1,-8,1,
        1,1,1
    );
    
    return convolution(kernel);
}

vec3 pp_edge_gray(){
    vec3 color=pp_edge_detect();
    float gray=dot(vec3(.2126,.7152,.0722),color);
    return vec3(gray,gray,gray);
}
