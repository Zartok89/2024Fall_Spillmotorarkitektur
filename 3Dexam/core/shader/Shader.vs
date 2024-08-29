#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aUV;
layout (location = 3) in vec3 aColor;
  
out vec3 ourColor; // output a color to the fragment shader
out vec2 UV;

// uniform mat4 transform;
uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;
uniform int test;

void main()
{
    gl_Position = projection * view * model * vec4(aPos, 1.0);
    ourColor = aColor; // set ourColor to the input color we got from the vertex data
    UV = vec2(aUV.x, aUV.y);
}