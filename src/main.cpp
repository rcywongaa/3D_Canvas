#include "cv_commons.hpp"

#define HEIGHT 480
#define WIDTH 640
#define VERTEX_SHADER_FILE PROJECT_DIRECTORY "/src/VertexShader.cpp"
#define FRAGMENT_SHADER_FILE PROJECT_DIRECTORY "/src/FragmentShader.cpp"
#define POS_ATTR_INDEX 0

using namespace cv;
using namespace glm;
using namespace std;

GLuint LoadShaders(const char * vertex_file_path,const char * fragment_file_path){

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
    std::vector<char> ProgramErrorMessage( std::max(InfoLogLength, int(1)) );
    glGetProgramInfoLog(ProgramID, InfoLogLength, NULL, &ProgramErrorMessage[0]);
    fprintf(stdout, "%s\n", &ProgramErrorMessage[0]);

    //glDeleteShader(VertexShaderID);
    //glDeleteShader(FragmentShaderID);

    return ProgramID;
}

int main( int argc, char** argv )
{
    //cout << getBuildInformation() << endl;
    GLenum err;
    Ptr<ORB> orbDetector;
    orbDetector = ORB::create();
    vector<KeyPoint> keypoints;
    VideoCapture cap(0); // open the video camera no. 0

    String face_cascade_name = String(PROJECT_DIRECTORY) + String("/data/haarcascades/haarcascade_frontalface_alt.xml");
    String eyes_cascade_name = String(PROJECT_DIRECTORY) + String("/data/haarcascades/haarcascade_eye_tree_eyeglasses.xml");
    CascadeClassifier face_cascade;
    CascadeClassifier eyes_cascade;
    if( !face_cascade.load( face_cascade_name ) ){ printf("--(!)Error loading %s\n", face_cascade_name.c_str()); return -1; };
    if( !eyes_cascade.load( eyes_cascade_name ) ){ printf("--(!)Error loading %s\n", eyes_cascade_name.c_str()); return -1; };

    if (!cap.isOpened())  // if not success, exit program
    {
        cout << "Cannot open the video cam" << endl;
        return -1;
    }

    if ( !glfwInit() )
    {
        cout << "Failed to initialize GLFW" << endl;
        return -1;
    }

    glfwWindowHint(GLFW_SAMPLES, 4); // 4x antialiasing
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3); // We want OpenGL 3.3
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // To make MacOS happy; should not be needed
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); //We don't want the old OpenGL
    GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "3D View", NULL, NULL);
    glfwMakeContextCurrent(window);
    if( window == NULL )
    {
        fprintf( stderr, "Failed to open GLFW window. If you have an Intel GPU, they are not 3.3 compatible. Try the 2.1 version of the tutorials.\n" );
        glfwTerminate();
        return -1;
    }
    //glfwSwapInterval(1);

    glewExperimental = true; // Needed in core profile
    if (glewInit() != GLEW_OK)
    {
        fprintf(stderr, "Error: %s\n", glewGetErrorString(glewInit()));
        return -1;
    }

    //VertexArrayObject stores the VertexBufferObject and VertexAttribArray settings
    GLuint vao_triangle;
    glGenVertexArrays(1, &vao_triangle);
    //All following settings modify the vao_triangle VAO
    glBindVertexArray(vao_triangle);
    GLfloat triangle_position_data[] = {
        -1.0f, -1.0f, 0.0f,
        1.0f, -1.0f, 0.0f,
        0.0f,  1.0f, 0.0f
    };
    GLuint triangle_position_buffer;
    glGenBuffers(1, &triangle_position_buffer);
    // The following commands will talk about our 'triangle_position_buffer' buffer
    glBindBuffer(GL_ARRAY_BUFFER, triangle_position_buffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(triangle_position_data), triangle_position_data, GL_STATIC_DRAW);
    //Configures the interpretation of triangle_position_buffer by vao_triangle
    glVertexAttribPointer(
            POS_ATTR_INDEX,  // attribute 0. No particular reason for 0, but must match the layout in the shader.
            3,                  // size
            GL_FLOAT,           // type
            GL_FALSE,           // normalized?
            0,                  // stride
            NULL                // array buffer offset
            );
    //Use VertexAttribute POS_ATTR_INDEX to interpret data stored in triangle_position_buffer
    glEnableVertexAttribArray(POS_ATTR_INDEX);
    //If using VAO, glDisableVertexAttribArray should never be called
    //Additional buffers & attributes may be required for UV, normal, texture, etc.
    //glBindVertexArray(0); //Optional, clear vao_triangle binding so we don't accidentally modify it any further
    //Additional VAOs for other objects may follow

    glClearColor(0.0f, 0.0f, 0.4f, 0.0f);
    GLuint programID = LoadShaders( VERTEX_SHADER_FILE, FRAGMENT_SHADER_FILE );
    // Get a handle for our "MVP" uniform.
    // Only at initialisation time.
    GLuint MatrixID = glGetUniformLocation(programID, "MVP");
    glUseProgram(programID);
    // Projection matrix : 45° Field of View, 4:3 ratio, display range : 0.1 unit <-> 100 units
    glm::mat4 Projection = glm::perspective(45.0f, 4.0f / 3.0f, 0.1f, 100.0f);

    Mat frame;
    Mat grayFrame;
    Mat faceFrame;
    vector<Rect> faces;
    //vector<Rect> eyes;
    while (1)
    {
        /********** OpenCV **********/
        bool bSuccess = cap.read(frame); // read a new frame from video
        if (!bSuccess) //if not success, break loop
        {
            cout << "Cannot read a frame from video stream" << endl;
            break;
        }
        faceFrame = frame.clone();
        cvtColor(frame, grayFrame, COLOR_BGR2GRAY);
        equalizeHist(grayFrame, grayFrame);
        face_cascade.detectMultiScale(grayFrame, faces);
        for (int i = 0; i < faces.size(); i++)
        {
            rectangle(faceFrame, faces[i], Scalar(255, 0, 0));
/*
            Mat faceROI = frame(faces[i]);
            eyes_cascade.detectMultiScale(faceROI, eyes);
            for (int j = 0; j < eyes.size(); j++)
            {
                Point faceCorner = faces[i].tl();
                Rect eye = eyes[j];
                rectangle(faceFrame, faceCorner + eye.tl(), faceCorner + eye.br(), Scalar(0, 255, 0));
            }
*/
        }
        imshow("Face", faceFrame);

        /********** OpenGL **********/
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        // Camera matrix
        glm::mat4 View = glm::lookAt(
            glm::vec3(4,3,3), // Camera
            glm::vec3(0,0,0), // and looks at the origin
            glm::vec3(0,1,0)  // Head is up (set to 0,-1,0 to look upside-down)
        );
        // Model matrix : an identity matrix (model will be at the origin)
        glm::mat4 Model = glm::mat4(1.0f);  // Changes for each model !
        // Our ModelViewProjection : multiplication of our 3 matrices
        glm::mat4 MVP = Projection * View * Model; // Remember, matrix multiplication is the other way around
        // Send our transformation to the currently bound shader,
        // in the "MVP" uniform
        // For each model you render, since the MVP will be different (at least the M part)
        glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP[0][0]);
        //Use data from vao_triangle draw, i.e. pass to shader
        glBindVertexArray(vao_triangle);
        glDrawArrays(GL_TRIANGLES, 0, 3);
        glfwSwapBuffers(window);
        glfwPollEvents();

        if (waitKey(30) == 27) //wait for 'esc' key press for 30ms. If 'esc' key is pressed, break loop
        {
            cout << "esc key is pressed by user" << endl;
            break;
        }

    }
}
