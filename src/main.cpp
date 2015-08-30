#include "cv_commons.hpp"

using namespace cv;
using namespace std;

int main( int argc, char** argv )
{
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

    double dWidth = cap.get(CV_CAP_PROP_FRAME_WIDTH); //get the width of frames of the video
    double dHeight = cap.get(CV_CAP_PROP_FRAME_HEIGHT); //get the height of frames of the video

    cout << "Frame size : " << dWidth << " x " << dHeight << endl;

//    namedWindow("Keypoints",CV_WINDOW_AUTOSIZE);

    Mat frame;
    Mat grayFrame;
    Mat faceFrame;
    vector<Rect> faces;
    vector<Rect> eyes;
    while (1)
    {

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
          Mat faceROI = frame(faces[i]);
          eyes_cascade.detectMultiScale(faceROI, eyes);
          for (int j = 0; j < eyes.size(); j++)
          {
            Point faceCorner = faces[i].tl();
            Rect eye = eyes[j];
            rectangle(faceFrame, faceCorner + eye.tl(), faceCorner + eye.br(), Scalar(0, 255, 0));
          }
        }

        imshow("Face", faceFrame);

        if (waitKey(30) == 27) //wait for 'esc' key press for 30ms. If 'esc' key is pressed, break loop
        {
            cout << "esc key is pressed by user" << endl;
            break;
        }

    }
}
