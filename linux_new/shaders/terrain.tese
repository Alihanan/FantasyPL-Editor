#version 450

layout (quads, fractional_odd_spacing, ccw) in;


// layout(binding = 0) uniform UniformBufferObject {
//     mat4 model;
//     mat4 view;
//     mat4 proj;
// } ubo;

layout(location = 0) in vec3 outLocalPosition[];
//layout(location = 1) in vec3 outNormal;

void main()
{
    float u = gl_TessCoord.x;
    float v = gl_TessCoord.y;

    // gl_Position = ubo.proj * ubo.view * ubo.model * vec4(outLocalPosition[0], 1.0);
    gl_Position = vec4(outLocalPosition[0], 1.0);
}