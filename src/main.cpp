#include "cv_commons.hpp"

using namespace cv;
using namespace std;

int main( int argc, char** argv )
{
    if( argc != 2)
    {
        cout <<" Usage: display_image ImageToLoadAndDisplay" << endl;
        return -1;
    }

    Mat image;
    image = imread(argv[1], CV_LOAD_IMAGE_COLOR);   // Read the file

    if(! image.data )                              // Check for invalid input
    {
        cout <<  "Could not open or find the image" << std::endl ;
        return -1;
    }

    char* imageName = argv[1];

    Mat gray_image;
    cvtColor( image, gray_image, CV_BGR2GRAY );

//    imwrite( "../Gray_Image.jpg", gray_image );

    namedWindow( imageName, CV_WINDOW_AUTOSIZE );
    namedWindow( "Gray image", CV_WINDOW_AUTOSIZE );

    imshow( imageName, image );
    imshow( "Gray image", gray_image );

    cout << "Gray Image =" << endl << gray_image << endl;

    waitKey(0);                                          // Wait for a keystroke in the window
    return 0;
}
