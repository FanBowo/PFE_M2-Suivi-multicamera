/*
 * f-w-calib.cxx
 * 
 * Copyright 2012 romain <sauce@T420A>
 * 
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
 * MA 02110-1301, USA.
 * 
 * 
 */


#include <iostream>
#include <opencv2/opencv.hpp>
#include <vector>
#include <stdio.h>
#include <stdlib.h>
#include <fstream>
#include <boost/lexical_cast.hpp>

using namespace cv;
using namespace std;

// 8 and 5 for large board, 10 and 7 for small board
int numCornersHor = 10;
int numCornersVer = 7;
int flags = 0;



void usage(char **argv){
	cout << "Usage : " << argv[0] << "-H <number of horizontal corners [10]> -V <number of vertical corners [7]> " << endl 
		 << "-pp <fix principal point[0]> -zt <zero tangent distortion[0]> -fr <fixed aspect ratio[0]" << endl; 
}


void parseCommandLine(int argc, char **argv){
	//all parameters have default values, so we
	//will display the usage only if explicitly requested
	if (argc > 1 && std::string(argv[1]) == "-h"){
		usage(argv);
		exit(0);
	}

	for (int i = 1; i < argc; i++) { 

		//horizontal corner number
		if (std::string(argv[i]) == "-H") {
        	assert(i+ 1 < argc);
        	try{
        		numCornersHor = boost::lexical_cast<int>(std::string(argv[i+1]));
		    }
    		catch(std::exception e){
    			cout << "Number of horizontal corners must be integer" << endl;
    			exit(1);
    		}
       	}

       	//vertical corner number
		if (std::string(argv[i]) == "-V") {
        	assert(i+ 1 < argc);
        	try{
        		numCornersVer = boost::lexical_cast<int>(std::string(argv[i+1]));
		    }
    		catch(std::exception e){
    			cout << "Number of vertical corners must be integer" << endl;
    			exit(1);
    		}
       	}

       	//fixed principal point?
		if (std::string(argv[i]) == "-pp") {
        	assert(i+ 1 < argc);
        	bool flagPP = false;
        	try{
        		flagPP = boost::lexical_cast<bool>(std::string(argv[i+1]));
		    }
    		catch(std::exception e){
    			cout << "Please specify 0 or 1 for fixed principal point" << endl;
    			exit(1);
    		}
    		if(flagPP)
    			flags = flags | CV_CALIB_FIX_PRINCIPAL_POINT;
       	}

       	//zero tangent distortion?
		if (std::string(argv[i]) == "-zt") {
        	assert(i+ 1 < argc);
        	bool flagZT = false;
        	try{
        		flagZT = boost::lexical_cast<bool>(std::string(argv[i+1]));
		    }
    		catch(std::exception e){
    			cout << "Please specify 0 or 1 for fixed principal point" << endl;
    			exit(1);
    		}
    		if(flagZT)
    			flags = flags | CV_CALIB_ZERO_TANGENT_DIST;
       	}

	//zero radial distortion for k2, k3
	if (std::string(argv[i]) == "-zr") {
        	assert(i+ 1 < argc);
        	bool flagZR = false;
        	try{
        		flagZR = boost::lexical_cast<bool>(std::string(argv[i+1]));
		    }
    		catch(std::exception e){
    			cout << "Please specify 0 or 1 for fixed principal point" << endl;
    			exit(1);
    		}
    		if(flagZR){
    			flags = flags | CV_CALIB_FIX_K2;
    			flags = flags | CV_CALIB_FIX_K3;
    			flags = flags | CV_CALIB_FIX_K1;
		}
       	}

       	//fixed aspect ratio?
		if (std::string(argv[i]) == "-fr") {
        	assert(i+ 1 < argc);
        	bool flagFR = false;
        	try{
        		flagFR = boost::lexical_cast<bool>(std::string(argv[i+1]));
		    }
    		catch(std::exception e){
    			cout << "Please specify 0 or 1 for fixed aspect ratio" << endl;
    			exit(1);
    		}
    		if(flagFR)
    			flags = flags | CV_CALIB_FIX_ASPECT_RATIO;
       	}



	}

}



int main(int argc, char **argv)
{
	vector<Mat> image;
	 
	int numBoards = 0;
	parseCommandLine(argc, argv);
	
	
	ostringstream * bob;
	Mat A;
	A = imread("image0.tiff",0);
	if (A.data == NULL)
	{
		cout<<"Images not found in the current folder"<<endl;
		exit (1);
	}
	image.push_back(A);
	numBoards++;
	do
	{
		bob = new ostringstream;
		*bob<<"image"<<numBoards<<".tiff";
		cout<<bob->str()<<endl;
		A = imread(bob->str(),0);
		if(A.data==NULL)
			cout<<"Error opening image"<<endl;
		else
		{
			image.push_back(A);
			numBoards++;
		}
			
		delete bob;
	} while (A.data!=NULL);
	cout<<"numBoards = "<<numBoards<<endl;

    int numSquares = numCornersHor * numCornersVer;
    Size board_sz = Size(numCornersHor, numCornersVer);    
     
     
    vector<vector<Point3f> > object_points; // point position in 3D
    vector<vector<Point2f> > image_points; 	// point position in image space/2D     
    
    vector<Point2f> corners;

    vector<Point3f> obj;
    for (int j = 0; j < numSquares; j++)
	{
		obj.push_back(Point3f(j/numCornersHor, j%numCornersHor, 0.0));	// On remplie lq liste de coordonnees 
	}
	
	/*
	 * Boucle d'acquisition des donnes images coordonnes des coins
	 */ 
	for (int i = 0; i < numBoards; i++)
	{
		bool found = findChessboardCorners(image[i], board_sz, corners, CV_CALIB_CB_ADAPTIVE_THRESH | CV_CALIB_CB_NORMALIZE_IMAGE | CV_CALIB_CB_FILTER_QUADS);
        cout<<"i = "<<i;
        if(found)
        {
			cout<<" -> OK"<<endl;
            cornerSubPix(image[i], corners, Size(11, 11), Size(-1, -1), TermCriteria(CV_TERMCRIT_EPS | CV_TERMCRIT_ITER, 100, 0.05));
			image_points.push_back(corners);
			object_points.push_back(obj);
        }
        else
			cout<<endl;

	}
	
	
	/*
	 * On determine les coef des calib de la camera
	 */
	 
    Mat intrinsic = Mat(3, 3, CV_64FC1);
    Mat distCoeffs = Mat(1, 5, CV_64FC1);
    vector<Mat> rvecs;
    vector<Mat> tvecs;	 
	ofstream outfile; 
	outfile.open("ChessBoardPoses.txt");
	FileStorage fs("camera.xml",FileStorage::WRITE);
    
    intrinsic.ptr<double>(0)[0] = 1;	// Init de la focale
    intrinsic.ptr<double>(1)[1] = 1;
	cout<<"width = "<<image[0].size().width<<endl;
	cout<<"height = "<<image[0].size().height<<endl;
	
	
	// argv[3] flag put optical center in the middle
	// argv[4] flag set tangential distortion to 0
	// argv[5] flag set factor 1 between fx and fy 
	/*
	if(atoi(argv[3]))
		flags = flags | CV_CALIB_FIX_PRINCIPAL_POINT;
	if(atoi(argv[4]))
		flags = flags | CV_CALIB_ZERO_TANGENT_DIST;
	if(atoi(argv[5])){
		flags = flags | CV_CALIB_FIX_ASPECT_RATIO;
	}
	*/

	cout<<"BEFORE"<<endl;	
    double error = calibrateCamera(object_points, image_points, image[0].size(), intrinsic, distCoeffs, rvecs, tvecs, flags);
	
	cout<<"Matrice de calibration :"<<endl;
	cout<<intrinsic<<endl;
	
	cout<<"Reprojection error :"<<endl;
	cout<<error<<endl;
	
	cout<<endl<<"Coef de distorsion :"<<endl;
	cout<<distCoeffs<<endl;
	
	// Sauvegarde des donnes camera dans le fichier xml
	fs <<"intrinsic"<<intrinsic<<"distorsion"<<distCoeffs;
	fs.release();
	
	// [R,t] du damier ecrit dans le fichier camera.cfg pour chaques images exploitables
	for (unsigned int i = 0; i < rvecs.size(); i++)
	{
		Mat Rot;
		Rodrigues(rvecs[i],Rot);
		outfile<<i<<endl<<Rot<<endl<<tvecs[i]<<endl;
		outfile<<endl;
	}
	
	outfile.close();

	/*
	 * Test du resultat...
	 */
	cout<<"size = "<<image.size()<<endl;
	Mat imageUndistorted;
	undistort(image.front(), imageUndistorted, intrinsic, distCoeffs);
	imshow("win1", image.front());
	imshow("win2", imageUndistorted);

	waitKey(0);
	  
    	
	return 0;
}
