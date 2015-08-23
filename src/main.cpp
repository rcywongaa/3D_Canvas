#include "cv_commons.hpp"

using namespace cv;
using namespace std;

int main( int argc, char** argv )
{
    Ptr<ORB> orbDetector;
    orbDetector = ORB::create();
    vector<KeyPoint> keypoints;
    VideoCapture cap(0); // open the video camera no. 0

    if (!cap.isOpened())  // if not success, exit program
    {
        cout << "Cannot open the video cam" << endl;
        return -1;
    }

    double dWidth = cap.get(CV_CAP_PROP_FRAME_WIDTH); //get the width of frames of the video
    double dHeight = cap.get(CV_CAP_PROP_FRAME_HEIGHT); //get the height of frames of the video

    cout << "Frame size : " << dWidth << " x " << dHeight << endl;

    namedWindow("MyVideo",CV_WINDOW_AUTOSIZE);
    namedWindow("Keypoints",CV_WINDOW_AUTOSIZE); 

    while (1)
    {
        Mat frame;

        bool bSuccess = cap.read(frame); // read a new frame from video

        if (!bSuccess) //if not success, break loop
        {
            cout << "Cannot read a frame from video stream" << endl;
            break;
        }

        orbDetector->detect(frame, keypoints);
        Mat keypointsFrame;
        drawKeypoints(frame, keypoints, keypointsFrame);

        imshow("MyVideo", frame); //show the frame in "MyVideo" window
        imshow("Keypoints", keypointsFrame); //show the frame in "MyVideo" window

        if (waitKey(30) == 27) //wait for 'esc' key press for 30ms. If 'esc' key is pressed, break loop
        {
            cout << "esc key is pressed by user" << endl;
            break; 
        }

    }
}
