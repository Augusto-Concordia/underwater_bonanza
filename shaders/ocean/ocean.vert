#version 330 core

struct Light {
    vec3 position;
    vec3 target;
    vec3 color;

    int type;
    float shadows_influence;
    vec3 attenuation;

// a light generally does not have information on how it's going to affect the objects, but this is a shortcut
    float ambient_strength;
    float diffuse_strength;
    float specular_strength;

    float spot_cutoff;

    mat4 light_view_projection;
};

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normals;

uniform Light u_lights[4];

uniform vec3 u_cam_pos;

uniform mat4 u_model_transform;
uniform mat4 u_view_projection;

uniform float light_near_plane = 0.1f;
uniform float light_far_plane = 400.0f;

uniform float u_time;

uniform float light_cutoff_inner;
uniform float light_cutoff_outer;

uniform float fakeWave;


out vec3 fragment_normal;
out vec3 fragment_position;
out vec3 fragment_position_model;
out vec3 fragment_position_original;
out vec4 fragment_position_light_space;

out float light_near;
out float light_far;

out float light_cut_near;
out float light_cut_far;

out float xDerivative;
out float yDerivative;
out float zDerivative;

out float time;
out float fakeWaveUV;


float getNewTime(float x, float z, float speed)
{
    return (sin(u_time*speed+x+z)+1.0f)/2.0f;
}

vec3 getOceanSample(float x, float z)
{
    float   yDisp =     sin(z*0.9f + x*0.7f            + getNewTime(x     , z , 2*2.0f) + u_time * 0.2f)*0.07f * 2.0f;
            yDisp +=    sin(z*1.4f + x*1.3f + 47.84f   + getNewTime(x+1.2f, z , 3*2.0f) + u_time * 0.1f)*0.05f * 1.0f;
            yDisp +=    sin(z*0.5f + x*0.8f - 7.12f    + getNewTime(x+6.5f, z , 1*2.0f) + u_time * 0.05f)*0.09f * 2.0f;

    float   xDisp =     cos(z*0.9f + x*0.7f            + getNewTime(x     , z , 2*2.0f) + u_time * 0.2f * 2.0f)*0.07f * 2.0f;
            xDisp +=    cos(z*1.4f + x*1.3f + 47.84f   + getNewTime(x+1.2f, z , 3*2.0f) + u_time * 0.1f * 2.0f)*0.05f * 1.0f;
            xDisp +=    cos(z*0.5f + x*0.8f - 7.12f    + getNewTime(x+6.5f, z , 1*2.0f) + u_time * 0.05f * 2.0f)*0.09f * 2.0f;

    yDisp *= 1.0f;                                                                             
    xDisp *= 1.0f;  

    return vec3(xDisp, yDisp, 0.0f);
}

void main()
{
    float x = position.x;
    float y = position.y;
    float z = position.z;
    
    
    float sampleOffset = 0.05f;
    vec3 oceanSample = position + getOceanSample(x, z);
    vec3 oceanSample2 = position + vec3(sampleOffset, 0.0f, 0.0f) + getOceanSample(x + sampleOffset, z);
    vec3 oceanSample3 = position + vec3(0.0f, 0.0f, sampleOffset) + getOceanSample(x, z + sampleOffset);
    vec3 newPos = oceanSample;

    if(fakeWave == 1.0f)
        newPos = position;

    vec3 normal = normalize(cross(oceanSample3 - oceanSample, oceanSample2 - oceanSample ));
    xDerivative = normal.x;
    yDerivative = normal.y;
    zDerivative = normal.z;

    time = u_time;
    fakeWaveUV = fakeWave;

    fragment_normal = mat3(u_model_transform) * normal;
    fragment_position = vec3(u_model_transform * vec4(newPos, 1.0));
    fragment_position_model = fragment_position;
    fragment_position_original = position;
    fragment_position_light_space = u_lights[0].light_view_projection * vec4(fragment_position, 1.0);
    gl_Position = u_view_projection * u_model_transform * vec4(newPos, 1.0);

}
