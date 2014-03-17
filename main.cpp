#include <cstdlib>
#include <ctime>
#include <iostream>
#include <set>
#include <stdlib.h>
#include <vector>
#include "GLee/GLee.h"
#ifdef __APPLE__
#include <OpenGL/OpenGL.h>
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif
#include "Maths/Maths.h"
#include "imageloader.h"
#include "vec3f.h"
#include "stdio.h"
#include <sstream>
using namespace std;

//Returns a random float from 0 to < 1
float randomFloat() {
	return (float)rand() / ((float)RAND_MAX + 1);
}
int Diff=1;
const float GRAVITY = 0.0f;
const float BOX_SIZE = 18.0f*(0.5+ Diff*0.5); //The length of one side of the box
//The amount of time between each time that we handle collisions and apply the
//effects of gravity
const float TIME_BETWEEN_UPDATES = .01f;
float timet1,timet2;
const int TIMER_MS = 25; //The number of milliseconds to which the timer is set
float cameraAngleX;
float cameraAngleY;
float cameraDistance;
void mouseCB(int button, int state, int x, int y);
void mouseMotionCB(int x, int y);
void drawScene(void);
int popno=0;
int windowWidth, windowHeight;
//int Diff=30;
float mouseX;
float mouseY;
bool mouseLeftDown,mouseRightDown,mouseMiddleDown;
MATRIX4X4 lightProjectionMatrix, lightViewMatrix;
MATRIX4X4 cameraProjectionMatrix, cameraViewMatrix;



float cx=-0.0;float cy = 0.0;float cz = -20.0;
VECTOR3D cameraPosition(cx,cy,cz);
VECTOR3D lightPosition(3.0f, -5.0f,-5.0f);

//Size of shadow map
const int shadowMapSize=512;

//Textures
GLuint shadowMapTexture;
//Stores information regarding a ball
struct Ball {
	Vec3f v; //Velocity
	Vec3f pos; //Position
	float r; //Radius
	Vec3f color;
};

enum Wall {WALL_LEFT, WALL_RIGHT, WALL_FAR, WALL_NEAR, WALL_TOP, WALL_BOTTOM};

//Stores a pair of balls
struct BallPair {
	Ball* ball1;
	Ball* ball2;
};

//Stores a ball and a wall
struct BallWallPair {
	Ball* ball;
	Wall wall;
};

const int MAX_OCTREE_DEPTH = 6;
const int MIN_BALLS_PER_OCTREE = 3;
const int MAX_BALLS_PER_OCTREE = 6;

LPVOID glutFonts[7] = {
    GLUT_BITMAP_9_BY_15,
    GLUT_BITMAP_8_BY_13,
    GLUT_BITMAP_TIMES_ROMAN_10,
    GLUT_BITMAP_TIMES_ROMAN_24,
    GLUT_BITMAP_HELVETICA_10,
    GLUT_BITMAP_HELVETICA_12,
    GLUT_BITMAP_HELVETICA_18
};

// Here is the function
void glutPrint(float x, float y, LPVOID font, char* text, float r, float g, float b, float a)
{
//    glMatrixMode(GL_PROJECTION);
//    glPushMatrix();
//    glLoadIdentity();
//    gluOrtho2D(0.0, windowWidth, 0.0, windowHeight);
//
//    glMatrixMode(GL_MODELVIEW);
//    glPushMatrix();
//    glLoadIdentity();
    if(!text || !strlen(text)) return;
//    bool blending = false;
//    if(glIsEnabled(GL_BLEND)) blending = true;
//    glEnable(GL_BLEND);
    glColor4f(r,g,b,a);
    glRasterPos2f(x,y);
    while (*text) {
        glutBitmapCharacter(font, *text);
        text++;
    }
//    if(!blending) glDisable(GL_BLEND);
//    glMatrixMode(GL_MODELVIEW);
//    glPopMatrix();
//
//    glMatrixMode(GL_PROJECTION);
//    glPopMatrix();
}



//Our data structure for making collision detection faster
class Octree {
	private:
		Vec3f corner1; //(minX, minY, minZ)
		Vec3f corner2; //(maxX, maxY, maxZ)
		Vec3f center;//((minX + maxX) / 2, (minY + maxY) / 2, (minZ + maxZ) / 2)

		/* The children of this, if this has any.  children[0][*][*] are the
		 * children with x coordinates ranging from minX to centerX.
		 * children[1][*][*] are the children with x coordinates ranging from
		 * centerX to maxX.  Similarly for the other two dimensions of the
		 * children array.
		 */
		Octree *children[2][2][2];
		//Whether this has children
		bool hasChildren;
		//The balls in this, if this doesn't have any children

	public:
		set<Ball*> balls;
		//The depth of this in the tree
		int depth;
		//The number of balls in this, including those stored in its children
		int numBalls;

		//Adds a ball to or removes one from the children of this
		void fileBall(Ball* ball, Vec3f pos, bool addBall) {
			//Figure out in which child(ren) the ball belongs
			for(int x = 0; x < 2; x++) {
				if (x == 0) {
					if (pos[0] - ball->r > center[0]) {
						continue;
					}
				}
				else if (pos[0] + ball->r < center[0]) {
					continue;
				}

				for(int y = 0; y < 2; y++) {
					if (y == 0) {
						if (pos[1] - ball->r > center[1]) {
							continue;
						}
					}
					else if (pos[1] + ball->r < center[1]) {
						continue;
					}

					for(int z = 0; z < 2; z++) {
						if (z == 0) {
							if (pos[2] - ball->r > center[2]) {
								continue;
							}
						}
						else if (pos[2] + ball->r < center[2]) {
							continue;
						}

						//Add or remove the ball
						if (addBall) {
							children[x][y][z]->add(ball);
						}
						else {
							children[x][y][z]->remove(ball, pos);
						}
					}
				}
			}
		}

		//Creates children of this, and moves the balls in this to the children
		void haveChildren() {
			for(int x = 0; x < 2; x++) {
				float minX;
				float maxX;
				if (x == 0) {
					minX = corner1[0];
					maxX = center[0];
				}
				else {
					minX = center[0];
					maxX = corner2[0];
				}

				for(int y = 0; y < 2; y++) {
					float minY;
					float maxY;
					if (y == 0) {
						minY = corner1[1];
						maxY = center[1];
					}
					else {
						minY = center[1];
						maxY = corner2[1];
					}

					for(int z = 0; z < 2; z++) {
						float minZ;
						float maxZ;
						if (z == 0) {
							minZ = corner1[2];
							maxZ = center[2];
						}
						else {
							minZ = center[2];
							maxZ = corner2[2];
						}

						children[x][y][z] = new Octree(Vec3f(minX, minY, minZ),
													   Vec3f(maxX, maxY, maxZ),
													   depth + 1);
					}
				}
			}

			//Remove all balls from "balls" and add them to the new children
			for(set<Ball*>::iterator it = balls.begin(); it != balls.end();
					it++) {
				Ball* ball = *it;
				fileBall(ball, ball->pos, true);
			}
			balls.clear();

			hasChildren = true;
		}

		//Adds all balls in this or one of its descendants to the specified set
		void collectBalls(set<Ball*> &bs) {
			if (hasChildren) {
				for(int x = 0; x < 2; x++) {
					for(int y = 0; y < 2; y++) {
						for(int z = 0; z < 2; z++) {
							children[x][y][z]->collectBalls(bs);
						}
					}
				}
			}
			else {
				for(set<Ball*>::iterator it = balls.begin(); it != balls.end();
						it++) {
					Ball* ball = *it;
					bs.insert(ball);
				}
			}
		}

		//Destroys the children of this, and moves all balls in its descendants
		//to the "balls" set
		void destroyChildren() {
			//Move all balls in descendants of this to the "balls" set
			collectBalls(balls);

			for(int x = 0; x < 2; x++) {
				for(int y = 0; y < 2; y++) {
					for(int z = 0; z < 2; z++) {
						delete children[x][y][z];
					}
				}
			}

			hasChildren = false;
		}

		//Removes the specified ball at the indicated position
		void remove(Ball* ball, Vec3f pos) {
			numBalls--;

			if (hasChildren && numBalls < MIN_BALLS_PER_OCTREE) {
				destroyChildren();
			}

			if (hasChildren) {
				fileBall(ball, pos, false);
			}
			else {
				balls.erase(ball);
			}
		}

		/* Helper fuction for potentialBallWallCollisions(vector).  Adds
		 * potential ball-wall collisions to cs, where w is the type of wall,
		 * coord is the relevant coordinate of the wall ('x', 'y', or 'z'), and
		 * dir is 0 if the wall is in the negative direction and 1 if it is in
		 * the positive direction.  Assumes that this is in the extreme
		 * direction of the coordinate, e.g. if w is WALL_TOP, the function
		 * assumes that this is in the far upward direction.
		 */
		void potentialBallWallCollisions(vector<BallWallPair> &cs,
										 Wall w, char coord, int dir) {
			if (hasChildren) {
				//Recursively call potentialBallWallCollisions on the correct
				//half of the children (e.g. if w is WALL_TOP, call it on
				//children above centerY)
				for(int dir2 = 0; dir2 < 2; dir2++) {
					for(int dir3 = 0; dir3 < 2; dir3++) {
						Octree *child;
						switch (coord) {
							case 'x':
								child = children[dir][dir2][dir3];
								break;
							case 'y':
								child = children[dir2][dir][dir3];
								break;
							case 'z':
								child = children[dir2][dir3][dir];
								break;
						}

						child->potentialBallWallCollisions(cs, w, coord, dir);
					}
				}
			}
			else {
				//Add (ball, w) for all balls in this
				for(set<Ball*>::iterator it = balls.begin(); it != balls.end();
						it++) {
					Ball* ball = *it;
					BallWallPair bwp;
					bwp.ball = ball;
					bwp.wall = w;
					cs.push_back(bwp);
				}
			}
		}
	public:
		//Constructs a new Octree.  c1 is (minX, minY, minZ), c2 is (maxX, maxY,
		//maxZ), and d is the depth, which starts at 1.
		Octree(Vec3f c1, Vec3f c2, int d) {
			corner1 = c1;
			corner2 = c2;
			center = (c1 + c2) / 2;
			depth = d;
			numBalls = 0;
			hasChildren = false;
		}

		//Destructor
		~Octree() {
			if (hasChildren) {
				destroyChildren();
			}
		}

		//Adds a ball to this
		void add(Ball* ball) {
			numBalls++;
			if (!hasChildren && depth < MAX_OCTREE_DEPTH &&
				numBalls > MAX_BALLS_PER_OCTREE) {
				haveChildren();
			}

			if (hasChildren) {
				fileBall(ball, ball->pos, true);
			}
			else {
				balls.insert(ball);
			}
		}

		//Removes a ball from this
		void remove(Ball* ball) {
			remove(ball, ball->pos);
		}

		//Changes the position of a ball in this from oldPos to ball->pos
		void ballMoved(Ball* ball, Vec3f oldPos) {
			remove(ball, oldPos);
			add(ball);
		}

		//Adds potential ball-ball collisions to the specified set
		void potentialBallBallCollisions(vector<BallPair> &collisions) {
			if (hasChildren) {
				for(int x = 0; x < 2; x++) {
					for(int y = 0; y < 2; y++) {
						for(int z = 0; z < 2; z++) {
							children[x][y][z]->
								potentialBallBallCollisions(collisions);
						}
					}
				}
			}
			else {
				//Add all pairs (ball1, ball2) from balls
				for(set<Ball*>::iterator it = balls.begin(); it != balls.end();
						it++) {
					Ball* ball1 = *it;
					for(set<Ball*>::iterator it2 = balls.begin();
							it2 != balls.end(); it2++) {
						Ball* ball2 = *it2;
						//This test makes sure that we only add each pair once
						if (ball1 < ball2) {
							BallPair bp;
							bp.ball1 = ball1;
							bp.ball2 = ball2;
							collisions.push_back(bp);
						}
					}
				}
			}
		}

		//Adds potential ball-wall collisions to the specified set
		void potentialBallWallCollisions(vector<BallWallPair> &collisions) {
			potentialBallWallCollisions(collisions, WALL_LEFT, 'x', 0);
			potentialBallWallCollisions(collisions, WALL_RIGHT, 'x', 1);
			potentialBallWallCollisions(collisions, WALL_BOTTOM, 'y', 0);
			potentialBallWallCollisions(collisions, WALL_TOP, 'y', 1);
			potentialBallWallCollisions(collisions, WALL_FAR, 'z', 0);
			potentialBallWallCollisions(collisions, WALL_NEAR, 'z', 1);
		}
};

//Puts potential ball-ball collisions in potentialCollisions.  It must return
//all actual collisions, but it need not return only actual collisions.
void potentialBallBallCollisions(vector<BallPair> &potentialCollisions,
								 vector<Ball*> &balls, Octree* octree) {

	octree->potentialBallBallCollisions(potentialCollisions);


}

//Puts potential ball-wall collisions in potentialCollisions.  It must return
//all actual collisions, but it need not return only actual collisions.
void potentialBallWallCollisions(vector<BallWallPair> &potentialCollisions,
								 vector<Ball*> &balls, Octree* octree) {

	octree->potentialBallWallCollisions(potentialCollisions);


}

//Moves all of the balls by their velocity times dt
void moveBalls(vector<Ball*> &balls, Octree* octree, float dt) {
	for(unsigned int i = 0; i < balls.size(); i++) {
		Ball* ball = balls[i];
		Vec3f oldPos = ball->pos;
		ball->pos += ball->v * dt*Diff*10;
		octree->ballMoved(ball, oldPos);
	}
}

//Decreases the y coordinate of the velocity of each ball by GRAVITY *
//TIME_BETWEEN_UPDATES
void applyGravity(vector<Ball*> &balls) {
	for(unsigned int i = 0; i < balls.size(); i++) {
		Ball* ball = balls[i];
		ball->v -= Vec3f(0, GRAVITY * TIME_BETWEEN_UPDATES, 0);
	}
}

//Returns whether two balls are colliding
bool testBallBallCollision(Ball* b1, Ball* b2) {
	//Check whether the balls are close enough
	float r = b1->r + b2->r;
	if ((b1->pos - b2->pos).magnitudeSquared() < r * r) {
		//Check whether the balls are moving toward each other
		Vec3f netVelocity = b1->v - b2->v;
		Vec3f displacement = b1->pos - b2->pos;
		return netVelocity.dot(displacement) < 0;
	}
	else
		return false;
}

//Handles all ball-ball collisions
void handleBallBallCollisions(vector<Ball*> &balls, Octree* octree) {
	vector<BallPair> bps;
	potentialBallBallCollisions(bps, balls, octree);
	for(unsigned int i = 0; i < bps.size(); i++) {
		BallPair bp = bps[i];

		Ball* b1 = bp.ball1;
		Ball* b2 = bp.ball2;
		if (testBallBallCollision(b1, b2)) {
			//Make the balls reflect off of each other
			Vec3f displacement = (b1->pos - b2->pos).normalize();
			b1->v -= 2 * displacement * b1->v.dot(displacement);
			b2->v -= 2 * displacement * b2->v.dot(displacement);
		}
	}
}

//Returns the direction from the origin to the wall
Vec3f wallDirection(Wall wall) {
	switch (wall) {
		case WALL_LEFT:
			return Vec3f(-1, 0, 0);
		case WALL_RIGHT:
			return Vec3f(1, 0, 0);
		case WALL_FAR:
			return Vec3f(0, 0, -1);
		case WALL_NEAR:
			return Vec3f(0, 0, 1);
		case WALL_TOP:
			return Vec3f(0, 1, 0);
		case WALL_BOTTOM:
			return Vec3f(0, -1, 0);
		default:
			return Vec3f(0, 0, 0);
	}
}

//Returns whether a ball and a wall are colliding
bool testBallWallCollision(Ball* ball, Wall wall) {
	Vec3f dir = wallDirection(wall);
	//Check whether the ball is far enough in the "dir" direction, and whether
	//it is moving toward the wall
	return ball->pos.dot(dir) + ball->r > BOX_SIZE / 2 &&
			ball->v.dot(dir) > 0;
}

//Handles all ball-wall collisions
void handleBallWallCollisions(vector<Ball*> &balls, Octree* octree) {
	vector<BallWallPair> bwps;
	potentialBallWallCollisions(bwps, balls, octree);
	for(unsigned int i = 0; i < bwps.size(); i++) {
		BallWallPair bwp = bwps[i];

		Ball* b = bwp.ball;
		Wall w = bwp.wall;
		if (testBallWallCollision(b, w)) {
			//Make the ball reflect off of the wall
			Vec3f dir = (wallDirection(w)).normalize();
			b->v -= 2 * dir * b->v.dot(dir);
		}
	}
}

//Applies gravity and handles all collisions.  Should be called every
//TIME_BETWEEN_UPDATES seconds.
void performUpdate(vector<Ball*> &balls, Octree* octree) {
	applyGravity(balls);
	handleBallBallCollisions(balls, octree);
	handleBallWallCollisions(balls, octree);
}

//Advances the state of the balls by t.  timeUntilUpdate is the amount of time
//until the next call to performUpdate.
void advance(vector<Ball*> &balls,
			 Octree* octree,
			 float t,
			 float &timeUntilUpdate) {
	while (t > 0) {
		if (timeUntilUpdate <= t) {
			moveBalls(balls, octree, timeUntilUpdate);
			performUpdate(balls, octree);
			t -= timeUntilUpdate;
			timeUntilUpdate = TIME_BETWEEN_UPDATES;
		}
		else {
			moveBalls(balls, octree, t);
			timeUntilUpdate -= t;
			t = 0;
		}
	}
}





vector<Ball*> _balls; //All of the balls in play
float _angle = 0.0f; //The camera angle
Octree* _octree; //An octree with all af the balls
//The amount of time until performUpdate should be called
float _timeUntilUpdate = 0;
GLuint _textureId;

//Deletes everything.  This should be called when exiting the program.
void cleanup() {
	for(unsigned int i = 0; i < _balls.size(); i++) {
		delete _balls[i];
	}
	delete _octree;
}

void handleKeypress(unsigned char key, int x, int y) {
	switch (key) {
		case 27: //Escape key
			cleanup();
			exit(0);
		case ' ':
			//Add 20 balls with a random position, velocity, radius, and color
			for(int i = 0; i < 30; i++) {
				Ball* ball = new Ball();
				ball->pos = Vec3f((8 * randomFloat() - 4),
								  (8 * randomFloat() - 4),
								  (8 * randomFloat() - 4));
				ball->v = Vec3f((8 * randomFloat() - 4)*.1,
								(8 * randomFloat() - 4)*.1,
								(8 * randomFloat() - 4)*.1);
				ball->r =  randomFloat() + 0.1f;
				ball->color = Vec3f( (randomFloat())*0.1f + 0.9f,
									 (randomFloat())*0.1f + 0.9f,
									 (randomFloat())*0.1f + 0.9f);
				_balls.push_back(ball);
				_octree->add(ball);

			}
			timet1=glutGet(GLUT_ELAPSED_TIME);
			drawScene();
			break;
        case 'd':
        case 'D':
        {


        int drawMode=0;
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
        }
            break;

        case 't':
        case 'T':
       // int Diff=1;
        Diff=++Diff %5;
        cout<<"THE DIICULTY IS"<<Diff<<"\n";
            break;


	}
}

VECTOR3D GetOGLPos(int x, int y)
{
    GLint viewport[4];
    GLdouble modelview[16];
    GLdouble projection[16];
    GLfloat winX, winY, winZ;
    GLdouble posX, posY, posZ;

    glGetDoublev( GL_MODELVIEW_MATRIX, modelview );
    glGetDoublev( GL_PROJECTION_MATRIX, projection );
    glGetIntegerv( GL_VIEWPORT, viewport );

    winX = (float)x;
    winY = (float)viewport[3] - (float)y;
    glReadPixels( x, int(winY), 1, 1, GL_DEPTH_COMPONENT, GL_FLOAT, &winZ );

    gluUnProject( winX, winY, winZ, modelview, projection, viewport, &posX, &posY, &posZ);

    return VECTOR3D(posX, posY, posZ);
}

bool BubblePop(int x,int y){
VECTOR3D v;
 v=GetOGLPos(x,y);
// cout<<"x component:"<< v.x<<"  y component:"<< v.y << " z component:"<< v.z<< " \n";
// cout << " ball size:"<< _balls.size()<< " \n";
for(unsigned int i = 0; i < _balls.size(); i++) {
		Ball* ball = _balls[i];
		//cout<<"ball postion x:"<< ball->pos[0]<<"  ball position y:"<< ball-> pos[1] << " ball position z : "<< ball -> pos[2]<<"radius:"<<ball->r<< " \n ";
		if ( ((-v.x <= ( ball->pos[0]+ ball->r) )&&(-v.x > ( ball->pos[0]-ball->r))) &&     //return true;
		 ((v.y <= ( ball->pos[1]+ ball->r))&&(v.y > ( ball->pos[1]- ball->r)) ) )  // &&// return true;
		 //(v.z <= ( ball->pos[2]+ ball->r)&&(v.z >= ( ball->pos[2]- ball->r) ) ))
		    {popno=i;
		     return true;
		    }
       // else return false;
	}
    return false;
}


GLuint loadTexture(Image* image) {
	GLuint textureId;
	glGenTextures(1, &textureId);
	glBindTexture(GL_TEXTURE_2D, textureId);
	glTexImage2D(GL_TEXTURE_2D,
				 0,
				 GL_RGBA,
				 image->width, image->height,
				 0,
				 GL_RGB,
				 GL_UNSIGNED_BYTE,
				 image->pixels);
	return textureId;
}

bool initRendering(void) {
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_LIGHTING);
	//glEnable(GL_LIGHT0);
	glEnable(GL_NORMALIZE);
	glEnable(GL_COLOR_MATERIAL);
	if(!GLEE_ARB_depth_texture || !GLEE_ARB_shadow)
	{
		printf("I require ARB_depth_texture and ARB_shadow extensionsn\n");
		return false;
	}
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
	//Load identity modelview
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);

    glClearDepth(1.0f);
	glDepthFunc(GL_LEQUAL);
	glEnable(GL_DEPTH_TEST);

	glEnable(GL_CULL_FACE);

    glGenTextures(1, &shadowMapTexture);
	glBindTexture(GL_TEXTURE_2D, shadowMapTexture);
	glTexImage2D(	GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, shadowMapSize, shadowMapSize, 0,
					GL_DEPTH_COMPONENT, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
    	//Use the color as the ambient and diffuse material
	glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
	glEnable(GL_COLOR_MATERIAL);

	//White specular material color, shininess 16
	glMaterialfv(GL_FRONT, GL_SPECULAR, white);
	glMaterialf(GL_FRONT, GL_SHININESS, 16.0f);

	glEnable(GL_BLEND); //Enable alpha blending
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); //Set the blend function

    glPushMatrix();

	glLoadIdentity();
	gluPerspective(45.0f, (float)windowWidth/(float)windowHeight, 1.0f, 100.0f);
//		gluPerspective(45.0f, 1.0f, 0.0f, 20.0f);
	glGetFloatv(GL_MODELVIEW_MATRIX,cameraProjectionMatrix );

    glLoadIdentity();
	gluLookAt(cameraPosition.x, cameraPosition.y, cameraPosition.z,
				0.0f, 0.0f, 0.0f,
				0.0f, 1.0f, 0.0f);
	glGetFloatv(GL_MODELVIEW_MATRIX, cameraViewMatrix);

	glLoadIdentity();
	gluPerspective(90.0f, 1.0f, 1.0f, 100.0f);
	glGetFloatv(GL_MODELVIEW_MATRIX,lightProjectionMatrix );

	glLoadIdentity();
	gluLookAt(	lightPosition.x, lightPosition.y, lightPosition.z,
				0.0f, 0.0f, 0.0f,
				0.0f, 1.0f, 0.0f);
	glGetFloatv(GL_MODELVIEW_MATRIX, lightViewMatrix);

   glPopMatrix();
	Image *image = loadBMP("bubble.bmp");
	_textureId = loadTexture(image);
	delete image;
	return true;
}
void printM(MATRIX4X4 a, string s){
    cout<<"8888888888888888   "<< s<<"~~~~~~~~~~~~~~~~~~~~~~~\n";
    cout << a.GetEntry(0)<<" "<<a.GetEntry(1)<< " "<< a.GetEntry(2)<< " "<< a.GetEntry(3)<<"\n";
    cout << a.GetEntry(4)<<" "<<a.GetEntry(5)<< " "<< a.GetEntry(6)<< " "<< a.GetEntry(7)<<"\n";
    cout << a.GetEntry(8)<<" "<<a.GetEntry(9)<< " "<< a.GetEntry(10)<< " "<< a.GetEntry(11)<<"\n";
    cout << a.GetEntry(12)<<" "<<a.GetEntry(13)<< " "<< a.GetEntry(14)<< " "<< a.GetEntry(15)<<"\n";
    cout<<"~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n";
}


void handleResize(int w, int h) {
	windowWidth=w;windowHeight=h;


 //   glViewport(0, 0, w, h);
	glPushMatrix();
	//glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(45.0, (float)windowWidth/windowHeight, 1.0, 100.0);

	glGetFloatv(GL_MODELVIEW_MATRIX, cameraProjectionMatrix);
	glPopMatrix();
}
int score=0;
void updateScore(){
score=score+10;

}
void endprog(){
cout<<" TIME IS UP YOUR SCORE IS:"<<score;

            cleanup();
			exit(0);
			int i;
            scanf("%i",i);
}

float A[1000];
float B[1000];
float C[1000];



void intersectionleft(int n,float x0,float y0,float z0,float r0)
{
    int i=0,j=0;
    float dist[1000];
    for(i=0;i<n;i++)
    {
        for(j=0;j<n;j++)
        {


        dist[i*n+j]=sqrtf(((-BOX_SIZE/2)-x0)*((-BOX_SIZE/2)-x0)+((-BOX_SIZE/2)-y0+j*BOX_SIZE/(n-1))*((-BOX_SIZE/2)-y0+j*BOX_SIZE/(n-1))+((-BOX_SIZE/2)-z0+i*BOX_SIZE/(n-1))*((-BOX_SIZE/2)-z0+i*BOX_SIZE/(n-1)));
        A[i*n+j]=x0+(r0*((-BOX_SIZE/2)-x0))/dist[i*n+j];
        B[i*n+j]=y0+(r0*((-BOX_SIZE/2)-y0+(j*(BOX_SIZE/(n-1)))))/dist[i*n+j];
        C[i*n+j]=z0+(r0*((-BOX_SIZE/2)-z0+(i*BOX_SIZE/(n-1))))/dist[i*n+j];

        }
    }
}


void intersectionbottom(int n,float x0,float y0,float z0,float r0)
{
    int i=0,j=0;
    float dist[1000];
    for(i=0;i<n;i++)
    {
        for(j=0;j<n;j++)
        {


        dist[i*n+j]=sqrtf(((-BOX_SIZE/2)-x0+j*BOX_SIZE/(n-1))*((-BOX_SIZE/2)-x0+j*BOX_SIZE/(n-1))+((-BOX_SIZE/2)-y0)*((-BOX_SIZE/2)-y0)+((-BOX_SIZE/2)-z0+i*BOX_SIZE/(n-1))*((-BOX_SIZE/2)-z0+i*BOX_SIZE/(n-1)));
        A[i*n+j]=x0+(r0*((-BOX_SIZE/2.3)-x0+(j*(BOX_SIZE/(n-1)))))/dist[i*n+j];
        B[i*n+j]=y0+(r0*((-BOX_SIZE/2)-y0))/dist[i*n+j];
        C[i*n+j]=z0+(r0*((-BOX_SIZE/2)-z0+(i*BOX_SIZE/(n-1))))/dist[i*n+j];

        }
    }
}
void intersectionbehind(int n,float x0,float y0,float z0,float r0)
{
    int i=0,j=0;
    float dist[1000];
    for(i=0;i<n;i++)
    {
        for(j=0;j<n;j++)
        {


        dist[i*n+j]=sqrtf(((-BOX_SIZE/2)-x0+j*BOX_SIZE/(n-1))*((-BOX_SIZE/2)-x0+j*BOX_SIZE/(n-1))+((-BOX_SIZE/2)-y0+i*BOX_SIZE/(n-1))*((-BOX_SIZE/2)-y0+i*BOX_SIZE/(n-1))+((-BOX_SIZE/2)-z0)*((-BOX_SIZE/2)-z0));
        A[i*n+j]=x0+(r0*((-BOX_SIZE/2)-x0+(j*(BOX_SIZE/(n-1)))))/dist[i*n+j];
        B[i*n+j]=y0+(r0*((-BOX_SIZE/2)-y0+(i*BOX_SIZE/(n-1))))/dist[i*n+j];
        C[i*n+j]=z0+(r0*((-BOX_SIZE/2)-z0))/dist[i*n+j];

        }
    }
}


void intersectionright(int n,float x0,float y0,float z0,float r0)
{
    int i=0,j=0;
    float dist[1000];
    for(i=0;i<n;i++)
    {
        for(j=0;j<n;j++)
        {


        dist[i*n+j]=sqrtf(((BOX_SIZE/2)-x0)*((BOX_SIZE/2)-x0)+((-BOX_SIZE/2)-y0+j*BOX_SIZE/(n-1))*((-BOX_SIZE/2)-y0+j*BOX_SIZE/(n-1))+((-BOX_SIZE/2)-z0+i*BOX_SIZE/(n-1))*((-BOX_SIZE/2)-z0+i*BOX_SIZE/(n-1)));
        A[i*n+j]=x0+(r0*((BOX_SIZE/2)-x0))/dist[i*n+j];
        B[i*n+j]=y0+(r0*((-BOX_SIZE/1.7)-y0+(j*(BOX_SIZE/(n-1)))))/dist[i*n+j];
        C[i*n+j]=z0+(r0*((-BOX_SIZE/2)-z0+(i*BOX_SIZE/(n-1))))/dist[i*n+j];

        }
    }
}


void intersectiontop(int n,float x0,float y0,float z0,float r0)
{
    int i=0,j=0;
    float dist[1000];
    for(i=0;i<n;i++)
    {
        for(j=0;j<n;j++)
        {


        dist[i*n+j]=sqrtf(((-BOX_SIZE/2)-x0+j*BOX_SIZE/(n-1))*((-BOX_SIZE/2)-x0+j*BOX_SIZE/(n-1))+((BOX_SIZE/2)-y0)*((BOX_SIZE/2)-y0)+((-BOX_SIZE/2)-z0+i*BOX_SIZE/(n-1))*((-BOX_SIZE/2)-z0+i*BOX_SIZE/(n-1)));
        A[i*n+j]=x0+(r0*((-BOX_SIZE/2)-x0+(j*(BOX_SIZE/(n-1)))))/dist[i*n+j];
        B[i*n+j]=y0+(r0*((BOX_SIZE/2)-y0))/dist[i*n+j];
        C[i*n+j]=z0+(r0*((-BOX_SIZE/2)-z0+(i*BOX_SIZE/(n-1))))/dist[i*n+j];

        }
    }
}



void draw1(){

    glPushMatrix();
    //glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

	glTranslatef(0.0f, 0.0f, -BOX_SIZE-2.0f);
	glRotatef(-_angle, 0.0f, 1.0f, 0.0f);
    glTranslatef(0, 0, -cameraDistance);
    glRotatef(cameraAngleX,1,0,0);
    glRotatef(cameraAngleY,0,1,0);
    GLfloat ambientColor[] = {1.0f, 1.0f, 1.0f, 1.0f};
      GLfloat ambientColor1[] = {0.2f, 0.2f, 0.2f, 1.0f};
//	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, ambientColor);

	GLfloat lightColor[] = {0.7f, 0.7f, 0.7f, 1.0f};
	GLfloat lightPos[] = {20.0f, 30.0f, -2.0f, 1.0f};
    GLfloat specular[] = {20.0f, 20.0f, 20.0f, 20.0f};
    GLfloat exp =126;
//	glLightfv(GL_LIGHT0, GL_DIFFUSE, lightColor);
//	glLightfv(GL_LIGHT0, GL_POSITION, lightPos);
//	glLightfv(GL_LIGHT0, GL_SPECULAR, specular);
//	glLightf(GL_LIGHT0, GL_SPOT_EXPONENT, exp);
//	glEnable(GL_LIGHT0);
	//Draw the top and the bottom of the box


    GLfloat mdl[16];


    float camera_org[3];
    glGetFloatv(GL_MODELVIEW_MATRIX, mdl);
    camera_org[0] = -(mdl[0] * mdl[12] + mdl[1] * mdl[13] + mdl[2] * mdl[14]);
    camera_org[1] = -(mdl[4] * mdl[12] + mdl[5] * mdl[13] + mdl[6] * mdl[14]);
    camera_org[2] = -(mdl[8] * mdl[12] + mdl[9] * mdl[13] + mdl[10] * mdl[14]);
	cx=camera_org[0];
	cy=camera_org[1];
	cz=camera_org[2];

    glPushMatrix();
////    GLfloat ambientColor[] = {0.5f, 0.5f, 0.5f, 1.0f};
    GLfloat x[]={1.0f};
    glLightModelfv(GL_LIGHT_MODEL_AMBIENT, ambientColor);
    char str[10];
    itoa(score, str, 10); // 10 - decimal;

    glutPrint(-BOX_SIZE/3,-BOX_SIZE/3,glutFonts[6],str,1.0,1.0,1.0,0.5); //print thr score

//	glLightModelfv(GL_LIGHT_MODEL_TWO_SIDE,x);
//    glEnable(GL_TEXTURE_2D);     //shadow disturbance
////    glDisable(GL_TEXTURE_2D); // comment for textures
//	glBindTexture(GL_TEXTURE_2D, _textureId);
//	glBindTexture(GL_TEXTURE_2D,shadowMapTexture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glColor3f(0.0f, 0.5f, 0.80f);
    glDisable(GL_CULL_FACE);
	glBegin(GL_QUADS);
//    //glPushMatrix();
	glNormal3f(0.0f, 1.0f, 0.0f);
////	glColor3f(0,1,0);
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(-BOX_SIZE / 2, -BOX_SIZE / 2, -BOX_SIZE / 2);
	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(-BOX_SIZE / 2, -BOX_SIZE / 2, BOX_SIZE / 2);
	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(BOX_SIZE / 2, -BOX_SIZE / 2, BOX_SIZE / 2);
	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(BOX_SIZE / 2, -BOX_SIZE / 2, -BOX_SIZE / 2);


    glNormal3f(-1.0f, 0.0f, 0.0f);
	glColor3f(0.0f, 0.0f, 0.0f);
	glTexCoord2f(0.0f, 0.0f);
    glVertex3f(BOX_SIZE / 2, -BOX_SIZE / 2, -BOX_SIZE / 2);
    glTexCoord2f(1.0f, 0.0f);
    glVertex3f(BOX_SIZE / 2, -BOX_SIZE / 2, BOX_SIZE / 2);
    glTexCoord2f(1.0f, 1.0f);
	glVertex3f(BOX_SIZE / 2, BOX_SIZE / 2, BOX_SIZE / 2);
    glTexCoord2f(0.0f, 1.0f);
	glVertex3f(BOX_SIZE / 2, BOX_SIZE / 2, -BOX_SIZE / 2);

	glEnd();
 //   glDisable(GL_TEXTURE_2D);
//
//    glEnable(GL_TEXTURE_2D);
//    glBindTexture(GL_TEXTURE_2D,shadowMapTexture);//changed for texture
//    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_LINEAR);
//    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);


  //  glEnable(GL_TEXTURE_2D);

//    glBegin(GL_QUADS);
//    glNormal3f(0.0f, -1.0f, 0.0f);
////	//glColor3f(0,0,1);
//	glTexCoord2f(0.0f, 0.0f);
//	glVertex3f(-BOX_SIZE / 2, -BOX_SIZE / 2, -BOX_SIZE / 2);
//	glTexCoord2f(1.0f, 0.0f);
//		glVertex3f(-BOX_SIZE / 2, -BOX_SIZE / 2, BOX_SIZE / 2);
//	glTexCoord2f(1.0f, 1.0f);
//	glVertex3f(BOX_SIZE / 2, -BOX_SIZE / 2, BOX_SIZE / 2);
//	glTexCoord2f(0.0f, 1.0f);
//	glVertex3f(BOX_SIZE / 2, -BOX_SIZE / 2, -BOX_SIZE / 2);
//
//    glColor3f(0.0f, 0.0f, 1.0f);
//	glNormal3f(0.0f, -1.0f, 0.0f);
//    glTexCoord2f(1.0f, 0.0f);
//    glVertex3f(-BOX_SIZE / 2, BOX_SIZE / 2, -BOX_SIZE / 2);
//	glTexCoord2f(1.0f, 1.0f);
//	glVertex3f(BOX_SIZE / 2, BOX_SIZE / 2, -BOX_SIZE / 2);
//    glTexCoord2f(0.0f, 1.0f);
//	glVertex3f(BOX_SIZE / 2, BOX_SIZE / 2, BOX_SIZE / 2);
//	glTexCoord2f(0.0f, 0.0f);
//	glVertex3f(-BOX_SIZE / 2, BOX_SIZE / 2, BOX_SIZE / 2);
//
//	glNormal3f(0.0f, 1.0f, 0.0f);
////	glTexCoord2f(1.0f, 0.0f);
//	glVertex3f(-BOX_SIZE / 2, BOX_SIZE / 2, -BOX_SIZE / 2);
////	glTexCoord2f(1.0f, 1.0f);
//	glVertex3f(BOX_SIZE / 2, BOX_SIZE / 2, -BOX_SIZE / 2);
////	glTexCoord2f(0.0f, 1.0f);
//	glVertex3f(BOX_SIZE / 2, BOX_SIZE / 2, BOX_SIZE / 2);
////	glTexCoord2f(0.0f, 0.0f);
//	glVertex3f(-BOX_SIZE / 2, BOX_SIZE / 2, BOX_SIZE / 2);
//
//    glNormal3f(0.0f, 0.0f, 1.0f);
//	glColor3f(1.0f, 1.0f,0.0f);
//	glVertex3f(-BOX_SIZE / 2, BOX_SIZE / 2, -BOX_SIZE / 2);
//	glColor3f(1.0f, 0.0f,0.0f);
//	glVertex3f(BOX_SIZE / 2, BOX_SIZE / 2, -BOX_SIZE / 2);
////	//glColor3f(1.0f, 0.0f,0.0f);
//    glVertex3f(BOX_SIZE / 2, -BOX_SIZE / 2, -BOX_SIZE / 2);
////	//glColor3f(1.0f, 0.0f,0.0f);
//	glVertex3f(-BOX_SIZE / 2, -BOX_SIZE / 2, -BOX_SIZE / 2);
//
//    glNormal3f(0.0f, 0.0f, -1.0f);
//	glColor3f(1.0f, 1.0f,0.0f);
//	glVertex3f(-BOX_SIZE / 2, BOX_SIZE / 2, -BOX_SIZE / 2);
////	//glColor3f(1.0f, 0.0f,0.0f);
//	glVertex3f(BOX_SIZE / 2, BOX_SIZE / 2, -BOX_SIZE / 2);
////	//glColor3f(1.0f, 0.0f,0.0f);
//    glVertex3f(BOX_SIZE / 2, -BOX_SIZE / 2, -BOX_SIZE / 2);
////	//glColor3f(1.0f, 0.0f,0.0f);
//	glVertex3f(-BOX_SIZE / 2, -BOX_SIZE / 2, -BOX_SIZE / 2);
//
//	glEnd();
    glBindTexture(GL_TEXTURE_2D,shadowMapTexture);//changed for texture
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);


	glShadeModel(GL_FLAT);
//	glDisable(GL_TEXTURE_2D);
    glPushMatrix();
////	 glMatrixMode(GL_MODELVIEW);
////    glLoadIdentity();

    glColor4f(0.0f, 0.5f, 0.80f,1.0f);
	glTranslatef(0,-BOX_SIZE/2,0);
		glScalef(1.0f, 0.05f, 1.0f);
        glutSolidCube(BOX_SIZE);

////    glTranslatef(0,BOX_SIZE/2,0);
////    glScalef(1.0f, 2.0f, 1.0f);
    glPopMatrix();

	glPushMatrix();
    glColor4f(0.5f, 0.5f, 0.80f,1.0f);
	glTranslatef(0,BOX_SIZE/2,0);
    glScalef(1.0f, 0.05f, 1.0f);
    glutSolidCube(BOX_SIZE);
    glPopMatrix();

   	glPushMatrix();
    glColor4f(0.5f, 0.0f, 0.80f,1.0f);
	glTranslatef(0,0,-BOX_SIZE/2);
		glRotatef(90,1,0,0);
		glScalef(1.0f, 0.05f, 1.0f);
			glutSolidCube(BOX_SIZE);
    glPopMatrix();

 	glPushMatrix();
    glColor4f(0.1f, 1.0f, 0.20f,1.0f);
	glTranslatef(BOX_SIZE/2,0,0);
		glRotatef(90,0,0,1);
		glScalef(1.0f, 0.05f, 1.0f);
			glutSolidCube(BOX_SIZE);
    glPopMatrix();

    glPopMatrix();


    glLightModelfv(GL_LIGHT_MODEL_AMBIENT, ambientColor1);
    glEnable(GL_CULL_FACE);
	//Draw the balls
	for(unsigned int i = 0; i < _balls.size(); i++) {
		Ball* ball = _balls[i];
		glPushMatrix();

		glTranslatef(ball->pos[0], ball->pos[1], ball->pos[2]);
		glColor4f(ball->color[0], ball->color[1], ball->color[2],0.3);
		glutSolidSphere(ball->r, 50, 50); //Draw a sphere
		glPopMatrix();

    /*    if(i>(_balls.size()/2)){         //uncomment for enviormant mapping

            glEnable(GL_TEXTURE_2D);
            glBindTexture(GL_TEXTURE_2D,_textureId);

            int n=30;
//            intersectionleft(n,ball->pos[0], ball->pos[1], ball->pos[2],ball->r);
//
//		glBegin(GL_QUADS);
        glColor4f(1.0,1.0, 1.0,0.3f);
//        for(int i=0;i<n-1;i++){
//        for(int j=0;j<n-1;j++){
//                glTexCoord2f(0.0+float(j*1/(n-1)), 0.0+float(i*1/(n-1)));
//                glVertex3f(A[i*n+j]-.05*ball->r,B[i*n+j],C[i*n+j]);
//
//                glTexCoord2f(0.25+float(j*1/(n-1)), 0.0+float(i*1/(n-1)));
//                glVertex3f(A[i*n+j+1]-.05*ball->r,B[i*n+j+1],C[i*n+j+1]);
//
//                glTexCoord2f(0.25+float(j*1/(n-1)), 0.25+float(i*1/(n-1)));
//                glVertex3f(A[i*n+j+n+1]-.05*ball->r,B[i*n+j+n+1],C[i*n+j+n+1]);
//
//                glTexCoord2f(0.0+float(j*1/(n-1)), 0.25+float(i*1/(n-1)));
//                glVertex3f(A[i*n+j+n]-.05*ball->r,B[i*n+j+n],C[i*n+j+n]);
//        }}

		intersectionbottom(n,ball->pos[0], ball->pos[1], ball->pos[2],ball->r);

		glBegin(GL_QUADS);
   // glColor4f(0.0f, 0.5f, 0.80f,0.3f);
        for(int i=0;i<n-1;i++){
        for(int j=0;j<n-1;j++){
            glTexCoord2f(0.0+float(j*1/(n-1)), 0.0+float(i*1/(n-1)));
            glVertex3f(A[i*n+j],B[i*n+j]-.1*ball->r,C[i*n+j]);

            glTexCoord2f(0.25+float(j*1/(n-1)), 0.0+float(i*1/(n-1)));
            glVertex3f(A[i*n+j+1],B[i*n+j+1]-.1*ball->r,C[i*n+j+1]);

            glTexCoord2f(0.25+float(j*1/(n-1)), 0.25+float(i*1/(n-1)));
            glVertex3f(A[i*n+j+n+1],B[i*n+j+n+1]-.1*ball->r,C[i*n+j+n+1]);

            glTexCoord2f(0.0+float(j*1/(n-1)), 0.25+float(i*1/(n-1)));
            glVertex3f(A[i*n+j+n],B[i*n+j+n]-.1*ball->r,C[i*n+j+n]);
	}}
        glEnd();
//        intersectionbehind(n,ball->pos[0], ball->pos[1], ball->pos[2],ball->r);
//
//		glBegin(GL_QUADS);
// //   glColor4f(1.0f, 1.0f,0.0f,0.3f);
//    for(int i=0;i<n-2;i++){
//        for(int j=0;j<n-2;j++){
//            glTexCoord2f(0.0+float(j*1/(n-1)), 0.0+float(i*1/(n-1)));
//            glVertex3f(A[i*n+j],B[i*n+j],C[i*n+j]-.05*ball->r);
//
//            glTexCoord2f(0.25+float(j*1/(n-1)), 0.0+float(i*1/(n-1)));
//            glVertex3f(A[i*n+j+1],B[i*n+j+1],C[i*n+j+1]-.05*ball->r);
//
//            glTexCoord2f(0.25+float(j*1/(n-1)), 0.25+float(i*1/(n-1)));
//        glVertex3f(A[i*n+j+n+1],B[i*n+j+n+1],C[i*n+j+n+1]-.05*ball->r);
//
//        glTexCoord2f(0.0+float(j*1/(n-1)), 0.25+float(i*1/(n-1)));
//        glVertex3f(A[i*n+j+n],B[i*n+j+n],C[i*n+j+n]-.05*ball->r);
//	}}
    intersectionright(n,ball->pos[0], ball->pos[1], ball->pos[2],ball->r);

		glBegin(GL_QUADS);
 //   glColor4f(0.1f, 1.0f, 0.20f,0.3f);
    for(int i=0;i<n-1;i++){
        for(int j=0;j<n-1;j++){
            glTexCoord2f(0.0+float(j*1/(n-1)), 0.0+float(i*1/(n-1)));
            glVertex3f(A[i*n+j]+.05*ball->r,B[i*n+j],C[i*n+j]);

            glTexCoord2f(0.25+float(j*1/(n-1)), 0.0+float(i*1/(n-1)));
            glVertex3f(A[i*n+j+1]+.05*ball->r,B[i*n+j+1],C[i*n+j+1]);

            glTexCoord2f(0.25+float(j*1/(n-1)), 0.25+float(i*1/(n-1)));
            glVertex3f(A[i*n+j+n+1]+.05*ball->r,B[i*n+j+n+1],C[i*n+j+n+1]);

            glTexCoord2f(0.0+float(j*1/(n-1)), 0.25+float(i*1/(n-1)));
            glVertex3f(A[i*n+j+n]+.05*ball->r,B[i*n+j+n],C[i*n+j+n]);
	}}
        glEnd();
//    intersectiontop(n,ball->pos[0], ball->pos[1], ball->pos[2],ball->r);

//		glBegin(GL_QUADS);
//   // glColor4f(0.5f, 0.5f, 0.80f,0.3f);
//    for(int i=0;i<n-2;i++){
//        for(int j=0;j<n-2;j++){
//            glTexCoord2f(0.0+float(j*1/(n-1)), 0.0+float(i*1/(n-1)));
//            glVertex3f(A[i*n+j],B[i*n+j],C[i*n+j]);
//
//            glTexCoord2f(0.25+float(j*1/(n-1)), 0.0+float(i*1/(n-1)));
//            glVertex3f(A[i*n+j+1],B[i*n+j+1],C[i*n+j+1]);
//
//            glTexCoord2f(0.25+float(j*1/(n-1)), 0.25+float(i*1/(n-1)));
//            glVertex3f(A[i*n+j+n+1],B[i*n+j+n+1],C[i*n+j+n+1]);
//
//            glTexCoord2f(0.0+float(j*1/(n-1)), 0.25+float(i*1/(n-1)));
//            glVertex3f(A[i*n+j+n],B[i*n+j+n],C[i*n+j+n]);
//	}}
	glEnd();
        }
        */


	glBindTexture(GL_TEXTURE_2D,shadowMapTexture);

	}
//	glutPrint(1.0,1.0,glutFonts[6],"hello",1.0,1.0,1.0,0.5);
//	glEnable(GL_CULL_FACE);
	glPopMatrix();
	}
void drawScene() {
//string lightProjectionMatri,lightViewMatri,cameraProjectionMatri,cameraViewMatri;
  // cout<<cx<<"     "<< cy <<"  "<< cz <<"   \n";
//    printM(lightProjectionMatrix,"lightProjectionMatri");
//    printM(lightViewMatrix,"lightViewMatri");
//    printM(cameraProjectionMatrix,"cameraProjectionMatri");
//    printM(cameraViewMatrix,"cameraViewMatri");
//
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glShadeModel(GL_SMOOTH);
////	glPushMatrix();
//
        glClear(GL_DEPTH_BUFFER_BIT);
        glMatrixMode(GL_PROJECTION);
        glLoadMatrixf(lightProjectionMatrix);

        glMatrixMode(GL_MODELVIEW);
        glLoadMatrixf(lightViewMatrix);

//        	Use viewport the same size as the shadow map
        glViewport(0, 0, shadowMapSize, shadowMapSize);
//
////        Draw back faces into the shadow map
        glCullFace(GL_FRONT);

//        Disable color writes, and use flat shading for speed
        glShadeModel(GL_SMOOTH);
            glColorMask(0, 0, 0, 0);
        draw1();
   //     glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, shadowMapTexture);
        glCopyTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, 0, 0, shadowMapSize, shadowMapSize);
//
//	//restore states
        glCullFace(GL_BACK);
        glShadeModel(GL_SMOOTH);
        glColorMask(1, 1, 1, 1);
////
////    glPopMatrix();
//
//
//////	2nd pass - Draw from camera's point of view
       glClear(GL_DEPTH_BUFFER_BIT);
//
        glMatrixMode(GL_PROJECTION);
        glLoadMatrixf(cameraProjectionMatrix);
     glCullFace(GL_BACK);
        glMatrixMode(GL_MODELVIEW);
        glLoadMatrixf(cameraViewMatrix);
//////
//
////        glLoadIdentity();
            glViewport(0, 0, windowWidth, windowHeight);
////
//	//Use dim light to represent shadowed areas
        glLightfv(GL_LIGHT1, GL_POSITION, VECTOR4D(lightPosition));
        glLightfv(GL_LIGHT1, GL_AMBIENT, white*.3);
        glLightfv(GL_LIGHT1, GL_DIFFUSE, white*.3);
        glLightfv(GL_LIGHT1, GL_SPECULAR, black);
        glEnable(GL_LIGHT1);
        glEnable(GL_LIGHTING);

        draw1();

//        //3rd
   //     glEnable(GL_TEXTURE_2D);
        glMatrixMode(GL_PROJECTION);
        glLoadMatrixf(cameraProjectionMatrix);

        glMatrixMode(GL_MODELVIEW);
        glLoadMatrixf(cameraViewMatrix);

        glLightfv(GL_LIGHT1, GL_DIFFUSE, white);
        glLightfv(GL_LIGHT1, GL_SPECULAR, white);

	//Calculate texture matrix for projection
	//This matrix takes us from eye space to the light's clip space
	//It is postmultiplied by the inverse of the current view matrix when specifying texgen
        static MATRIX4X4 biasMatrix(0.5f, 0.0f, 0.0f, 0.0f,
								0.0f, 0.5f, 0.0f, 0.0f,
								0.0f, 0.0f, 0.5f, 0.0f,
								0.5f, 0.5f, 0.5f, 1.0f);	//bias from [-1, 1] to [0, 1]
        MATRIX4X4 textureMatrix=biasMatrix*lightProjectionMatrix*lightViewMatrix;

	//Set up texture coordinate generation.
        glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_EYE_LINEAR);
        glTexGenfv(GL_S, GL_EYE_PLANE, textureMatrix.GetRow(0));
        glEnable(GL_TEXTURE_GEN_S);

            glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_EYE_LINEAR);
        glTexGenfv(GL_T, GL_EYE_PLANE, textureMatrix.GetRow(1));
        glEnable(GL_TEXTURE_GEN_T);

        glTexGeni(GL_R, GL_TEXTURE_GEN_MODE, GL_EYE_LINEAR);
        glTexGenfv(GL_R, GL_EYE_PLANE, textureMatrix.GetRow(2));
        glEnable(GL_TEXTURE_GEN_R);

        glTexGeni(GL_Q, GL_TEXTURE_GEN_MODE, GL_EYE_LINEAR);
        glTexGenfv(GL_Q, GL_EYE_PLANE, textureMatrix.GetRow(3));
        glEnable(GL_TEXTURE_GEN_Q);

	//Bind & enable shadow map texture
        glBindTexture(GL_TEXTURE_2D, shadowMapTexture);
        glEnable(GL_TEXTURE_2D);

	//Enable shadow comparison
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE_ARB, GL_COMPARE_R_TO_TEXTURE);

	//Shadow comparison should be true (ie not in shadow) if r<=texture
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC_ARB, GL_LEQUAL);

	//Shadow comparison should generate an INTENSITY result
        glTexParameteri(GL_TEXTURE_2D, GL_DEPTH_TEXTURE_MODE_ARB, GL_INTENSITY);
//
//	//Set alpha test to discard false comparisons
        glAlphaFunc(GL_GEQUAL, 0.2f);
        glEnable(GL_ALPHA_TEST);
        draw1();


        glDisable(GL_TEXTURE_2D);
        glDisable(GL_TEXTURE_GEN_S);
        glDisable(GL_TEXTURE_GEN_T);
        glDisable(GL_TEXTURE_GEN_R);
        glDisable(GL_TEXTURE_GEN_Q);

//	//Restore other states
        glDisable(GL_LIGHTING);
        glDisable(GL_ALPHA_TEST);
//
    	glMatrixMode(GL_PROJECTION);
        glPushMatrix();
        glLoadIdentity();
        gluOrtho2D(-1.0f, 1.0f, -1.0f, 1.0f);

        glMatrixMode(GL_MODELVIEW);
        glPushMatrix();
        glLoadIdentity();
//
//	//Print text
////	glRasterPos2f(-1.0f, 0.9f);
////	for(unsigned int i=0; i<strlen(fpsString); ++i)
////		glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, fpsString[i]);
//
//	//reset matrices
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        glPopMatrix();
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();
        glPopMatrix();
//   // glPopMatrix();




    timet2= glutGet(GLUT_ELAPSED_TIME);
   // cout<<"time:"<<timet<<"\n";
    if((timet2-timet1)>20000) endprog();
    glFinish();
	glutSwapBuffers();
	glutPostRedisplay();
}

//Called every TIMER_MS milliseconds
void update(int value) {
	advance(_balls, _octree, (float)TIMER_MS / 1000.0f, _timeUntilUpdate);
//	_angle += (float)TIMER_MS / 100;     // UNCOMMENT FOR ROTATION
	if (_angle > 360) {
		_angle -= 360;
	}

	glutPostRedisplay();
	glutTimerFunc(TIMER_MS, update, 0);
}

int main(int argc, char** argv) {
	srand((unsigned int)time(0)); //Seed the random number generator

	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowSize(400, 400);

	glutCreateWindow("bubble");
	initRendering();
    cout<<"PRESS SPACE TO START AND PRESS T TO SET DIFFICULTY(BETWEEN O & 4)";
	_octree = new Octree(Vec3f(-BOX_SIZE / 2, -BOX_SIZE / 2, -BOX_SIZE / 2),
						 Vec3f(BOX_SIZE / 2, BOX_SIZE / 2, BOX_SIZE / 2), 1);

	glutDisplayFunc(drawScene);
	glutKeyboardFunc(handleKeypress);
	glutReshapeFunc(handleResize);
    glutMouseFunc(mouseCB);
    glutMotionFunc(mouseMotionCB);
	glutTimerFunc(TIMER_MS, update, 0);

	glutMainLoop();
	return 0;
}



void mouseCB(int button, int state, int x, int y)
{
    mouseX = x;
    mouseY = y;

    if(button == GLUT_LEFT_BUTTON)
    {
        if(state == GLUT_DOWN)
        {
            mouseLeftDown = true;
        }
        else if(state == GLUT_UP)
            mouseLeftDown = false;
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
    else if(button == GLUT_MIDDLE_BUTTON)
    {
        if(state == GLUT_DOWN)
        {
            mouseMiddleDown = true;
              bool p=false;
            p= BubblePop(mouseX,mouseY);
       //     cout<<"the vaule of a p u is ~~~~~  " <<p<< " \n ";
            if (p) {updateScore();
            Ball* b1=_balls[popno];
            _octree->remove(b1);
            vector<Ball*>::iterator it1 = _balls.begin();
            it1=it1+popno;

            _balls.erase(it1);
            //cout<<"check :"<<mouseX<<"\n";
 //           cout<<"pos x:"<<b1->pos[0]<<"pos y:"<<b1->pos[1]<<"\n";
            glutPrint(b1->pos[0],b1->pos[1],glutFonts[5],"pop",1.0,0.0,1.0,0.5);
            glutSwapBuffers();
     //       draw1();
            }
        }
        else if(state == GLUT_UP)
            mouseMiddleDown = false;
    }
}


void mouseMotionCB(int x, int y)
{
    if(mouseLeftDown)
    {
        cameraAngleY += (x - mouseX);
        cameraAngleX += (y - mouseY);
        mouseX = x;
        mouseY = y;

    }

    if(mouseRightDown)
    {
        cameraDistance -= (y - mouseY) * 0.2f;
        mouseY = y;
    }

    if (mouseMiddleDown)
    {

    }
}





