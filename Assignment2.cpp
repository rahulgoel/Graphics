

#include <iostream>
#include <GL/glut.h>
#include <math.h>
#include <string.h>
#include <stack>
#include <stdio.h>
#include <fstream>
#include <sstream>

using namespace std;


int originx,originy=0;
double interpol[] ={0,0,0};
enum pnamel {Ambient,Specular,Position,Diffuse};
enum light{light0,light1,light2,light3,light4};
enum pname {AmbientD,SpecularD,ShininessD,DiffuseD};
enum pname2{front,frontback};
int polygon=0;
#define checkImageWidth 640
#define checkImageHeight 640
GLubyte checkImage[checkImageHeight][checkImageWidth][3];




 float ambient[]={0,0,0};
float diffuse[]={0,0,0};
float position[]={0,0,0};
float specular[]={0,0,0};
//void setambient(const GLfloat *params);
//void setdiffuse(const GLfloat *params);
//void setposition(const GLfloat *params);
//void setspecular(const GLfloat *params);
//

bool InvertMatrix(const double m[16], double invOut[16])
{
    double inv[16], det;
    int i;

    inv[0] = m[5]  * m[10] * m[15] -
             m[5]  * m[11] * m[14] -
             m[9]  * m[6]  * m[15] +
             m[9]  * m[7]  * m[14] +
             m[13] * m[6]  * m[11] -
             m[13] * m[7]  * m[10];

    inv[4] = -m[4]  * m[10] * m[15] +
              m[4]  * m[11] * m[14] +
              m[8]  * m[6]  * m[15] -
              m[8]  * m[7]  * m[14] -
              m[12] * m[6]  * m[11] +
              m[12] * m[7]  * m[10];

    inv[8] = m[4]  * m[9] * m[15] -
             m[4]  * m[11] * m[13] -
             m[8]  * m[5] * m[15] +
             m[8]  * m[7] * m[13] +
             m[12] * m[5] * m[11] -
             m[12] * m[7] * m[9];

    inv[12] = -m[4]  * m[9] * m[14] +
               m[4]  * m[10] * m[13] +
               m[8]  * m[5] * m[14] -
               m[8]  * m[6] * m[13] -
               m[12] * m[5] * m[10] +
               m[12] * m[6] * m[9];

    inv[1] = -m[1]  * m[10] * m[15] +
              m[1]  * m[11] * m[14] +
              m[9]  * m[2] * m[15] -
              m[9]  * m[3] * m[14] -
              m[13] * m[2] * m[11] +
              m[13] * m[3] * m[10];

    inv[5] = m[0]  * m[10] * m[15] -
             m[0]  * m[11] * m[14] -
             m[8]  * m[2] * m[15] +
             m[8]  * m[3] * m[14] +
             m[12] * m[2] * m[11] -
             m[12] * m[3] * m[10];

    inv[9] = -m[0]  * m[9] * m[15] +
              m[0]  * m[11] * m[13] +
              m[8]  * m[1] * m[15] -
              m[8]  * m[3] * m[13] -
              m[12] * m[1] * m[11] +
              m[12] * m[3] * m[9];

    inv[13] = m[0]  * m[9] * m[14] -
              m[0]  * m[10] * m[13] -
              m[8]  * m[1] * m[14] +
              m[8]  * m[2] * m[13] +
              m[12] * m[1] * m[10] -
              m[12] * m[2] * m[9];

    inv[2] = m[1]  * m[6] * m[15] -
             m[1]  * m[7] * m[14] -
             m[5]  * m[2] * m[15] +
             m[5]  * m[3] * m[14] +
             m[13] * m[2] * m[7] -
             m[13] * m[3] * m[6];

    inv[6] = -m[0]  * m[6] * m[15] +
              m[0]  * m[7] * m[14] +
              m[4]  * m[2] * m[15] -
              m[4]  * m[3] * m[14] -
              m[12] * m[2] * m[7] +
              m[12] * m[3] * m[6];

    inv[10] = m[0]  * m[5] * m[15] -
              m[0]  * m[7] * m[13] -
              m[4]  * m[1] * m[15] +
              m[4]  * m[3] * m[13] +
              m[12] * m[1] * m[7] -
              m[12] * m[3] * m[5];

    inv[14] = -m[0]  * m[5] * m[14] +
               m[0]  * m[6] * m[13] +
               m[4]  * m[1] * m[14] -
               m[4]  * m[2] * m[13] -
               m[12] * m[1] * m[6] +
               m[12] * m[2] * m[5];

    inv[3] = -m[1] * m[6] * m[11] +
              m[1] * m[7] * m[10] +
              m[5] * m[2] * m[11] -
              m[5] * m[3] * m[10] -
              m[9] * m[2] * m[7] +
              m[9] * m[3] * m[6];

    inv[7] = m[0] * m[6] * m[11] -
             m[0] * m[7] * m[10] -
             m[4] * m[2] * m[11] +
             m[4] * m[3] * m[10] +
             m[8] * m[2] * m[7] -
             m[8] * m[3] * m[6];

    inv[11] = -m[0] * m[5] * m[11] +
               m[0] * m[7] * m[9] +
               m[4] * m[1] * m[11] -
               m[4] * m[3] * m[9] -
               m[8] * m[1] * m[7] +
               m[8] * m[3] * m[5];

    inv[15] = m[0] * m[5] * m[10] -
              m[0] * m[6] * m[9] -
              m[4] * m[1] * m[10] +
              m[4] * m[2] * m[9] +
              m[8] * m[1] * m[6] -
              m[8] * m[2] * m[5];

    det = m[0] * inv[0] + m[1] * inv[4] + m[2] * inv[8] + m[3] * inv[12];

    if (det == 0)
        return false;

    det = 1.0 / det;

    for (i = 0; i < 16; i++)
        invOut[i] = inv[i] * det;

    return true;
}
void setambient(const GLfloat *  params){
float a1,a2,a3;
a1=  *(params);
a2 = *(params+1);
a3 = *(params+2);
cout<<"DEDDE"<<a1<<a2<<a3;

ambient[0]=a1;
ambient[1]=a2;
ambient[2]=a3;
}
void setdiffuse(const GLfloat *  params){
float a1,a2,a3;
a1=  *(params);
a2 = *(params+1);
a3 = *(params+2);
diffuse[0]=a1;
diffuse[1]=a2;
diffuse[2]=a3;
}
void setposition(const GLfloat *  params){
float a1,a2,a3;
a1=  *(params);
a2 = *(params+1);
a3 = *(params+2);
position[0]=a1;
position[1]=a2;
position[2]=a3;
}
void setspecular(const GLfloat *  params){
float a1,a2,a3;
a1=  *(params);
a2 = *(params+1);
a3 = *(params+2);
specular[0]=a1;
specular[1]=a2;
specular[2]=a3;
}



float AmbientDA[]={1,1,1};
float DiffuseDA[]={1,1,1};
float SpecularDA[]={1,1,1};
float ShininessDA=1;
void setAmbient(const GLfloat *  params){
float a1,a2,a3;
a1=  *(params);
a2 = *(params+1);
a3 = *(params+2);
//cout<<"DEDDE"<<a1<<a2<<a3;
AmbientDA[0]=a1;
AmbientDA[1]=a2;
AmbientDA[2]=a3;
}
void setDiffuse(const GLfloat *  params){
float a1,a2,a3;
a1=  *(params);
a2 = *(params+1);
a3 = *(params+2);
DiffuseDA[0]=a1;
DiffuseDA[1]=a2;
DiffuseDA[2]=a3;
}
void setSpecular(const GLfloat *  params){
float a1,a2,a3;
a1=  *(params);
a2 = *(params+1);
a3 = *(params+2);
SpecularDA[0]=a1;
SpecularDA[1]=a2;
SpecularDA[2]=a3;
}
void setShininess(const GLfloat *  params){
float a1,a2,a3;
a1=  *(params);

ShininessDA=a1;

}
 void myglmaterial(GLenum  face,  GLenum  pname,  const GLfloat *  params){
 if(pname==AmbientD) setAmbient(params);
 else if(pname==DiffuseD) setDiffuse(params);
 else if (pname==SpecularD) setSpecular(params);
 else if(pname == ShininessD) setShininess(params);
 else cout << "invalid";
 }
void mygllight(GLenum  light,  GLenum  pnamel,  const GLfloat *  params){
if(light==light0){
    if(pnamel==Ambient) setambient(params);
   else if(pnamel==Position) setposition(params);
   else if(pnamel==Diffuse) setdiffuse(params);
   else if(pnamel==Specular) setspecular(params);
   else cout <<"err";
}
//else if(light==light1){
//    if(pnamel==Ambient) l1.setambient(params);
//   else if(pnamel==Position) l1.setposition(params);
//   else if(pnamel==Diffuse) l1.setdiffuse(params);
//   else if(pnamel==Specular) l1.setspecular(params);
//   else cout <<"err";
//}
//else if(light== light2){
//    if(pnamel==Ambient) l2.setambient(params);
//   else if(pnamel==Position) l2.setposition(params);
//   else if(pnamel==Diffuse) l2.setdiffuse(params);
//   else if(pnamel==Specular) l2.setspecular(params);
//   else cout <<"err";
//}
//else if(light==light3){
//    if(pnamel==Ambient) l3.setambient(params);
//   else if(pnamel==Position) l3.setposition(params);
//   else if(pnamel==Diffuse) l3.setdiffuse(params);
//   else if(pnamel==Specular) l3.setspecular(params);
//   else cout <<"err";
//}
//else if(light== light4){
//    if(pnamel==Ambient) l4.setambient(params);
//   else if(pnamel==Position) l4.setposition(params);
//   else if(pnamel==Diffuse) l4.setdiffuse(params);
//   else if(pnamel==Specular) l4.setspecular(params);
//   else cout <<"err";
//}
else cout<<"invalid";
}

void myglFlush(void){
glDrawBuffer(GL_FRONT_AND_BACK);
}

void makeCheckImage(void)
{
   int i, j, c;

   for (i = 0; i < checkImageHeight; i++) {
      for (j = 0; j < checkImageWidth; j++) {
       //  c = ((((i&0x8)==0)^((j&0x8))==0))*255;
         checkImage[i][j][0] = 0;
         checkImage[i][j][1] = 0;
         checkImage[i][j][2] = 0;
      }
   }

}
bool addvertex=false;

void myglbegin(string mode){
    if (mode == "triangles")
        addvertex =true;
    else
        cout<<"invalid mode";
}

float arrayv[3][5] ;
int vertexcount=0;
int n1=0;int n2 =0; int n3 =1;
void myglNormal3f(GLfloat  x,  GLfloat  y,  GLfloat  z)
{
    n1=x;
    n2=y;
    n3=z;
}



void myglvertex3fv(const GLfloat *v){
if(addvertex==true) {
arrayv[vertexcount][0] = *v;
arrayv[vertexcount][1] = *(v+1);
arrayv[vertexcount][2] = *(v+2);
arrayv[vertexcount][3] = n1;
arrayv[vertexcount][4] = n2;
arrayv[vertexcount][5] = n3;
vertexcount++;
}
}
void myglvertex3f(GLfloat  x,  GLfloat  y,  GLfloat  z){
if(addvertex==true) {
arrayv[vertexcount][0] = x;
arrayv[vertexcount][1] = y;
arrayv[vertexcount][2] = z;
arrayv[vertexcount][3] = n1;
arrayv[vertexcount][4] = n2;
arrayv[vertexcount][5] = n3;
vertexcount++;
}


}
void interpolate(int x1, int y1, int x2 , int y2 , int x3 , int y3 , int xq , int yq){
double a,b,c,d,l,m,n,k,p,D,q,r,s,x,y,z;
a=1; b=1; c=1; d=-1;
l=x1;m=x2;n=x3;k=-1*xq;
p=y1; q= y2; r = y3 ; s= -1*yq;
//ax+by+cz+d=0q
//lx+my+nz+k=0
//px+qy+rz+s=0

 D = (a*m*r+b*p*n+c*l*q)-(a*n*q+b*l*r+c*m*p);
 x = ((b*r*k+c*m*s+d*n*q)-(b*n*s+c*q*k+d*m*r))/D;
 y = ((a*n*s+c*p*k+d*l*r)-(a*r*k+c*l*s+d*n*p))/D;
 z = ((a*q*k+b*l*s+d*m*p)-(a*m*s+b*p*k+d*l*q))/D;

 interpol[0]=x;
 interpol[1]=y;
 interpol[2]=z;
}

ofstream myfile ("log.txt");
void WriteLogFile(string szString)
{

  if (myfile.is_open())
  {
    myfile << szString << "\n";

    //myfile.close();
  }
  else cout << "Unable to open file";

}


void drawTriangle(float array[][5]){
    int vxmax=0; int vx2=0;int vxmin=0;int vymax=0; int vy2=0;int vymin=0;
    int i1=0;float x1,y1,z1,x2,y2,z2,x3,y3,z3;
    float nx1,nx2,nx3,ny1,ny2,ny3,nz1,nz2,nz3;
        x1=array[i1][0];
    y1=array[i1][1];
    z1=array[i1][2];
    nx1=array[i1][3];
    ny1=array[i1][4];
    nz1=array[i1][5];
    x2=array[i1+1][0];
    y2=array[i1+1][1];
    z2=array[i1+1][2];

    nx2=array[i1+1][3];
    ny2=array[i1+1][4];
    nz2=array[i1+1][5];

    x3=array[i1+2][0];
    y3=array[i1+2][1];
    z3=array[i1+2][2];


    nx3=array[i1+2][3];
    ny3=array[i1+2][4];
    nz3=array[i1+2][5];

    x1=x1+originx;
    x2=x2+originx;
    x3=x3+originx;
    y1=y1+originy;
    y2=y2+originy;
    y3=y3+originy;


    if(x1 >= x2 && x1 >= x3 ){
     if (x2 > x3){
        vxmax= x1;
        vx2=x2;
        vxmin=x3;
        }
     else{ vxmax=x1;
        vxmin=x2;
        vx2=x3;
        }
    }
    else if(x2 >= x1 && x2 >= x3) {
        if (x1 > x3){
            vx2=x1;
            vxmax=x2;
            vxmin=x3;
        }
        else{ vxmin=x1;
            vx2=x2;
            vxmax=x3;
        }
    }
    else {
        if(x1>x2){
            vxmin=x2;
            vx2=x1;
            vxmax=x3;}

        else{vx2=x2;
            vxmin=x1;
            vxmax=x3;
        }

    }

 if(y1 >= y2 && y1 >= y3 ){
     if (y2 > y3){
        vymax=y1;
        vy2=y2;
        vymin=y3;
        }
     else{ vymax=y1;
        vymin=y2;
        vy2=y3;
        }
 }
    else if(y2 >= y1 && y2 >= y3) {
        if (y1 > y3){
            vy2=y1;
            vymax=y2;
            vymin=y3;
        }
        else{ vymin=y1;
            vy2=y3;
            vymax=y2;
        }
    }
    else {
        if(y1>y2){
            vymin=y2;
            vy2=y1;
            vymax=y3;}

        else{vy2=y2;
            vymin=y1;
            vymax=y3;
        }

    }

int a1,b1,c1,a2,b2,c2,a3,b3,c3;
a1=y1-y2;
b1=x2-x1;
c1=x1*(y2-y1)-y1*(x2-x1);

a2=y2-y3;
b2=x3-x2;
c2=x2*(y3-y2)-y2*(x3-x2);

a3=y3-y1;
b3=x1-x3;
c3=x3*(y1-y3)-y3*(x1-x3);
int chk1; int chk2 ; int chk3 ;

chk1= a1*x3+b1*y3+c1;
chk2= a2*x1+b2*y1+c2;
chk3= a3*x2+b2*y1+c3;


if(chk1<0){
    a1=a1*-1;
    b1=b1*-1;
    c1=c1*-1;}
if(chk2<0){
    a2=a2*-1;
    b2=b2*-1;
    c2=c2*-1;}
if(chk3<0){
    a3=a3*-1;
    b3=b3*-1;
    c3=c3*-1;}
int i ,j;
stack<string> s1;
//int z = (vymax-vymin)*(vxmax-vxmin);
//z=z+100000;
//float arrayL[z];
cout << "a1 : "<< a1 <<" b1 :" << b1 <<" c1 :"<< c1<< "\n a2: "<< a2<< "b2:" << b2 << "c2:" << c2 << " \n a3 :"<< a3 << " b2 :" << b2 << "c3 : "<< c3;
cout << " \n vymin :" << vymin << "vymax :" << vymax << "vxmin:"<< vxmin << "vxmin :"<< vxmax;
cout << " x1 ,y1 :"<< x1 << ","<< y1 << "\n x2,y2 :" << x2 << "," << y2 << " \n x3 , y3" << x3 << ", "<< y3;
for (j = vymin; j <= vymax; j++) {
for (i = vxmin; i <= vxmax; i++) {
int e0 = a1*i + b1*j + c1;
int e1 = a2*i + b2*j + c2;
int e2 = a3*i + b3*j + c3;
//cout << "e0 :" << e0 << "e1 :"<< e1 << " e2  :"<< e2<< "j"<< j<< "\n";
if (e0 >= 0 && e1 >= 0 && e2 >= 0){
double in1,in2,in3,zin,nxin,nyin,nzin,r,g,b,d,dl;
double lightx,lighty,lightz;
double v1,v2,v3,v4,v5,v6;
double lightxn,lightyn,lightzn;
double nxinn,nyinn,nzinn,dot;
double sx,sy,sz,ds,dots;
int red,blue, green;
double amb1,diff1,spec1,amb2,diff2,spec2,amb3,diff3,spec3;

interpolate(x1,y1,x2,y2,x3,y3,i,j);
in1=interpol[0];
in2=interpol[1];
in3=interpol[2];
zin=z1*in1+z2*in2+z3*in3;
nxin=nx1*in1+nx2*in2+nx3*in3;
nyin=ny1*in1+ny2*in2+ny3*in3;
nzin=nz1*in1+nz2*in2+nz3*in3;
d=sqrt(nxin*nxin+nyin*nyin+nzin*nzin);
nxinn=nxin/d;
nyinn=nyin/d;
nzinn=nzin/d;

lightx=position[0]-i;
lighty=position[1]-j;
lightz=position[2]-zin;
dl=sqrt(lightx*lightx+lighty*lighty+lightz*lightz);
lightxn=lightx/dl;
lightyn=lighty/dl;
lightzn=lightz/dl;
dot=nxinn*lightxn+nyinn*lightyn+nzinn*lightzn;
if(dot<0)dot=0;
sx=lightx;
sy=lighty;
sz=lightz+1;
ds=sqrt(sx*sx+sy*sy+sz*sz);
sx=sx/ds;
sy=sy/ds;
sz=sz/ds;
dots=sx*lightxn+sy*lightyn+sz*lightzn;
if(dots<0)dots=0;
amb1=ambient[0]*AmbientDA[0];
amb2=ambient[1]*AmbientDA[1];
amb3=ambient[2]*AmbientDA[2];
diff1=dot*diffuse[0]*DiffuseDA[0];
diff2=dot*diffuse[1]*DiffuseDA[1];
diff3=dot*diffuse[2]*DiffuseDA[2];
spec1=pow(dots,ShininessDA)*specular[0]*SpecularDA[0];
spec2=pow(dots,ShininessDA)*specular[1]*SpecularDA[1];
spec3=pow(dots,ShininessDA)*specular[2]*SpecularDA[2];
r=amb1 + diff1+ spec1;
g=amb2 + diff2+ spec2;
b=amb3 + diff3+ spec3;
stringstream Print;
string gg;
Print  << i <<" "<<  j<<" ," <<  polygon<<" ," <<  nxinn <<" ,"<<  nyinn <<" ,"<<  nzinn  <<" amb red"<<amb1<<" amb blue" <<  amb2<<" amb green" << amb3<<"diff red :" <<  diff1<<" ," << diff2 <<" ," <<  diff3 <<" ,"<< spec1<<" ,"  << spec2 <<" ," << spec3<< " zed "<< zin;
//  Print << "amb red lihjt : "<< l0.ambient[0] <<"amb green l:" << l0.ambient[1] << "amb blue l :"<< l0.ambient[2];
  //Print << " lihjt pott: "<< position[0] <<"amb green l:" << position[1] << "amb blue l :"<< position[2];
s1.push(Print.str());
//delete Print;
WriteLogFile(Print.str());

red=(int)(r*255/3);
green=(int)(g*255/3);
blue=(int)(b*255/3);
//cout << "red " << red << " blue :" << blue << " green :"<< green;
checkImage[i][j][0] = red;
checkImage[i][j][1] = green;
checkImage[i][j][2] = blue;

}
}
}
}



void myglend(){
addvertex=false;
//float[][] *p;
//*p= arrayv;
int i,x1,y1,z1,x2,y2,z2,x3,y3,z3;
//for(i=0;i<vertexcount;i=i+3){
//    *p=arrayv+i;
//  //  drawTriangle(2,3,4,5,6,7);
    drawTriangle(arrayv );
    polygon++;
    myfile.close();


vertexcount =0;
}



void origin(int x, int y){
    originx=x;
    originy=y;
}
void drawline(int x,int y , int z , int a){
float slope;
slope = (z-x)/(a-y);
int w= z-x;
int i,j;
int linex,liney;
linex=originx+x;
liney=originy+y;
for(i=0;i<w;i++){
j=round(i*slope);
checkImage[linex+i][liney+j][0]=67;
}
}

void init(void)
{
   glClearColor (0.0, 1.0, 0.0, 0.0);
   //glShadeModel(GL_FLAT);
   makeCheckImage();
   glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
}
void display(void)
{
   glClear(GL_COLOR_BUFFER_BIT);
   glRasterPos2i(0, 0);
   float am[]={.1,.1,.1};
   mygllight(light0,Ambient,am);
   float pos[]={10,10,100};
   mygllight(light0,Position,pos);
     float dif[]={.5,.8,1};
   mygllight(light0,Diffuse,dif);
    float spec[]={.9,1,1};
   mygllight(light0,Specular,spec);


    float dam[]={.6,.3,.9};
    myglmaterial(front,AmbientD,dam);
     float diff[]={.1,.3,.9};
    myglmaterial(front,DiffuseD,diff);
     float spec1[]={.6,.3,.9};
    myglmaterial(front,SpecularD,spec1);

     float shin[]={10};
    myglmaterial(front,ShininessD,shin);

    string s = "Log File";
    //ptr *p=&s;
    WriteLogFile(s);

 //  drawline(23,56,234,234);
 //  drawTriangle(100,100,10,220,200,200);
   myglbegin("triangles");
   myglNormal3f(1,2,3);
   myglvertex3f(20,20,10);
  myglNormal3f(1,1,1);
   myglvertex3f(350,455,10);
   myglNormal3f(4,2,3);
    myglvertex3f(555,600,10);
    myglend();

   glDrawPixels(checkImageWidth, checkImageHeight, GL_RGB,
                GL_UNSIGNED_BYTE, checkImage);

   myglFlush();
    glFlush();
 //  glutSwapBuffers();
 //  glFinish();
}
//
//void setupMaterials() {
//	glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient);
//	glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
//	glMaterialfv(GL_FRONT, GL_SHININESS, mat_shininess);
//	glLightfv(GL_LIGHT0, GL_POSITION, light_position);
//	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, model_ambient);
//}

//void changeColour(GLfloat r, GLfloat g, GLfloat b, GLfloat A) {
//	model_ambient[0] = r;
//	model_ambient[1] = g;
//	model_ambient[2] = b;
//	model_ambient[3] = A;
//	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, model_ambient);
//}

//void init(void) {
//	glClearColor(0.0, 0.0, 0.0, 1.0);
//	setupMaterials();
//
//	glEnable(GL_LIGHTING);
//	glEnable(GL_LIGHT0);
//	glEnable(GL_DEPTH_TEST);
//	glEnable(GL_CULL_FACE);
//	glFrontFace(GL_CCW);
//	glShadeModel(GL_SMOOTH);
//	  makeCheckImage();
//   glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
//}
void myglViewport(GLint  x,  GLint  y,  GLsizei  width,  GLsizei  height){
    float sx,sy;
    sx= (float)width;
    sy=(float)height;
    float m[4];
    glGetFloatv(GL_VIEWPORT,m);

    sx=width/m[2];
    sy=height/m[3];
    cout << " width " << sx << " height " << sy  << " end \n";
glScalef(sx,sy,1);
glTranslatef(m[0]*(1-sx),m[1]*(1-sy),0);
glTranslatef(x,y,0);glTranslatef(x,y,0);

}

void reshape(int w, int h) {
	// glViewport(0, 0, (GLsizei) w, (GLsizei) h);
	  myglViewport(0,0,(GLsizei) w, (GLsizei) h);
   glMatrixMode(GL_PROJECTION);
   glLoadIdentity();
   glOrtho (0.0, w, 0.0, h, -1.0, 1.0);
   glMatrixMode(GL_MODELVIEW);

}

//void display(void) {
//	int slices = 30;
//	int stacks = 30;
//	float radius = 0.2;
//
//	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
//
//    glPushMatrix();
//        changeColour(0.0, 0.0, 1.0, 1.0);
//        glTranslatef(0.0, 0.0, 0.1);
//        glutSolidSphere(radius, slices, stacks);
//    glPopMatrix();
//
//	glFlush();
//    glutSwapBuffers();
//}

void keyboard(unsigned char key, int x, int y) {
	switch (key) {
		case 27:
			exit(0); // Exit the application if 'Esc' key is pressed
	}
}

//void animate() {
//	glutPostRedisplay();
//}

int main(int argc, char * argv[]) {
    glutInit(&argc, argv);
    glutInitDisplayMode (GLUT_SINGLE | GLUT_RGB );
    glutInitWindowSize (800, 600);
    glutInitWindowPosition (100, 100);
    glutCreateWindow (argv[0]);   init();
    glutKeyboardFunc (keyboard);
    glutDisplayFunc (display);
    glutReshapeFunc (reshape);
 //   glutIdleFunc(animate);
    glutMainLoop();
    return 0;
}
