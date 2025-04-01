#version 330

struct MaterialProperties
{
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float shininess;
};

struct LightProperties
{
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    vec4 position;
    vec4 spotDirection;
    float spotCutoff;
};


in vec3 fNormal;
in vec4 fPosition;
in vec4 fTexCoord;

const int MAXLIGHTS = 10;

uniform MaterialProperties material;
uniform LightProperties light[MAXLIGHTS];
uniform int numLights;

/* texture */
uniform sampler2D image;

out vec4 fColor;

int spotlight(LightProperties light, vec4 fPosition){

    if( light.spotCutoff < 0 ) return 1;


    vec3 lightVec;
    vec3 spotDir;
    float cosAngle;

    lightVec = normalize(light.position.xyz - fPosition.xyz);
    spotDir = normalize(-light.spotDirection.xyz);
    cosAngle = dot(lightVec,spotDir);

    if(cosAngle > light.spotCutoff) 
        return 1;
    else   
        return -1;
}

void main()
{
    vec3 lightVec,viewVec,reflectVec;
    vec3 normalView;
    vec3 ambient,diffuse,specular;
    float nDotL,rDotV;


    fColor = vec4(0,0,0,1);

    for (int i=0;i<numLights;i++)
    {
        if (light[i].position.w!=0)
            lightVec = normalize(light[i].position.xyz - fPosition.xyz);
        else
            lightVec = normalize(-light[i].position.xyz);

        vec3 tNormal = fNormal;
        normalView = normalize(tNormal.xyz);
        nDotL = dot(normalView,lightVec);

        viewVec = -fPosition.xyz;
        viewVec = normalize(viewVec);

        reflectVec = reflect(-lightVec,normalView);
        reflectVec = normalize(reflectVec);

        rDotV = max(dot(reflectVec,viewVec),0.0);

        if (spotlight(light[i],fPosition) == 1 ) {
            ambient = material.ambient * light[i].ambient;
            diffuse = material.diffuse * light[i].diffuse * max(nDotL,0);
            if (nDotL>0) {
                specular = material.specular * light[i].specular * pow(rDotV,material.shininess);
            }
            else {
                specular = vec3(0,0,0);
            }
            ambient = material.ambient;
        }
        else {
            ambient = vec3(0,0,0);
            diffuse = vec3(0,0,0);
            specular = vec3(0,0,0);
        }
            
        fColor = fColor + vec4(ambient+diffuse+specular,1.0);

    }
    fColor = fColor * texture(image,fTexCoord.st);

    
}
