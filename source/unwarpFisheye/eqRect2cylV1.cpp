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
    std::vector<Mat> Equirect(1);
    std::ofstream outfile;
    if (remove("Debug_out.txt") != 0)
        perror("Error deleting file");
    else
        puts("File successfully deleted");

    outfile.open("Debug_out.txt", std::ios_base::app);
    // Height maps from -60 degree to 60 degree
    float outImageHeight = 2000, outImageWidth = 3 * outImageHeight;
    std::vector<Mat> cylProjection(1, Mat(outImageHeight, outImageWidth, CV_8UC3));

    float radPerDegree = ((float)PI / 180);
    float HfovFisheye = radPerDegree * 185;
    float VfovFisheye = radPerDegree * 85;

    float ipd = 6.4;          // in cm
    float ipd_by_2 = ipd / 2; // in cm r
    float rig_radius = 9.6;   // in cm (r)
    float cylRadius = 100000;     // in cm (R) vary this parameter, find out math to find this parameter
    float distBwCamCyl = cylRadius - rig_radius;
    float heightCyl = 2 * distBwCamCyl * tan(VfovFisheye / 2);
    float VfovCylFromCenter = 2 * atan((heightCyl * 0.5) / cylRadius);
    float heightPerPixelCyl = heightCyl / outImageHeight;
    float numberOfCameras = 6; // float for safeside, when combining with float and int expressions
    float theta_cyl, phi, theta_spherical, phi_spherical, hCyl_at_i_j, Hangle_per_pixel_sph, Vangle_per_pixel_sph, temp, base;
    std::string eqRectImStr, result_str, debug_output;

    Point2f eqRect;
    ///////////////////////////////////////////////////
    for (int iter = 0; iter < numberOfCameras; iter++)
    {
        eqRectImStr = "/media/gunman/Data/thesis/stitch/unwarpingFisheye/captures_0to5_argus/out/outHfov185Vfov85_" + std::to_string(iter) + ".png";
        // eqRectImStr = "/media/gunman/Data/thesis/stitch/unwarpingFisheye/testEqRect_image.png";
        result_str = "/media/gunman/Data/thesis/stitch/unwarpingFisheye/captures_0to5_argus/out/CylProjection_" + std::to_string(iter) + ".png";
        Equirect[0] = imread(eqRectImStr, CV_LOAD_IMAGE_COLOR);
        Hangle_per_pixel_sph = (2 * PI) / Equirect[0].cols;
        Vangle_per_pixel_sph = (PI) / Equirect[0].rows;
        // Equirect0 generation
        for (int i = 0; i < cylProjection[0].rows; i++)
        {
            // heightPerPixelCyl = heightCyl / outImageHeight;
            hCyl_at_i_j = (-0.5) * heightCyl + heightPerPixelCyl * i;
            for (int j = 0; j < cylProjection[0].cols; j++)
            {
                // find corresponding u,v i.e elevation and rotation in equirectangular image that correspons to i,j th pixel in cylinder
                // row means height
                if (j < outImageWidth / 2)
                    theta_cyl = ((j * 2 * PI) / outImageWidth) ; //- HfovFisheye / 2;
                else
                    theta_cyl = ((j * 2 * PI) / outImageWidth) ; //- HfovFisheye / 2;

                temp = abs(atan2((cylRadius * sin(theta_cyl)), (cylRadius * cos(theta_cyl) - rig_radius)));
                if (j < outImageWidth / 2) // to sweep from 0 to 2*PI, but as atan2 gives from 0 to PI
                    theta_spherical = atan2((cylRadius * sin(theta_cyl)), (cylRadius * cos(theta_cyl) - rig_radius));
                else
                    theta_spherical = 2 * PI - temp;

                //base = sqrt( (Rsin(theta_cyl)-r) + (Rcos(theta_cyl))  );
                base = sqrt(std::pow(cylRadius * sin(theta_cyl), 2) + std::pow((cylRadius * cos(theta_cyl) - rig_radius), 2));
                phi_spherical = atan2(hCyl_at_i_j, base); // phi_spherical will run from -VfovCyl/2 to -VfovCyl/2
                // ? phi_spherical = atan2( (hCyl_at_i_j * sin(theta_spherical)), (cylRadius * sin(theta_cyl)));
                // thetad = (2 * PI) * (i / cylProjection[0].cols);

                eqRect.x = int(theta_spherical / Hangle_per_pixel_sph);
                eqRect.y = int((phi_spherical + PI*0.5)/ Vangle_per_pixel_sph)  ;

                if (i == 0)
                {
                    debug_output = "Cyl at i j " + std::to_string(i) + ", " + std::to_string(j);
                    debug_output += " is  at Cyl theta is " + std::to_string(theta_cyl * 57.29);
                    debug_output += " is  at Spherical theta is " + std::to_string(theta_spherical * 57.29);
                    debug_output += ", Shperical phi is " + std::to_string(phi_spherical * 57.29);
                    debug_output += " hCyl_at_i_j" + std::to_string(hCyl_at_i_j);
                    debug_output += "\n";
                    outfile << debug_output;
                }
                if(eqRect.x >= Equirect[0].cols || eqRect.y >= Equirect[0].rows || eqRect.x < 0 || eqRect.y < 0)
                continue;
                cylProjection[0].at<Vec3b>(Point(j, i)) = Equirect[0].at<Vec3b>(eqRect.y, eqRect.x); // weird opencv way of accessing row coloumn (y, x)
                //  imwrite(result_str, cylProjection[0]);

            }

        }
        result_str = "/media/gunman/Data/thesis/stitch/unwarpingFisheye/captures_0to5_argus/out/CylProjection_" + std::to_string(iter) + ".png";
        imwrite(result_str, cylProjection[0]);
        cylProjection[0].setTo(Scalar(0));
    }
    clock_t end = clock();
    double elapsed_secs = double(end - begin) / CLOCKS_PER_SEC;
    cout << "elapsed time " << elapsed_secs << "\n";
    outfile.close();
    return 0;
}
