#version 150

// Use this as the basis for each of your vertex shaders.
// Be sure to replace "SHADER" with the specific shader type
// and "YOUR_NAME_HERE" with your own name.


// Texture vertex shader

// @author  RIT CS Department
// @author  Cinto Alapatt


// Vertex attributes


// Vertex position (in model space)
in vec4 vPosition;

// Normal vector at vertex (in model space)
in vec3 vNormal;

// Texture coordinate for this vertex
in vec2 vTexCoord;


// Uniform data
// Camera and projection matrices
uniform mat4 viewMat;   // view (camera)
uniform mat4 projMat;   // projection
uniform mat4 modelMat;  // composite

// Light position is given in world space
uniform vec4 lightPosition;

// OUTGOING DATA

// Vectors to "attach" to vertex and get sent to fragment shader
// Vectors and points will be passed in "eye" space
out vec3 lPos;
out vec3 vPos;
out vec3 vNorm;

// ADD ANY OUTGOING VARIABLES YOU NEED HERE
out vec2 texCoord; 

void main()
{
    // create the modelview matrix
    mat4 modelViewMat = viewMat * modelMat;

    // All vectors need to be converted to "eye" space
    // All vectors should also be normalize
    vec4 vertexInEye = modelViewMat * vPosition;
    vec4 lightInEye = viewMat * lightPosition;
    
    mat3 normMat = inverse( transpose( mat3(modelViewMat) ) );
    vec4 normalInEye = vec4( normMat * vNormal, 0.0 );

    // pass our vertex data to the fragment shader
    lPos = lightInEye.xyz;
    vPos = vertexInEye.xyz;
    vNorm = normalInEye.xyz;

    gl_Position =  projMat * modelViewMat *  vPosition;


    // Add any code you need to implement texture mapping here.
    texCoord = vTexCoord;
}
