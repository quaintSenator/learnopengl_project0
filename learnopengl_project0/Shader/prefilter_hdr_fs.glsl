#version 330 core

uniform float roughness;
uniform samplerCube environmentMap;
out vec4 FragColor;
in vec3 localPos;

const float PI = 3.14159265359;

float DistributionGGX(vec3 N, vec3 H, float roughness)
{
    float a = roughness*roughness;
    float a2 = a*a;
    float NdotH = max(dot(N, H), 0.0);
    float NdotH2 = NdotH*NdotH;

    float nom   = a2;
    float denom = (NdotH2 * (a2 - 1.0) + 1.0);
    denom = PI * denom * denom;

    return nom / denom;
}

float RadicalInverse_VdC(uint bits) 
{
    bits = (bits << 16u) | (bits >> 16u);
    bits = ((bits & 0x55555555u) << 1u) | ((bits & 0xAAAAAAAAu) >> 1u);
    bits = ((bits & 0x33333333u) << 2u) | ((bits & 0xCCCCCCCCu) >> 2u);
    bits = ((bits & 0x0F0F0F0Fu) << 4u) | ((bits & 0xF0F0F0F0u) >> 4u);
    bits = ((bits & 0x00FF00FFu) << 8u) | ((bits & 0xFF00FF00u) >> 8u);
    return float(bits) * 2.3283064365386963e-10; // / 0x100000000
}
vec2 Hammersley(uint i, uint N)
{
    return vec2(float(i)/float(N), RadicalInverse_VdC(i));
}  
/*对于GGX进行重要性采样的意义是，粗糙的表面会让镜面反射采样方向从单一向量方向(绝对光滑)
随着表面变得粗糙越发发散成一大个波瓣，因此生成重要性采样的函数肯定要把roughness
纳入参数
生成重要性采样的函数，生成的所谓采样是一个vec3, 即波瓣内的一条向量，我们可以在切线空间
生成这个采样，并进而用半球的球坐标描述这个向量，那么我们其实只要生成两个值就能支持采样，
分别把两个值映射到天顶角[0, pi/2]和航向角[0, 2*pi]，然后计算球坐标就能得到最终的向量

这个过程当中有几个重要的默认：
1. 对低差异序列的所有x进行线性变换f(x),对所有y进行线性变换g(y),组合成的f(x),g(y)依然是低差异序列
2. 再把x和y当作航向角和天顶角计算球坐标，计算得到的球坐标依然是低差异序列*/
vec3 GetImportanceSampleDir_WorldSpace(vec2 hammersleyX2, vec3 N, float roughness)
{
    //hammersleyX2.xy in [0, 1]
    float phi = 2.0 * PI * hammersleyX2.x; //航向角
    float cosTheta = hammersleyX2.y; //cos天顶角
    //这一步，根据迪士尼的PBR方案抄写，不要深究有什么意义，PBR当中很多项都是经验公式
    //但是我们能够知道的是，这个公式中roughness越大，pow越大，-1的负数绝对值越小，从而分母越大
    //从而costheta越小，意味着theta天顶角越大，意味着lobe越散、越宽、越大，这和分析是对的上的  
    cosTheta = (1.0 - cosTheta) / (1.0 + (pow(roughness, 4) - 1.0) * cosTheta);
    cosTheta = sqrt(cosTheta);
    
    float sinTheta = sqrt(1.0 - cosTheta * cosTheta);
    // from spherical coordinates to cartesian coordinates
    vec3 H;
    H.x = cos(phi) * sinTheta;
    H.y = sin(phi) * sinTheta;
    H.z = cosTheta;

    // from tangent-space vector to world-space sample vector
    vec3 up        = abs(N.z) < 0.999 ? vec3(0.0, 0.0, 1.0) : vec3(1.0, 0.0, 0.0);
    //如果法线太过挺拔，几乎贴着世界天顶方向，这里选择用x作为Up，gpt的解释是如果此时仍然用z作为up方向，
    //会发现x和y都是小的离谱的两个量，容易放大误差
    vec3 tangent   = normalize(cross(up, N));
    vec3 bitangent = cross(N, tangent);

    vec3 sampleVec = tangent * H.x + bitangent * H.y + N * H.z;
    return normalize(sampleVec);
}

void main()
{
    vec3 N = normalize(localPos);//天空盒惯例，法线 = 世界坐标 - 原点
    //重要假设，观察方向 = 法线方向 = 反射方向，背后的原理是
    vec3 R = N;
    vec3 V = R;
    
    const uint SAMPLE_COUNT = 1024u;
    vec3 prefilteredColor = vec3(0.0);
    float totalWeight = 0.0;
    
    for(uint i = 0u; i < SAMPLE_COUNT; i++)
    {
        vec2 Xi = Hammersley(i, SAMPLE_COUNT);
        vec3 H = GetImportanceSampleDir_WorldSpace(Xi, N, roughness);
        //H是重要性采样生成的采样方向，N是切线空间天顶/法线/观察方向/反射方向
        //后文似乎把H当成了半程向量
        vec3 L = normalize(2.0 * dot(V, H) * H - V);
        //推了好久，才确认，这里L = V关于H的反射，还挺抽象的，因为一般默认就认为N是法线了，其实这里N=V不是法线，把H当成法线了
        
        float NdotL = max(dot(N, L), 0.0);//这也挺奇怪的，这样写，N和L刚才是分居H两侧的对称向量啊，难道H真的是半程向量？
        if(NdotL > 0.0)
        {
            prefilteredColor += texture(environmentMap, L).rgb * NdotL;
            totalWeight += NdotL;
            //权重权衡还考虑了costheta的问题，没有用粗暴+1
            //如果一个采样的天顶角很大，那么他对于prefilteredColor的贡献就小，这时候不应该简单平均，加权是合理的
        } 
    }
    prefilteredColor /= totalWeight;
    
    FragColor = vec4(prefilteredColor, 1.0);
    
}


