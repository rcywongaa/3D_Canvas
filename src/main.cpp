#include "cv_commons.hpp"
#include "gl_commons.hpp"
#include "Vao.hpp"

#include <boost/ptr_container/ptr_vector.hpp>

#define HEIGHT 480
#define WIDTH 640
#define VERTEX_SHADER_FILE PROJECT_DIRECTORY "/src/VertexShader.glsl"
#define FRAGMENT_SHADER_FILE PROJECT_DIRECTORY "/src/FragmentShader.glsl"
#define POS_ATTR_INDEX 0
#define SAMPLE_MODEL "/models/CornellBox/CornellBox-Original.obj"

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

    GLuint shader_id = LoadShaders( VERTEX_SHADER_FILE, FRAGMENT_SHADER_FILE );

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
    printf("# of materials = %d, # of lights = %d\n", scene->mNumMaterials, scene->mNumLights);
    boost::ptr_vector<Vao> meshes;
    boost::ptr_vector<Vao> lights;
    for (unsigned int i = 0; i < scene->mNumMeshes; i++)
    {
        aiMesh* mesh = scene->mMeshes[i];
        std::vector<GLfloat> positions;
        std::vector<GLfloat> normals;
        aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
        for (unsigned int j = 0; j < mesh->mNumVertices; j++)
        {
            aiVector3D vert = mesh->mVertices[j];
            positions.push_back(vert.x);
            positions.push_back(vert.y);
            positions.push_back(vert.z);
            if (mesh->mNormals != NULL)
            {
                aiVector3D normal = mesh->mNormals[j];
                normals.push_back(normal.x);
                normals.push_back(normal.y);
                normals.push_back(normal.z);
            }
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
        aiColor3D kd;
        material->Get(AI_MATKEY_COLOR_DIFFUSE, kd);
        aiColor3D ks;
        material->Get(AI_MATKEY_COLOR_SPECULAR, ks);
        aiColor3D ka;
        material->Get(AI_MATKEY_COLOR_AMBIENT, ka);
        aiColor3D ke;
        material->Get(AI_MATKEY_COLOR_EMISSIVE, ke);
        GLfloat shininess;
        material->Get(AI_MATKEY_SHININESS, shininess);
        shininess /= 4.0f; //Assimp bug
        aiString name;
        material->Get(AI_MATKEY_NAME, name);
        vao->init(positions, indices, normals);
        vao->set_shader(shader_id);
        vao->set_ambient(ka.r, ka.g, ka.b);
        vao->set_diffuse(kd.r, kd.g, kd.b);
        vao->set_specular(ks.r, ks.g, ks.b, shininess);
        vao->set_emissive(ke.r, ke.g, ke.b);
        if (vao->is_light()) lights.push_back(vao);
        else meshes.push_back(vao);
        //Beware of non-ASCII or whitespace messing up mtl
        printf("Mesh %d: Primitive Type = %d, # of vertices = %d, # of faces = %d\n",
                i, mesh->mPrimitiveTypes, mesh->mNumVertices, mesh->mNumFaces);
        printf("mtl #%d - %s: ka = (%f,%f,%f), kd = (%f,%f,%f), ks = (%f,%f,%f), ke = (%f,%f,%f), shininess = %f\n",
                mesh->mMaterialIndex, name.C_Str(), ka.r, ka.g, ka.b, kd.r, kd.g, kd.b, ks.r, ks.g, ks.b, ke.r, ke.g, ke.b, shininess);
    }

    if (lights.size() == 0)
    {
        vector<GLfloat> light_position;
        light_position.push_back(1.0f);
        light_position.push_back(2.0f);
        light_position.push_back(3.0f);
        GLfloat light_color[3] = {1.0f, 1.0f, 1.0f};
        printf("No lights, creating one at (%f, %f, %f)\n", light_position[0], light_position[1], light_position[2]);
        Vao* light = new Vao();
        light->init(light_position);
        light->set_shader(shader_id);
        light->set_emissive(light_color[0], light_color[1], light_color[2]);
        lights.push_back(light);
    }

    glClearColor(0.0f, 0.0f, 0.4f, 0.0f);

    // Get a handle for our "MVP" uniform at initialisation time.
    GLuint mvp_id = glGetUniformLocation(shader_id, "MVP");
    GLuint eye_position_id = glGetUniformLocation(shader_id, "eye_position");
    glUseProgram(shader_id);

    // Projection matrix : 45° Field of View, 4:3 ratio, display range : 0.1 unit <-> 100 units
    glm::mat4 Projection = glm::perspective(45.0f, 4.0f / 3.0f, 0.1f, 100.0f);
    glm::vec3 eye_position = glm::vec3(0, 0, 5);

    Rect max_face = Rect(0, 0, 0, 0);
    glm::vec2 max_face_center = vec2(0, 0);
    double max_face_area = 0;
    //vector<Rect> eyes;

#ifdef OPENCL
    cl_platform_id platform_id;
    cl_uint num_platforms;
    cl_char vendor_info[1024] = "NULL";
    cl_device_id device_id;
    cl_uint num_devices;
    cl_char device_info[1024] = "NULL";
    cl_char extensions[1024] = "NULL";
    cl_context context;
    cl_int err;
    cl_program program;
    clGetPlatformIDs(1, &platform_id, &num_platforms);
    clGetPlatformInfo(platform_id, CL_PLATFORM_VENDOR, sizeof(vendor_info), &vendor_info, NULL);
    printf("Platform #%u(%s) out of %u platforms\n", platform_id, vendor_info, num_platforms);
    clGetDeviceIDs(platform_id, CL_DEVICE_TYPE_GPU, 1, &device_id, &num_devices);
    clGetDeviceInfo(device_id, CL_DEVICE_NAME, sizeof(device_info), &device_info, NULL);
    clGetDeviceInfo(device_id, CL_DEVICE_EXTENSIONS, sizeof(extensions), &extensions, NULL);
    printf("Device #%u(%s) out of %u devices\n", device_id, device_info, num_devices);
    //printf("Supported extensions: %s\n", extensions);

   // Create the properties for this context.
    cl_context_properties context_properties[] = {
        // We need to add information about the OpenGL context with
        // which we want to exchange information with the OpenCL context.
        #if defined (WIN32)
        // We should first check for cl_khr_gl_sharing extension.
        CL_GL_CONTEXT_KHR , (cl_context_properties) wglGetCurrentContext() ,
        CL_WGL_HDC_KHR , (cl_context_properties) wglGetCurrentDC() ,
        #elif defined (__linux__)
        // We should first check for cl_khr_gl_sharing extension.
        CL_GL_CONTEXT_KHR , (cl_context_properties) glXGetCurrentContext() ,
        CL_GLX_DISPLAY_KHR , (cl_context_properties) glXGetCurrentDisplay() ,
        #endif
        CL_CONTEXT_PLATFORM , (cl_context_properties) platform_id,
        0 , 0 ,
    };

    context = clCreateContext(context_properties, 1, &device_id, NULL, NULL, &err);
    printf("Create shared context %s\n", err == CL_SUCCESS ? "SUCCESS" : "FAIL");
#endif

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
        eye_position = vec3(-(max_face_center.x - WIDTH/2) / WIDTH * 5, (-(max_face_center.y - HEIGHT/2)) / HEIGHT * 5, 5);
        // Camera matrix
        glm::mat4 View = glm::lookAt(
                //Note that the hardware camera flips the world coordinates (it is not a mirror)
                //TODO: Make z coordinates proportional to model size
                eye_position, // Camera position (4, 3, 3)
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
        glUniformMatrix4fv(mvp_id, 1, GL_FALSE, &MVP[0][0]);
        glUniform3fv(eye_position_id, 1, &eye_position[0]);

        if (lights.size() == 1)
        {
            //TODO: Render lightsource one by one
            lights[0].draw();
            // Apply other MVP for other objects
            for (unsigned int i = 0; i < meshes.size(); i++)
            {
                meshes[i].draw();
            }
        }
        else printf("Only single light source is supported...\n");

        glfwSwapBuffers(window);
        glfwPollEvents();

        if (waitKey(30) == 27) //wait for 'esc' key press for 30ms. If 'esc' key is pressed, break loop
        {
            cout << "esc key is pressed by user" << endl;
            break;
        }

    }
}
