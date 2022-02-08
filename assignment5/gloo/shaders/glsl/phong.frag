#version 330 core

out vec4 frag_color;

uniform sampler2D ambient_sampler;
uniform sampler2D diffuse_sampler;
uniform sampler2D specular_sampler;
uniform sampler2D shadowmap;
uniform mat4 world_to_light_ndc_matrix;


//ambient occlusion variables
uniform mat4 projection;

uniform sampler2D normal_sampler;
uniform sampler2D noise_sampler;
uniform sampler2D position_sampler;

uniform vec3 ssao_samples[64];

// SSAO Parameters
float ssao_kernel_radius = 1.0f;
float ssao_power = 2.0f;
float ssao_bias = 1e-4;

uniform float scr_w; // Screen width
uniform float scr_h; // Screen height

const vec2 noise_scale = vec2(1280.0 / 8.0, 720.0 / 8.0);

out float ambient_occlusion;
//

struct AmbientLight {
    bool enabled;
    vec3 ambient;
};

struct PointLight {
    bool enabled;
    vec3 position;
    vec3 diffuse;
    vec3 specular;
    vec3 attenuation;
};

struct DirectionalLight {
    bool enabled;
    vec3 direction;
    vec3 diffuse;
    vec3 specular;
};

struct SpotLight {
    bool enabled;
    vec3 position;
    vec3 spot_direction;
    vec3 color;
    float cutoff_angle;
};


struct Material {
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float shininess;
};

in vec3 world_position;
in vec3 world_normal;
in vec2 tex_coord;

uniform vec3 camera_position;

uniform Material material; // material properties of the object
uniform AmbientLight ambient_light;
uniform PointLight point_light;
uniform DirectionalLight directional_light;
uniform SpotLight spot_light;

vec3 CalcAmbientLight();
vec3 CalcPointLight(vec3 normal, vec3 view_dir);
vec3 CalcDirectionalLight(vec3 normal, vec3 view_dir);
vec3 CalcSpotLight();

void main() {
    vec3 normal = normalize(world_normal);
    vec3 view_dir = normalize(camera_position - world_position);

    frag_color = vec4(0.0);

    if (ambient_light.enabled) {
        frag_color += vec4(CalcAmbientLight(), 1.0);
    }

    if (point_light.enabled) {
        frag_color += vec4(CalcPointLight(normal, view_dir), 1.0);
    }

    if (directional_light.enabled) {
        frag_color += vec4(CalcDirectionalLight(normal, view_dir), 1.0);
    }

    if (spot_light.enabled) {
        frag_color += vec4(CalcSpotLight(), 1.0);
    }

}

vec3 GetAmbientColor() {
    // Screen position of the pixel
    // vec2 frag_coord = vec2(gl_FragCoord.x / scr_w, gl_FragCoord.y / scr_h);
    // vec3 normal_amb = texture(normal_sampler, frag_coord.xy).xyz;
    // vec3 position = texture(position_sampler, frag_coord.xy).xyz;

    // vec3 rvec = texture(noise_sampler, gl_FragCoord.xy * noise_scale).xyz; // Picks random vector to orient the hemisphere
    // vec3 tangent = normalize(rvec - normal_amb * dot(rvec, normal_amb));
    // vec3 bitangent = cross(normal_amb, tangent);
    // mat3 tbn = mat3(tangent, bitangent, normal_amb); // f: Tangent -> View space

    // ambient_occlusion = 0.0;
    // const int num_ssao_samples = 64;
    // for (int i = 0; i < num_ssao_samples; i++) {
    //     vec3 sampled = position + tbn * ssao_samples[i] * ssao_kernel_radius;

    //     vec4 point = vec4(sampled, 1.0);
    //     point = projection * point;
    //     point.xy /= point.w;
    //     point.xy = point.xy * 0.5 + 0.5;

    //     float point_depth = texture(position_sampler, point.xy).z;

    //     if (point_depth >= sampled.z + ssao_bias) { ambient_occlusion += 1.0; }
    // }
    // // Calculate the average and invert to get OCCLUSION
    // ambient_occlusion = 1.0 - (ambient_occlusion / float(num_ssao_samples));
    // // Enhance the effect
    // //ambient_occlusion = pow(ambient_occlusion, ssao_power);

    return texture(ambient_sampler, tex_coord).rgb;
    //return material.ambient;

}

vec3 GetDiffuseColor() {
    return texture(diffuse_sampler, tex_coord).rgb;
    //return material.diffuse;
}

vec3 GetSpecularColor() {
    return texture(specular_sampler, tex_coord).rgb;
    //return material.specular;
}

vec3 CalcAmbientLight() {
    return ambient_light.ambient * GetAmbientColor();
}

vec3 CalcPointLight(vec3 normal, vec3 view_dir) {
    PointLight light = point_light;
    vec3 light_dir = normalize(light.position - world_position);

    float diffuse_intensity = max(dot(normal, light_dir), 0.0);
    vec3 diffuse_color = diffuse_intensity * light.diffuse * GetDiffuseColor();

    vec3 reflect_dir = reflect(-light_dir, normal);
    float specular_intensity = pow(
        max(dot(view_dir, reflect_dir), 0.0), material.shininess);
    vec3 specular_color = specular_intensity *
        light.specular * GetSpecularColor();

    float distance = length(light.position - world_position);
    float attenuation = 1.0 / (light.attenuation.x +
        light.attenuation.y * distance +
        light.attenuation.z * (distance * distance));

    return attenuation * (diffuse_color + specular_color);
}

vec3 CalcDirectionalLight(vec3 normal, vec3 view_dir) {
    //added after
    vec3 ndc_position = vec3(world_to_light_ndc_matrix*vec4(world_position, 1.0));
    vec3 tex_position = 0.5*(ndc_position + vec3(1.0, 1.0, 1.0));
    float this_depth = tex_position.z;
    float bias = 1e-4;
    float occluder_depth = texture(shadowmap, tex_position.xy).r;

    //PCF
    float shadow = 0.0;
    vec2 texelSize = 1.0 / textureSize(shadowmap, 0);
    //samples 3x3 area around tex_position
    for(int x = -1; x <= 1; ++x)
    {
        for(int y = -1; y <= 1; ++y)
        {
            float occluder_depth2 = texture(shadowmap, tex_position.xy + vec2(x, y) * texelSize).r;
            shadow += this_depth - bias > occluder_depth2 ? 1.0 : 0.0;
        }
    }
    shadow /= 9.0;

    //implementation before PCF
    //if (occluder_depth + bias < this_depth) {
      //return vec3( 0.0 , 0.0, 0.0);
    //}

    DirectionalLight light = directional_light;
    vec3 light_dir = normalize(-light.direction);
    float diffuse_intensity = max(dot(normal, light_dir), 0.0);
    //after PCF, subtract shadow value from light intensity, therefore averaging light value
    vec3 diffuse_color = diffuse_intensity * (light.diffuse - shadow) * GetDiffuseColor();

    vec3 reflect_dir = reflect(-light_dir, normal);
    float specular_intensity = pow(
        max(dot(view_dir, reflect_dir), 0.0), material.shininess);
    vec3 specular_color = specular_intensity *
         light.specular * GetSpecularColor();

    vec3 final_color = diffuse_color + specular_color;
    return final_color;
}

vec3 CalcSpotLight() {
    vec3 ndc_position = vec3(world_to_light_ndc_matrix*vec4(world_position, 1.0));
    vec3 tex_position = 0.5*(ndc_position + vec3(1.0, 1.0, 1.0));
    float this_depth = tex_position.z;
    float bias = 1e-4;
    float occluder_depth = texture(shadowmap, tex_position.xy).r;

    //implementation before PCF
    if (occluder_depth + bias < this_depth) {
      return vec3(0.0 , 0.0, 0.0);
    }

    SpotLight light = spot_light;
    vec3 light_dir = normalize(light.position - world_position);
    float theta = dot(light_dir, light.spot_direction);
    if(theta < light.cutoff_angle){
      return light.color;
    }
    return vec3(0.0);
}
