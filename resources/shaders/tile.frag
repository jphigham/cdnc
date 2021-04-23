#version 330 core
in vec2 TexCoords;
out vec4 color;

uniform sampler2D tile;
uniform vec3 tileColor;

void main()
{
    
    color = vec4(tileColor, 1.0) * texture(tile, TexCoords);
}