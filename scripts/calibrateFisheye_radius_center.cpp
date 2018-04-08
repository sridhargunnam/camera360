#include <iostream>
#include <opencv2/opencv.hpp>

using namespace std;
using namespace cv;

int main()
{
    // Read the image
    Mat image = imread("fisheye_bright_screen2.jpg");
//    Mat image = imread("out_bright2.jpg");
    Mat grayscale;
    cvtColor(image, grayscale, COLOR_BGR2GRAY);

    // Threshold the image
    // Since the edge is always black, lets use this to our advantage
    Mat mask;
    threshold(grayscale, mask, 20, 255, CV_8UC1);
   // imshow("gray mask", mask); waitKey(0);
    // Find the largest contour in the mask
    // Then find the center of gravity of all the points
    // If this is accurate enough it will be very close near the center
    vector< vector<Point> > contours;
    findContours(mask, contours, RETR_EXTERNAL, CHAIN_APPROX_NONE);
    drawContours(mask, contours, -1, 255, 2);

    // Loop over contours and find the largest ones based on the area
    // This will ensure that small artefacts do not break this down
    vector<Point> largest_contour = contours[0];
    for(size_t i=0; i<contours.size(); i++){
        if(contourArea(contours[i]) > contourArea(largest_contour)){
            largest_contour = contours[i];
//	    cout << " the largest counter is " <<  largest_contour << endl ;
        }
    }

    // Now find the minimum enclosing circle
    Point2f center;
    float radius;
    minEnclosingCircle(largest_contour, center, radius);
    Point2f cenI(653, 371);

    // Draw the centerpoint on the original image
    circle(image, center, 2, Scalar(0,0,255), -1);
    circle(image, center, 10, Scalar(0,255,0), 2);
    circle(image, center, radius, (0,0,255), 2);
//  circle(image, center, int(radius), Scalar(255,0,0), 2);
    stringstream temp;
    temp << "the radius = " << radius << " /" << " the center = " << center;
    putText(image, temp.str(), Point(50,50), FONT_HERSHEY_COMPLEX, 1, Scalar(0,255,0));
    std::cout << " radius = " << radius << endl;
    std::cout << " center Cx= " << center.x  <<  " center Cy= " <<  center.y << endl;
    imshow("result", image); waitKey(0);
    imwrite("result_circel_detect.jpg", image);
    return 0;
}
