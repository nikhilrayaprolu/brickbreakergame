#include <iostream>
#include <cmath>
#include <fstream>
#include <vector>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <map>
#include <thread>
#include <ao/ao.h>
#include <mpg123.h>
#include <FTGL/ftgl.h>
#include <SOIL/SOIL.h>
#define XMAX 400
#define XMIN -400
#define YMAX 300
#define YMIN -300
#define out1(x)cout<<#x<<" is "<<x<<endl
#define out2(x,y)cout<<#x<<" is "<<x<<" "<<#y <<" is "<<y<<endl
#define out3(x,y,z)cout<<#x<<" is "<<x<<" "<<#y<<" is "<<y<<" "<<#z<<" is "<<z<<endl;
#define out4(a,b,c,d)cout<<#a<<" is "<<a<<" "<<#b<<"  is "<<b<<" "<<#c<<" is "<<c<<" "<<#d<<" is "<<d<<endl;
int fbwidth,fbheight;
using namespace std;
struct VAO {
    GLuint VertexArrayID;
    GLuint VertexBuffer;
    GLuint ColorBuffer;
    GLuint TextureBuffer;
    GLuint TextureID;


    GLenum PrimitiveMode;
    GLenum FillMode;
    int NumVertices;
};
typedef struct VAO VAO;

struct GLMatrices {
	glm::mat4 projection;
	glm::mat4 model;
	glm::mat4 view;
	GLuint MatrixID;
    GLuint TexMatrixID;
} Matrices;
struct FTGLFont {
    FTFont* font;
    GLuint fontMatrixID;
    GLuint fontColorID;
} GL3Font;
GLuint programID,fontProgramID,textureProgramID;


typedef struct COLOR{
    float r;
    float g;
    float b;
} COLOR;

COLOR grey = {168.0/255.0,168.0/255.0,168.0/255.0};
COLOR gold = {218.0/255.0,165.0/255.0,32.0/255.0};
COLOR coingold = {255.0/255.0,223.0/255.0,0.0/255.0};
COLOR red = {255.0/255.0,51.0/255.0,51.0/255.0};
COLOR lightgreen = {57/255.0,230/255.0,0/255.0};
COLOR darkgreen = {51/255.0,102/255.0,0/255.0};
COLOR black = {30/255.0,30/255.0,21/255.0};
COLOR blue = {0,0,1};
COLOR darkbrown = {46/255.0,46/255.0,31/255.0};
COLOR lightbrown = {95/255.0,63/255.0,32/255.0};
COLOR brown1 = {117/255.0,78/255.0,40/255.0};
COLOR brown2 = {134/255.0,89/255.0,40/255.0};
COLOR brown3 = {46/255.0,46/255.0,31/255.0};
COLOR cratebrown = {153/255.0,102/255.0,0/255.0};
COLOR cratebrown1 = {121/255.0,85/255.0,0/255.0};
COLOR cratebrown2 = {102/255.0,68/255.0,0/255.0};
COLOR skyblue2 = {113/255.0,185/255.0,209/255.0};
COLOR skyblue1 = {123/255.0,201/255.0,227/255.0};
COLOR skyblue = {132/255.0,217/255.0,245/255.0};
COLOR cloudwhite = {229/255.0,255/255.0,255/255.0};
COLOR cloudwhite1 = {204/255.0,255/255.0,255/255.0};
COLOR lightpink = {255/255.0,122/255.0,173/255.0};
COLOR darkpink = {255/255.0,51/255.0,119/255.0};
COLOR white = {255/255.0,255/255.0,255/255.0};

typedef struct object{
    VAO *object;
    string id;
    string name;

    COLOR color;
    double x_speed;
    double y_speed;
    double x;
    double y;
    double radius;
    double height,width;
    double radius_x;
    double radius_y;
    double status;
    double angle;
    double inAir;
    double fixed;
    double r;
    int isRotating;
    int direction; //0 for clockwise and 1 for anticlockwise for animation
    float remAngle; //the remaining angle to finish animation
    int isMovingAnim;
    int dx;
    int dy;
    map <string ,bool> collisionedmirrors;
} object;
float score = 0;
map <string, object> fallingbricks;
map <string, object> objects;
map <string, object> boxes;
map <string, object> cannonObjects;
map <string, object> cannonbullets;
map <string, object> mirrorobjects;
map <string, object> boxcircle;

double MAXV = 20;
float x_change = 0; //For the camera pan
float y_change = 0; //For the camera pan
int zoom_camera = 1;
int window_width = 1200;
int window_height = 600;
int boxspeed=10;
int cannonverticalspeed=10;
int fallingbricksspeed = 12;
int cannonanglespeed=3;
int lastreleasebullet=glfwGetTime();
int minimumbulletreleasetime=1;
int scoreshootblack=5;
int scoreshootnonblack=5;
int scorecatchsamecolor=3;
int scorecatchdiffcolor=3;
int gameover=0;
int gamestart=0;
int countnonblackshoot=0;
int maxnonblackshoot=5;
void* play_audio(string audioFile);
struct object createRectangle (string name, float weight, COLOR colorA, COLOR colorB, COLOR colorC, COLOR colorD, float x, float y, float height, float width, string component);
/* Function to load Shaders - Use it as it is */
GLuint LoadShaders(const char * vertex_file_path,const char * fragment_file_path) {

	// Create the shaders
	GLuint VertexShaderID = glCreateShader(GL_VERTEX_SHADER);
	GLuint FragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);

	// Read the Vertex Shader code from the file
	std::string VertexShaderCode;
	std::ifstream VertexShaderStream(vertex_file_path, std::ios::in);
	if(VertexShaderStream.is_open())
	{
		std::string Line = "";
		while(getline(VertexShaderStream, Line))
			VertexShaderCode += "\n" + Line;
		VertexShaderStream.close();
	}

	// Read the Fragment Shader code from the file
	std::string FragmentShaderCode;
	std::ifstream FragmentShaderStream(fragment_file_path, std::ios::in);
	if(FragmentShaderStream.is_open()){
		std::string Line = "";
		while(getline(FragmentShaderStream, Line))
			FragmentShaderCode += "\n" + Line;
		FragmentShaderStream.close();
	}

	GLint Result = GL_FALSE;
	int InfoLogLength;

	// Compile Vertex Shader
	printf("Compiling shader : %s\n", vertex_file_path);
	char const * VertexSourcePointer = VertexShaderCode.c_str();
	glShaderSource(VertexShaderID, 1, &VertexSourcePointer , NULL);
	glCompileShader(VertexShaderID);

	// Check Vertex Shader
	glGetShaderiv(VertexShaderID, GL_COMPILE_STATUS, &Result);
	glGetShaderiv(VertexShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
	std::vector<char> VertexShaderErrorMessage(InfoLogLength);
	glGetShaderInfoLog(VertexShaderID, InfoLogLength, NULL, &VertexShaderErrorMessage[0]);
	fprintf(stdout, "%s\n", &VertexShaderErrorMessage[0]);

	// Compile Fragment Shader
	printf("Compiling shader : %s\n", fragment_file_path);
	char const * FragmentSourcePointer = FragmentShaderCode.c_str();
	glShaderSource(FragmentShaderID, 1, &FragmentSourcePointer , NULL);
	glCompileShader(FragmentShaderID);

	// Check Fragment Shader
	glGetShaderiv(FragmentShaderID, GL_COMPILE_STATUS, &Result);
	glGetShaderiv(FragmentShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
	std::vector<char> FragmentShaderErrorMessage(InfoLogLength);
	glGetShaderInfoLog(FragmentShaderID, InfoLogLength, NULL, &FragmentShaderErrorMessage[0]);
	fprintf(stdout, "%s\n", &FragmentShaderErrorMessage[0]);

	// Link the program
	fprintf(stdout, "Linking program\n");
	GLuint ProgramID = glCreateProgram();
	glAttachShader(ProgramID, VertexShaderID);
	glAttachShader(ProgramID, FragmentShaderID);
	glLinkProgram(ProgramID);

	// Check the program
	glGetProgramiv(ProgramID, GL_LINK_STATUS, &Result);
	glGetProgramiv(ProgramID, GL_INFO_LOG_LENGTH, &InfoLogLength);
	std::vector<char> ProgramErrorMessage( max(InfoLogLength, int(1)) );
	glGetProgramInfoLog(ProgramID, InfoLogLength, NULL, &ProgramErrorMessage[0]);
	fprintf(stdout, "%s\n", &ProgramErrorMessage[0]);

	glDeleteShader(VertexShaderID);
	glDeleteShader(FragmentShaderID);

	return ProgramID;
}

static void error_callback(int error, const char* description)
{
    fprintf(stderr, "Error: %s\n", description);
}

void quit(GLFWwindow *window)
{
    glfwDestroyWindow(window);
    glfwTerminate();
//    exit(EXIT_SUCCESS);
}


/* Generate VAO, VBOs and return VAO handle */
struct VAO* create3DObject (GLenum primitive_mode, int numVertices, const GLfloat* vertex_buffer_data, const GLfloat* color_buffer_data, GLenum fill_mode=GL_FILL)
{
    struct VAO* vao = new struct VAO;
    vao->PrimitiveMode = primitive_mode;
    vao->NumVertices = numVertices;
    vao->FillMode = fill_mode;

    // Create Vertex Array Object
    // Should be done after CreateWindow and before any other GL calls
    glGenVertexArrays(1, &(vao->VertexArrayID)); // VAO
    glGenBuffers (1, &(vao->VertexBuffer)); // VBO - vertices
    glGenBuffers (1, &(vao->ColorBuffer));  // VBO - colors

    glBindVertexArray (vao->VertexArrayID); // Bind the VAO 
    glBindBuffer (GL_ARRAY_BUFFER, vao->VertexBuffer); // Bind the VBO vertices 
    glBufferData (GL_ARRAY_BUFFER, 3*numVertices*sizeof(GLfloat), vertex_buffer_data, GL_STATIC_DRAW); // Copy the vertices into VBO
    glVertexAttribPointer(
                          0,                  // attribute 0. Vertices
                          3,                  // size (x,y,z)
                          GL_FLOAT,           // type
                          GL_FALSE,           // normalized?
                          0,                  // stride
                          (void*)0            // array buffer offset
                          );

    glBindBuffer (GL_ARRAY_BUFFER, vao->ColorBuffer); // Bind the VBO colors 
    glBufferData (GL_ARRAY_BUFFER, 3*numVertices*sizeof(GLfloat), color_buffer_data, GL_STATIC_DRAW);  // Copy the vertex colors
    glVertexAttribPointer(
                          1,                  // attribute 1. Color
                          3,                  // size (r,g,b)
                          GL_FLOAT,           // type
                          GL_FALSE,           // normalized?
                          0,                  // stride
                          (void*)0            // array buffer offset
                          );

    return vao;
}
struct VAO* create3DTexturedObject (GLenum primitive_mode, int numVertices, const GLfloat* vertex_buffer_data, const GLfloat* texture_buffer_data, GLuint textureID, GLenum fill_mode=GL_FILL)
{
    struct VAO* vao = new struct VAO;
    vao->PrimitiveMode = primitive_mode;
    vao->NumVertices = numVertices;
    vao->FillMode = fill_mode;
    vao->TextureID = textureID;

    // Create Vertex Array Object
    // Should be done after CreateWindow and before any other GL calls
    glGenVertexArrays(1, &(vao->VertexArrayID)); // VAO
    glGenBuffers (1, &(vao->VertexBuffer)); // VBO - vertices
    glGenBuffers (1, &(vao->TextureBuffer));  // VBO - textures

    glBindVertexArray (vao->VertexArrayID); // Bind the VAO
    glBindBuffer (GL_ARRAY_BUFFER, vao->VertexBuffer); // Bind the VBO vertices
    glBufferData (GL_ARRAY_BUFFER, 3*numVertices*sizeof(GLfloat), vertex_buffer_data, GL_STATIC_DRAW); // Copy the vertices into VBO
    glVertexAttribPointer(
            0,                  // attribute 0. Vertices
            3,                  // size (x,y,z)
            GL_FLOAT,           // type
            GL_FALSE,           // normalized?
            0,                  // stride
            (void*)0            // array buffer offset
    );

    glBindBuffer (GL_ARRAY_BUFFER, vao->TextureBuffer); // Bind the VBO textures
    glBufferData (GL_ARRAY_BUFFER, 2*numVertices*sizeof(GLfloat), texture_buffer_data, GL_STATIC_DRAW);  // Copy the vertex colors
    glVertexAttribPointer(
            2,                  // attribute 2. Textures
            2,                  // size (s,t)
            GL_FLOAT,           // type
            GL_FALSE,           // normalized?
            0,                  // stride
            (void*)0            // array buffer offset
    );

    return vao;
}

/* Generate VAO, VBOs and return VAO handle - Common Color for all vertices */
struct VAO* create3DObject (GLenum primitive_mode, int numVertices, const GLfloat* vertex_buffer_data, const GLfloat red, const GLfloat green, const GLfloat blue, GLenum fill_mode=GL_FILL)
{
    GLfloat* color_buffer_data = new GLfloat [3*numVertices];
    for (int i=0; i<numVertices; i++) {
        color_buffer_data [3*i] = red;
        color_buffer_data [3*i + 1] = green;
        color_buffer_data [3*i + 2] = blue;
    }

    return create3DObject(primitive_mode, numVertices, vertex_buffer_data, color_buffer_data, fill_mode);
}

/* Render the VBOs handled by VAO */
void draw3DObject (struct VAO* vao)
{
    // Change the Fill Mode for this object
    glPolygonMode (GL_FRONT_AND_BACK, vao->FillMode);

    // Bind the VAO to use
    glBindVertexArray (vao->VertexArrayID);

    // Enable Vertex Attribute 0 - 3d Vertices
    glEnableVertexAttribArray(0);
    // Bind the VBO to use
    glBindBuffer(GL_ARRAY_BUFFER, vao->VertexBuffer);

    // Enable Vertex Attribute 1 - Color
    glEnableVertexAttribArray(1);
    // Bind the VBO to use
    glBindBuffer(GL_ARRAY_BUFFER, vao->ColorBuffer);

    // Draw the geometry !
    glDrawArrays(vao->PrimitiveMode, 0, vao->NumVertices); // Starting from vertex 0; 3 vertices total -> 1 triangle
}
/* Create an OpenGL Texture from an image */
GLuint createTexture (const char* filename)
{
    GLuint TextureID;
    // Generate Texture Buffer
    glGenTextures(1, &TextureID);
    // All upcoming GL_TEXTURE_2D operations now have effect on our texture buffer
    glBindTexture(GL_TEXTURE_2D, TextureID);
    // Set our texture parameters
    // Set texture wrapping to GL_REPEAT
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    // Set texture filtering (interpolation)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    // Load image and create OpenGL texture
    int twidth, theight;
    unsigned char* image = SOIL_load_image(filename, &twidth, &theight, 0, SOIL_LOAD_RGB);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, twidth, theight, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
    glGenerateMipmap(GL_TEXTURE_2D); // Generate MipMaps to use
    SOIL_free_image_data(image); // Free the data read from file after creating opengl texture
    glBindTexture(GL_TEXTURE_2D, 0); // Unbind texture when done, so we won't accidentily mess it up

    return TextureID;
}
void draw3DTexturedObject (struct VAO* vao)
{
    if(vao == NULL)return;
    // Change the Fill Mode for this object
    glPolygonMode (GL_FRONT_AND_BACK, vao->FillMode);

    // Bind the VAO to use
    glBindVertexArray (vao->VertexArrayID);

    // Enable Vertex Attribute 0 - 3d Vertices
    glEnableVertexAttribArray(0);
    // Bind the VBO to use
    glBindBuffer(GL_ARRAY_BUFFER, vao->VertexBuffer);

    // Bind Textures using texture units
    glBindTexture(GL_TEXTURE_2D, vao->TextureID);

    // Enable Vertex Attribute 2 - Texture
    glEnableVertexAttribArray(2);
    // Bind the VBO to use
    glBindBuffer(GL_ARRAY_BUFFER, vao->TextureBuffer);

    // Draw the geometry !
    glDrawArrays(vao->PrimitiveMode, 0, vao->NumVertices); // Starting from vertex 0; 3 vertices total -> 1 triangle

    // Unbind Textures to be safe
    glBindTexture(GL_TEXTURE_2D, 0);
}
/**************************
 * Customizable functions *
 **************************/

float triangle_rot_dir = 1;
float rectangle_rot_dir = 1;
bool triangle_rot_status = true;
bool rectangle_rot_status = true;
int redleft,redright,greenleft,greenright;
/* Executed when a regular key is pressed/released/held-down */
/* Prefered for Keyboard events */
int comparecolors(struct COLOR color1, struct COLOR color2){
    if(color1.b==color2.b && color1.g==color2.g && color1.r==color2.r)
        return 1;
    else return 0;
}
//Set the audioFile parameter to the file name first before calling this function
void* play_audio(string audioFile){
    mpg123_handle *mh;
    unsigned char *buffer;
    size_t buffer_size;
    size_t done;
    int err;

    int driver;
    ao_device *dev;

    ao_sample_format format;
    int channels, encoding;
    long rate;

    /* initializations */
    ao_initialize();
    driver = ao_default_driver_id();
    mpg123_init();
    mh = mpg123_new(NULL, &err);
    buffer_size = mpg123_outblock(mh);
    buffer = (unsigned char*) malloc(buffer_size * sizeof(unsigned char));

    /* open the file and get the decoding format */
    mpg123_open(mh, &audioFile[0]);
    mpg123_getformat(mh, &rate, &channels, &encoding);

    /* set the output format and open the output device */
    format.bits = mpg123_encsize(encoding) * 8;
    format.rate = rate;
    format.channels = channels;
    format.byte_format = AO_FMT_NATIVE;
    format.matrix = 0;
    dev = ao_open_live(driver, &format, NULL);

    /* decode and play */
    char *p =(char *)buffer;
    while (mpg123_read(mh, buffer, buffer_size, &done) == MPG123_OK)
        ao_play(dev, p, done);

    /* clean up */
    free(buffer);
    ao_close(dev);
    mpg123_close(mh);
    mpg123_delete(mh);
}

void createbullet(){
    if(glfwGetTime()-lastreleasebullet>minimumbulletreleasetime){
        lastreleasebullet=glfwGetTime();
    double launch_angle=cannonObjects["cannonrect"].angle;
    struct object rectangle=createRectangle("bullet",10000,blue,blue,blue,blue,cannonObjects["cannoncircle"].x,cannonObjects["cannoncircle"].y,7,50,"bullet");
    rectangle.x_speed=(floor(10*cos(launch_angle*(M_PI/180))*100000)/100000);
    rectangle.y_speed=(floor(10*sin(launch_angle*(M_PI/180))*100000)/100000);
    rectangle.angle=launch_angle;
    cannonbullets["bullet"+to_string(glfwGetTime())]=rectangle;
        cannonObjects["cannoncircle"].x-=30;
    }
}
void keyboard (GLFWwindow* window, int key, int scancode, int action, int mods)
{
     // Function is called first on GLFW_PRESS.

    if (action == GLFW_RELEASE) {
        switch (key) {
            case GLFW_KEY_C:
                rectangle_rot_status = !rectangle_rot_status;
                break;
            case GLFW_KEY_P:
                triangle_rot_status = !triangle_rot_status;
                break;
            case GLFW_KEY_X:
                // do something ..
                break;
            case GLFW_KEY_LEFT:
                greenleft=0;
                redleft=0;
                break;
            case GLFW_KEY_RIGHT:
                redright=0;
                greenright=0;
                break;
            case GLFW_KEY_N:
                fallingbricksspeed+=2;
                break;
            case GLFW_KEY_M:
                fallingbricksspeed-=2;
                break;

            default:
                break;
        }
    }
    else if (action != GLFW_RELEASE) {
        switch (key) {
            case GLFW_KEY_ESCAPE:
                quit(window);
                break;
            case GLFW_KEY_LEFT:
                if(mods == GLFW_MOD_CONTROL)
                    redleft=1;
                else if(mods == GLFW_MOD_ALT)
                    greenleft=1;
                else
                    x_change+=5;
                break;
            case GLFW_KEY_RIGHT:

                if(mods == GLFW_MOD_CONTROL)
                    redright=1;
                else if(mods == GLFW_MOD_ALT)
                    greenright=1;
                else
                    x_change-=5;
                break;
            case GLFW_KEY_S:
                cannonObjects["cannoncircle"].y+=cannonverticalspeed;
                break;
            case GLFW_KEY_F:
                cannonObjects["cannoncircle"].y-=cannonverticalspeed;
                break;
            case GLFW_KEY_A:
                cannonObjects["cannonrect"].angle+=cannonanglespeed;
                break;
            case GLFW_KEY_D:
                cannonObjects["cannonrect"].angle-=cannonanglespeed;
                break;
            case GLFW_KEY_SPACE:
                createbullet();
                break;
            case GLFW_KEY_C:
                gamestart=1;
                break;
            case GLFW_KEY_UP:
                out1(zoom_camera);
                zoom_camera+=1;
                out1(zoom_camera);
                break;
            case GLFW_KEY_DOWN:
                zoom_camera=zoom_camera-1;
                break;
            default:
                break;

        }
    }
}

/* Executed for character input (like in text boxes) */
void keyboardChar (GLFWwindow* window, unsigned int key)
{
	switch (key) {
		case 'Q':
		case 'q':
            quit(window);
            break;
		default:
			break;
	}
}
int mouseleftpressed=0;
int mouseleftfirstpressed=0;
int mouserightpressed=0;
int mouserightfirstpressed=0;
double mouse_x,mouse_y;
double mouse_x_old,mouse_y_old;
int moveredbox=0;
int movegreenbox=0;
int movecannonbox=0;
void mouseright(GLFWwindow* window){
    if(mouserightpressed){

        glfwGetCursorPos(window,&mouse_x,&mouse_y);
        if(!mouserightfirstpressed){
            mouse_x_old=mouse_x;
            mouse_y_old=mouse_y;
            mouserightfirstpressed=1;
        }
        x_change=mouse_x-mouse_x_old;
        y_change=mouse_y-mouse_y_old;
    }
}
void mouseleft(GLFWwindow* window){


    out2(boxes["red"].x,boxes["red"].y);
    glfwGetCursorPos(window,&mouse_x,&mouse_y);
    mouse_x=-400+mouse_x*800/fbwidth;
    mouse_y=300-mouse_y*600/fbheight;
    out2(mouse_x,mouse_y);
    if(!mouseleftfirstpressed && mouse_x > boxes["red"].x-boxes["red"].width/2  && mouse_x < boxes["red"].x+boxes["red"].width/2 && mouse_y > boxes["red"].y-boxes["red"].height/2 && mouse_y < boxes["red"].y+boxes["red"].height/2){
        mouseleftfirstpressed=1;
        moveredbox=1;
    }
    if(!mouseleftfirstpressed && mouse_x > boxes["green"].x-boxes["green"].width/2  && mouse_x < boxes["green"].x+boxes["green"].width/2 && mouse_y > boxes["green"].y-boxes["green"].height/2 && mouse_y < boxes["green"].y+boxes["green"].height/2){
        mouseleftfirstpressed=1;
        movegreenbox=1;
    }
    if(!mouseleftfirstpressed && mouse_x > cannonObjects["cannoncircle"].x-cannonObjects["cannoncircle"].r  && mouse_x < cannonObjects["cannoncircle"].x+cannonObjects["cannoncircle"].r && mouse_y > cannonObjects["cannoncircle"].y-cannonObjects["cannoncircle"].r && mouse_y < cannonObjects["cannoncircle"].y+cannonObjects["cannoncircle"].r){
        mouseleftfirstpressed=1;
        movecannonbox=1;
    }


    out1(mouseleftfirstpressed);
    if(mouseleftfirstpressed){
        out1(moveredbox);
        if(moveredbox){
            boxes["red"].x=mouse_x;

        }
        else if(movegreenbox){
            boxes["green"].x=mouse_x;
        }
        else if(movecannonbox){
            cannonObjects["cannoncircle"].y=mouse_y;
        }
    }
    if(!(moveredbox || movegreenbox || movecannonbox)){
        out1("camehere");
        double angle_y=mouse_y-cannonObjects["cannoncircle"].y;
        double angle_x=mouse_x-cannonObjects["cannoncircle"].x;
        double finalangle=angle_y/angle_x;
        out1(finalangle);
        cannonObjects["cannonrect"].angle=atan (finalangle) * 180 / M_PI;
    }
}
/* Executed when a mouse button is pressed/released */
void mouseButton (GLFWwindow* window, int button, int action, int mods)
{
    switch (button) {
        case GLFW_MOUSE_BUTTON_LEFT:
            out1(action);
            if (action != GLFW_RELEASE){
                out1("mouse pressed");
                mouseleftpressed=1;
                mouseleft(window);

            }

            if(action == GLFW_RELEASE){
                moveredbox=0;
                movegreenbox=0;
                mouseleftfirstpressed=0;
                mouseleftpressed=0;
                movecannonbox=0;
            }

            break;
        case GLFW_MOUSE_BUTTON_RIGHT:
            if (action == GLFW_PRESS){
                mouserightpressed=1;



            }
            if (action == GLFW_RELEASE) {
                mouserightpressed=0;
            }
            break;
        default:
            break;
    }
}
void mousescroll(GLFWwindow* window, double xoffset, double yoffset)
{
    if (yoffset==-1) {
        zoom_camera /= 1.1; //make it bigger than current size
    }
    else if(yoffset==1){
        zoom_camera *= 1.1; //make it bigger than current size
    }
    if (zoom_camera<=1) {
        zoom_camera = 1;
    }
    if (zoom_camera>=4) {
        zoom_camera=4;
    }
    if(x_change-400.0f/zoom_camera<-400)
        x_change=-400+400.0f/zoom_camera;
    else if(x_change+400.0f/zoom_camera>400)
        x_change=400-400.0f/zoom_camera;
    if(y_change-300.0f/zoom_camera<-300)
        y_change=-300+300.0f/zoom_camera;
    else if(y_change+300.0f/zoom_camera>300)
        y_change=300-300.0f/zoom_camera;
    Matrices.projection = glm::ortho((float)(-400.0f/zoom_camera+x_change), (float)(400.0f/zoom_camera+x_change), (float)(-300.0f/zoom_camera+y_change), (float)(300.0f/zoom_camera+y_change), 0.1f, 500.0f);
}
//Ensure the panning does not go out of the map
void check_pan(){
    if(x_change-400.0f/zoom_camera<-400)
        x_change=-400+400.0f/zoom_camera;
    else if(x_change+400.0f/zoom_camera>400)
        x_change=400-400.0f/zoom_camera;
    if(y_change-300.0f/zoom_camera<-300)
        y_change=-300+300.0f/zoom_camera;
    else if(y_change+300.0f/zoom_camera>300)
        y_change=300-300.0f/zoom_camera;
}


/* Executed when window is resized to 'width' and 'height' */
/* Modify the bounds of the screen here in glm::ortho or Field of View in glm::Perspective */
void reshapeWindow (GLFWwindow* window, int width, int height)
{
     fbwidth=width, fbheight=height;
    /* With Retina display on Mac OS X, GLFW's FramebufferSize
     is different from WindowSize */
    glfwGetFramebufferSize(window, &fbwidth, &fbheight);

	GLfloat fov = 90.0f;

	// sets the viewport of openGL renderer
	glViewport (0, 0, (GLsizei) fbwidth, (GLsizei) fbheight);

	// set the projection matrix as perspective
	/* glMatrixMode (GL_PROJECTION);
	   glLoadIdentity ();
	   gluPerspective (fov, (GLfloat) fbwidth / (GLfloat) fbheight, 0.1, 500.0); */
	// Store the projection matrix in a variable for future use
    // Perspective projection for 3D views
    // Matrices.projection = glm::perspective (fov, (GLfloat) fbwidth / (GLfloat) fbheight, 0.1f, 500.0f);

    // Ortho projection for 2D views
    Matrices.projection = glm::ortho((float)(-400.0f/zoom_camera+x_change), (float)(400.0f/zoom_camera+x_change), (float)(-300.0f/zoom_camera+y_change), (float)(300.0f/zoom_camera+y_change), 0.1f, 500.0f);
}

int checkCollisionRight(object col_object, object my_object){
    if(col_object.x>my_object.x && col_object.y+col_object.height/2>my_object.y-my_object.height/2 && col_object.y-col_object.height/2<my_object.y+my_object.height/2 && col_object.x-col_object.width/2<my_object.x+my_object.width/2 && col_object.x+col_object.width/2>my_object.x-my_object.width/2){
        return 1;
    }
    return 0;
}

int checkCollisionLeft(object col_object, object my_object){
    if(col_object.x<my_object.x && col_object.y+col_object.height/2>my_object.y-my_object.height/2 && col_object.y-col_object.height/2<my_object.y+my_object.height/2 && col_object.x+col_object.width/2>my_object.x-my_object.width/2 && col_object.x-col_object.width/2<my_object.x+my_object.width/2){
        return 1;
    }
    return 0;
}

int checkCollisionTop(object col_object, object my_object){
    if(col_object.y>my_object.y && col_object.x+col_object.width/2>my_object.x-my_object.width/2 && col_object.x-col_object.width/2<my_object.x+my_object.width/2 && col_object.y-col_object.height/2<my_object.y+my_object.height/2 && col_object.y+col_object.height/2>my_object.y-my_object.height/2){
        return 1;
    }
    return 0;
}

int checkCollisionBottom(object col_object, object my_object){
    if(col_object.y<my_object.y && col_object.x+col_object.width/2>my_object.x-my_object.width/2 && col_object.x-col_object.width/2<my_object.x+my_object.width/2 && col_object.y+col_object.height/2>my_object.y-my_object.height/2 && col_object.y-col_object.height/2<my_object.y+my_object.height/2){
        return 1;
    }
    return 0;
}
int checkCollision(string name,float dx,float dy){
 int any_collide=0;
    if(name=="redbox"){
        for(map<string,object>::iterator it2=fallingbricks.begin();it2!=fallingbricks.end();it2++){
            object col_object=fallingbricks[it2->first];
            object my_object=boxes["red"];
            if(col_object.status==0)
                continue;
            if(( checkCollisionRight(col_object,my_object)) || (checkCollisionLeft(col_object,my_object)) || (checkCollisionTop(col_object,my_object)) || (checkCollisionBottom(col_object,my_object))){
                if(comparecolors(fallingbricks[it2->first].color,red)){
                    fallingbricks[it2->first].status=0;
                    score+=scorecatchsamecolor;
                }else if(comparecolors(fallingbricks[it2->first].color,lightgreen)){
                    fallingbricks[it2->first].status=0;
                    score-=scorecatchdiffcolor;
                }else if(comparecolors(fallingbricks[it2->first].color,black)){
                    fallingbricks[it2->first].status=0;
                    gameover=1;

                }
                cout <<" FallingBrickCaptured " << endl;

            }
        }

    }
    if(name=="greenbox"){
        for(map<string,object>::iterator it2=fallingbricks.begin();it2!=fallingbricks.end();it2++){
            object col_object=fallingbricks[it2->first];
            object my_object=boxes["green"];
            if(col_object.status==0)
                continue;
            if((checkCollisionRight(col_object,my_object)) || (checkCollisionLeft(col_object,my_object)) || (checkCollisionTop(col_object,my_object)) || (checkCollisionBottom(col_object,my_object))){
                fallingbricks[it2->first].status=0;
                out1("greencollision");
                if(comparecolors(fallingbricks[it2->first].color,lightgreen)){
                    out1("fallengreen");
                    score+=scorecatchsamecolor;
                }else if(comparecolors(fallingbricks[it2->first].color,red)){
                    score-=scorecatchdiffcolor;

                }else if(comparecolors(fallingbricks[it2->first].color,black)){
                    fallingbricks[it2->first].status=0;
                    gameover=1;

                }else{
                    gameover=1;
                }
                cout <<" FallingBrickCaptured " << endl;

            }
        }

    }
    if(name=="cannonball"){
        for(map<string,object>::iterator it1=cannonbullets.begin();it1!=cannonbullets.end();it1++){


        for(map<string,object>::iterator it2=fallingbricks.begin();it2!=fallingbricks.end();it2++){
            object col_object=fallingbricks[it2->first];
            object my_object=cannonbullets[it1->first];
            if(col_object.status==0)
                continue;
            if(cannonbullets[it1->first].status &&((checkCollisionRight(col_object,my_object)) || (checkCollisionLeft(col_object,my_object)) || (checkCollisionTop(col_object,my_object)) || (checkCollisionBottom(col_object,my_object)))){

                fallingbricks[it2->first].status=0;
                cannonbullets[it1->first].status=0;
                cout <<" FallingBrickCapturedbybullet " << endl;
                if(comparecolors(fallingbricks[it2->first].color,black))
                    score+=scoreshootblack;
                else{
                    score-=scoreshootnonblack;
                    countnonblackshoot+=1;
                    if(maxnonblackshoot<countnonblackshoot){
                        gameover=1;
                    }
                }


            }
        }

    }}
    if(name=="mirror"){
        for(map<string,object>::iterator it1=mirrorobjects.begin();it1!=mirrorobjects.end();it1++){

        for(map<string,object>::iterator it2=cannonbullets.begin();it2!=cannonbullets.end();it2++){
            object col_object=cannonbullets[it2->first];
            object my_object=mirrorobjects[it1->first];
            if(col_object.status==0)
                continue;
            if(!cannonbullets[it2->first].collisionedmirrors[mirrorobjects[it1->first].name]){
            if(( checkCollisionRight(col_object,my_object)) || (checkCollisionLeft(col_object,my_object)) || (checkCollisionTop(col_object,my_object)) || (checkCollisionBottom(col_object,my_object))){
                out1(cannonbullets[it2->first].angle);
                out1(it1->first);
                cannonbullets[it2->first].collisionedmirrors[mirrorobjects[it1->first].name]==1;
                cannonbullets[it2->first].angle=2*my_object.angle-col_object.angle;

                cout <<" FallingBrickCaptured yaya" << endl;

            }
            }        }

    }}

}
void display_string(float x,float y,char *str,float fontScaleValue){
    glm::vec3 fontColor = glm::vec3(0,0,0);
    Matrices.view = glm::lookAt(glm::vec3(0,0,3), glm::vec3(0,0,0), glm::vec3(0,1,0));
    Matrices.model = glm::mat4(1.0f);
    glm::mat4 translateText = glm::translate(glm::vec3(x,y,0));
    glm::mat4 scaleText = glm::scale(glm::vec3(fontScaleValue,fontScaleValue,fontScaleValue));
    Matrices.model *= (translateText * scaleText);
    glm::mat4 MVP = Matrices.projection * Matrices.view * Matrices.model;
    glUniformMatrix4fv(GL3Font.fontMatrixID, 1, GL_FALSE, &MVP[0][0]);
    glUniform3fv(GL3Font.fontColorID, 1, &fontColor[0]);
    GL3Font.font->Render(str);

}
// Creates the triangle object used in this sample code
struct object createTriangle (string name, float weight, COLOR color, float x[], float y[], string component, int fill)
{
    /* ONLY vertices between the bounds specified in glm::ortho will be visible on screen */

    /* Define vertex array as used in glBegin (GL_TRIANGLES) */
    float xc=(x[0]+x[1]+x[2])/3;
    float yc=(y[0]+y[1]+y[2])/3;
    GLfloat vertex_buffer_data [] = {
            x[0]-xc,y[0]-yc,0, // vertex 0
            x[1]-xc,y[1]-yc,0, // vertex 1
            x[2]-xc,y[2]-yc,0 // vertex 2
    };

    GLfloat color_buffer_data [] = {
            color.r,color.g,color.b, // color 1
            color.r,color.g,color.b, // color 2
            color.r,color.g,color.b // color 3
    };

    // create3DObject creates and returns a handle to a VAO that can be used later
    VAO *triangle;
    if(fill==1)
        triangle=create3DObject(GL_TRIANGLES, 3, vertex_buffer_data, color_buffer_data, GL_FILL);
    else
        triangle=create3DObject(GL_TRIANGLES, 3, vertex_buffer_data, color_buffer_data, GL_LINE);
    object vishsprite = {};
    vishsprite.color = color;
    vishsprite.name = name;
    vishsprite.object = triangle;
    vishsprite.x=(x[0]+x[1]+x[2])/3; //Position of the sprite is the position of the centroid
    vishsprite.y=(y[0]+y[1]+y[2])/3;
    vishsprite.height=-1; //Height of the sprite is undefined
    vishsprite.width=-1; //Width of the sprite is undefined
    vishsprite.status=1;
    if(component=="fallingbrick")
        fallingbricks[name]=vishsprite;
    else
        objects[name]=vishsprite;
    return vishsprite;
}

// Creates the rectangle object used in this sample code
struct object createRectangle (string name, float weight, COLOR colorA, COLOR colorB, COLOR colorC, COLOR colorD, float x, float y, float height, float width, string component)
{
    // GL3 accepts only Triangles. Quads are not supported
    float w=width/2,h=height/2;
    GLfloat vertex_buffer_data [] = {
            -w,-h,0, // vertex 1
            -w,h,0, // vertex 2
            w,h,0, // vertex 3

            w,h,0, // vertex 3
            w,-h,0, // vertex 4
            -w,-h,0  // vertex 1
    };

    GLfloat color_buffer_data [] = {
            colorA.r,colorA.g,colorA.b, // color 1
            colorB.r,colorB.g,colorB.b, // color 2
            colorC.r,colorC.g,colorC.b, // color 3

            colorC.r,colorC.g,colorC.b, // color 4
            colorD.r,colorD.g,colorD.b, // color 5
            colorA.r,colorA.g,colorA.b // color 6
    };

    // create3DObject creates and returns a handle to a VAO that can be used later
    VAO *rectangle = create3DObject(GL_TRIANGLES, 6, vertex_buffer_data, color_buffer_data, GL_FILL);
    object vishsprite = {};
    vishsprite.color = colorA;
    vishsprite.name = name;
    vishsprite.object = rectangle;
    vishsprite.x=x;
    vishsprite.y=y;
    vishsprite.height=height;
    vishsprite.width=width;
    vishsprite.x_speed=0;
    vishsprite.y_speed=0;
    vishsprite.status=1;
    vishsprite.angle=0;
    if(component=="fallingbrick")
        fallingbricks[name]=vishsprite;
    if(component=="box"){
        boxes[name]=vishsprite;
    }
    if(component=="cannon")
        cannonObjects[name]=vishsprite;
    if(component=="mirror")
        mirrorobjects[name]=vishsprite;
    return vishsprite;
}

struct object createCircle (string name, float weight, COLOR color, float x, float y, float r, int NoOfParts, string component, int fill)
{
    int parts = NoOfParts;
    float radius = r;
    GLfloat vertex_buffer_data[parts*9];
    GLfloat color_buffer_data[parts*9];
    int i,j;
    float angle=(2*M_PI/parts);
    float current_angle = 0;
    for(i=0;i<parts;i++){
        for(j=0;j<3;j++){
            color_buffer_data[i*9+j*3]=color.r;
            color_buffer_data[i*9+j*3+1]=color.g;
            color_buffer_data[i*9+j*3+2]=color.b;
        }
        vertex_buffer_data[i*9]=0;
        vertex_buffer_data[i*9+1]=0;
        vertex_buffer_data[i*9+2]=0;
        vertex_buffer_data[i*9+3]=radius*cos(current_angle);
        vertex_buffer_data[i*9+4]=radius*sin(current_angle);
        vertex_buffer_data[i*9+5]=0;
        vertex_buffer_data[i*9+6]=radius*cos(current_angle+angle);
        vertex_buffer_data[i*9+7]=radius*sin(current_angle+angle);
        vertex_buffer_data[i*9+8]=0;
        current_angle+=angle;
    }
    VAO* circle;
    if(fill==1)
        circle = create3DObject(GL_TRIANGLES, (parts*9)/3, vertex_buffer_data, color_buffer_data, GL_FILL);
    else
        circle = create3DObject(GL_TRIANGLES, (parts*9)/3, vertex_buffer_data, color_buffer_data, GL_LINE);
    object vishsprite = {};
    vishsprite.color = color;
    vishsprite.name = name;
    vishsprite.object = circle;
    vishsprite.x=x;
    vishsprite.y=y;
    vishsprite.height=2*r; //Height of the sprite is 2*r
    vishsprite.width=2*r; //Width of the sprite is 2*r
    vishsprite.status=1;
    vishsprite.inAir=0;
    vishsprite.x_speed=0;
    vishsprite.y_speed=0;
    vishsprite.radius=r;
    vishsprite.r=r;
    if(component =="cannon")
        cannonObjects[name]=vishsprite;
    if(component=="boxcircle")
        boxcircle[name]=vishsprite;
    return vishsprite;
}
VAO* rectangle;
void createBackground(GLuint textureID)
{

    // GL3 accepts only Triangles. Quads are not supported
    static const GLfloat vertex_buffer_data [] = {
            XMIN,YMIN,0, // vertex 1
            XMAX,YMIN,0, // vertex 2
            XMAX, YMAX,0, // vertex 3

            XMAX, YMAX,0, // vertex 3
            XMIN, YMAX,0, // vertex 4
            XMIN,YMIN,0  // vertex 1
    };

    static const GLfloat color_buffer_data [] = {
            1,0,0, // color 1
            0,0,1, // color 2
            0,1,0, // color 3

            0,1,0, // color 3
            0.3,0.3,0.3, // color 4
            1,0,0  // color 1
    };

    // Texture coordinates start with (0,0) at top left of the image to (1,1) at bot right
    static const GLfloat texture_buffer_data [] = {
            0,1, // TexCoord 1 - bot left
            1,1, // TexCoord 2 - bot right
            1,0, // TexCoord 3 - top right

            1,0, // TexCoord 3 - top right
            0,0, // TexCoord 4 - top left
            0,1  // TexCoord 1 - bot left
    };

    // create3DTexturedObject creates and returns a handle to a VAO that can be used later
    rectangle = create3DTexturedObject(GL_TRIANGLES, 6, vertex_buffer_data, texture_buffer_data, textureID, GL_FILL);
}

inline float get_dist(float x,float y,float x1,float y1){
    return sqrt((x-x1)*(x-x1)+(y-y1)*(y-y1));
}
bool check_collision(object a,object b){
    float dist = get_dist(a.x,a.y,b.x,b.y);

    float exp1= a.radius+b.radius;
    //out2(dist,exp1);
    if(dist < exp1) return true;
    return false;
}
bool check_collision_rect(object a,object b){
    if((abs(a.x - b.x )<a.radius_x+b.radius_x) && (abs(a.y-b.y)<a.radius_y+b.radius_y)) return true;
    return false;
}
float camera_rotation_angle = 90;
float rectangle_rotation = 0;
float triangle_rotation = 0;
void drawObject(object sampleobject)
{
    glm::mat4 VP = Matrices.projection * Matrices.view;
    glm::mat4 MVP;  // MVP = Projection * View * Model

    Matrices.model = glm::mat4(1.0f);

    glm::mat4 ObjectTransform;
    glm::mat4 translateObject = glm::translate (glm::vec3(sampleobject.x, sampleobject.y, 0.0f)); // glTranslatef
    glm::mat4 rotateTriangle = glm::rotate((float)((sampleobject.angle)*M_PI/180.0f), glm::vec3(0,0,1));  // rotate about vector (1,0,0)
    ObjectTransform=translateObject*rotateTriangle;
    Matrices.model *= ObjectTransform;
    MVP = VP * Matrices.model; // MVP = p * V * M

    glUniformMatrix4fv(Matrices.MatrixID, 1, GL_FALSE, &MVP[0][0]);

    draw3DObject(sampleobject.object);
}

/* Render the scene with openGL */
/* Edit this function according to your assignment */
void draw (GLFWwindow* window, int width, int height)
{
    reshapeWindow (window, width, height);
  // clear the color and depth in the frame buffer
  glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  // use the loaded shader program
  // Don't change unless you know what you are doing
  glUseProgram (programID);
  // Eye - Location of camera. Don't change unless you are sure!!
  glm::vec3 eye ( 5*cos(camera_rotation_angle*M_PI/180.0f), 0, 5*sin(camera_rotation_angle*M_PI/180.0f) );
  // Target - Where is the camera looking at.  Don't change unless you are sure!!
  glm::vec3 target (0, 0, 0);
  // Up - Up vector defines tilt of camera.  Don't change unless you are sure!!
  glm::vec3 up (0, 1, 0);
  // Compute Camera matrix (view)
  // Matrices.view = glm::lookAt( eye, target, up ); // Rotating Camera for 3D
  //  Don't change unless you are sure!!
  Matrices.view = glm::lookAt(glm::vec3(0,0,3), glm::vec3(0,0,0), glm::vec3(0,1,0)); // Fixed camera for 2D (ortho) in XY plane
    glm::mat4 VP = Matrices.projection * Matrices.view;

    // Send our transformation to the currently bound shader, in the "MVP" uniform
    // For each model you render, since the MVP will be different (at least the M part)
    //  Don't change unless you are sure!!
    glm::mat4 MVP;	// MVP = Projection * View * Model

    glUseProgram(textureProgramID);

    // Pop matrix to undo transformations till last push matrix instead of recomputing model matrix
    // glPopMatrix ();
    Matrices.model = glm::mat4(1.0f);

    glm::mat4 translateRectangle = glm::translate (glm::vec3(2, 0, 0));        // glTranslatef
    glm::mat4 rotateRectangle = glm::rotate((float)(rectangle_rotation*M_PI/180.0f), glm::vec3(0,0,1)); // rotate about vector (-1,1,1)
    Matrices.model *= (translateRectangle * rotateRectangle);
    MVP = VP * Matrices.model;

    // Copy MVP to texture shaders
    glUniformMatrix4fv(Matrices.TexMatrixID, 1, GL_FALSE, &MVP[0][0]);

    // Set the texture sampler to access Texture0 memory
    glUniform1i(glGetUniformLocation(textureProgramID, "texSampler"), 0);

    // draw3DObject draws the VAO given to it using current MVP matrix
    draw3DTexturedObject(rectangle);

    glUseProgram (programID);

    // Load identity to model matrix


    // draw3DObject draws the VAO given to it using current MVP matrix

    // Pop matrix to undo transformations till last push matrix instead of recomputing model matrix
// glPopMatrix ();
if(!gamestart){
    float fontScaleValue = 76 ;
    glm::vec3 fontColor = glm::vec3(1,0,0);
    glUseProgram(fontProgramID);
    char score_str[30];
    sprintf(score_str,"BRICK BREAKER");
    display_string(0,0,score_str,fontScaleValue);
    sprintf(score_str,"INSTRUCTIONS:-");
    display_string(0,20,score_str,fontScaleValue);
    sprintf(score_str,"TO CONTINUE PRESS C");
    display_string(-100,100,score_str,fontScaleValue);


}
    if(gamestart && !gameover){
  // Compute ViewProject matrix as view/camera might not be changed for this frame (basic scenario)
  //  Don't change unless you are sure!!
    for(map<string,object>::iterator it=fallingbricks.begin();it!=fallingbricks.end();it++){
        string current = it->first; //The name of the current object
        if(fallingbricks[current].status)
            drawObject(fallingbricks[current]);
    }
    for(map<string,object>::iterator it=mirrorobjects.begin();it!=mirrorobjects.end();it++){
        string current = it->first; //The name of the current object
        if(mirrorobjects[current].status)
            drawObject(mirrorobjects[current]);
    }

    for(map<string,object>::iterator it=boxes.begin();it!=boxes.end();it++){
        string current = it->first; //The name of the current object
        drawObject(boxes[current]);
    }
        for(map<string,object>::iterator it=boxcircle.begin();it!=boxcircle.end();it++){
            string current = it->first; //The name of the current object
            Matrices.model = glm::mat4(1.0f);

            glm::mat4 ObjectTransform;
            glm::mat4 translateObject = glm::translate (glm::vec3(boxes[it->first].x, boxes[it->first].y+boxes[it->first].height/2, 0.0f)); // glTranslatef
            glm::mat4 rotateTriangle = glm::rotate((float)((0)*M_PI/180.0f), glm::vec3(1,0,0));  // rotate about vector (1,0,0)
            ObjectTransform=translateObject*rotateTriangle;
            Matrices.model *= ObjectTransform;
            MVP = VP * Matrices.model; // MVP = p * V * M

            glUniformMatrix4fv(Matrices.MatrixID, 1, GL_FALSE, &MVP[0][0]);

            draw3DObject(boxcircle[current].object);


        }
    for(map<string,object>::iterator it=cannonObjects.begin();it!=cannonObjects.end();it++){
        string current = it->first; //The name of the current object
        if(cannonObjects[current].status==0)
            continue;
        glm::mat4 VP = Matrices.projection * Matrices.view;
        glm::mat4 MVP;  // MVP = Projection * View * Model


        Matrices.model = glm::mat4(1.0f);

        /* Render your scene */
        glm::mat4 ObjectTransform;
        glm::mat4 translateObject = glm::translate (glm::vec3(cannonObjects[current].x, cannonObjects[current].y, 0.0f)); // glTranslatef
        float x_diff,y_diff;
        x_diff=(cannonObjects["cannoncircle"].x-cannonObjects[current].x);
        y_diff=cannonObjects["cannoncircle"].y-cannonObjects[current].y;

        glm::mat4 translateObject1 = glm::translate (glm::vec3(x_diff, y_diff, 0.0f)); // glTranslatef
        glm::mat4 rotateTriangle = glm::rotate((float)((cannonObjects[current].angle)*M_PI/180.0f), glm::vec3(0,0,1));  // rotate about vector (1,0,0)

        ObjectTransform=translateObject*translateObject1*rotateTriangle;
        Matrices.model *= ObjectTransform;
        MVP = VP * Matrices.model; // MVP = p * V * M


        glUniformMatrix4fv(Matrices.MatrixID, 1, GL_FALSE, &MVP[0][0]);

        draw3DObject(cannonObjects[current].object);
        //glPopMatrix ();
    }
    for(map<string,object>::iterator it=cannonbullets.begin();it!=cannonbullets.end();it++){
        string current = it->first; //The name of the current object
        if(cannonbullets[current].status==0)
            continue;
        glm::mat4 VP = Matrices.projection * Matrices.view;
        glm::mat4 MVP;  // MVP = Projection * View * Model

            Matrices.model = glm::mat4(1.0f);

        /* Render your scene */
        glm::mat4 ObjectTransform;
        glm::mat4 translateObject = glm::translate (glm::vec3(cannonbullets[current].x, cannonbullets[current].y, 0.0f)); // glTranslatef
        float x_diff,y_diff;
        glm::mat4 translateObject1 = glm::translate (glm::vec3(x_diff, y_diff, 0.0f)); // glTranslatef
        glm::mat4 rotateTriangle = glm::rotate((float)((cannonbullets[current].angle)*M_PI/180.0f), glm::vec3(0,0,1));  // rotate about vector (1,0,0)
        glm::mat4 translateObject2 = glm::translate (glm::vec3(-x_diff, -y_diff, 0.0f)); // glTranslatef
        ObjectTransform=translateObject*translateObject1*translateObject2*rotateTriangle;
        Matrices.model *= ObjectTransform;
        MVP = VP * Matrices.model; // MVP = p * V * M

        glUniformMatrix4fv(Matrices.MatrixID, 1, GL_FALSE, &MVP[0][0]);

        draw3DObject(cannonbullets[current].object);
        //glPopMatrix ();
    }
    if(redleft)
        checkCollision("redbox",-1*boxspeed,0);
    if(redright)
        checkCollision("redbox",1*boxspeed,0);
    if(greenleft)
        checkCollision("greenbox",-1*boxspeed,0);
    if(greenright)
        checkCollision("greenbox",1*boxspeed,0);
    if(!redleft && !redright){
        checkCollision("redbox",0,0);
    }

    if(!greenleft && !greenright)
        checkCollision("greenbox",0,0);
    checkCollision("cannonball",0,0);
    checkCollision("mirror",0,0);


    float fontScaleValue = 36 ;
    glm::vec3 fontColor = glm::vec3(0,0,0);
    glUseProgram(fontProgramID);
    char score_str[50];
    sprintf(score_str,"Your present score %lf brick speed %d",score,fallingbricksspeed);
    display_string(-100,300 - 30,score_str,fontScaleValue);

    if(mouseleftpressed){
        mouseleft(window);
    }
    mouseright(window);
        if(cannonObjects["cannoncircle"].x!=-400 && lastreleasebullet<glfwGetTime()-0.5)
            cannonObjects["cannoncircle"].x=-400;
}
    if(gameover){
        float fontScaleValue = 76 ;
        glm::vec3 fontColor = glm::vec3(1,0,0);
        glUseProgram(fontProgramID);
        char score_str[30];
        sprintf(score_str,"BRICK BREAKER");
        display_string(0,0,score_str,fontScaleValue);
        display_string(10,10,score_str,fontScaleValue);
        sprintf(score_str,"Your final score %lf",score);
        display_string(-100,100,score_str,fontScaleValue);

    }
  // Increment angles
  float increments = 1;

  //camera_rotation_angle++; // Simulating camera rotation
}

/* Initialise glfw window, I/O callbacks and the renderer to use */
/* Nothing to Edit here */
GLFWwindow* initGLFW (int width, int height)
{
    GLFWwindow* window; // window desciptor/handle

    glfwSetErrorCallback(error_callback);
    if (!glfwInit()) {
//        exit(EXIT_FAILURE);
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    window = glfwCreateWindow(width, height, "Sample OpenGL 3.3 Application", NULL, NULL);

    if (!window) {
        glfwTerminate();
//        exit(EXIT_FAILURE);
    }

    glfwMakeContextCurrent(window);
    gladLoadGLLoader((GLADloadproc) glfwGetProcAddress);
    glfwSwapInterval( 1 );

    /* --- register callbacks with GLFW --- */

    /* Register function to handle window resizes */
    /* With Retina display on Mac OS X GLFW's FramebufferSize
     is different from WindowSize */
    glfwSetFramebufferSizeCallback(window, reshapeWindow);
    glfwSetWindowSizeCallback(window, reshapeWindow);

    /* Register function to handle window close */
    glfwSetWindowCloseCallback(window, quit);

    /* Register function to handle keyboard input */
    glfwSetKeyCallback(window, keyboard);      // general keyboard input
    glfwSetCharCallback(window, keyboardChar);  // simpler specific character handling

    /* Register function to handle mouse click */
    glfwSetMouseButtonCallback(window, mouseButton);  // mouse button clicks
    glfwSetScrollCallback(window, mousescroll);

    return window;
}

/* Initialize the OpenGL rendering properties */
/* Add all the models to be created here */
void initGL (GLFWwindow* window, int width, int height)
{glEnable(GL_BLEND);
    createRectangle("red",10000,red,red,red,red,50,-275,50,50,"box");
    createCircle("red",10000,red,0,0,25,50,"boxcircle",1);
    createCircle("green",10000,lightgreen,0,0,25,50,"boxcircle",1);
    createRectangle("green",10000,lightgreen,lightgreen,lightgreen,lightgreen,-50,-275,50,50,"box");
    createRectangle("cannonrect",10000,blue,blue,blue,blue,0,0,30,200,"cannon");
    createCircle("cannoncircle",10000,blue,-400,0,30,15,"cannon",1);

    createRectangle("mirror2",10000,grey,grey,grey,grey,0,250,14,100,"mirror");
    createRectangle("mirror3",10000,grey,grey,grey,grey,300,-200,14,100,"mirror");
    createRectangle("mirror4",10000,grey,grey,grey,grey,300,270,14,100,"mirror");

    mirrorobjects["mirror2"].angle=150;

    mirrorobjects["mirror3"].angle=70;
    mirrorobjects["mirror4"].angle=150;
    glActiveTexture(GL_TEXTURE0);
    // load an image file directly as a new OpenGL texture
    // GLuint texID = SOIL_load_OGL_texture ("beach.png", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_TEXTURE_REPEATS); // Buggy for OpenGL3
    GLuint textureID = createTexture("background.png");
    // check for an error during the load process
    if(textureID == 0 )
        cout << "SOIL loading error: '" << SOIL_last_result() << "'" << endl;

    // Create and compile our GLSL program from the texture shaders
    textureProgramID = LoadShaders( "TextureRender.vert", "TextureRender.frag" );
    // Get a handle for our "MVP" uniform
    Matrices.TexMatrixID = glGetUniformLocation(textureProgramID, "MVP");
    createBackground(textureID);

    // Create and compile our GLSL program from the shaders
	programID = LoadShaders( "Sample_GL.vert", "Sample_GL.frag" );
	// Get a handle for our "MVP" uniform
	Matrices.MatrixID = glGetUniformLocation(programID, "MVP");

	
	reshapeWindow (window, width, height);

    // Background color of the scene
	glClearColor (0.3f, 0.3f, 0.3f, 0.0f); // R, G, B, A
	glClearDepth (1.0f);

	glEnable (GL_DEPTH_TEST);
	glDepthFunc (GL_LEQUAL);

    const char* fontfile = "/usr/share/fonts/truetype/lato/Lato-Italic.ttf";
    GL3Font.font = new FTExtrudeFont(fontfile); // 3D extrude style rendering

    if(GL3Font.font->Error())
    {
        cout << "Error: Could not load font `" << fontfile << "'" << endl;
        glfwTerminate();
        exit(EXIT_FAILURE);
    }

    // Create and compile our GLSL program from the font shaders
    fontProgramID = LoadShaders( "fontrender.vert", "fontrender.frag" );
    GLint fontVertexCoordAttrib, fontVertexNormalAttrib, fontVertexOffsetUniform;
    fontVertexCoordAttrib = glGetAttribLocation(fontProgramID, "vertexPosition");
    fontVertexNormalAttrib = glGetAttribLocation(fontProgramID, "vertexNormal");
    fontVertexOffsetUniform = glGetUniformLocation(fontProgramID, "pen");
    GL3Font.fontMatrixID = glGetUniformLocation(fontProgramID, "MVP");
    GL3Font.fontColorID = glGetUniformLocation(fontProgramID, "fontColor");

    GL3Font.font->ShaderLocations(fontVertexCoordAttrib, fontVertexNormalAttrib, fontVertexOffsetUniform);
    GL3Font.font->FaceSize(1);
    GL3Font.font->Depth(0);
    GL3Font.font->Outset(0, 0);
    GL3Font.font->CharMap(ft_encoding_unicode);
    cout << "VENDOR: " << glGetString(GL_VENDOR) << endl;
    cout << "RENDERER: " << glGetString(GL_RENDERER) << endl;
    cout << "VERSION: " << glGetString(GL_VERSION) << endl;
    cout << "GLSL: " << glGetString(GL_SHADING_LANGUAGE_VERSION) << endl;
}
int randomgenerator(int min,int max){
    return rand()%(max-min + 1) + min;
}
int main (int argc, char** argv)
{
	int width = 600;
	int height = 600;

    GLFWwindow* window = initGLFW(width, height);
    thread(play_audio,"audio/background.mp3").detach();
	initGL (window, width, height);

    double last_update_time = glfwGetTime(), current_time;
    double last_update1_time = glfwGetTime();
    double last_update2_time = glfwGetTime();
    int t=0;
    /* Draw in loop */
    while (!glfwWindowShouldClose(window)) {

        // OpenGL Draw commands
        draw(window,width,height);
        // Swap Frame Buffer in double buffering
        glfwSwapBuffers(window);

        // Poll for Keyboard and mouse events
        glfwPollEvents();

        // Control based on time (Time based transformation like 5 degrees rotation every 0.5s)
        current_time = glfwGetTime(); // Time in seconds
        if ((current_time - last_update1_time) >= (2*12/max(min(fallingbricksspeed,13),10)) ){
            last_update1_time = current_time;
            int colorpicker = randomgenerator(0, 2);
            int xpicker=randomgenerator(-200, 400);
            out1(xpicker);
            while((xpicker>-50 && xpicker<50) || (xpicker>250 && xpicker<350)) {

                xpicker = randomgenerator(-200, 400);
            }

            if (colorpicker == 0) {
                createRectangle("fallingbrick" + to_string(current_time), 10000, red, red, red, red,
                                xpicker, 295, 10, 10, "fallingbrick");
            } else if (colorpicker == 1) {
                createRectangle("fallingbrick" + to_string(current_time), 10000, black, black, black, black,
                                xpicker, 295, 10, 10, "fallingbrick");
            } else {
                createRectangle("fallingbrick" + to_string(current_time), 10000, lightgreen, lightgreen, lightgreen,
                                lightgreen, xpicker, 295, 10, 10, "fallingbrick");
            }

        }
        if ((current_time - last_update_time) >= 0.005) { // atleast 0.5s elapsed since last frame
            // do something every 0.5 seconds ..

            last_update_time = current_time;


            for (map<string, object>::iterator it = fallingbricks.begin(); it != fallingbricks.end(); it++) {
                string current = it->first; //The name of the current object
                fallingbricks[current].y -= fallingbricksspeed/10;

            }

            for(map<string,object>::iterator it=cannonbullets.begin();it!=cannonbullets.end();it++){
                string current = it->first; //The name of the current object

                cannonbullets[current].x+=(floor(10*cos(cannonbullets[current].angle*(M_PI/180))*100000)/100000)*1;
                cannonbullets[current].y+=(floor(10*sin(cannonbullets[current].angle*(M_PI/180))*100000)/100000)*1;

            }
        }

        if ((current_time - last_update2_time) >= 0.05) { // atleast 0.5s elapsed since last frame
            // do something every 0.5 seconds ..

            last_update2_time = current_time;
            if (redleft == 1) {
                boxes[boxes.find("red")->first].x -= boxspeed;
            }
            if (redright == 1) {
                boxes[boxes.find("red")->first].x += boxspeed;
            }
            if (greenleft == 1) {
                boxes[boxes.find("green")->first].x -= boxspeed;
            }
            if (greenright == 1) {
                boxes[boxes.find("green")->first].x += boxspeed;
            }
        }
    }

    glfwTerminate();
//    exit(EXIT_SUCCESS);
}
