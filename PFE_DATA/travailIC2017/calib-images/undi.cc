#include <iostream>
#include <opencv2/opencv.hpp>
#include <vector>
#include <stdio.h>
#include <stdlib.h>
#include <fstream>
#include <gvars3/instances.h>

using namespace cv;
using namespace std;

int main(int argc, char **argv)
{

	/*
	* argv[1] - input image
	* argv[2] - output image
	* argv[3] - intrinsic file
	*/

	if(argc != 4){
		cout<<"Wrong number of command line parameters for "<< argv[0] << endl;
		return -1;
	}
	std::cout << sizeof(unsigned short int) << endl;

	//GUI.LoadFile("config-calib.cfg");
    //string first = GV3::get<string>("first_camera","0");
    //string second = GV3::get<string>("second_camera","1");
    //FileStorage fs0("cam"+first+"/camera.xml",FileStorage::READ);
    //FileStorage fs1("cam"+second+"/camera.xml",FileStorage::READ);
	FileStorage fs0(argv[3],FileStorage::READ);
    if (!fs0.isOpened())
    {
		cout<<"erreur ouverture fichier conf"<<endl;
		exit(1);
	}
	Mat Img0;
	Mat intrinsic0 = Mat(3, 3, CV_32FC1);
    Mat distCoeffs0;
    fs0["intrinsic"]>>intrinsic0;
	fs0["distorsion"]>>distCoeffs0;
	fs0.release();
	
	Mat image0;
	string path0 = string(argv[1]); 
	image0 = imread(path0,0);
	
	undistort(image0, Img0, intrinsic0, distCoeffs0);	

	
	imwrite(argv[2],Img0);
	
	return 0;
}