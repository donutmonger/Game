#version 330

in vec2 Texcoord;
in vec3 surface_normal;
in vec3 light_vector;
in vec3 viewing_vector;

out vec4 outColor;

uniform float time;
uniform sampler2D diffuse_texture;
uniform sampler2D specular_texture;
uniform sampler2D normal_map;
uniform sampler2D emissive_texture;

void main() {
    
    vec3 light_color = vec3(1.0, 0.1, 0.0);
    float light_power = abs(0.25*sin(2.0*time) + 1.0) * 5.0;
    float intensity = light_power / (pow(light_vector.x, 2) + pow(light_vector.y, 2) + pow(light_vector.z, 2));
    
    float cosTheta = dot(normalize(surface_normal), normalize(light_vector));
    cosTheta = clamp(cosTheta, 0.0, 1.0);

    vec3 reflection = reflect(-normalize(light_vector), normalize(surface_normal));
    float cosAlpha = clamp(dot(normalize(viewing_vector), reflection), 0.0, 1.0);

    vec4 diffuse = texture(diffuse_texture, Texcoord);
    vec4 specular = texture(specular_texture, Texcoord);
    vec4 emissive = texture(emissive_texture, Texcoord);

    vec4 ambient_component = vec4(0.1, 0.1, 0.1, 1.0) * diffuse;

    vec4 diffuse_component = vec4(diffuse.rgb * intensity * cosTheta, diffuse.w);

    float specularity = specular.w;
    vec4 specular_component = vec4(specular.rgb * specularity * intensity * pow(cosAlpha,15), specular.w);

    vec4 emissive_component = vec4(emissive.rgb, 1.0);

    // ambient_component  = vec4(0.0, 0.0, 0.0, 0.0);
    // diffuse_component  = vec4(0.0, 0.0, 0.0, 0.0);
    // specular_component = vec4(0.0, 0.0, 0.0, 0.0);
    // emissive_component = vec4(1.0, 0.0, 1.0, 1.0);

    // outColor = vec4(1.0, 0.0, 1.0, 1.0);

    vec4 texel = mix(vec4(light_color, 1.0) * (diffuse_component + specular_component + ambient_component),
                emissive_component, emissive.a);
    if (texel.a < 0.5){
        discard;
    }
    outColor = texel;
}