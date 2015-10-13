#include "cv_commons.hpp"
#include "gl_commons.hpp"
#include "Vao.hpp"

#include <boost/ptr_container/ptr_vector.hpp>

#define HEIGHT 480
#define WIDTH 640
#define VERTEX_SHADER_FILE PROJECT_DIRECTORY "/src/VertexShader.cpp"
#define FRAGMENT_SHADER_FILE PROJECT_DIRECTORY "/src/FragmentShader.cpp"
#define POS_ATTR_INDEX 0
#define SAMPLE_MODEL "/models/cornell_box.obj"

using namespace cv;
using namespace glm;
using namespace std;


int main( int argc, char** argv )
{
    //cout << getBuildInformation() << endl;
    Ptr<ORB> orbDetector;
    orbDetector = ORB::create();
    vector<KeyPoint> keypoints;
    VideoCapture cap(0); // open the video camera no. 0

    //String face_cascade_name = String(PROJECT_DIRECTORY) + String("/data/haarcascades/haarcascade_frontalface_alt.xml");
    String eyes_cascade_name = String(PROJECT_DIRECTORY) + String("/data/haarcascades/haarcascade_eye_tree_eyeglasses.xml");
    String face_cascade_name = String(PROJECT_DIRECTORY) + String("/data/lbpcascades/lbpcascade_frontalface.xml");
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

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);

    std::vector<GLfloat> triangle_position_data = {
        -1.0f, -1.0f, 0.0f,
        1.0f, -1.0f, 0.0f,
        0.0f,  1.0f, 0.0f
    };
    Vao* triangle = new Vao();
    triangle->init(triangle_position_data);

    String model_file = String(PROJECT_DIRECTORY) + String(SAMPLE_MODEL);
    if (argc == 2)
    {
        printf("Input model: %s\n", argv[1]);
        model_file = argv[1];
    }
    Assimp::Importer importer;
    const aiScene* scene = importer.ReadFile(model_file,
            aiProcess_CalcTangentSpace |
            aiProcess_Triangulate |
            aiProcess_GenNormals |
            aiProcess_JoinIdenticalVertices |
            aiProcess_SortByPType);
    if (!scene)
    {
        cout << importer.GetErrorString() << endl;
        return -1;
    }
    boost::ptr_vector<Vao> meshes;
    for (unsigned int i = 0; i < scene->mNumMeshes; i++)
    {
        aiMesh* mesh = scene->mMeshes[i];
        std::vector<GLfloat> positions;
        for (unsigned int j = 0; j < mesh->mNumVertices; j++)
        {
            aiVector3D vert = mesh->mVertices[j];
            positions.push_back(vert.x);
            positions.push_back(vert.y);
            positions.push_back(vert.z);
            //printf("(%f, %f, %f)\n", vert.x, vert.y, vert.z);
        }
        std::vector<GLuint> indices;
        for (unsigned int j = 0; j < mesh->mNumFaces; j++)
        {
            aiFace face = mesh->mFaces[j];
            for (unsigned int k = 0; k < face.mNumIndices; k++)
            {
                //printf("Indice = %d\n", face.mIndices[k]);
                indices.push_back(face.mIndices[k]);
            }
        }
        Vao* vao = new Vao();
        vao->init(positions, indices);
        meshes.push_back(vao);
        printf("Primitive Type = %d, # of vertices = %d, # of faces = %d, # of indices = %ld\n",
                mesh->mPrimitiveTypes, mesh->mNumVertices, mesh->mNumFaces, indices.size());
    }

    glClearColor(0.0f, 0.0f, 0.4f, 0.0f);
    GLuint programID = LoadShaders( VERTEX_SHADER_FILE, FRAGMENT_SHADER_FILE );
    // Get a handle for our "MVP" uniform.
    // Only at initialisation time.
    GLuint MatrixID = glGetUniformLocation(programID, "MVP");
    glUseProgram(programID);
    // Projection matrix : 45° Field of View, 4:3 ratio, display range : 0.1 unit <-> 100 units
    glm::mat4 Projection = glm::perspective(45.0f, 4.0f / 3.0f, 0.1f, 100.0f);

    Rect max_face = Rect(0, 0, 0, 0);
    glm::vec2 max_face_center = vec2(0, 0);
    double max_face_area = 0;
    //vector<Rect> eyes;
    while (1)
    {
        Mat frame;
        Mat grayFrame;
        Mat faceFrame;
        vector<Rect> faces;
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
        if (!faces.empty())
        {
            max_face = Rect(0, 0, 0, 0);
            max_face_center = vec2(0, 0);
            max_face_area = 0;
        }

        for (vector<Rect>::iterator it_face = faces.begin(); it_face != faces.end(); it_face++)
        {
            if (it_face->area() > max_face_area)
            {
                max_face = *it_face;
                max_face_area = max_face.area();
                max_face_center = glm::vec2(max_face.x + max_face.width/2, max_face.y + max_face.height/2);
            }
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
        circle(faceFrame, CvPoint(max_face_center.x, max_face_center.y), 5, Scalar(0, 0, 255), -1);
        rectangle(faceFrame, max_face, Scalar(255, 0, 0));
        imshow("Face", faceFrame);

        /********** OpenGL **********/
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        // Camera matrix
        glm::mat4 View = glm::lookAt(
                //Note that the hardware camera flips the world coordinates (it is not a mirror)
                //TODO: Make z coordinates proportional to model size
                glm::vec3(-(max_face_center.x - WIDTH/2) / WIDTH * 5, (-(max_face_center.y - HEIGHT/2)) / HEIGHT * 5, 10), // Camera position (4, 3, 3)
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
        //triangle->draw();

        // Apply other MVP for other objects
        for (unsigned int i = 0; i < meshes.size(); i++)
        {
            meshes[i].draw();
        }
        glfwSwapBuffers(window);
        glfwPollEvents();

        if (waitKey(30) == 27) //wait for 'esc' key press for 30ms. If 'esc' key is pressed, break loop
        {
            cout << "esc key is pressed by user" << endl;
            break;
        }

    }
}
