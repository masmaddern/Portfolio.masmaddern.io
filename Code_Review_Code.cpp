/*
 * Cup.cpp
 *
 *  Created on: Aug 2, 2020
 *      Author: masma
 */
#include <iostream>
#include <GL/glew.h>
#include <GL/freeglut.h>

//glm math header inclusions
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

using namespace std; //standard namespace

#define WINDOW_TITLE "Cup.Final" //window title macro

//shader macro
#ifndef GLSL
#define GLSL(Version, Source) "#version " #Version "\n" #Source
#endif

//variable declarations for shader, window size, buffer array objects
GLint shaderProgram, WindowWidth = 800, WindowHeight = 600;
GLuint VBO, VAO;

GLfloat cameraSpeed = 0.0005f; //shutter speed per frame

GLchar currentKey; //will store key pressed

GLfloat lastMouseX = 44, lastMouseY = 300;
GLfloat mouseXOffset, mouseYOffset, yaw = 0.0f, pitch = 0.0f; //mouse offset, yaw, pitch variables
GLfloat sensitivity = 0.5f;
bool mouseDetected = true;

//global vector declarations
glm::vec3 cameraPosition = glm::vec3(0.0f, 0.0f, 0.0f); //initial cam position placed 5 units in z
glm::vec3 CameraUpY = glm::vec3(0.0f, 1.0f, 0.0f); //temp y unit vector
glm::vec3 CameraForwardZ = glm::vec3(0.0f, 0.0f, -1.0f); //temp z unit vector
glm::vec3 front;

//function prototypes
void UResizeWindow(int, int);
void URenderGraphics(void);
void UCreateShader(void);
void UCreateBuffers(void);

void UMouseMove(int x, int y);

void draw_cylinder(GLfloat radius,
                   GLfloat height,
                   GLubyte R,
                   GLubyte G,
                   GLubyte B)
{
    GLfloat x              = 0.0;
    GLfloat y              = 0.0;
    GLfloat angle          = 0.0;
    GLfloat angle_stepsize = 0.1;

    /** Draw the tube */
    glColor3ub(R-40,G-40,B-40);
    glBegin(GL_QUAD_STRIP);
    angle = 0.0;
        while( angle < 2* 3.1415926 ) {
            x = radius * cos(angle);
            y = radius * sin(angle);
            glVertex3f(x, y , height);
            glVertex3f(x, y , 0.0);
            angle = angle + angle_stepsize;
        }
        glVertex3f(radius, 0.0, height);
        glVertex3f(radius, 0.0, 0.0);
    glEnd();

    /** Draw the circle on top of cup */
    glColor3ub(R,G,B);
    glBegin(GL_POLYGON);
    angle = 0.0;
        while( angle < 2* 3.1415926 ) {
            x = radius * cos(angle);
            y = radius * sin(angle);
            glVertex3f(x, y , height);
            angle = angle + angle_stepsize;
        }
        glVertex3f(radius, 0.0, height);
    glEnd();
}
/**************************** display() ******************************
 * The glut callback function to draw the polygons on the screen.
 *
 *   @parameter1: Nothing
 *
 *   @return: Nothing
 */
void display(void)
{
    glClear(GL_COLOR_BUFFER_BIT);
    glLoadIdentity();

    glTranslatef(0.0,-0.4,-3.0);
    glRotatef(-40, 1.0, 0.0, 0.0);

    draw_cylinder(0.3, 1.0, 255, 160, 100);

    glFlush();
}

/**************************** reshape() ******************************
 * The glut callback function that responsible when the window is
 * resized.
 *
 *   @parameter1: width  = Current window's width
 *   @parameter2: height = Current window's height
 *
 *   @return: Nothing
 */
void reshape(int width, int height)
{
    if (width == 0 || height == 0) return;

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(40.0, (GLdouble)width/(GLdouble)height,
                   0.5, 20.0);

    glMatrixMode(GL_MODELVIEW);
    glViewport(0, 0, width, height);
}

int main(int argc, char **argv)
{
    /** Initialize glut */
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
    glutInitWindowSize(640,480);
    glutCreateWindow("Cup");
    glClearColor(0.0,0.0,0.0,0.0);
    glutDisplayFunc(display);
    glutReshapeFunc(reshape);

    glutMainLoop();

    return 0;
}
//vertex shader source code
const GLchar * vertexShaderSource = GLSL(330,
       layout (location = 0) in vec3 position;
       layout (location = 1) in vec3 color;

       out vec3 mobileColor; //variable to transfer color data to fragment shader

       //global variables for the transform matrices
       uniform mat4 model;
       uniform mat4 view;
       uniform mat4 projection;

void main()
       gl_Position = projection * view * model * vec4(position, 1.0f); // transforms vertices to clip coordinates
       mobileColor = color; //references incoming color data
       }
       );

//fragment shader source code
const GLchar * fragmentShaderSource = GLSL(330,

       in vec3 mobileColor;

       out vec4 gpuColor;

void main(){

gpuColor = vec4(mobileColor, 1.0);

       }
   );

//main program
int main(int argc, char* argv[])
       {
       glutInit(&argc, argv);
       glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);
       glutInitWindowSize(WindowWidth, WindowHeight);
       glutCreateWindow(WINDOW_TITLE);

       glutReshapeFunc(UResizeWindow);

       glewExperimental = GL_TRUE;
       if(glewInit() !=GLEW_OK)
       {
           std::cout <<"Failed to initialize GLEW" << std::endl;
           return -1;
       }
UCreateShader();
UCreateBuffers();

//use shader program
glUseProgram(shaderProgram);

glClearColor(0.0f, 0.0f, 1.0f, 0.0f);

glutDisplayFunc(URenderGraphics);

glutPassiveMotionFunc(UMouseMove); //detects mouse movement

glutMainLoop();

//destroys buffer objects once used
glDeleteVertexArrays(1, &VAO);
glDeleteBuffers(1, &VBO);

return 0;
}

//resizes the window
void UResizeWindow(int w, int h)
{
   WindowWidth = w;
   WindowHeight = h;
   glViewport(0, 0, WindowWidth, WindowHeight);
}

//renders graphics
void URenderGraphics(void)
{

   glEnable(GL_DEPTH_TEST); // enable z-depth

   glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
   glBindVertexArray(VAO);
   CameraForwardZ = front;
   if(currentKey == 'w')
       cameraPosition += cameraSpeed * CameraForwardZ;

   if(currentKey == 's' )
       cameraPosition -= cameraSpeed * CameraForwardZ;

   if(currentKey == 'a' )
       cameraPosition -= glm::normalize(glm::cross(CameraForwardZ, CameraUpY)) * cameraSpeed;

   if(currentKey == 'd' )
       cameraPosition += glm::normalize(glm::cross(CameraForwardZ, CameraUpY)) * cameraSpeed;

//transforms the object
   glm::mat4 model;
   model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f));
   model = glm::rotate(model, 45.0f, glm::vec3(0.0f, 1.0f, 0.0f));
   model = glm::scale(model, glm::vec3(2.0f, 2.0f, 2.0f));

//transforms the camera
   glm::mat4 view;
   view = glm::lookAt(CameraForwardZ, cameraPosition, CameraUpY);

//creates a perspective projection
   glm::mat4 projection;
   projection = glm::perspective(45.0f, (GLfloat)WindowWidth / (GLfloat)WindowHeight, 0.1f, 100.0f);

//retrieves and passes transform matrices to the Shader program
   GLint modelLoc = glGetUniformLocation(shaderProgram, "model");
   GLint viewLoc = glGetUniformLocation(shaderProgram, "view");
   GLint projLoc = glGetUniformLocation(shaderProgram, "projection");

   glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
   glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
   glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));

   glutPostRedisplay();

//draws triangles
   glDrawArrays(GL_TRIANGLES, 0, 36);

   glBindVertexArray(0); //deactivate the vertex array object

   glutSwapBuffers();

}

void UCreateShader()
       {

//vertex shader
   GLint vertexShader = glCreateShader(GL_VERTEX_SHADER); //creates vertex shader
   glShaderSource(vertexShader, 1, &vertexShaderSource, NULL); //attaches the vertex shader to the source code
   glCompileShader(vertexShader); //compiles the vertex shader

//fragment shader
   GLint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER); //creates the vertex shader
   glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
   glCompileShader(fragmentShader);

//shader program
   shaderProgram = glCreateProgram(); //creates the shader program and returns an id
   glAttachShader(shaderProgram, vertexShader);
   glAttachShader(shaderProgram, fragmentShader);
   glLinkProgram(shaderProgram);

   glDeleteShader(vertexShader);
   glDeleteShader(fragmentShader);
}

void UCreateBuffers()
{
   GLfloat vertices[] = {
		   			-0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
		   			 0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
		   			 0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
		   			 0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
		   			-0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
		   			-0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,

		   			-0.5f, -0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
		   			 0.5f, -0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
		   			 0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
		    		 0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
		   			-0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
		   			-0.5f, -0.5f,  0.5f,  0.0f,  1.0f,  0.0f,

		   			-0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
		   			-0.5f,  0.5f, -0.5f,  0.0f,  0.0f,  1.0f,
		   			-0.5f, -0.5f, -0.5f,  0.0f,  0.0f,  1.0f,
		   			-0.5f, -0.5f, -0.5f,  0.0f,  0.0f,  1.0f,
		   			-0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
		   	    	-0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,

		   			 0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
		   			 0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
		   			 0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
		   			 0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
		   			 0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
		   			 0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,

		   			-0.5f, -0.5f, -0.5f,  0.0f,  0.0f,  1.0f,
		   			 0.5f, -0.5f, -0.5f,  0.0f,  0.0f,  1.0f,
		   			 0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
		   			 0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
		   		    -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
		   			-0.5f, -0.5f, -0.5f,  0.0f,  0.0f,  1.0f,

		   		    -0.5f,  0.5f, -0.5f,  1.0f,  1.0f,  0.0f,
		   			 0.5f,  0.5f, -0.5f,  1.0f,  1.0f,  0.0f,
		   			 0.5f,  0.5f,  0.5f,  1.0f,  1.0f,  0.0f,
		   			 0.5f,  0.5f,  0.5f,  1.0f,  1.0f,  0.0f,
		   			-0.5f,  0.5f,  0.5f,  1.0f,  1.0f,  0.0f,
		   			-0.5f,  0.5f, -0.5f,  1.0f,  1.0f,  0.0f,
   };

   glGenVertexArrays(1, &VAO);
   glGenBuffers(1, &VBO);

   glBindVertexArray(VAO);

//activate the VBO
   glBindBuffer(GL_ARRAY_BUFFER, VBO);
   glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW); //copy vertices to VBO

//set attribute pointer 0 to hold Position data
   glVertexAttribPointer(0,3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)0);
   glEnableVertexAttribArray(0);


   glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
   glEnableVertexAttribArray(1); //enables vertex attribute

   glBindVertexArray(0);
}

//implements the UMouseMove function
       void UMouseMove(int x, int y)
       {


           if(mouseDetected)
           {
               lastMouseX = x;
               lastMouseY = y;
               mouseDetected = false;
           }

                       mouseXOffset = x - lastMouseX;
                       mouseYOffset = lastMouseY - y;

                       lastMouseX = x;
                       lastMouseY = y;

                       mouseXOffset *= sensitivity;
                       mouseYOffset *= sensitivity;

                       yaw += mouseXOffset;
                       pitch += mouseYOffset;

                       front.x = 10.0f * cos(yaw);
                       front.y = 10.0f * sin(pitch);
                       front.z = sin(yaw) * cos(pitch) * 10.f;
                           }
