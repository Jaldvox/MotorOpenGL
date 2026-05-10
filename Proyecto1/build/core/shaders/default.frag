#version 330 core
out vec4 FragColor;

in vec3 FragPos;
in vec3 Normal;
in vec2 vTexCoords;
in vec4 FragPosLightSpace;

// -------------------------------------------------------
// Constante: debe coincidir con LightManager::MAX_POINT_LIGHTS
// -------------------------------------------------------
#define MAX_POINT_LIGHTS 8

// -------------------------------------------------------
// Structs
// -------------------------------------------------------
struct Material {
    vec3  ambient;
    vec3  diffuse;
    vec3  specular;
    float shininess;
};

struct GlobalLight {        // Luz direccional unica (sol)
    vec3  color;
    vec3  direction;
    float intensity;
};

struct PointLight {         // Luces puntuales
    vec3  position;
    vec3  color;
    float intensity;
    float constant;
    float linear;
    float quadratic;
};

// -------------------------------------------------------
// Uniforms
// -------------------------------------------------------
uniform GlobalLight globalLight;

uniform PointLight  pointLights[MAX_POINT_LIGHTS];
uniform int         numPointLights;

uniform Material    material;
uniform sampler2D   albedoMap;
uniform vec2        albedoTiling;
uniform vec2        albedoOffset;

uniform bool        useAlbedoTex;
uniform vec3        cameraPos;

uniform sampler2D shadowMap;

// Función de shadow 
float calcShadow(vec4 fragPosLightSpace, vec3 norm, vec3 lightDir) {
    // Pasar de clip space [-1,1] a [0,1]
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    projCoords = projCoords * 0.5 + 0.5;

    // Si está fuera del frustum de la luz, no hay sombra
    if (projCoords.z > 1.0) return 0.0;

    float closestDepth = texture(shadowMap, projCoords.xy).r;
    float currentDepth = projCoords.z;

    // Bias dinámico para evitar shadow acne
    float bias = max(0.0001 * (1.0 - dot(norm, lightDir)), 0.00001);

    // PCF (Percentage Closer Filtering) suaviza los bordes
    float shadow = 0.0;
    vec2 texelSize = 1.0 / textureSize(shadowMap, 0);
    for (int x = -2; x <= 2; ++x) {
        for (int y = -2; y <= 2; ++y) {
            float pcfDepth = texture(shadowMap,
                projCoords.xy + vec2(x, y) * texelSize).r;
            shadow += (currentDepth - bias > pcfDepth) ? 1.0 : 0.0;
        }
    }
    return shadow / 25.0;
}

// -------------------------------------------------------
// Funciones de calculo de luz
// -------------------------------------------------------

vec3 calcGlobalLight(GlobalLight light, vec3 norm, vec3 viewDir, vec3 baseColor) {
    vec3 lightDir = normalize(-light.direction);

    // Ambient
    vec3 ambient = material.ambient * light.color;

    // Diffuse
    float diff   = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * material.diffuse * light.color;

    // Specular
    vec3 halfwayDir = normalize(lightDir + viewDir);
    float spec = pow(max(dot(norm, halfwayDir), 0.0), 16.0);
    vec3  specular   = material.specular * spec * light.color * step(0.0001, diff);

    float shadow = calcShadow(FragPosLightSpace, norm, lightDir);
    // El ambient nunca se oscurece; diffuse y specular sí
    vec3 lighting = ambient + (1.0 - shadow) * (diffuse + specular);

    return lighting * light.intensity * baseColor;
}

vec3 calcPointLight(PointLight light, vec3 norm, vec3 viewDir, vec3 baseColor) {
    vec3  toLight     = light.position - FragPos;
    vec3  lightDir    = normalize(toLight);
    float dist        = length(toLight);
    float attenuation = 1.0 / (light.constant
                              + light.linear    * dist
                              + light.quadratic * dist * dist);

    // Diffuse
    float diff   = max(dot(norm, lightDir), 0.0);
    vec3  diffuse = diff * material.diffuse * light.color;

    // Specular
    vec3 halfwayDir = normalize(lightDir + viewDir);
    float spec = pow(max(dot(norm, halfwayDir), 0.0), 16.0);
    vec3  specular   = material.specular * spec * light.color * step(0.0001, diff);

    // No sumamos ambient por punto: el global ya la aporta
    return (diffuse + specular) * light.intensity * attenuation * baseColor;
}

// -------------------------------------------------------
// Main
// -------------------------------------------------------
void main() {
    vec2 finalUV = (vTexCoords * albedoTiling) + albedoOffset;

    vec4 baseColor = useAlbedoTex ? texture(albedoMap, finalUV) : vec4(material.diffuse, 1);
    vec3 norm      = normalize(Normal);
    vec3 viewDir   = normalize(cameraPos - FragPos);

    // Luz direccional global
    vec3 result = calcGlobalLight(globalLight, norm, viewDir, baseColor.rgb);

    // Luces puntuales
    int n = min(numPointLights, MAX_POINT_LIGHTS);
    for (int i = 0; i < n; ++i) {
        result += calcPointLight(pointLights[i], norm, viewDir, baseColor.rgb);
    }

    FragColor = vec4(result, 1.0);
}
