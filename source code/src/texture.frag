#version 150
// @author  RIT CS Department
// @author  Cinto Alapatt

// INCOMING DATA

// Data coming from the vertex shader
in vec3 lPos;
in vec3 vPos;
in vec3 vNorm;
in vec2 texCoord;

// Data coming from the application
uniform vec4 lightColor;
uniform vec4 ambientLight;
uniform vec4 diffuseColor;
uniform vec4 ambientColor;
uniform vec4 specularColor;
uniform float specExp;
uniform vec3 kCoeff;
uniform sampler2D texturefront;
uniform sampler2D textureback;

// OUTGOING DATA
out vec4 fragColor;

void main()
{
    // calculate lighting vectors
    vec3 L = normalize(lPos - vPos);
    vec3 N = normalize(vNorm);
    vec3 R = normalize(reflect(-L, N));
    vec3 V = normalize(-vPos);

    vec4 ambient  = vec4(0.0);  // ambient color component
    vec4 diffuse  = vec4(0.0);  // diffuse color component
    vec4 specular = vec4(0.0);  // specular color component
    float specDot;  // specular dot(R,V) ^ specExp value

    if (gl_FrontFacing)
    {
        ambient = ambientLight * texture(texturefront, texCoord) * max(dot(N, L), 0.0);
        diffuse = lightColor * texture(texturefront, texCoord);
        specDot = pow(max(dot(R, V), 0.0), specExp);
        specular = lightColor * texture(texturefront, texCoord) * specDot;
    }
    else
    {
        ambient = ambientLight * texture(textureback, texCoord) * max(dot(N, L), 0.0);
        diffuse = lightColor * texture(textureback, texCoord);
        specDot = pow(max(dot(R, V), 0.0), specExp);
        specular = lightColor * texture(textureback, texCoord) * specDot;
    }

    // final color
    vec4 color = (kCoeff.x * ambient) +
                  (kCoeff.y * diffuse) +
                  (kCoeff.z * specular);

    fragColor = color;
}
