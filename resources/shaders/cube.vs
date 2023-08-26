
#version 330

// Input vertex attributes
in vec3 vertexPosition;
in vec2 vertexTexCoord;
in vec3 vertexNormal;
in vec4 vertexColor;

in mat4 instanceTransform;

// Input uniform values
uniform mat4 mvp;
uniform mat4 matNormal;

// Output vertex attributes (to fragment shader)
out vec3 fragPosition;
out vec2 fragTexCoord;
out vec4 fragColor;
out vec3 fragNormal;

// NOTE: Add here your custom variables

void main()
{
    vec2 position = vec2(0, 0);

                   // x1  y1   x2   y2
    vec4 rect = vec4(0.2, 0.3, 0.4, 0.5);
    vec2 hv = step(rect.xy, position) * step(position, rect.zw);
    float onOff = hv.x * hv.y;

    gl_FragColor = mix(vec4(0,0,0,0), vec4(1,0,0,0), onOff);
}