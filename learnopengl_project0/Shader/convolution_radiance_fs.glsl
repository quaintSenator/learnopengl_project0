#version 330 core
out vec4 FragColor;
in vec3 worldPos;

uniform samplerCube environmentCubemap;
const float PI = 3.14159265359;

void main()
{
    vec3 irradiance = vec3(0.0);
    vec3 N = normalize(worldPos);//从世界原点到fragPos连线，即为半球枢轴/法线
    vec3 Z = N;
    vec3 Up = vec3(0.0, 1.0, 0.0);
    //+Z cross +X = +Y 右手系
    vec3 X = normalize(cross(Up, Z));
    vec3 Y = normalize(cross(Z, X));
    
    float angular_sample_delta = 0.025;
    int reimann_sample_count = 0;
    for(float phi = 0; phi < 2.0 * PI; phi += angular_sample_delta)
    {
        for(float theta = 0; theta < 0.5 * PI; theta += angular_sample_delta)
        {
            vec3 spherePos = vec3(sin(theta) * cos(phi),
                sin(theta) * sin(phi),
                cos(theta)
            );
            vec3 sample_dir_WorldSpace = spherePos.x * X + spherePos.y * Y + spherePos.z * Z;
            irradiance += texture(environmentCubemap, sample_dir_WorldSpace).rgb * cos(theta) * sin(theta);
            reimann_sample_count += 1;
        }
    }
    irradiance = PI * irradiance * (1.0 / float(reimann_sample_count));
    //平均 形成辐照度贴图纹素
    FragColor = vec4(irradiance, 1.0);
}