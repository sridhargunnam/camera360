// g++ -std=c++11 ./prepareData4fb.cpp `pkg-config --libs --cflags opencv` -o prepareData4fb

// Equirectangular projection with correction of fisheye center and the radius
// Shift the center of fisheye circle from image center to the calculated center and calculated radius therfore covers extra region
// project onto cylinder and visualize
// make omp working
#include <omp.h>
#include <iostream>
#include <sstream>
#include <time.h>
#include <stdio.h>
#include <string>
#include <fstream>
#include <cmath>
 // for creating directory 
#include <sys/stat.h>
#include <stdlib.h>
// #include <opencv2/core/core.hpp>
// // #include <opencv2/imgproc/imgproc.hpp>
// // #i nclude <opencv2/calib3d/calib3d.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <vector>
using namespace std;
using namespace cv;

int main(int argc, char **argv)
{
    float PI = 3.1415926536;

    clock_t begin = clock();
    std::vector<Mat> fisheye(1);
    int numberOfCameras = 6;
    std::string fisheyeImage, result_str1, result_str2;
    
    std::string initdirname = "/media/gunman/Data/thesis/stitch/available_solutions/Surround360/jetsonData/rgb/";
    std::string dirname;
    for (int iter = 0; iter < numberOfCameras; iter++)
    {

        fisheyeImage = "/media/gunman/Data/thesis/stitch/unwarpingFisheye/centerOfRoom/moderate_room_lighting/image000" + std::to_string(iter) + ".jpg";
        fisheye[0] = imread(fisheyeImage, CV_LOAD_IMAGE_COLOR);
        dirname = initdirname;
        dirname += "cam" + std::to_string(iter+1);
        mkdir(dirname.c_str(), 0775);
        result_str1 = dirname + "/000000"  + ".png";
        // result_str2 = dirname + "/00000" + std::to_string(iter) + ".png";
        imwrite(result_str1, fisheye[0]);
        fisheye[0].setTo(Scalar(0));
    }
    clock_t end = clock();
    double elapsed_secs = double(end - begin) / CLOCKS_PER_SEC;
    cout << "elapsed time " << elapsed_secs << "\n";
    return 0;
}
