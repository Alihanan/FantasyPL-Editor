#version 450

// define the number of CPs in the output patch
layout (vertices = 4) out;

// uniform vec3 gEyeWorldPos;

// attributes of the input CPs
layout(location = 0) in vec3 WorldPos_CS_in[];
// in vec2 TexCoord_CS_in[];
// in vec3 Normal_CS_in[];

// attributes of the output CPs
layout(location = 0) out vec3 WorldPos_ES_in[];
// out vec2 TexCoord_ES_in[];
// out vec3 Normal_ES_in[];

void main()
{
    // Set the control points of the output patch
    // TexCoord_ES_in[gl_InvocationID] = TexCoord_CS_in[gl_InvocationID];
    // Normal_ES_in[gl_InvocationID] = Normal_CS_in[gl_InvocationID];
    WorldPos_ES_in[gl_InvocationID] = WorldPos_CS_in[gl_InvocationID];
    gl_out[gl_InvocationID].gl_Position = gl_in[gl_InvocationID].gl_Position;
    // Calculate the distance from the camera to the three control points
    // float EyeToVertexDistance0 = distance(gEyeWorldPos, WorldPos_ES_in[0]);
    // float EyeToVertexDistance1 = distance(gEyeWorldPos, WorldPos_ES_in[1]);
    // float EyeToVertexDistance2 = distance(gEyeWorldPos, WorldPos_ES_in[2]);

    // Calculate the tessellation levels
    // gl_TessLevelOuter[0] = 2.0f;//GetTessLevel(EyeToVertexDistance1, EyeToVertexDistance2);
    // gl_TessLevelOuter[1] = 2.0f;//GetTessLevel(EyeToVertexDistance2, EyeToVertexDistance0);
    // gl_TessLevelOuter[2] = 2.0f;//GetTessLevel(EyeToVertexDistance0, EyeToVertexDistance1);
    // gl_TessLevelInner[0] = gl_TessLevelOuter[2];
    if (gl_InvocationID == 0)
    {
        gl_TessLevelOuter[0] = 16;
        gl_TessLevelOuter[1] = 16;
        gl_TessLevelOuter[2] = 16;
        gl_TessLevelOuter[3] = 16;

        gl_TessLevelInner[0] = 16;
        gl_TessLevelInner[1] = 16;
    }
}
float GetTessLevel(float Distance0, float Distance1)
{
    float AvgDistance = (Distance0 + Distance1) / 2.0;

    if (AvgDistance <= 2.0) {
        return 10.0;
    }
    else if (AvgDistance <= 5.0) {
        return 7.0;
    }
    else {
        return 3.0;
    }
}