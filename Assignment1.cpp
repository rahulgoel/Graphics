
#include <iostream>
#include <GL/glut.h>
#include "stdio.h"
#include  "math.h"
using namespace std;

GLfloat mat_ambient[] = {0.5, 0.5, 0.5, 1.0};
GLfloat mat_specular[] = {1.0, 1.0, 1.0, 1.0};
GLfloat mat_shininess[] = {50.0};
GLfloat light_position[] = {10.0, 10.0, 10.0, 0.0};
GLfloat model_ambient[] = {1.0, 0.5, 0.5, 1.0};
GLfloat cameraAngleX=0.0f;

void keyboard(unsigned char key, int x, int y);
void mouse(int button, int stat, int x, int y);
void mouseMotion(int x, int y);

void DrawCuboid(float x, float y, float z);
void Ball(void);
void translate1(float x, float y, float z);
void scale1(float x, float y, float z);
void rotate1(float a,float x, float y, float z);
void perspective1(float s,float x, float y, float z);
void printMatrix(void);
void moveHand(int a );
bool mouseLeftDown;
bool mouseMiddleDown;
bool mouseRightDown,call1,call2,reset,stop,mirrormode;
float mouseX, mouseY;
int drawMode;
float cameraAngleY=0;
float cameraDistance=0;
float cameraDistancex=0;
float ballmovex=0;
float ballmovey=0;
float angle=0.1;
float angle2=0.1;
int mode=0;
int modeball=0;
int mouserelease=0;
int mirror=0;
void setupMaterials() {
	glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient);
	glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
	glMaterialfv(GL_FRONT, GL_SHININESS, mat_shininess);
	glLightfv(GL_LIGHT0, GL_POSITION, light_position);
	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, model_ambient);
}

void translate1(float x, float y, float z){
     GLfloat m[16];
   // int i;int j;

 // glGetFloatv(GL_MODELVIEW_MATRIX,m);
    m[0] = 1;   m[1] = 0;   m[2] = 0;   m[3] = 0;
    m[4] = 0;   m[5] = 1;   m[6] = 0;   m[7] =0;
    m[8] = 0;   m[9] = 0;   m[10]= 1;   m[11]= 0;
    m[12] = x; m[13] = y; m[14] = z; m[15] =1;
    glMultMatrixf(m);
   // glLoadMatrixf(m);
}

void scale1(float x, float y, float z){
     GLfloat m[16];
   // int i;int j;

 // glGetFloatv(GL_MODELVIEW_MATRIX,m);
    m[0] = x;   m[1] = 0;   m[2] = 0;   m[3] = 0;
    m[4] = 0;   m[5] = y;   m[6] = 0;   m[7] =0;
    m[8] = 0;   m[9] = 0;   m[10]= z;   m[11]= 0;
    m[12] = 0; m[13] = 0; m[14] = 0; m[15] =1;
    glMultMatrixf(m);
   // glLoadMatrixf(m);
}

void rotate1(float a,float x, float y, float z){
     GLfloat m[16];
    float i;float j;float k;float w;
    float c;float s;
    float e; float f;
    e=(a*3.141592)/180;
    c = cos(e); s = sin(e);
    w=sqrt(x*x+y*y+z*z);
    i=x/w; j= y/w; k = z/w;
 // glGetFloatv(GL_MODELVIEW_MATRIX,m);
    m[0] = i*i*(1-c)+c;   m[1] = j*i*(1-c)+ k*s;   m[2] = i*k*(1-c)-j*s;   m[3] = 0;
    m[4] = i*j*(1-c)- k*s;   m[5] = j*j*(1-c)+c;   m[6] = j*k*(1-c)+i*s;   m[7] =0;
    m[8] = i*k*(1-c)+j*s;   m[9] = j*k*(1-c)-i*s;   m[10]= k*k*(1-c)+c;   m[11]= 0;
    m[12] = 0; m[13] = 0; m[14] = 0; m[15] =1;
    glMultMatrixf(m);
   // glLoadMatrixf(m);
}

void perspective1(float a,float x, float y, float z){
     GLfloat m[16];
    float i;float j;float k;
    float e;
    e=(a*3.141592)/180;
    i=e/2;
    k=tan(i);
    j=1/k;
 // glGetFloatv(GL_MODELVIEW_MATRIX,m);
    m[0] = j/x;   m[1] = 0;   m[2] = 0;   m[3] = 0;
    m[4] = 0;   m[5] = j;   m[6] = 0;   m[7] =0;
    m[8] = 0;   m[9] = 0;   m[10]= (y+z)/(y-z);   m[11]= -1;
    m[12] = 0; m[13] = 0; m[14] = 2*y*z/(y-z); m[15] =0;
    glMultMatrixf(m);
   // glLoadMatrixf(m);
}


void changeColour(GLfloat r, GLfloat g, GLfloat b, GLfloat A) {
	model_ambient[0] = r;
	model_ambient[1] = g;
	model_ambient[2] = b;
	model_ambient[3] = A;
	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, model_ambient);
}

void init(void) {
	glClearColor(0.0, 0.0, 0.0, 1.0);
	setupMaterials();

	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glEnable(GL_DEPTH_TEST);
	 glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
    glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
    glHint(GL_POLYGON_SMOOTH_HINT, GL_NICEST);

    glEnable(GL_TEXTURE_2D);
    glEnable(GL_CULL_FACE);
   glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_LINE_SMOOTH);

     // track material ambient and diffuse from surface color, call it before glEnable(GL_COLOR_MATERIAL)
    glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
    glEnable(GL_COLOR_MATERIAL);
	glEnable(GL_CULL_FACE);
	glFrontFace(GL_CCW);
	glShadeModel(GL_SMOOTH);
	  glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
 //   gluLookAt(-3, -3, -3, 0,1, 1, 0, 1,1); //
}

void reshape(int w, int h) {
	GLfloat fAspect;
	if(h==0) h=1;

	glViewport(0,0,w,h);

	fAspect = (GLfloat)w / (GLfloat)h;

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	perspective1(60, fAspect, 0.5, 100.0);
	glTranslatef(0,0,-1);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}
GLfloat vertices2[] = { 1, 1, 1,  -1, 1, 1,  -1,-1, 1,   1,-1, 1,   // v0,v1,v2,v3 (front)
                        1, 1, 1,   1,-1, 1,   1,-1,-1,   1, 1,-1,   // v0,v3,v4,v5 (right)
                        1, 1, 1,   1, 1,-1,  -1, 1,-1,  -1, 1, 1,   // v0,v5,v6,v1 (top)
                       -1, 1, 1,  -1, 1,-1,  -1,-1,-1,  -1,-1, 1,   // v1,v6,v7,v2 (left)
                       -1,-1,-1,   1,-1,-1,   1,-1, 1,  -1,-1, 1,   // v7,v4,v3,v2 (bottom)
                        1,-1,-1,  -1,-1,-1,  -1, 1,-1,   1, 1,-1 }; // v4,v7,v6,v5 (back)


GLfloat normals2[]  = { 0, 0, 1,   0, 0, 1,   0, 0, 1,   0, 0, 1,   // v0,v1,v2,v3 (front)
                        1, 0, 0,   1, 0, 0,   1, 0, 0,   1, 0, 0,   // v0,v3,v4,v5 (right)
                        0, 1, 0,   0, 1, 0,   0, 1, 0,   0, 1, 0,   // v0,v5,v6,v1 (top)
                       -1, 0, 0,  -1, 0, 0,  -1, 0, 0,  -1, 0, 0,   // v1,v6,v7,v2 (left)
                        0,-1, 0,   0,-1, 0,   0,-1, 0,   0,-1, 0,   // v7,v4,v3,v2 (bottom)
                        0, 0,-1,   0, 0,-1,   0, 0,-1,   0, 0,-1 }; // v4,v7,v6,v5 (back)

// color array
GLfloat colors2[]   = { 1, 1, 1,   1, 1, 0,   1, 0, 0,   1, 0, 1,   // v0,v1,v2,v3 (front)
                        1, 1, 1,   1, 0, 1,   0, 0, 1,   0, 1, 1,   // v0,v3,v4,v5 (right)
                        1, 1, 1,   0, 1, 1,   0, 1, 0,   1, 1, 0,   // v0,v5,v6,v1 (top)
                        1, 1, 0,   0, 1, 0,   0, 0, 0,   1, 0, 0,   // v1,v6,v7,v2 (left)
                        0, 0, 0,   0, 0, 1,   1, 0, 1,   1, 0, 0,   // v7,v4,v3,v2 (bottom)
                        0, 0, 1,   0, 0, 0,   0, 1, 0,   0, 1, 1 }; // v4,v7,v6,v5 (back)

// index array of vertex array for glDrawElements() & glDrawRangeElement()
GLubyte indices[]  = { 0, 1, 2,3,        // front
                       4, 5, 6,7 ,        // right
                       8, 9,10,11,       // top
                      12,13,14,15,        // left
                      16,17,18,19 ,      // bottom
                      20,21,22,23 };    // back
void displayCube(void) {



	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  glEnableClientState(GL_NORMAL_ARRAY);
    glEnableClientState(GL_COLOR_ARRAY);
    glEnableClientState(GL_VERTEX_ARRAY);
    glColor3f(1, 0, 1);

       glNormalPointer(GL_FLOAT, 0, normals2);
    glColorPointer(3, GL_FLOAT, 0, colors2);
    glVertexPointer(3, GL_FLOAT, 0, vertices2);

     glPushMatrix();
 //   translate1(0, 0, -5);
              // move to bottom-left corner

                translate1(0, 0, -cameraDistance);
                rotate1(cameraAngleX,1,0,0);
                rotate1(cameraAngleY,0,1,0);

  //  rotate1(34,1,1,1);

    glDrawElements(GL_QUADS, 24, GL_UNSIGNED_BYTE, indices);
 glPopMatrix();
glDisableClientState(GL_VERTEX_ARRAY);  // disable vertex arrays
    glDisableClientState(GL_COLOR_ARRAY);
    glDisableClientState(GL_NORMAL_ARRAY);

	glFlush();
    glutSwapBuffers();
}

bool pos = true;
int count=0;


void moveHand(int a){
glPushMatrix();
cout << "hello";
translate1(0,-3.5,-10);
    rotate1(angle,1,0,0);
  glPopMatrix();
if (modeball==0){
     if (pos){
    count += 1;
   angle=angle+1;
   angle2=angle2+1;
    }
    else {
    count -= 1;
   angle =angle-1;
   angle2=angle2-1;
    }

    if (count <= 0)
    pos = true;
    else if (count >= 90)
    pos = false;
}
else{

     if (pos){
    count += 1;
   angle=angle+1;
   angle2=angle2+1;
    }
    else if(stop){
    count=55;
    if (call1==true)angle=55;
    else if (call2== true)angle2=55;}
    else {
    count -= 1;
   angle =angle-1;
   angle2=angle2+1;
    }

    if (count <= 0)
    pos = true;
    else if (count == 55)
    {stop=true;
    pos =false;}
        else if(count>55){
           if (call1==true) angle=55;
           else if (call2== true)angle2=55;
            pos=true;
            count=55;

            stop=true;
    }
}

    glutPostRedisplay();
    if(mode==1)
    glutTimerFunc(23,moveHand,0);
//    if(mouseLeftDown&&modeball==0){
//       glutTimerFunc(23,moveHand,0);
//    }

//    else if(mode==0 && mouseLeftDown==true)
//    glutTimerFunc(23,moveHand,0);

}



void displayCuboid(void) {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT| GL_STENCIL_BUFFER_BIT);
    glMatrixMode (GL_MODELVIEW);
  //  printMatrix();
    glPushMatrix();
//            glLoadIdentity();
    cout << "af1 \n";
               // printMatrix();
        cout << "mouserelease \n";
            cout << mouserelease;

                translate1(cameraDistancex, 0, -cameraDistance);
                rotate1(cameraAngleX,1,0,0);
                rotate1(cameraAngleY,0,1,0);
//
//            if(call1==true) angle2=0;
//            else if(call2 = true) angle =0;


            glPushMatrix();
            if(call1==true){
               translate1(7.5+ballmovex,-ballmovey-4,-4);
               call2=false;
               }
//
//            if(call1==true) angle2=0;
//            else if(call2 = true) angle =0;

                cout << "af2 \n";
               // printMatrix();
             //   glLoadIdentity();
              //  translate1(0,0,-6);
                DrawCuboid(2.7,3.5,1);

                translate1(0,5,0);
                DrawCuboid(5,6,1);

                glPushMatrix();
                    cout << "/n";cout << count; cout <<"/n" << angle;



                    glPushMatrix();
                        translate1(-2,3.7,0);
                        rotate1(9,0,0,1);
                        glPushMatrix();
                            rotate1(-9,0,0,1);
                            translate1(0,-2.2,0);

                            rotate1(angle,1,0,0);
                            translate1(0,2.2,0);
                            rotate1(9,0,0,1);
                            DrawCuboid(.7,1,1);
                        glPopMatrix();
                        translate1(-.25,1.2,0);
                        glPushMatrix();
                               rotate1(-9,0,0,1);
                            translate1(2.5,-2.7,0);

                         rotate1(angle*1.5,1,0,0);
                           translate1(-2.5,2.7,0);
                           rotate1(9,0,0,1);
                            DrawCuboid(.7,1.2,1);
                        glPopMatrix();
                        translate1(0,1.2,0);
                        glPushMatrix();
                           rotate1(-9,0,0,1);
                            translate1(2.5,-3.6,0);


                            rotate1(angle*1.7,1,0,0);
                            translate1(-2.5,3.6,0);
                            rotate1(9,0,0,1);
                            DrawCuboid(.7,1,1);
                        glPopMatrix();
                        translate1(0,1.2,0);
                        glPushMatrix();
                               rotate1(-9,0,0,1);
                         translate1(2.5,-4.5,0);
                             rotate1(angle*1.87,1,0,0);
                              translate1(-2.5,4.5,0);
                              rotate1(9,0,0,1);
                            DrawCuboid(.7,1,1);
                        glPopMatrix();
                    glPopMatrix();



                    glPushMatrix();
                     translate1(-0.5,3.8,0);
                        rotate1(3,0,0,1);
                        glPushMatrix();
                            rotate1(-3,0,0,1);
                            translate1(0,-2.2,0);

                            rotate1(angle,1,0,0);
                            translate1(0,2.2,0);
                            rotate1(3,0,0,1);
                            DrawCuboid(.7,1,1);
                        glPopMatrix();
                        translate1(-.12,1.4,0);
                        glPushMatrix();
                               rotate1(-3,0,0,1);
                            translate1(2.5,-2.7,0);

                         rotate1(angle*1.5,1,0,0);
                           translate1(-2.5,2.7,0);
                           rotate1(3,0,0,1);
                            DrawCuboid(.7,1.2,1);
                        glPopMatrix();
                        translate1(0,1.4,0);
                        glPushMatrix();
                           rotate1(-3,0,0,1);
                            translate1(2.5,-3.6,0);


                            rotate1(angle*1.7,1,0,0);
                            translate1(-2.5,3.6,0);
                            rotate1(3,0,0,1);
                            DrawCuboid(.7,1.2,1);
                        glPopMatrix();
                        translate1(0,1.4,0);
                        glPushMatrix();
                               rotate1(-3,0,0,1);
                         translate1(2.5,-4.5,0);
                             rotate1(angle*1.87,1,0,0);
                              translate1(-2.5,4.5,0);
                              rotate1(3,0,0,1);
                            DrawCuboid(.7,1.1,1);
                        glPopMatrix();
                    glPopMatrix();
//                           rotate1(3,0,0,1);
//                         translate1(-0.8,3.7,0);
//                        rotate1(5,0,0,1);
//                        DrawCuboid(.7,1,1);
//                        translate1(-.08,1.4,0);
//                        DrawCuboid(.7,1.2,1);
//                        translate1(0,1.4,0);
//                        DrawCuboid(.7,1.2,1);
//                         translate1(0,1.4,0);
//                        DrawCuboid(.7,1.1,1);
//                        glPopMatrix();

                     glPushMatrix();

                    glPushMatrix();
                     translate1(0.5,3.7,0);
                        rotate1(-3,0,0,1);
                        glPushMatrix();
                            rotate1(3,0,0,1);
                            translate1(0,-2.2,0);

                            rotate1(angle,1,0,0);
                            translate1(0,2.2,0);
                            rotate1(-3,0,0,1);
                            DrawCuboid(.7,1.1,1);
                        glPopMatrix();
                        translate1(.12,1.3,0);
                        glPushMatrix();
                               rotate1(3,0,0,1);
                            translate1(2.5,-2.7,0);

                         rotate1(angle*1.5,1,0,0);
                           translate1(-2.5,2.7,0);
                           rotate1(-3,0,0,1);
                            DrawCuboid(.7,1.2,1);
                        glPopMatrix();
                        translate1(0,1.2,0);
                        glPushMatrix();
                           rotate1(3,0,0,1);
                            translate1(2.5,-3.6,0);


                            rotate1(angle*1.7,1,0,0);
                            translate1(-2.5,3.6,0);
                            rotate1(-3,0,0,1);
                            DrawCuboid(.7,1.1,1);
                        glPopMatrix();
                        translate1(0,1.2,0);
                        glPushMatrix();
                               rotate1(3,0,0,1);
                            translate1(2.5,-4.5,0);
                             rotate1(angle*1.87,1,0,0);
                              translate1(-2.5,4.5,0);
                              rotate1(-3,0,0,1);
                            DrawCuboid(.7,1.1,1);
                        glPopMatrix();
                    glPopMatrix();



//                         translate1(0.5,3.7,0);
//                        rotate1(-3,0,0,1);
//                        DrawCuboid(.7,1.1,1);
//                        translate1(.05,1.3,0);
//                        DrawCuboid(.7,1.2,1);
//                        translate1(0,1.2,0);
//                        DrawCuboid(.7,1.1,1);
//                         translate1(0,1.2,0);
//                        DrawCuboid(.7,1.1,1);
                glPopMatrix();

                  // glPushMatrix();

                    glPushMatrix();
                     translate1(2,3.5,0);
                        rotate1(-9,0,0,1);
                        glPushMatrix();
                            rotate1(9,0,0,1);
                            translate1(0,-2.2,0);

                            rotate1(angle,1,0,0);
                            translate1(0,2.2,0);
                            rotate1(-9,0,0,1);
                            DrawCuboid(.5,1,1);
                        glPopMatrix();
                        translate1(.05,1,0);
                        glPushMatrix();
                               rotate1(9,0,0,1);
                            translate1(2.5,-2.7,0);

                            rotate1(angle*1.5,1,0,0);
                           translate1(-2.5,2.7,0);
                           rotate1(-9,0,0,1);
                            DrawCuboid(.5,1,1);
                        glPopMatrix();
                        translate1(0,1,0);
                        glPushMatrix();
                           rotate1(9,0,0,1);
                            translate1(2.5,-3.6,0);


                            rotate1(angle*1.7,1,0,0);
                            translate1(-2.5,3.6,0);
                            rotate1(-9,0,0,1);
                            DrawCuboid(.5,1,1);
                        glPopMatrix();
                        translate1(0,1,0);
                        glPushMatrix();
                               rotate1(9,0,0,1);
                            translate1(2.5,-4.5,0);
                             rotate1(angle*1.87,1,0,0);
                              translate1(-2.5,4.5,0);
                              rotate1(-9,0,0,1);
                            DrawCuboid(.5,.9,1);
                        glPopMatrix();
//                         translate1(2,3.5,0);
//                        rotate1(-9,0,0,1);
//                        DrawCuboid(.5,1,1);
//                        translate1(.05,1,0);
//                        DrawCuboid(.5,.8,1);
//                        translate1(0,1,0);
//                        DrawCuboid(.5,.7,1);
//                         translate1(0,1,0);
//                        DrawCuboid(.5,.5,1);
                    glPopMatrix();

                    glPushMatrix();
                            translate1(-3.5,.7,0);
                            rotate1(30,0,0,1);

                            glPushMatrix();
                            rotate1(-30,0,0,1);
                            translate1(2,0,0);
                            rotate1(angle*.7,1,1,0);
                            if(call1==true){
                              rotate1(angle*.7,1,1,0);
                               }
                            translate1(-2,0,0);
                            rotate1(30,0,0,1);
                            DrawCuboid(1,2.5,1);
                            glPopMatrix();


                            rotate1(-30,0,0,1);
                            translate1(-.5,2.6,0);
                            glPushMatrix();
                            translate1(3,0,0);
                            rotate1(angle*1,1,1,0);
                            if(call1==true){

                            rotate1(angle*0.9,1,1.2,1);
                            translate1(-angle*.03,angle*.04,-angle*.004);
                            }
                            translate1(-3,0,0);
                            DrawCuboid(1,2.5,1);

                            DrawCuboid(1,2.5,1);
                            glPopMatrix();


                    glPopMatrix();
                glPopMatrix();
            glPopMatrix();

        translate1(15,0,0);
        scale1(-1,1,1);
        //translate1(-7,0,0);
            glPushMatrix();


            if(call2==true){
                translate1(7.5-ballmovex,-ballmovey-4,-4);
                call1=false;
            }


            if(mirrormode==false){

                cout << "af2 \n";
              //  printMatrix();
             //   glLoadIdentity();
              //  translate1(0,0,-6);
                DrawCuboid(2.7,3.5,1);

                translate1(0,5,0);
                DrawCuboid(5,6,1);

                glPushMatrix();
                    cout << "/n";cout << count; cout <<"/n";
                    rotate1(angle2,1,0,0);


                    glPushMatrix();
                        translate1(-2,3.7,0);
                        rotate1(15,0,1,0);
                        DrawCuboid(.7,1,1);
                        translate1(-.25,1.2,0);
                        DrawCuboid(.7,1.2,1);
                        translate1(0,1.2,0);
                        DrawCuboid(.7,1,1);
                        translate1(0,1.2,0);
                        DrawCuboid(.7,1,1);
                    glPopMatrix();

                    glPushMatrix();
                         translate1(-0.8,3.7,0);
                        rotate1(9,0,1,0);
                        DrawCuboid(.7,1,1);
                        translate1(-.08,1.4,0);
                        DrawCuboid(.7,1.2,1);
                        translate1(0,1.4,0);
                        DrawCuboid(.7,1.2,1);
                         translate1(0,1.4,0);
                        DrawCuboid(.7,1.1,1);
                        glPopMatrix();

                     glPushMatrix();
                         translate1(0.5,3.7,0);
                        rotate1(-7,0,1,0);
                        DrawCuboid(.7,1.1,1);
                        translate1(.05,1.3,0);
                        DrawCuboid(.7,1.2,1);
                        translate1(0,1.2,0);
                        DrawCuboid(.7,1.1,1);
                         translate1(0,1.2,0);
                        DrawCuboid(.7,1.1,1);
                    glPopMatrix();

                    glPushMatrix();
                         translate1(2,3.5,0);
                        rotate1(-15,0,1,0);
                        DrawCuboid(.5,1,1);
                        translate1(.05,1,0);
                        DrawCuboid(.5,.8,1);
                        translate1(0,1,0);
                        DrawCuboid(.5,.7,1);
                         translate1(0,1,0);
                        DrawCuboid(.5,.5,1);
                    glPopMatrix();

                    glPushMatrix();
                            translate1(-3.5,.7,0);
                            rotate1(30,0,0,1);
                            DrawCuboid(1,2.5,1);
                            rotate1(-30,0,0,1);
                            translate1(-.5,2.6,0);
                            DrawCuboid(1,2.5,1);
                    glPopMatrix();
                glPopMatrix();

            }
            else if(mirrormode==true){


            DrawCuboid(2.7,3.5,1);

                translate1(0,5,0);
                DrawCuboid(5,6,1);

                glPushMatrix();
                    cout << "/n";cout << count; cout <<"/n" << angle;



                    glPushMatrix();
                        translate1(-2,3.7,0);
                        rotate1(9,0,0,1);
                        glPushMatrix();
                            rotate1(-9,0,0,1);
                            translate1(0,-2.2,0);

                            rotate1(angle2,1,0,0);
                            translate1(0,2.2,0);
                            rotate1(9,0,0,1);
                            DrawCuboid(.7,1,1);
                        glPopMatrix();
                        translate1(-.25,1.2,0);
                        glPushMatrix();
                               rotate1(-9,0,0,1);
                            translate1(2.5,-2.7,0);

                         rotate1(angle2*1.5,1,0,0);
                           translate1(-2.5,2.7,0);
                           rotate1(9,0,0,1);
                            DrawCuboid(.7,1.2,1);
                        glPopMatrix();
                        translate1(0,1.2,0);
                        glPushMatrix();
                           rotate1(-9,0,0,1);
                            translate1(2.5,-3.6,0);


                            rotate1(angle2*1.7,1,0,0);
                            translate1(-2.5,3.6,0);
                            rotate1(9,0,0,1);
                            DrawCuboid(.7,1,1);
                        glPopMatrix();
                        translate1(0,1.2,0);
                        glPushMatrix();
                               rotate1(-9,0,0,1);
                         translate1(2.5,-4.5,0);
                             rotate1(angle2*1.87,1,0,0);
                              translate1(-2.5,4.5,0);
                              rotate1(9,0,0,1);
                            DrawCuboid(.7,1,1);
                        glPopMatrix();
                    glPopMatrix();



                    glPushMatrix();
                     translate1(-0.5,3.8,0);
                        rotate1(3,0,0,1);
                        glPushMatrix();
                            rotate1(-3,0,0,1);
                            translate1(0,-2.2,0);

                            rotate1(angle2,1,0,0);
                            translate1(0,2.2,0);
                            rotate1(3,0,0,1);
                            DrawCuboid(.7,1,1);
                        glPopMatrix();
                        translate1(-.12,1.4,0);
                        glPushMatrix();
                               rotate1(-3,0,0,1);
                            translate1(2.5,-2.7,0);

                         rotate1(angle2*1.5,1,0,0);
                           translate1(-2.5,2.7,0);
                           rotate1(3,0,0,1);
                            DrawCuboid(.7,1.2,1);
                        glPopMatrix();
                        translate1(0,1.4,0);
                        glPushMatrix();
                           rotate1(-3,0,0,1);
                            translate1(2.5,-3.6,0);


                            rotate1(angle2*1.7,1,0,0);
                            translate1(-2.5,3.6,0);
                            rotate1(3,0,0,1);
                            DrawCuboid(.7,1.2,1);
                        glPopMatrix();
                        translate1(0,1.4,0);
                        glPushMatrix();
                               rotate1(-3,0,0,1);
                         translate1(2.5,-4.5,0);
                             rotate1(angle2*1.87,1,0,0);
                              translate1(-2.5,4.5,0);
                              rotate1(3,0,0,1);
                            DrawCuboid(.7,1.1,1);
                        glPopMatrix();
                    glPopMatrix();
//                           rotate1(3,0,0,1);
//                         translate1(-0.8,3.7,0);
//                        rotate1(5,0,0,1);
//                        DrawCuboid(.7,1,1);
//                        translate1(-.08,1.4,0);
//                        DrawCuboid(.7,1.2,1);
//                        translate1(0,1.4,0);
//                        DrawCuboid(.7,1.2,1);
//                         translate1(0,1.4,0);
//                        DrawCuboid(.7,1.1,1);
//                        glPopMatrix();

                     glPushMatrix();

                    glPushMatrix();
                     translate1(0.5,3.7,0);
                        rotate1(-3,0,0,1);
                        glPushMatrix();
                            rotate1(3,0,0,1);
                            translate1(0,-2.2,0);

                            rotate1(angle2,1,0,0);
                            translate1(0,2.2,0);
                            rotate1(-3,0,0,1);
                            DrawCuboid(.7,1.1,1);
                        glPopMatrix();
                        translate1(.12,1.3,0);
                        glPushMatrix();
                               rotate1(3,0,0,1);
                            translate1(2.5,-2.7,0);

                         rotate1(angle2*1.5,1,0,0);
                           translate1(-2.5,2.7,0);
                           rotate1(-3,0,0,1);
                            DrawCuboid(.7,1.2,1);
                        glPopMatrix();
                        translate1(0,1.2,0);
                        glPushMatrix();
                           rotate1(3,0,0,1);
                            translate1(2.5,-3.6,0);


                            rotate1(angle2*1.7,1,0,0);
                            translate1(-2.5,3.6,0);
                            rotate1(-3,0,0,1);
                            DrawCuboid(.7,1.1,1);
                        glPopMatrix();
                        translate1(0,1.2,0);
                        glPushMatrix();
                               rotate1(3,0,0,1);
                            translate1(2.5,-4.5,0);
                             rotate1(angle2*1.87,1,0,0);
                              translate1(-2.5,4.5,0);
                              rotate1(-3,0,0,1);
                            DrawCuboid(.7,1.1,1);
                        glPopMatrix();
                    glPopMatrix();



//                         translate1(0.5,3.7,0);
//                        rotate1(-3,0,0,1);
//                        DrawCuboid(.7,1.1,1);
//                        translate1(.05,1.3,0);
//                        DrawCuboid(.7,1.2,1);
//                        translate1(0,1.2,0);
//                        DrawCuboid(.7,1.1,1);
//                         translate1(0,1.2,0);
//                        DrawCuboid(.7,1.1,1);
                glPopMatrix();

                  // glPushMatrix();

                    glPushMatrix();
                     translate1(2,3.5,0);
                        rotate1(-9,0,0,1);
                        glPushMatrix();
                            rotate1(9,0,0,1);
                            translate1(0,-2.2,0);

                            rotate1(angle2,1,0,0);
                            translate1(0,2.2,0);
                            rotate1(-9,0,0,1);
                            DrawCuboid(.5,1,1);
                        glPopMatrix();
                        translate1(.05,1,0);
                        glPushMatrix();
                               rotate1(9,0,0,1);
                            translate1(2.5,-2.7,0);

                            rotate1(angle2*1.5,1,0,0);
                           translate1(-2.5,2.7,0);
                           rotate1(-9,0,0,1);
                            DrawCuboid(.5,1,1);
                        glPopMatrix();
                        translate1(0,1,0);
                        glPushMatrix();
                           rotate1(9,0,0,1);
                            translate1(2.5,-3.6,0);


                            rotate1(angle2*1.7,1,0,0);
                            translate1(-2.5,3.6,0);
                            rotate1(-9,0,0,1);
                            DrawCuboid(.5,1,1);
                        glPopMatrix();
                        translate1(0,1,0);
                        glPushMatrix();
                               rotate1(9,0,0,1);
                            translate1(2.5,-4.5,0);
                             rotate1(angle2*1.87,1,0,0);
                              translate1(-2.5,4.5,0);
                              rotate1(-9,0,0,1);
                            DrawCuboid(.5,.9,1);
                        glPopMatrix();
//                         translate1(2,3.5,0);
//                        rotate1(-9,0,0,1);
//                        DrawCuboid(.5,1,1);
//                        translate1(.05,1,0);
//                        DrawCuboid(.5,.8,1);
//                        translate1(0,1,0);
//                        DrawCuboid(.5,.7,1);
//                         translate1(0,1,0);
//                        DrawCuboid(.5,.5,1);
                    glPopMatrix();

                    glPushMatrix();
                            translate1(-3.5,.7,0);
                            rotate1(30,0,0,1);

                            glPushMatrix();
                            rotate1(-30,0,0,1);
                            translate1(2,0,0);
                            rotate1(angle2*.7,1,1,0);
                            if(call2==true){
                              rotate1(angle2*.7,1,1,0);
                               }
                            translate1(-2,0,0);
                            rotate1(30,0,0,1);
                            DrawCuboid(1,2.5,1);
                            glPopMatrix();


                            rotate1(-30,0,0,1);
                            translate1(-.5,2.6,0);
                            glPushMatrix();
                            translate1(3,0,0);
                            rotate1(angle2*1,1,1,0);
                            if(call2==true){

                            rotate1(angle2*0.9,1,1.2,1);
                            translate1(-angle2*.03,angle2*.04,-angle2*.004);
                            }
                            translate1(-3,0,0);
                            DrawCuboid(1,2.5,1);

                            DrawCuboid(1,2.5,1);
                            glPopMatrix();


                    glPopMatrix();
                glPopMatrix();


            }

            glPopMatrix();

          //  printMatrix();

            translate1(7,0,0);
           // glPushMatrix();
    if(modeball==1){
         glPushMatrix();
         translate1(-ballmovex,-ballmovey,0);
         cout << "ball move x  " << -ballmovex << "ball move y " << -ballmovey;
         if(mouserelease ==4){
            if(ballmovex >0){
            call2 = true;

            }
            else if(ballmovex <=0){
            call1 = true;


            }

                            //
            if(call1==true) angle2=0;
            else if(call2 == true) angle =0;
         }
         glutSolidSphere(3,30,30);
        glutPostRedisplay();

        glPopMatrix();
        }
        if (reset==true) {
            call1=false;
            call2=false;
            glLoadIdentity();
            ballmovex=0;
            ballmovey=0;
            mouserelease=0;
            count=0;
            angle=0;
            stop=false;
            pos=true;
            modeball=0;
            mode=0;
            mirror=0;
            reset=false;
            angle2=0;

        }

    glPopMatrix();
//    glPushMatrix();
//
  //    DrawCuboid(4,6,1);
//
//
//    glPopMatrix();

 glutSwapBuffers();
}


void printMatrix(void){
  GLdouble matrix[16];
    int i;int j;
  glGetDoublev(GL_MODELVIEW_MATRIX,matrix);
  printf("model view matrix\n");
  for (i=0; i<4; i++) {
    for (j=0; j<4; j++) {
      cout << matrix[j*4+i] ;
      printf(" ");
    }
    printf("\n");
  }

}

void DrawCuboid(float x, float y, float z){
float a,b,c;

a=x/2;
b=y/2;
c=z/2;

GLfloat vertices[] = { a, b, c,  -a, b, c,  -a,-b ,c,   a,-b, c,   // v0,v1,v2,v3 (front)
                        a, b, c,   a,-b, c,   a,-b,-c,   a, b,-c,   // v0,v3,v4,v5 (right)
                        a, b, c,   a, b,-c,  -a, b,-c,  -a, b, c,   // v0,v5,v6,v1 (top)
                       -a, b, c,  -a, b,-c,  -a,-b,-c,  -a,-b, c,   // v1,v6,v7,v2 (left)
                       -a,-b,-c,   a,-b,-c,   a,-b, c,  -a,-b, c,   // v7,v4,v3,v2 (bottom)
                        a,-b,-c,  -a,-b,-c,  -a, b,-c,   a, b,-c }; // v4,v7,v6,v5 (back)


GLfloat normals[]  = { 0, 0, c,   0, 0, c,   0, 0, c,   0, 0, c,   // v0,v1,v2,v3 (front)
                        a, 0, 0,   a, 0, 0,   a, 0, 0,   a, 0, 0,   // v0,v3,v4,v5 (right)
                        0, b, 0,   0, b, 0,   0, b, 0,   0, b, 0,   // v0,v5,v6,v1 (top)
                       -a, 0, 0,  -a, 0, 0,  -a, 0, 0,  -a, 0, 0,   // v1,v6,v7,v2 (left)
                        0,-b, 0,   0,-b, 0,   0,-b, 0,   0,-b, 0,   // v7,v4,v3,v2 (bottom)
                        0, 0,-c,   0, 0,-c,   0, 0,-c,   0, 0,-c }; // v4,v7,v6,v5 (back)

                        GLfloat colors[]   = { 1, 1, 1,   1, 1, 0,   1, 0, 0,   1, 0, 1,   // v0,v1,v2,v3 (front)
                        1, 1, 1,   1, 0, 1,   0, 0, 1,   0, 1, 1,   // v0,v3,v4,v5 (right)
                        1, 1, 1,   0, 1, 1,   0, 1, 0,   1, 1, 0,   // v0,v5,v6,v1 (top)
                        1, 1, 0,   0, 1, 0,   0, 0, 0,   1, 0, 0,   // v1,v6,v7,v2 (left)
                        0, 0, 0,   0, 0, 1,   1, 0, 1,   1, 0, 0,   // v7,v4,v3,v2 (bottom)
                        0, 0, 1,   0, 0, 0,   0, 1, 0,   0, 1, 1 }; // v4,v7,v6,v5 (back)

// index array of vertex array for glDrawElements() & glDrawRangeElement()
GLubyte indices1[]  = { 0, 1, 2,3,        // front
                       4, 5, 6,7 ,        // right
                       8, 9,10,11,       // top
                      12,13,14,15,        // left
                      16,17,18,19 ,      // bottom
                      20,21,22,23 };



  glEnableClientState(GL_NORMAL_ARRAY);
    glEnableClientState(GL_COLOR_ARRAY);
    glEnableClientState(GL_VERTEX_ARRAY);
  //  glColor3f(1, 0, 1);

       glNormalPointer(GL_FLOAT, 0, normals);
    glColorPointer(3, GL_FLOAT, 0, colors);
    glVertexPointer(3, GL_FLOAT, 0, vertices);

     glPushMatrix();
 //   translate1(0, 0, -5);
              // move to bottom-left corner
              //   translate1(0, 0, -cameraDistance);
              //  rotate1(cameraAngleX,1,0,0);
             //   rotate1(cameraAngleY,0,1,0);


  //  rotate1(34,1,1,1);

            glDrawElements(GL_QUADS, 24, GL_UNSIGNED_BYTE, indices1);
    glPopMatrix();
glDisableClientState(GL_VERTEX_ARRAY);  // disable vertex arrays
    glDisableClientState(GL_COLOR_ARRAY);
    glDisableClientState(GL_NORMAL_ARRAY);

//	glFlush();
   // glutSwapBuffers();
   glutPostRedisplay();
}

void Ball(void){
                }

//void animate() {
//	glutPostRedisplay();
//}

int main(int argc, char * argv[]) {
    glutInit(&argc, argv);
    glutInitDisplayMode (GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
   // glutTimerFunc(33, timerCB, 33);
    glutInitWindowSize (800, 600);

    glutCreateWindow (argv[0]);   init();
    glutKeyboardFunc (keyboard);
    glutMouseFunc(mouse);
    glutMotionFunc(mouseMotion);
    glutDisplayFunc (displayCuboid);
 //   glutTimerFunc(23,moveHand,0);
    glutReshapeFunc (reshape);

//    glutIdleFunc(animate);
    glutMainLoop();
    return 0;
}
//void timerCB(int millisec)
//{
//    //rotate1(1,0,0,1);
//    glutTimerFunc(millisec, timerCB, millisec);
//   glutPostRedisplay();
//}
//

void keyboard(unsigned char key, int x, int y)
{
    switch(key)
    {
    case 27: // ESCAPE
        exit(0);
        break;

    case ' ':
        break;


    case 'm':
    case 'M':
    mode = ++mode % 2;
    if (mode==0 ){
    glutTimerFunc(23,moveHand,0);
    cout << "hello";
    }

    else if (mode ==1){
        cout << "hello";
    glutTimerFunc(23,moveHand,0);
    }
    break;



    case 'a':
    case 'A':
    modeball = ++modeball % 2;
    if (modeball==0){
        reset=true;
    }
    else if(modeball==1){
        reset=false;
    }

    break;

    case 's':
    case 'S':
    mirror= ++mirror % 2;
    if (mirror==0){
        mirrormode=false;
    }
    else if(modeball==1){
        mirrormode=true;
    }

    break;


    case 'd': // switch rendering modes (fill -> wire -> point)
    case 'D':
        drawMode = ++drawMode % 3;
        if(drawMode == 0)        // fill mode
        {
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
            glEnable(GL_DEPTH_TEST);
            glEnable(GL_CULL_FACE);
        }
        else if(drawMode == 1)  // wireframe mode
        {
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
            glDisable(GL_DEPTH_TEST);
            glDisable(GL_CULL_FACE);
        }
        else                    // point mode
        {
            glPolygonMode(GL_FRONT_AND_BACK, GL_POINT);
            glDisable(GL_DEPTH_TEST);
            glDisable(GL_CULL_FACE);
        }
        break;

    default:
        ;
    }
}


void mouse(int button, int state, int x, int y)
{
    mouseX = x;
    mouseY = y;
if(button == GLUT_LEFT_BUTTON)
    {
        if (modeball==1) mouserelease=4;
        if(state == GLUT_DOWN)
        {
            mouseLeftDown = true;
            if( modeball==1 ){
            mouserelease=2;   }
        }
        else if(state == GLUT_UP)
            mouseLeftDown = false;
            if(mouserelease==2 && modeball==1){
            mouserelease=1;}
    }


    if(button == GLUT_MIDDLE_BUTTON)
    {
        if(state == GLUT_DOWN)
        {
            mouseMiddleDown = true;
        }
        else if(state == GLUT_UP)
            mouseMiddleDown = false;
    }

    else if(button == GLUT_RIGHT_BUTTON)
    {
        if(state == GLUT_DOWN)
        {
            mouseRightDown = true;
        }
        else if(state == GLUT_UP)
            mouseRightDown = false;
    }


    }


void mouseMotion(int x, int y)
{


    if(mouseLeftDown&&modeball==1)
    {
        ballmovex += (x - mouseX)*.08f;
        ballmovey += (y - mouseY)*.08f;
        mouseX = x;
        mouseY = y;
    }


    if(mouseMiddleDown)
    {
        cameraAngleY += (x - mouseX);
        cameraAngleX += (y - mouseY);
        mouseX = x;
        mouseY = y;
    }
    if(mouseRightDown)
    {
        cameraDistance -= (y - mouseY) * 0.2f;
        cameraDistancex -=(x-mouseX)*.2f;
        mouseY = y;
        mouseX=x;
    }
}

