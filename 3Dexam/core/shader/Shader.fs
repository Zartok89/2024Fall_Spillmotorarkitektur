#version 330 core  
out vec4 FragColor;  

in vec3 ourColor;  
in vec2 UV;  
in vec3 FragPos;  
in vec3 Normal;  

uniform sampler2D texture1;  
uniform bool useTexture;  

uniform vec3 lightPos;  
uniform vec3 viewPos;  
uniform vec3 lightColor;  

void main()  
{  
    vec3 ambient, diffuse, specular;  
    
    // Ambient  
    float ambientStrength = 0.2; 
    ambient = ambientStrength * lightColor;  
    
    // Diffuse   
    vec3 norm = normalize(Normal);  
    vec3 lightDir = normalize(lightPos - FragPos);  
    float diff = max(dot(norm, lightDir), 0.0);  
    diffuse = diff * lightColor;  
    
    // Specular  
    float specularStrength = 0.6;
    vec3 viewDir = normalize(viewPos - FragPos);  
    vec3 reflectDir = reflect(-lightDir, norm);  
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);  
    specular = specularStrength * spec * lightColor;  
    
    // Combine results  
    vec3 phongResult = (ambient + diffuse + specular) * ourColor;  

    if (useTexture)  
    {  
        vec4 texColor = texture(texture1, UV);  
        phongResult *= texColor.rgb;  
    }  

    // Apply gamma correction  
    vec3 gammaCorrectedColor = pow(phongResult, vec3(1.0/2.2));  
    FragColor = vec4(gammaCorrectedColor, 1.0);  
}  