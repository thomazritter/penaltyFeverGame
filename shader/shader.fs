#version 330
out vec4 color;
in vec4 finalColor;
void main()
{
    color = finalColor;
}