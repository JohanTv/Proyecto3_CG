#version 330 core
out vec4 FragColor;

in vec3 fColor;
in vec3 normal;
in vec3 FragPos;
vec3 lightPos=vec3(10.0);
vec3 lightColor=vec3(1.0);
void main(){
    //FragColor = vec4(fColor, 1.0);
    //FragColor = vec4(normal, 1.0);
    // ambient
    float ambientStrength = 0.1;
    vec3 ambient = ambientStrength * lightColor;

    // diffuse
    vec3 norm = normalize(normal);
    vec3 lightDir = normalize(lightPos - FragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * lightColor;

    vec3 result = (ambient + diffuse) * fColor;
    //vec3 result = (ambient) * fColor;
    FragColor = vec4(result, 1.0);
} 