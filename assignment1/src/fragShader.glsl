#version 330 core

out vec4 FragColor;
in vec3 Normal;
in vec3 FragPos;
in vec3 light_pos_view_1;
in vec3 light_pos_view_2;

// object color
vec4 object_color = vec4(0.9f, 0.9f, 0.9f, 1.0f);

vec4 ordinary_light_output;

void calc_ordinary() {
     // two light sources color
     vec3 light_color_1 = vec3(0.5, 0.4, 0.1);
     vec3 light_color_2 = vec3(0.7, 0.2, 0.7);

     // Ambient
     float ambient_factor = 0.1;  // ambient strength * light color = ambient color

     // Diffuse
     vec3 normal_n = normalize(Normal);
     vec3 light_direction_1 = normalize(light_pos_view_1 - FragPos);  // direction vector from lightsource to fragment position
     vec3 light_direction_2 = normalize(light_pos_view_2 - FragPos);
     float diffuse_factor_1 = max(dot(normal_n, light_direction_1), 0.0);  // calc dot product to get the effect of lightsource to fragment
     float diffuse_factor_2 = max(dot(normal_n, light_direction_2), 0.0);

     // Specular
     float specular_strength = 0.8;
     vec3 reflectDir1_view = reflect(-light_direction_1, normal_n);  // direction from fragment to lightsource
     vec3 reflectDir2_view = reflect(-light_direction_2, normal_n);
     float specular_factor_1 = specular_strength * pow(max(dot(reflectDir1_view, normalize(-FragPos)), 0.0), 32);
     float specular_factor_2 = specular_strength * pow(max(dot(reflectDir2_view, normalize(-FragPos)), 0.0), 32);

     // Sum
     vec4 ordinary_light_1 = (ambient_factor + diffuse_factor_1 + specular_factor_1) * vec4(light_color_1, 1.0f) * object_color;
     vec4 ordinary_light_2 = (ambient_factor + diffuse_factor_2 + specular_factor_2) * vec4(light_color_2, 1.0f) * object_color;

     ordinary_light_output = ordinary_light_1 + ordinary_light_2;
}

// light source 3: carrying a spotlight with the camera
uniform vec3 spot_position;  // camera position = light position = view position
uniform vec3 spot_direction;  // camera gaze (front)

vec4 spotlight_output;

// light source 3: carrying a spotlight with the camera
void calc_spotlight() {
     float dist = length(spot_position - FragPos);
     float attenuation = 1.0 / (1.0 + 0.0014 * dist + 0.000007 * (dist * dist));

     vec3 norm = normalize(Normal);
     vec3 LightDirection = normalize(spot_position - FragPos);
     vec3 ViewDirection = normalize(spot_position - FragPos);

     // Ambient Light
     vec3 ambient = vec3(0.05, 0.05, 0.05);  // not too big since there's enough ambient from 2 ordinary lights

     // Diffuse Light
     float DiffuseStrength = max(0.0, dot(norm, LightDirection));
     vec3 diffuse = DiffuseStrength * vec3(0.60, 0.60, 0.25);
     diffuse *= attenuation;

     // Specular Light
     vec3 ReflectDirection = reflect(-LightDirection, norm);
     float SpecularStrength = pow(max(dot(ViewDirection, ReflectDirection), 0.0), 32);
     vec3 specular = SpecularStrength * vec3(1.0, 1.0, 1.0);
     specular *= attenuation;

     float theta = dot(LightDirection, normalize(-spot_direction));
     float epsilon = (cos(radians(5.0)) - cos(radians(10.0)));
     float intensity = clamp((theta - cos(radians(10.0))) / epsilon, 0.0, 1.0);
     diffuse *= intensity;
     specular *= intensity;

     // Result Color
     spotlight_output = vec4((ambient + diffuse + specular) * object_color.xyz, 1.0);
}

void main() {
     // [[deprecated]] Paint the object with pure orange
     // FragColor = vec4(1.0, 0.5, 0.2, 1.0);

     calc_ordinary();
     calc_spotlight();
     FragColor = ordinary_light_output + spotlight_output;
}



