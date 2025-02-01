#version 410 core

out vec4 fColor;
float a = 0.4f;
uniform bool toggle_light;
void main() 
{    
    if (!toggle_light) {
    fColor = vec4(1.0f,1.0f,1.0f, a);
    }
    else {
    fColor = vec4(0.0f,0.0f,0.0f, a);
    }
}
