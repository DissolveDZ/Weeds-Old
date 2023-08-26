#version 330

in vec3 vertexPosition;
in vec2 vertexTexCoord;
in vec3 vertexNormal;
in vec4 vertexColor;

in mat4 instanceTransform;

uniform mat4 mvp;
uniform mat4 matNormal;

out vec3 fragPosition;
out vec2 fragTexCoord;
out vec4 fragColor;
out vec3 fragNormal;


void main()
{
    mat4 mvpi = mvp*instanceTransform;
    fragPosition = vec3(mvpi*vec4(vertexPosition, 1.0));
    fragTexCoord = vertexTexCoord;
    fragColor = vertexColor;
    fragNormal = normalize(vec3(matNormal*vec4(vertexNormal, 1.0)));
    gl_Position = mvpi*vec4(vertexPosition, 1.0);
}