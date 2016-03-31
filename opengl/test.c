#include <stdio.h>
#include <GL/gl.h>
#include <GL/glut.h>
#include <GL/glu.h>
#include <math.h>
 
void myDisplay(void)
{
 glClear(GL_COLOR_BUFFER_BIT);
 glRectf(-05,-0.5,0.5,0.5);
 glFlush();
 
}
 
int main(int argc, char* argv[])
{
   glutInit(&argc, argv);
   glutInitDisplayMode(GLUT_RGB | GLUT_SINGLE);
   glutInitWindowPosition(100,100);
   glutInitWindowSize(400,400);
   glutCreateWindow("my first opengl program");
   glutDisplayFunc(&myDisplay);
   glutMainLoop();
   
 return 0;
}
