//
//  Models.cpp
//
//  Assignment-specific code for objects.  This module will typically be
//  heavily modified to fit the needs of each assignment.
//
//  Created by Warren R. Carithers on 2021/09/02.
//  Based on earlier versions created by Joe Geigel and Warren R. Carithers
//  Copyright 2021 Rochester Institute of Technology. All rights reserved.
//
//  Contributor:  Cinto Alapatt
//

#include <iostream>
#include <cmath>

#if defined(_WIN32) || defined(_WIN64)
#include <windows.h>
#endif

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "Models.h"
#include <algorithm>


// data for the three objects
#include "CylinderData.h"

#include "Sphere20.h"
#include "Cube20.h"
#include "Teapot.h"
#include "fork.h"


using namespace std;

//
// PRIVATE GLOBALS
//

//
// PUBLIC GLOBALS
//

// object names (must match the sequence in Models.h)
const char *objects[ N_OBJECTS ] = {
     "Cylinder", "Discs", "Sphere","Sphere2","Sphere3","Cube","Cube2","Cube3","SemiSphere","Prism","Prism2","Plate","Bread1","Bread1a","Bread2","Bread2a","Bread3","Bread3a","Teapot","Cylinder2","Fork","Cylinder3"
};

//
// PRIVATE FUNCTIONS
//




// wrapSide() - convert side to U V coordinates
TexCoord wrapSide(Vertex vertex) {
    TexCoord t;
    
    float pi = 3.141592654;
    float fi = atan(vertex.x / vertex.z);

    t.u = fi / pi;
    t.v = 1.0f - (vertex.y + 0.5f); // Invert the Y coordinate 
    return t;
}


// wrapDisc() - convert disc to UV coordinate
TexCoord wrapDisc(Vertex vertex) {
    TexCoord t;
    t.u = vertex.x + 0.5f;
    t.v = 1.0f - (vertex.z + 0.5f); // Invert the Y coordinate 
    return t;
}

///
/// makeCylinder() - create the cylinder body
///
void makeCylinder( Canvas &C )
{
    // Only use the vertices for the body itself
    for( int i = body.first; i <= body.last - 2; i += 3 ) {

        // Calculate the base indices of the three vertices
        int point1 = cylinderElements[i];
        int point2 = cylinderElements[i + 1];
        int point3 = cylinderElements[i + 2];

        Vertex p1 = cylinderVertices[point1];
        Vertex p2 = cylinderVertices[point2];
        Vertex p3 = cylinderVertices[point3];


        // Calculate the normal vectors for each vertex
        Normal n1, n2, n3;

        // Normals on the body run from the axis to the vertex, and
        // are in the XZ plane; thus, for a vertex at (Px,Py,Pz), the
        // corresponding point on the axis is (0,Py,0), and the normal is
        // P - Axis, or just (Px,0,Pz).

        n1 = { p1.x, 0.0f, p1.z };
        n2 = { p2.x, 0.0f, p2.z };
        n3 = { p3.x, 0.0f, p3.z };

        // Add this triangle to the collection
        C.addTriangleWithNorms( p1, n1, p2, n2, p3, n3 );
        
        // You will need to add code here to determine texture
        // coordinates for each vertex, and will need to call the
        // addTextureCoords() function to add those coordinates
        // to the Canvas.
        C.addTextureCoords(wrapSide(p1), wrapSide(p2), wrapSide(p3));

    }
}

///
/// makeDiscs() - create the cylinder discs
///
static void makeDiscs( Canvas &C )
{
    // Only use the vertices for the top and bottom discs

    for( int disc = 0; disc < 2; ++disc ) {

        // Select the starting and ending indices, and create the surface
        // normal for this disc.  For the top and bottom, the normals are
        // parallel to the Y axis.  Points on the disk all have Y == 0.5,
        // and those on the bottom have Y == -0.5.
        int first, last;
        Normal nn;

        if( disc == 0 ) { // bottom disc
            first = bdisc.first;
            last  = bdisc.last;
            nn = { 0.0f, -1.0f, 0.0f };
        } else {
            first = tdisc.first;
            last  = tdisc.last;
            nn = { 0.0f, 1.0f, 0.0f };
        }

        // Create the triangles
        for( int i = first; i <= last - 2; i += 3 ) {

            // Calculate the base indices of the three vertices
            int point1 = cylinderElements[i];
            int point2 = cylinderElements[i + 1];
            int point3 = cylinderElements[i + 2];

            Vertex p1 = cylinderVertices[point1];
            Vertex p2 = cylinderVertices[point2];
            Vertex p3 = cylinderVertices[point3];

            // Add this triangle to the collection
            C.addTriangleWithNorms( p1, nn, p2, nn, p3, nn );

            // You will need to add code here to determine texture
            // coordinates for each vertex, and will need to call the
            // addTextureCoords() function to add those coordinates
            // to the Canvas.  Use planar mapping; remember that Y
            // is constant for all the disc vertices, and remember that
            // the disc coordinates range from -0.5 to 0.5 in X and Z,
            // but texture coordinates need to range from 0.0 to 1.0.
            C.addTextureCoords(wrapDisc(p1), wrapDisc(p2), wrapDisc(p3));
        }
    }
}
///
// convertSphereUV() - convert vertex coodinate into UV coordinate
///
// Since the r = 0.5, (based on the data in CylinderData.h)
// so for cylinder side, Phi keeps same, Theta = acos(y*2)
TexCoord convertSphere(Vertex vertex) {
    TexCoord t;
    float Pi = 3.141592654;
    float fi = atan(vertex.x / vertex.z) ;
    float Theta = acos(vertex.y * 2.0f) ;

    float u= Pi / fi;
    float v = Theta / Pi;
    t.u = u;
    t.v = 1.0f - v; //  invert the Y coordinate 
    return t;
}

///
// makeSphere() - create a sphere object
///
void makeSphere(Canvas& C)
{
    for (int i = 0; i < sphereElementsLength - 2; i += 3) {

        // Calculate the base indices of the three vertices
        int point1 = sphereElements[i];
        int point2 = sphereElements[i + 1];
        int point3 = sphereElements[i + 2];

        Vertex p1 = sphereVertices[point1];
        Vertex p2 = sphereVertices[point2];
        Vertex p3 = sphereVertices[point3];

        // Calculate the surface normals
        
        Normal n1 = { sphereVertices[point1].x,
                      sphereVertices[point1].y,
                      sphereVertices[point1].z };

        Normal n2 = { sphereVertices[point2].x,
                      sphereVertices[point2].y,
                      sphereVertices[point2].z };

        Normal n3 = { sphereVertices[point3].x,
                      sphereVertices[point3].y,
                      sphereVertices[point3].z };

        // Add triangle and vertex normals
        C.addTriangleWithNorms(p1, n1, p2, n2, p3, n3);
        C.addTextureCoords(convertSphere(p1), convertSphere(p2), convertSphere(p3));
  
    }
}
// vertex coordinate of a cube to texture coordinates

TexCoord convertCube(const Vertex& vertex)
{
    TexCoord t;

    float u, v;
    float scale = 5.0f; // Set the scaling factor for repeating the texture

    // Lambda function to find the maximum absolute value
    auto max_abs = [](float a, float b) { return std::abs(a) > std::abs(b) ? a : b; };
    float max_val = std::abs(max_abs(max_abs(vertex.x, vertex.y), vertex.z));

    // Calculate the texture coordinates 
    if (std::abs(vertex.x) == max_val)
    {
        u = vertex.z / max_val * 0.5f + 0.5f;
        v = vertex.y / max_val * 0.5f + 0.5f;
    }
    else if (std::abs(vertex.y) == max_val)
    {
        u = vertex.x / max_val * 0.5f + 0.5f;
        v = vertex.z / max_val * 0.5f + 0.5f;
    }
    else
    {
        u = vertex.x / max_val * 0.5f + 0.5f;
        v = vertex.y / max_val * 0.5f + 0.5f;
    }

    t.u = u * scale; 
    t.v = (1.0f - v) * scale; 

    return t;
}
///
/// makeCube- internal function to create a cube from its vertex list
///
/// @param C        which Canvas object to use
///
void makeCube(Canvas& C)
{
    for (int face = 0; face < 6; ++face) {

        // Select the starting and ending indices
        int start, end;
        Normal nn;

        if (face == 0) // the right face
        {
            start = rface.first;
            end = rface.last;
            nn = { 1.0f, 0.0f, 0.0f };
        }
        else if (face == 1) // the left face
        {
            start = lface.first;
            end = lface.last;
            nn = { -1.0f, 0.0f, 0.0f };
        }
        else if (face == 2) // the top face
        {
            start = tface.first;
            end = tface.last;
            nn = { 0.0f, 1.0f, 0.0f };
        }
        else if (face == 3) // the bottom face
        {
            start = bface.first;
            end = bface.last;
            nn = { 0.0f, -1.0f, 0.0f };
        }
        else if (face == 4) // front face
        {
            start = fface.first;
            end = fface.last;
            nn = { 0.0f, 0.0f, 1.0f };
        }
        else //  back face
        {
            start = xface.first;
            end = xface.last;
            nn = { 0.0f, 0.0f, -1.0f };
        }

        // Create the triangles
        for (int i = start; i <= end - 2; i += 3) {

            // Calculate the base indices of the three vertices
            int point1 = cubeElements[i];
            int point2 = cubeElements[i + 1];
            int point3 = cubeElements[i + 2];

            Vertex p1 = cubeVertices[point1];
            Vertex p2 = cubeVertices[point2];
            Vertex p3 = cubeVertices[point3];

            // Add this triangle to the collection
            C.addTriangleWithNorms(p1, nn, p2, nn, p3, nn);
            // Add texture coordinates for the triangle
            C.addTextureCoords(convertCube(p1), convertCube(p2), convertCube(p3));
        }
    }
}


TexCoord convertSemiSphereUV(Vertex vertex) {
    TexCoord t;
    float Pi =  3.141592654 ;
    float fi =  atan(vertex.x / vertex.z) ;
    float Theta = acos(-vertex.y); 

    float u = Pi / fi ;
    float v = Theta / (Pi / 2.0f) ; 
    t.u = u;
    t.v = 1.0f - v; //  invert the Y coordinate 
    return t;
}
///
/// makeSemiSphere - internal function to create a semisphere from its vertex list
///
/// @param C        which Canvas object to use
///
void makeSemiSphere(Canvas& C)
{
    for (int i = 0; i < sphereElementsLength - 2; i += 3) {

        // Calculate the base indices of the three vertices
        int point1 = sphereElements[i];
        int point2 = sphereElements[i + 1];
        int point3 = sphereElements[i + 2];

        Vertex p1 = sphereVertices[point1];
        Vertex p2 = sphereVertices[point2];
        Vertex p3 = sphereVertices[point3];

        //  vertices' y coordinates are less than or equal to 0
        if (p1.y <= 0.0f && p2.y <= 0.0f && p3.y <= 0.0f) {

            // Calculate the surface normals
            Normal n1 = { sphereVertices[point1].x,
                          sphereVertices[point1].y,
                          sphereVertices[point1].z };

            Normal n2 = { sphereVertices[point2].x,
                          sphereVertices[point2].y,
                          sphereVertices[point2].z };

            Normal n3 = { sphereVertices[point3].x,
                          sphereVertices[point3].y,
                          sphereVertices[point3].z };

            // Add triangle and vertex normals
            C.addTriangleWithNorms(p1, n1, p2, n2, p3, n3);
            C.addTextureCoords(convertSemiSphereUV(p1), convertSemiSphereUV(p2), convertSemiSphereUV(p3));
        }
    }
}
///
/// createPrism - internal function to create a Prism from its vertex list
///
/// @param C        which Canvas object to use
///
void makePrism(Canvas& C)
{
    // Define triangular base vertices
    Vertex A1 = { 0.0f, 0.0f, 0.0f };
    Vertex B1 = { 1.0f, 0.0f, 0.0f };
    Vertex C1 = { 0.0f, 0.0f, 1.0f };
    Vertex D1 = { 0.0f, 1.0f, 0.0f };
    Vertex E1 = { 1.0f, 1.0f, 0.0f };
    Vertex F1 = { 0.0f, 1.0f, 1.0f };

    // Define face normals
    Normal n1 = { 0.0f, -1.0f, 0.0f }; // bottom face normal
    Normal n2 = { 0.0f, 1.0f, 0.0f };  // top face normal
    Normal n3 = { 0.0f, 0.0f, -1.0f }; // front face normal
    Normal n4 = { -1.0f, 0.0f, 0.0f }; // left face normal
    Normal n5 = { 0.707107f, 0.0f, 0.707107f };  // right face normal

    // Triangles for the bottom and top 
    C.addTriangleWithNorms(A1, n1, B1, n1, C1, n1);
    C.addTriangleWithNorms(D1, n2, E1, n2, F1, n2);

    // Triangles for the 3 rectangular sides
    C.addTriangleWithNorms(A1, n3, D1, n3, B1, n3);
    C.addTriangleWithNorms(B1, n3, D1, n3, E1, n3);
    C.addTriangleWithNorms(A1, n4, C1, n4, D1, n4);
    C.addTriangleWithNorms(C1, n4, F1, n4, D1, n4);
    C.addTriangleWithNorms(B1, n5, E1, n5, C1, n5);
    C.addTriangleWithNorms(C1, n5, E1, n5, F1, n5);
}
///
/// createTeapot - internal function to create a teapot from its vertex list
///
/// @param C        which Canvas object to use
///
 void makeTeapot(Canvas& C)
{
    for (int i = 0; i < teapotElementsLength - 2; i += 3) {
        C.addTriangleWithNorms(
            teapotVertices[teapotElements[i]], teapotNormals[teapotNormalIndices[i]],
            teapotVertices[teapotElements[i + 1]], teapotNormals[teapotNormalIndices[i + 1]],
            teapotVertices[teapotElements[i + 2]], teapotNormals[teapotNormalIndices[i + 2]]
        );

    }
}
 void makeLeftTeapot(Canvas& C)
 {
     for (int i = 0; i < teapotElementsLength - 2; i += 3) {
         Vertex v1 = teapotVertices[teapotElements[i]];
         Vertex v2 = teapotVertices[teapotElements[i + 1]];
         Vertex v3 = teapotVertices[teapotElements[i + 2]];

         // Check if any vertex has x < 0
         if (v1.x > 0 || v2.x > 0 || v3.x > 0) {
             continue; // Skip this triangle
         }

         // Add the triangle with normals
         C.addTriangleWithNorms(
             v1, teapotNormals[teapotNormalIndices[i]],
             v2, teapotNormals[teapotNormalIndices[i + 1]],
             v3, teapotNormals[teapotNormalIndices[i + 2]]
         );
     }
 }

 /// makeFork - internal function to create a fork from its vertex list
///
/// @param C        which Canvas object to use
 void makeFork(Canvas& C)
 {
     for (int i = 0; i < forkElementsLength - 2; i += 3) {
         C.addTriangleWithNorms(
             forkVertices[forkElements[i]], forkNormals[forkNormalIndices[i]],
             forkVertices[forkElements[i + 1]], forkNormals[forkNormalIndices[i + 1]],
             forkVertices[forkElements[i + 2]], forkNormals[forkNormalIndices[i + 2]]
         );
     }
 }




//
// PUBLIC FUNCTIONS
//

///
/// Create an object
///
/// @param C      the Canvas we'll be using
/// @param obj    which object to draw
/// @param buf    BufferSet to use for the object
///

void createObject(Canvas& C, Object obj, BufferSet& buf)
{
    // start with a fresh Canvas
    C.clear();

    // create the specified object
    switch (obj) {
    case Cylinder:  makeCylinder(C);  break;
    case Discs:     makeDiscs(C);     break;
    case Sphere:    makeSphere(C);    break; 
    case Sphere2:   makeSphere(C);    break;
    case Sphere3:   makeSphere(C);    break;
    case Cube:      makeCube(C);      break;
    case Cube2:     makeCube(C);      break;
    case Cube3:     makeCube(C);      break;
    case SemiSphere:makeSemiSphere(C);break;
    case Prism:     makePrism(C);     break;
    case Prism2:    makePrism(C);     break;
    case Plate:     makeCube(C);      break;
    case Plateside: makeCube(C);      break;
    case Bread1:    makeCube(C);      break;
    case Bread2:    makeCube(C);      break;
    case Bread3:    makeCube(C);      break;
    case Bread1a:   makeCube(C);      break;
    case Bread2a:   makeCube(C);      break;
    case Bread3a:   makeCube(C);      break;
    case Teapot:    makeTeapot(C);    break;
    case Cylinder2: makeCylinder(C);  break;
    case Fork:      makeFork(C);      break;
    case Cylinder3: makeCylinder(C);  break;
    case Cylinder4: makeLeftTeapot(C);  break;
    }

    // create the buffers for the object
    buf.createBuffers(C);
}

