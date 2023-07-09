#version 450

const vec2 OFFSETS[6] = vec2[](
    vec2(-1.0, -1.0),
    vec2(-1.0, 1.0),
    vec2(1.0, -1.0),
    vec2(1.0, -1.0),
    vec2(-1.0, 1.0),
    vec2(1.0, 1.0)
);

layout (location = 0) out vec2 fragOffset;

layout(set = 0, binding = 0) uniform GlobalUbo {
    mat4 projection;
    mat4 view;
    vec4 ambientLightColor; // w is intensity
    vec3 lightPosition;
    vec4 lightColor;
} UBO;

const float LIGHT_RADIUS = 0.05;

void main() {
    fragOffset = OFFSETS[gl_VertexIndex];
    vec3 cameraRightWorld = {UBO.view[0][0], UBO.view[1][0], UBO.view[2][0]};
    vec3 cameraUpWorld = {UBO.view[0][1], UBO.view[1][1], UBO.view[2][1]};

    vec3 positionWorld = UBO.lightPosition.xyz
    + LIGHT_RADIUS * fragOffset.x * cameraRightWorld
    + LIGHT_RADIUS * fragOffset.y * cameraUpWorld;

    gl_Position = UBO.projection * UBO.view * vec4(positionWorld, 1.0);
}
