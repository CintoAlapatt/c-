//
//  Materials.cpp
//
//  Simple class for setting up material property parameters.
//
//  Created by Warren R. Carithers 2021/11/11
//  Based on code created by Joe Geigel on 1/23/13.
//  Copyright 2021 Rochester Institute of Technology. All rights reserved.
//
//  Contributor:  Cinto Alapatt
//

#include <iostream>


#include "Materials.h"

#include "Models.h"
#include "Lighting.h"
#include "Utils.h"

#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <glm/geometric.hpp>
#include <glm/gtc/type_ptr.hpp>

// this is here in case you are using SOIL;
// if you're not, it can be deleted.
#include <SOIL.h>

using namespace std;

// reflective coefficients
static glm::vec3 k( 0.5, 0.7f, 1.0f );

// specular highlights (order depends upon the Object type in Models.h)
static GLfloat specExp[N_OBJECTS] = {
    20.0f,  // Cylinder
    20.0f,  // Discs
    20.0f,  //sphere
    20.0f,  //sphere
    20.0f,  //sphere
    120.0f,  //Cube
    30.0f,  //Cube2
    30.0f,  //Cube3
    20.0f,   //SemiSphere
    80.0f,   //Prism
    80.0f,   //Prism2
    90.0f,  //Plate
    90.0f,  //Plateside
    120.0f,  //bread1
    120.0f,  //bread1a
    90.0f,  //bread2
    90.0f,  //bread2a
    90.0f,   //bread3
    90.0f,  //bread3a
    20.0f,   //teapot
    20.0f,  //cylinder
    90.0f,  //fork
    90.0f,  //cylinder3
    90.0f    //cylinder4
};
 
// These variables are used in the framework only when doing Phong shading
static glm::vec4 specular           (1.00f, 1.00f, 1.00f, 1.00f);

static glm::vec4 cyl_ambient        (0.50f, 0.10f, 0.90f, 1.00f);
static glm::vec4 cyl_diffuse        (0.89f, 0.00f, 0.00f, 1.00f);

static glm::vec4 sph_ambient        (0.80f, 0.70f, 0.10f, 1.00f); // yellowish ambient color
static glm::vec4 sph_diffuse        (0.95f, 0.80f, 0.10f, 1.00f); // yellowish diffuse color

static glm::vec4 cube_ambient       (0.50f, 0.70f, 0.90f, 1.00f);
static glm::vec4 cube_diffuse       (0.89f, 0.70f, 0.90f, 1.00f);

static glm::vec4 cube2_ambient      (0.60f, 0.40f, 0.30f, 1.00f); // wooden-like ambient color
static glm::vec4 cube2_diffuse      (0.75f, 0.55f, 0.40f, 1.00f); // wooden-like diffuse color

static glm::vec4 cube3_ambient      (0.10f, 0.50f, 0.10f, 1.00f);
static glm::vec4 cube3_diffuse      (0.00f, 0.89f, 0.00f, 1.00f);

static glm::vec4 semisph_ambient    (0.10f, 0.70f, 0.90f, 1.00f);
static glm::vec4 semisph_diffuse    (0.19f, 0.70f, 0.90f, 1.00f);

static glm::vec4 prism_ambient      (0.65f, 0.65f, 0.65f, 1.00f); // soft pastel ambient color
static glm::vec4 prism_diffuse      (0.85f, 0.85f, 0.85f, 1.00f); // soft pastel diffuse

static glm::vec4 prism2_ambient     (1.00f, 0.94f, 0.82f, 1.00f); // soft pastel ambient color
static glm::vec4 prism2_diffuse     (1.00f, 0.94f, 0.82f, 1.00f);

static glm::vec4 plate_ambient      (0.65f, 0.65f, 0.65f, 1.00f); // soft pastel ambient color
static glm::vec4 plate_diffuse      (0.85f, 0.85f, 0.85f, 1.00f); //color

static glm::vec4 plate2_ambient     (0.65f, 0.65f, 0.65f, 1.00f); // soft pastel ambient color
static glm::vec4 plate2_diffuse     (0.85f, 0.85f, 0.85f, 1.00f); //color

static glm::vec4 bread1_ambient     (0.90f, 0.85f, 0.80f, 1.00f);
static glm::vec4 bread1_diffuse     (0.90f, 0.85f, 0.80f, 1.00f);

static glm::vec4 bread1a_ambient    (0.38f, 0.94f, 0.88f, 1.0f);
static glm::vec4 bread1a_diffuse    (0.38f, 0.94f, 0.88f, 1.0f);

static glm::vec4 bread2_ambient     (0.90f, 0.85f, 0.80f, 1.00f);
static glm::vec4 bread2_diffuse     (0.90f, 0.85f, 0.80f, 1.00f);

static glm::vec4 bread2a_ambient    (0.38f, 0.94f, 0.88f, 1.0f);
static glm::vec4 bread2a_diffuse    (0.38f, 0.94f, 0.88f, 1.0f);

static glm::vec4 bread3_ambient     (0.90f, 0.85f, 0.80f, 1.00f);
static glm::vec4 bread3_diffuse     (0.90f, 0.85f, 0.80f, 1.00f);

static glm::vec4 bread3a_ambient    (0.38f, 0.94f, 0.88f, 1.0f);
static glm::vec4 bread3a_diffuse    (0.38f, 0.94f, 0.88f, 1.0f);

static glm::vec4 teapot_ambient     (0.38f, 0.94f, 0.88f, 1.0f);
static glm::vec4 teapot_diffuse     (0.38f, 0.94f, 0.88f, 1.0f);

static glm::vec4 cyl2_ambient       (0.50f, 0.10f, 0.90f, 1.00f);
static glm::vec4 cyl2_diffuse       (0.89f, 0.00f, 0.00f, 1.00f);

static glm::vec4 fork_ambient       (0.25f, 0.25f, 0.25f, 1.00f);
static glm::vec4 fork_diffuse       (0.40f, 0.40f, 0.40f, 1.00f);

static glm::vec4 cyl3_ambient(0.50f, 0.10f, 0.90f, 1.00f);
static glm::vec4 cyl3_diffuse(0.89f, 0.00f, 0.00f, 1.00f);

static glm::vec4 cyl4_ambient(0.50f, 0.10f, 0.90f, 1.00f);
static glm::vec4 cyl4_diffuse(0.89f, 0.00f, 0.00f, 1.00f);




// Add any global definitions and/or variables you need here.

///
/// This function initializes all texture-related data structures for
/// the program.  This is where texture buffers should be created, where
/// texture images should be loaded, etc.
///
/// You will need to write this function, and maintain all of the values
/// needed to be sent to the various shaders.
///
void flipVertically(unsigned char* image, int width, int height, int channels) {
    int rowSize = width * channels;
    unsigned char* rowBuffer = (unsigned char*)malloc(rowSize);

    for (int y = 0; y < height / 2; y++) {
        int oppositeY = height - y - 1;
        memcpy(rowBuffer, image + y * rowSize, rowSize);
        memcpy(image + y * rowSize, image + oppositeY * rowSize, rowSize);
        memcpy(image + oppositeY * rowSize, rowBuffer, rowSize);
    }
    free(rowBuffer);
}

void initTextures(void)
{
    // Load and configure heads texture
    int width, height;
    unsigned char* image;
    GLuint  textureWall, textureDisc, texturetable, textureBack, texturebasket, texturecloth,textureplate, textureside,texturebreadtop,texturebreadside ;

    // Generate texture IDs for all the texturing
    glGenTextures(1, &textureWall);
    glGenTextures(1, &textureDisc);
    glGenTextures(1, &texturetable);
    glGenTextures(1, &textureBack);
    glGenTextures(1, &texturebasket);
    glGenTextures(1, &texturecloth);
    glGenTextures(1, &textureplate);
    glGenTextures(1, &texturebreadtop);
    glGenTextures(1, &texturebreadside);
    glGenTextures(1, &textureside);




    // Load table texture
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texturetable);
    image = SOIL_load_image("wood039.jpg", &width, &height, 0, SOIL_LOAD_RGB);
    if (image == 0) {
        printf("SOIL loading error: '%s'\n", SOIL_last_result());
    }
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
    glGenerateMipmap(GL_TEXTURE_2D);
    SOIL_free_image_data(image);

    // Load backgorund texture
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, textureBack);
    image = SOIL_load_image("wood047.jpg", &width, &height, 0, SOIL_LOAD_RGB);
    if (image == 0) {
        printf("SOIL loading error: '%s'\n", SOIL_last_result());
    }
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
    glGenerateMipmap(GL_TEXTURE_2D);
    SOIL_free_image_data(image);

    // Load wall texture
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, textureWall);
    image = SOIL_load_image("pa092.png", &width, &height, 0, SOIL_LOAD_RGB);
    if (image == 0) {
        printf("SOIL loading error: '%s'\n", SOIL_last_result());
    }
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
    glGenerateMipmap(GL_TEXTURE_2D);
    SOIL_free_image_data(image);


    // Load  disc texture
    glActiveTexture(GL_TEXTURE3);
    glBindTexture(GL_TEXTURE_2D, textureDisc);
    image = SOIL_load_image("coffee.jpg", &width, &height, 0, SOIL_LOAD_RGB);
    if (image == 0) {
        printf("SOIL loading error: '%s'\n", SOIL_last_result());
    }
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
    glGenerateMipmap(GL_TEXTURE_2D);
    SOIL_free_image_data(image);

    // Load basket texture
    glActiveTexture(GL_TEXTURE4);
    glBindTexture(GL_TEXTURE_2D, texturebasket);
    image = SOIL_load_image("basket-texture.jpg", &width, &height, 0, SOIL_LOAD_RGB);
    if (image == 0) {
        printf("SOIL loading error: '%s'\n", SOIL_last_result());
    }
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
    glGenerateMipmap(GL_TEXTURE_2D);
    SOIL_free_image_data(image);

    // Load basket texture
    glActiveTexture(GL_TEXTURE5);
    glBindTexture(GL_TEXTURE_2D, texturecloth);
    image = SOIL_load_image("cloth018.png", &width, &height, 0, SOIL_LOAD_RGB);
    if (image == 0) {
        printf("SOIL loading error: '%s'\n", SOIL_last_result());
    }
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
    glGenerateMipmap(GL_TEXTURE_2D);
    SOIL_free_image_data(image);

    // Load plate texture
    glActiveTexture(GL_TEXTURE6);
    glBindTexture(GL_TEXTURE_2D, textureplate);
    image = SOIL_load_image("platebottom1.jpg", &width, &height, 0, SOIL_LOAD_RGB);
    if (image == 0) {
        printf("SOIL loading error: '%s'\n", SOIL_last_result());
    }
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
    glGenerateMipmap(GL_TEXTURE_2D);
    SOIL_free_image_data(image);

    // Load breadtop texture
    glActiveTexture(GL_TEXTURE7);
    glBindTexture(GL_TEXTURE_2D, texturebreadtop);
    image = SOIL_load_image("breadtop.png", &width, &height, 0, SOIL_LOAD_RGB);
    if (image == 0) {
        printf("SOIL loading error: '%s'\n", SOIL_last_result());
    }
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
    glGenerateMipmap(GL_TEXTURE_2D);
    SOIL_free_image_data(image);

    // Load breadside texture
    glActiveTexture(GL_TEXTURE8);
    glBindTexture(GL_TEXTURE_2D, texturebreadside);
    image = SOIL_load_image("breadtop.jpg", &width, &height, 0, SOIL_LOAD_RGB);
    if (image == 0) {
        printf("SOIL loading error: '%s'\n", SOIL_last_result());
    }
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
    glGenerateMipmap(GL_TEXTURE_2D);
    SOIL_free_image_data(image);

    // Load plate texture
    glActiveTexture(GL_TEXTURE9);
    glBindTexture(GL_TEXTURE_2D, textureside);
    image = SOIL_load_image("plate.png", &width, &height, 0, SOIL_LOAD_RGB);
    if (image == 0) {
        printf("SOIL loading error: '%s'\n", SOIL_last_result());
    }
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
    glGenerateMipmap(GL_TEXTURE_2D);
    SOIL_free_image_data(image);

}
///
/// This function sets up the appearance parameters for the object.
///
/// @param program        The ID of an OpenGL (GLSL) shader program to which
///                       parameter values are to be sent
/// @param obj            The object type of the object being drawn
/// @param usingTextures  Are we texture-mapping this object?
///
void setMaterials(GLuint program, Object obj, bool usingTextures)
{
    ///////////////////////////////////////////////////
    // CODE COMMON TO PHONG SHADING AND TEXTURE MAPPING
    //
    // DO NOT REMOVE THIS SECTION OF CODE
    ///////////////////////////////////////////////////

    // Set the specular exponent for the object
    GLint loc = getUniformLoc(program, "specExp");
    glUniform1f(loc, specExp[obj]);


    // Send down the reflective coefficients
    loc = getUniformLoc(program, "kCoeff");
    if (loc >= 0) {
        glUniform3fv(loc, 1, glm::value_ptr(k));
    }

    ///////////////////////////////////////////////////////////
    // CODE DIFFERING BETWEEN PHONG SHADING AND TEXTURE MAPPING
    ///////////////////////////////////////////////////////////
    
    // texturing
    switch (obj) {
    case Cylinder:
        glUniform1i(glGetUniformLocation(program, "texturefront"), 2); // cylinder using texture unit 2
        break;
    case Discs:
        glUniform1i(glGetUniformLocation(program, "texturefront"), 3);
        break;
    case Cube:
        glUniform1i(glGetUniformLocation(program, "texturefront"), 1);
        break;
    case Cube2:
        glUniform1i(glGetUniformLocation(program, "texturefront"), 0);
        break;
    case SemiSphere:
        glUniform1i(glGetUniformLocation(program, "texturefront"), 4);
        break;
    case Cube3:
        glUniform1i(glGetUniformLocation(program, "texturefront"), 5);
        break;
    case Cylinder2:
        glUniform1i(glGetUniformLocation(program, "texturefront"), 2); // cylinder using texture unit 2
        break;
    case Plate:
        glUniform1i(glGetUniformLocation(program, "texturefront"), 2);
        break;
    case Plateside:
        glUniform1i(glGetUniformLocation(program, "texturefront"), 6);
        break;
    case Bread1a:
        glUniform1i(glGetUniformLocation(program, "texturefront"), 7);
        break;
    case Bread2a:
        glUniform1i(glGetUniformLocation(program, "texturefront"), 7);
        break;
    case Bread3a:
        glUniform1i(glGetUniformLocation(program, "texturefront"), 7);
        break;
    case Bread1:
        glUniform1i(glGetUniformLocation(program, "texturefront"), 8);
        break;
    case Bread2:
        glUniform1i(glGetUniformLocation(program, "texturefront"), 8);
        break;
    case Bread3:
        glUniform1i(glGetUniformLocation(program, "texturefront"), 8);
        break;
    case Cylinder3:
        glUniform1i(glGetUniformLocation(program, "texturefront"), 4);
        break;
    case Cylinder4:
        glUniform1i(glGetUniformLocation(program, "texturefront"), 6);
        break;
      
    default:
       
        // specular color is identical for the objects
        loc = getUniformLoc(program, "specularColor");
        if (loc >= 0) {
            glUniform4fv(loc, 1, glm::value_ptr(specular));
        }

        // ambient and diffuse vary from one object to another
        GLint dloc = getUniformLoc(program, "diffuseColor");
        GLint aloc = getUniformLoc(program, "ambientColor");
        //phong shading for spheres , prism, teapot and fork
        switch (obj) {
        case Sphere:
            if (aloc >= 0) {
                glUniform4fv(aloc, 1, glm::value_ptr(sph_ambient));
            }
            if (dloc >= 0) {
                glUniform4fv(dloc, 1, glm::value_ptr(sph_diffuse));
            }
            break;
        case Sphere2:
            if (aloc >= 0) {
                glUniform4fv(aloc, 1, glm::value_ptr(sph_ambient));
            }
            if (dloc >= 0) {
                glUniform4fv(dloc, 1, glm::value_ptr(sph_diffuse));
            }
            break;
        case Sphere3:
            if (aloc >= 0) {
                glUniform4fv(aloc, 1, glm::value_ptr(sph_ambient));
            }
            if (dloc >= 0) {
                glUniform4fv(dloc, 1, glm::value_ptr(sph_diffuse));
            }
            break;
        case Prism:
            if (aloc >= 0) {
                glUniform4fv(aloc, 1, glm::value_ptr(prism_ambient));
            }
            if (dloc >= 0) {
                glUniform4fv(dloc, 1, glm::value_ptr(prism_diffuse));
            }
            break;
        case Prism2:
            if (aloc >= 0) {
                glUniform4fv(aloc, 1, glm::value_ptr(prism2_ambient));
            }
            if (dloc >= 0) {
                glUniform4fv(dloc, 1, glm::value_ptr(prism2_diffuse));
            }
            break;
        case Teapot:
            if (aloc >= 0) {
                glUniform4fv(aloc, 1, glm::value_ptr(teapot_ambient));
            }
            if (dloc >= 0) {
                glUniform4fv(dloc, 1, glm::value_ptr(teapot_diffuse));
            }
            break;
        case Fork:
            if (aloc >= 0) {
                glUniform4fv(aloc, 1, glm::value_ptr(fork_ambient));
            }
            if (dloc >= 0) {
                glUniform4fv(dloc, 1, glm::value_ptr(fork_diffuse));
            }
            break;
        }
        break;
    }
}

