#version 450

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 color;
layout(location = 2) in vec3 normal;
layout(location = 3) in vec2 uv;

layout(location = 0) out vec3 fragColor;

layout(set = 0, binding = 0) uniform GlobalUBO {
    mat4 projectionViewMatrix;
    vec3 directionToLight;
} UBO;

layout(push_constant) uniform Push {
    mat4 modelMatrix;
    mat4 normalMatrix;
} push;

const float AMBIENT = 0.09;

//    gl_Position = vec4(push.tranform * position + push.offset, 0.0, 1.0);
void main() {
    gl_Position = UBO.projectionViewMatrix * push.modelMatrix * vec4(position, 1.0);

//    mat3 normalMatrix = transpose(inverse(mat3(push.modelMatrix)));
//    vec3 normalWorldSpace = normalize(normalMatrix * normal);

    vec3 normalWorldSpace = normalize(mat3(push.normalMatrix) * normal);

    float lightIntensity = AMBIENT + max(dot(normalWorldSpace, UBO.directionToLight), 0);

    fragColor = lightIntensity * color;
}