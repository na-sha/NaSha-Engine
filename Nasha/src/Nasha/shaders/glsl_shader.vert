#version 450

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 color;
layout(location = 2) in vec3 normal;
layout(location = 3) in vec2 uv;

layout(location = 0) out vec3 fragColor;

layout(set = 0, binding = 0) uniform GlobalUBO {
    mat4 projectionViewMatrix;
    vec4 ambientLightColor;
    vec3 lightPosition;
    vec4 lightColor;
} UBO;

layout(push_constant) uniform Push {
    mat4 modelMatrix;
    mat4 normalMatrix;
} push;

const float AMBIENT = 0.09;

//    gl_Position = vec4(push.tranform * position + push.offset, 0.0, 1.0);
void main() {
    vec4 positionWorld = push.modelMatrix * vec4(position, 1.0);
    gl_Position = UBO.projectionViewMatrix * positionWorld;

    vec3 normalWorldSpace = normalize(mat3(push.normalMatrix) * normal);

    vec3 directionToLight = UBO.lightPosition - positionWorld.xyz;

    vec3 lightColor = UBO.lightColor.xyz * UBO.lightColor.w;
    vec3 ambientLight = UBO.ambientLightColor.xyz * UBO.ambientLightColor.w;
    vec3 diffuseLight = lightColor * max(dot(normalWorldSpace, normalize(directionToLight)), 0);

    fragColor = (diffuseLight + ambientLight) * color;
}