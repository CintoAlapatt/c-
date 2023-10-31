//
//  Application.cpp
//
//  Assignment-specific code.
//

//

#include <cstring>
#include <iostream>

#if defined(_WIN32) || defined(_WIN64)
#include <windows.h>
#endif

//
// GLEW and GLFW header files also pull in the OpenGL definitions
//
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>

#include "Application.h"

#include "Buffers.h"
#include "Canvas.h"
#include "Lighting.h"
#include "Materials.h"
#include "Models.h"
#include "ShaderSetup.h"
#include "Types.h"
#include "Utils.h"
#include "Viewing.h"

using namespace std;

//
// PRIVATE GLOBALS
//

// names of our GLSL shader files
static const char *vs_p = "p150.vert";
static const char *fs_p = "p150.frag";
static const char *vs_t = "texture.vert";
static const char *fs_t = "texture.frag";

// our Canvas
static Canvas *canvas;

// buffers for our shapes
static BufferSet buffers[N_OBJECTS];

// shader program handles
static GLuint phong, texture;

// our VAO
static GLuint vao;

// do we need to do a display() call?
static bool updateDisplay = true;

// animation control
static bool animating[N_OBJECTS];  // individual animation flags
static float angles[N_OBJECTS];    // individual rotation angles

// which object(s) to texture map
static bool map_obj[N_OBJECTS];

// object transformations

static glm::vec3 cyl_s(     .45f,    .50f,   .65f);//scaling
static glm::vec3 cyl_x(     1.5f,   0.5f,   .3f);//moving

static glm::vec3 cyl2_s     (.45f, .65f, .65f);//scaling
static glm::vec3 cyl2_x     (1.5f, 0.5f, .3f);//moving

static glm::vec3 sph_s(     .6f,   .3f,    .3f);//scaling
static glm::vec3 sph_x(     1.2f, 1.4f,   -1.6f);//moving

static glm::vec3 sph2_s(    .7f, .5f, .5f);//scaling
static glm::vec3 sph2_x(    .86f, 1.5f, -2.2f);//moving

static glm::vec3 sph3_s(     .7f, .5f, .5f);//scaling
static glm::vec3 sph3_x(   1.6f, 1.62f, -1.65f);//moving

static glm::vec3 cube_s(    10.f,   10.f,   1.f);//scaling
static glm::vec3 cube_x(    -1.25f, 1.5f,   -4.5f);//moving

static glm::vec3 cube2_s(    10.f,    3.f,     15.f);//scaling
static glm::vec3 cube2_x(   -1.25f, -1.5f,  -3.5f);//moving

static glm::vec3 cube3_s(   4.f,    .75f,    3.f);   //scaling
static glm::vec3 cube3_x(   0.0f,   0.5f,   -2.5f);  //moving

static glm::vec3 semisph_s(     1.5f, 1.5f, 1.5f);//scaling
static glm::vec3 semisph_x(     1.25f, 1.6f, -2.f);//moving

static glm::vec3 prism_s(       2.5f,   1.25f, 1.0f);    //scaling
static glm::vec3 prism_x(       -1.60f, -1.2f, 2.f);    //moving

static glm::vec3 prism2_s(       2.5f, .07f, 1.0f);    //scaling
static glm::vec3 prism2_x(      -1.60f, .1f, 2.f);    //moving

static glm::vec3 plate_s(       1.51f, .19f, 2.51f);    //scaling
static glm::vec3 plate_x(       0.45f, .02f, .5f);

static glm::vec3 plate2_s       (1.5f, .2f, 2.5f);    //scaling
static glm::vec3 plate2_x       (0.45f, .02f, .5f);

static glm::vec3 bread1_s       (.81f, .159f,  .51f);    //scaling
static glm::vec3 bread1_x       (-0.1f, .25f, .2f); //moving

static glm::vec3 bread1a_s      (.80f, .16f, .50f);    //scaling
static glm::vec3 bread1a_x      (-0.1f, .25f, .2f); //moving


static glm::vec3 bread2_s       (.81f, .159f, .51f);    //scaling
static glm::vec3 bread2_x       (-0.05f, .45f, .2f); //moving

static glm::vec3 bread2a_s      (.8f, .16f, .5f);    //scaling
static glm::vec3 bread2a_x      (-0.05f, .45f, .2f); //moving

static glm::vec3 bread3_s       (1.f, .159f,.7f);    //scaling
static glm::vec3 bread3_x       (0.7f, .25f, 1.0f); //moving

static glm::vec3 bread3a_s      (.99f, .16f, .69f);    //scaling
static glm::vec3 bread3a_x      (0.7f, .25f, 1.0f); //moving

static glm::vec3 teapot_s       (2.1f, 2.8f, 2.1f);    //scaling
static glm::vec3 teapot_x       (-.8f, .9f, -2.0f); //moving



static glm::vec3 fork_s         (.08f, .08f, 0.01f);//scaling
static glm::vec3 fork_x         (-1.1f, .3f, 1.2f);//moving

static glm::vec3 cyl3_s         (1.f, .15f, 1.f);//scaling
static glm::vec3 cyl3_x         (1.2f, 1.7f,-1.7f);//moving

static glm::vec3 cyl4_s     (1.1f, 1.f, .8f);//scaling
static glm::vec3 cyl4_x     (1.4f, .28f, .3f);//moving


// light animation
static bool animateLight = false;
static float delta = 1.0f;
static const float lightMax = 50.0f, lightMin = -50.0f;

//
// PUBLIC GLOBALS
//

//
// Drawing-related variables
//

// dimensions of the drawing window
int w_width  = 1500;
int w_height = 1200;

// drawing window title
const char *w_title = "Texture Mapping";

// GL context we're using (we assume 3.2, for GLSL 1.50)
int gl_maj = 3;
int gl_min = 2;

// our GLFWwindow
GLFWwindow *w_window;

//
// PRIVATE FUNCTIONS
//

///
/// process the command-line arguments
///
/// @param argc   number of command-line arguments
/// @param argv   command-line argument strings
///
static void args( const int argc, char *argv[] ) {
    bool mapAll = true;

    // command-line arguments specify which objects to texture-map
    for( int i = 1; i < argc; ++i ) {
        switch( argv[i][0] ) {
        case 'c':
            mapAll = false;  map_obj[Cylinder] = true;
            break;
        case 'd':
            mapAll = false;  map_obj[Discs] = true;
            break;
        case 'q':
            mapAll = false; map_obj[Sphere] = true;
            break;
        case 'x':
            mapAll = false;
            break;
        default:
            cerr << "bad object character '" << argv[i][0]
                 << "' ignored" << endl;
        }
    }

    // if we're mapping everything, indicate that
    if( mapAll ) {
        for( int i = 0; i < N_OBJECTS; ++i ) {
            map_obj[i] = true;
        }
    }
}

///
/// Create our shapes
///
/// @param C  - the Canvas to use when drawing the objects
///
static void createImage(Canvas& C)
{

    createObject(C, Cylinder,  buffers[Cylinder]);
    createObject(C, Discs, buffers[Discs]);
    createObject(C, Sphere,  buffers[Sphere]);
    createObject(C, Sphere2, buffers[Sphere2]);
    createObject(C, Sphere3, buffers[Sphere3]);
    createObject(C, Cube, buffers[Cube]);
    createObject(C, Cube2, buffers[Cube2]);
    createObject(C, Cube3, buffers[Cube3]);
    createObject(C, SemiSphere, buffers[SemiSphere]);
    createObject(C, Prism,  buffers[Prism]);
    createObject(C, Prism2, buffers[Prism2]);
    createObject(C, Plate, buffers[Plate]);
    createObject(C, Plateside, buffers[Plateside]);
    createObject(C, Bread1, buffers[Bread1]);
    createObject(C, Bread1a, buffers[Bread1a]);
    createObject(C, Bread2, buffers[Bread2]);
    createObject(C, Bread2a, buffers[Bread2a]);
    createObject(C, Bread3, buffers[Bread3]);
    createObject(C, Bread3a, buffers[Bread3a]);
    createObject(C, Teapot, buffers[Teapot]);
    createObject(C, Cylinder2, buffers[Cylinder2]);
    createObject(C, Fork, buffers[Fork]);
    createObject(C, Cylinder3, buffers[Cylinder3]);
    createObject(C, Cylinder4, buffers[Cylinder4]);

}


///
/// Increment or reset a rotation angle.
///
/// @param obj  the object being rotated
///
static void rotate(int obj, float amt) {
    angles[obj] += amt;
    if (angles[obj] >= 360.0f) {
        angles[obj] = 0.0f;
    }
}
///
/// Animation routine
///
static void animate(void) {

    if (animating[Teapot]) {
        rotate(Teapot, 2.0f);
        updateDisplay = true;
    }

    if (animating[Cylinder]) {
        rotate(Cylinder, 1.0f);
        rotate(Discs, 1.0f);
        updateDisplay = true;
    }

    if (animateLight) {
        if ((delta > 0.0f && lightpos.x >= lightMax) ||
            (delta < 0.0f && lightpos.x <= lightMin)) {
            delta *= -1.0f;
        }
        lightpos.x += delta;
        updateDisplay = true;
    }
}
//
// Event callback routines for this assignment
//

///
/// Handle keyboard input
///
/// @param window   GLFW window being used
/// @param key      code for the key that was pressed
/// @param scan     scan code of the key
/// @param action   type of event (press, release, etc.)
/// @param mods     modifiers in use (shift, etc.)
///
///
/// Handle keyboard input
///
/// @param window   GLFW window being used
/// @param key      code for the key that was pressed
/// @param scan     scan code of the key
/// @param action   type of event (press, release, etc.)
/// @param mods     modifiers in use (shift, etc.)
///
static void keyboard(GLFWwindow* window, int key, int scan,
    int action, int mods)
{
    bool any_anim = false;

    // only react to "press" events
    if (action != GLFW_PRESS) {
        return;
    }

    switch (key) {

        // termination
    case GLFW_KEY_ESCAPE:  // FALL THROUGH
    case GLFW_KEY_Q:
        glfwSetWindowShouldClose(w_window, 1);
        // return without updating the display
        return;
        // NOTREACHED

    // animation control

    case GLFW_KEY_A:    // start/stop all object animation
        // NOTE: does not affect light source animation!
        // are we currently animating anything?
        for (int i = 0; i < N_OBJECTS; ++i) {
            any_anim = any_anim || animating[i];
        }
        // invert the condition
        any_anim = !any_anim;
        // propogate it
        for (int i = 0; i < N_OBJECTS; ++i) {
            animating[i] = any_anim;
        }
        break;

    case GLFW_KEY_S: // start/stop animating the quad (square)
        animating[Teapot] = !animating[Teapot];
        break;

    case GLFW_KEY_C: // start/stop animating the cylinder and discs
        animating[Cylinder] = !animating[Cylinder];
        animating[Discs] = !animating[Discs];
        break;

    case GLFW_KEY_L:    // start/stop animating the light
        animateLight = !animateLight;
        break;

        // light position

    case GLFW_KEY_I: // move the light into the scene
        lightpos[2] -= 2.0f;
        break;

    case GLFW_KEY_O: // move the light out from the scene
        lightpos[2] += 2.0f;
        break;

        // print out potentially useful information

    case GLFW_KEY_R: // rotation angles
        cerr << "Rotation: quad " << angles[Teapot]
            << ", cyl/disc " << angles[Cylinder] << endl;
        break;

    case GLFW_KEY_P: // light position
        cerr << "Light is at (" << lightpos[0] << "," << lightpos[1]
            << "," << lightpos[2] << ")" << endl;
        break;

        // Reset parameters

    case GLFW_KEY_1: // reset all object rotations
        angles[Teapot] = angles[Cylinder] = angles[Discs] = 0.0f;
        break;

    case GLFW_KEY_2: // reset light position
        lightpos[0] = lpDefault[0];
        lightpos[1] = lpDefault[1];
        lightpos[2] = lpDefault[2];
        lightpos[3] = lpDefault[3];
        break;

        // help message
    case GLFW_KEY_H:
        cout << "  Key(s)             Action" << endl;
        cout << "=========   =======================" << endl;
        cout << "ESC, q, Q   Terminate the program" << endl;
        cout << "  h, H      Print this message" << endl;
        cout << "  a, A      Toggle all object animations" << endl;
        cout << "  s, S      Toggle square (quad) animation" << endl;
        cout << "  c, C      Toggle cylinder animation" << endl;
        cout << "  l, L      Toggle light animation" << endl;
        cout << "  i, I      Move light toward the objects" << endl;
        cout << "  o, O      Move light away from the objects" << endl;
        cout << "  p, P      Print light position" << endl;
        cout << "  r, R      Print rotation angles" << endl;
        cout << "   1        Reset all object rotations" << endl;
        cout << "   2        Reset light position" << endl;
        // return without updating the display
        return;
        // NOTREACHED

    default:
        cerr << "Unknown keycode " << key << " ignored" << endl;
        // return without updating the display
        return;
        // NOTREACHED
    }

    updateDisplay = true;
}

///
/// Set the titlebar in the display window
///
void setTitle( void ) {
    char buf[128];

    strcpy( buf, "Texture mapping:" );
    
    if( map_obj[Cylinder] ) strcat( buf, " Cylinder" );
    if( map_obj[Discs] )    strcat( buf, " Discs" );
    if( map_obj[Sphere] )   strcat( buf, " Sphere" );
    if (map_obj[Cube])   strcat(buf, " Cube");


    glfwSetWindowTitle( w_window, buf );
}


///
/// Display the current image
///
static void display( void )
{
    // clear the frame buffer
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

    // set our titlebar
    setTitle();

    // check for any errors to this point
    checkErrors( "display init" );

    // draw the individual objects
    for( int obj = 0; obj < N_OBJECTS; ++obj ) {

        // select the proper shader program
        GLuint program;
        if (obj == Cylinder || obj == Cylinder3 || obj == Cylinder4 ||obj == Discs||obj==Cube2||obj==Cube||obj==Cube3||obj==SemiSphere|| obj == Cylinder2||obj==Plate || obj == Plateside ||obj==Bread1a || obj == Bread2a || obj == Bread3a||obj == Bread1 || obj == Bread2 || obj == Bread3){
            program = texture;
        }
        else {
            program = phong;
        }
        glUseProgram(program);

        // set up the common transformations
        setCamera( program );
        setProjection( program );
        checkErrors( "display camera" );

        // and our lighting
        setLighting( program );
        checkErrors( "display lighting" );

        // set texture parameters OR material properties
        setMaterials( program, (Object) obj, map_obj[obj] );
        checkErrors( "display materials" );

        // select the correct rotation angles
        glm::vec3 ang( angles[obj], angles[obj], angles[obj] );
        glm::vec3 ang1(angles[obj], 80, angles[obj]);
        glm::vec3 ang2(angles[obj], 220, angles[obj]);
        glm::vec3 ang3(angles[obj], 45, angles[obj]);
        glm::vec3 ang4(angles[obj], 30, angles[obj]);
        glm::vec3 ang5(angles[obj], 30, angles[obj]);
        glm::vec3 ang6(angles[obj], 30, angles[obj]);
        glm::vec3 ang7(angles[obj], 220, angles[obj]);
        glm::vec3 ang8(angles[obj], 220, angles[obj]);
        glm::vec3 ang9(angles[obj], 100, angles[obj]);
        glm::vec3 ang10(angles[obj], 80, angles[obj]);
        glm::vec3 ang11(angles[obj], 220, angles[obj]);
        glm::vec3 ang12(angles[obj], 220, angles[obj]);
        glm::vec3 ang13(angles[obj], 100, angles[obj]);
        glm::vec3 ang14(angles[obj], 220, angles[obj]);
        glm::vec3 ang15(280, angles[obj], 110);
        glm::vec3 ang16(270, 0, 60);
        glm::vec3 ang17(0, 160, 00);

        // send all the transformation data
        switch( obj ) {
    
        case Cylinder:  // FALL THROUGH
        case Discs:
            setTransforms( program, cyl_s, ang, cyl_x );
            break;
        case Sphere:
            
            setTransforms(program, sph_s, ang5, sph_x);
            break;
        case Sphere2:
            
            setTransforms(program, sph2_s, ang3, sph2_x);
            break;
        case Sphere3:
            
            setTransforms(program, sph3_s, ang4, sph3_x);
            break;
        case Cube:
            setTransforms(program, cube_s, ang, cube_x);
            break;
        case Cube2:
            setTransforms(program, cube2_s, ang, cube2_x);
            break;
        case Cube3:
            setTransforms(program, cube3_s, ang, cube3_x);
            break;
        case SemiSphere:
            setTransforms(program, semisph_s, ang, semisph_x);
            break;
        case Prism:
            setTransforms(program, prism_s, ang10, prism_x);
            break;
        case Prism2:
            setTransforms(program, prism2_s, ang1, prism2_x);
            break;
        case Plate:
            setTransforms(program, plate_s, ang2, plate_x);
            break;
        case Plateside:
            setTransforms(program, plate2_s, ang14, plate2_x);
            break;
        case Bread1:
            setTransforms(program, bread1_s, ang7, bread1_x);
            break;
        case Bread1a:
            setTransforms(program, bread1a_s, ang11, bread1a_x);
            break;
        case Bread2:
            setTransforms(program, bread2_s, ang8, bread2_x);
            break;
        case Bread2a:
            setTransforms(program, bread2a_s, ang12, bread2a_x);
            break;
        case Bread3:
            setTransforms(program, bread3_s, ang9, bread3_x);
            break;
        case Bread3a:
            setTransforms(program, bread3a_s, ang13, bread3a_x);
            break;
        case Teapot:
            setTransforms(program, teapot_s, ang6, teapot_x);
            break;
        case Cylinder2:
            setTransforms(program, cyl2_s, ang, cyl2_x);
            break;
        case Fork:
            setTransforms(program, fork_s, ang15, fork_x);
            break;
        case Cylinder3:
            setTransforms(program, cyl3_s, ang16, cyl3_x);
            break;
        case Cylinder4:
            setTransforms(program, cyl4_s, ang17, cyl4_x);
            break;
        }

        checkErrors( "display xforms" );

        // draw it
        buffers[obj].selectBuffers( program,
            "vPosition", NULL, "vNormal", 
            map_obj[obj] ? "vTexCoord" : NULL );
        checkErrors( "display select" );

        glDrawElements( GL_TRIANGLES, buffers[obj].numElements,
                        GL_UNSIGNED_INT, (void *) 0 );
        checkErrors( "display draw" );
    }
}

///
/// OpenGL initialization
///
/// @return boolean indicating status of initialization
///
static bool init( void )
{
    // Check the OpenGL major version
    if( gl_maj < 3 || (gl_maj == 3 && gl_min < 2) ) {
        // select the other Phong shader
        vs_p = "p120.vert";
        fs_p = "p120.frag";
        // warn about GLSL versions
        cerr << "Caution: GL version may not allow GLSL 1.50+"
             << " code to compile!" << endl;
    }
    checkErrors( "init start" );

    // Load shaders and use the resulting shader program
    ShaderError error;
    phong = shaderSetup( vs_p, fs_p, &error );
    if( !phong ) {
        cerr << "Error setting up phong shader - "
             << errorString(error) << endl;
        return( false );
    }
    checkErrors( "init shaders 1" );

    texture = shaderSetup( vs_t, fs_t, &error );
    if( !texture ) {
        cerr << "Error setting up texture shader - "
             << errorString(error) << endl;
        return( false );
    }
    checkErrors( "init shaders 2" );

#ifdef DEBUG
    // Define the CPP symbol 'DEBUG' and recompile to enable the compilation
    // of this code into your program for debugging purposes

    // Dump the list of active global variables in the shader programs
    cout << "Shader actives" << endl;
    cout << "----------" << endl << "Phong shader " << phong << endl;
    dumpActives( phong );
    cout << "----------" << endl << "Texture shader " << texture << endl;
    dumpActives( texture );
    checkErrors( "init actives" );
#endif

    // create our Canvas
    canvas = new Canvas( w_width, w_height );
    if( canvas == NULL ) {
        cerr << "Error - cannot create Canvas" << endl;
        return( false );
    }

    // need a VAO if we're using a core context;
    // doesn't hurt even if we're not using one
    glGenVertexArrays( 1, &vao );
    glBindVertexArray( vao );
    checkErrors( "init vao" );

    // OpenGL state initialization
    glEnable( GL_DEPTH_TEST );
    glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );
    glClearColor( 0.0f, 0.0f, 0.0f, 0.0f );
    glDepthFunc( GL_LEQUAL );
    glClearDepth( 1.0f );
    checkErrors( "init setup" );

    // create the geometry for our shapes.
    createImage( *canvas );
    checkErrors( "init image" );

    // initialize all texture-related things
    initTextures();
    checkErrors( "init textures" );

    

    return( true );
}

//
// PUBLIC FUNCTIONS
//

///
/// application-specific processing
///
/// @param argc   command-line argument count
/// @param argv   command-line argument strings
///
void application( int argc, char *argv[] )
{
    // process command-line arguments
    if( argc > 1 ) {
        args( argc, argv );
    } else {
        for( int i = 0; i < N_OBJECTS; ++i ) {
            map_obj[i] = true;
        }
    }

    // set up the objects and the scene
    if( !init() ) {
        return;
    }

    checkErrors( "after init" );
    // loop until it's time to quit
    while (!glfwWindowShouldClose(w_window)) {
        animate();
        if (updateDisplay) {
            updateDisplay = false;
            display();
            glfwSwapBuffers(w_window);
            checkErrors("event loop");
        }
        glfwPollEvents();
    }
 
}
