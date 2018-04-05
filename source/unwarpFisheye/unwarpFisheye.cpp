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
// #include <opencv2/core/core.hpp>
// // #include <opencv2/imgproc/imgproc.hpp>
// // #include <opencv2/calib3d/calib3d.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <vector>
using namespace std;
using namespace cv;

int main(int argc, char **argv)
{
    clock_t begin = clock();
    std::vector<Mat> fisheye(1); // 3840 X 2160

    fisheye[0] = imread("/media/gunman/Data/thesis/stitch/trial_stitch/captures_0to5_argus/img0.jpg", CV_LOAD_IMAGE_COLOR);

    float PI = 3.1415926536;
    float radPerDegree = ((float)PI / 180);
    // calculating fov of fisheye camera http: //www.bobatkins.com/photography/technical/field_of_view.html // using equidistant projection model
    // HFOV VFOV
    // 170 101.5  scaled to 1/2.5 inch sensor from datasheet
    // 134, 94 from datasheet of fisheye lens 143 and 81 are interpolated values for 1/2.5 image sensor
    float camRadCxCy[6][3] = {
        {1963.65, 1960.51, 1115},
        {1967.57, 1942.19, 1111.15},
        {1960.29, 1965.02, 1064.82},
        {1962.74, 1981.67, 1169.57},
        {1967.36, 1855.69, 1081.11},
        {1963.64, 1949.43, 1082.01}};
    //float Hfov = radPerDegree * 143.47;
    float Hfov = radPerDegree * 185;
    float Vfov = radPerDegree * 85;
    float lon, lat, r1, lon2, width, height, r2, phi, theta, imphi, imtheta;
    Point2f inP;
    Point3f psph;

    // float outImageHeight = fisheye[0].rows, outImageWidth = fisheye[0].cols;
    float outImageHeight = 1000, outImageWidth = 2 * outImageHeight;
    //float outImageHeight = 1000, outImageWidth = 2 * outImageHeight;

    std::vector<Mat> outImage(1, Mat(outImageHeight, outImageWidth, CV_8UC3));
    std::cout << "outImage cols = " << outImage[0].cols << endl;
    std::cout << "outImage rows = " << outImage[0].rows << endl;
    width = fisheye[0].cols;
    height = fisheye[0].rows;
    string inpImStr;
    float cam0_radius, centerCx, centerCy, sheight, swidth;
    float numberOfCameras = 6 ;
    for (int iter = 0; iter < numberOfCameras; iter++)
    {
        cam0_radius = camRadCxCy[iter][0];
        centerCx = camRadCxCy[iter][1];
        centerCy = camRadCxCy[iter][2];
        inpImStr = "/media/gunman/Data/thesis/stitch/trial_stitch/captures_0to5_argus/img" + std::to_string(iter) + ".jpg";
        fisheye[0] = imread(inpImStr, CV_LOAD_IMAGE_COLOR);
        // Equirect0 generation
        for (int i = 0; i < outImage[0].cols; i++)
        {
            for (int j = 0; j < outImage[0].rows; j++)
            {
                // sheight = fisheye[0].cols;
                // swidth = fisheye[0].rows;
                theta = (2*PI) * (i / outImageWidth - 0.5) - (2*PI/numberOfCameras) * iter + PI/2; // -pi to pi
                phi = PI * (j / outImageHeight - 0.5);  // -pi/2 to pi/2
                psph.x = cos(phi) * sin(theta);
                psph.y = cos(phi) * cos(theta);
                psph.z = sin(phi);
                theta = atan2(psph.z, psph.x);
                phi = atan2(sqrt(psph.x * psph.x + psph.z * psph.z), psph.y);
                r1 = (cam0_radius * 2) * (phi / Hfov); // dividing by Hfov as the full fisheye correcpond to Hfov
                // r1 = (cam0_radius * 2 * (Vfov / Hfov) ) * (phi / Vfov);
                // r2 = (cam0_radius * 2 * (Vfov / Hfov)) * (phi / Vfov);
                // r2 = cam0_radius * phi / Vfov;
                inP.x = 0.5 * fisheye[0].cols + r1 * cos(theta) + (centerCx - (fisheye[0].cols / 2));
                inP.y = 0.5 * fisheye[0].rows + r1 * sin(theta) + (centerCy - (fisheye[0].rows / 2));
                ///
                Point inP2((int)inP.x, (int)inP.y);
                if (inP2.x >= fisheye[0].cols || inP2.y >= fisheye[0].rows)
                    continue;
                if (inP2.x < 0 || inP2.y < 0)
                    continue;
                Vec3b color = fisheye[0].at<Vec3b>(inP2);
                outImage[0].at<Vec3b>(Point(i, j)) = color;
            }
        }
        std::string result_str = "/media/gunman/Data/thesis/stitch/trial_stitch/captures_0to5_argus/out/outHfov185Vfov85_" + std::to_string(iter) + ".png";
        imwrite(result_str, outImage[0]);
        outImage[0].setTo(Scalar(0));
    }
    clock_t end = clock();
    double elapsed_secs = double(end - begin) / CLOCKS_PER_SEC;
    cout << "elapsed time " << elapsed_secs << "\n";
    return 0;
}
