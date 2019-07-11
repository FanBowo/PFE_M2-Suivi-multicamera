#include <stdio.h>
#include <opencv2/opencv.hpp>
#include <fstream>

using namespace cv;
Mat image1,image2;
std::vector<cv::Point2f>  Matches0;
std::vector<cv::Point2f>  Matches1;

int main(int argc, char** argv )
{
    	if(argc != 4){
		std::cout<<"Wrong number of command line parameters for "<< argv[0] << std::endl;
		std::cout<<"Para1:pic1\n Para2:pic2\n Para3:InliersInTwoPic"<<std::endl;
		exit(1);
	}
	
	image1 = cv::imread( argv[1], 1 );
	image2=cv::imread ( argv[2], 1 );

	cv::FileStorage fs(argv[3],cv::FileStorage::READ);
    if (!fs.isOpened())
    {
		std::cout<<"erreur ouverture fichier conf"<<std::endl;
		exit(1);
	}
										
	
	fs["Points0"]>>Matches0;
	fs["Points1"]>>Matches1;
	fs.release();
	
    cv::namedWindow("Cam1", WINDOW_AUTOSIZE );
	cv::namedWindow("Cam2", WINDOW_AUTOSIZE );
    cv::imshow("Cam1", image1);
	cv::imshow("Cam2", image2);
	
	std::vector<cv::Point2f>::iterator itMatches0= Matches0.begin();
	std::vector<cv::Point2f>::iterator itMatches1= Matches1.begin();
	
	cv::Point2f TempPoint1;
	cv::Point2f TempPoint2;
	
	std::cout << "Nombres de Inliers au imag0  " << Matches0.size()<<std::endl;
	std::cout << "Nombres de Inliers au imag1  " << Matches1.size()<<std::endl;
	
	for(;itMatches0!=Matches0.end()&&itMatches1!=Matches1.end();itMatches0++,itMatches1++){
		TempPoint1=(*itMatches0);
		TempPoint2=(*itMatches1);
		std::string PointIndex= std::to_string(itMatches0-Matches0.begin());
		cv::circle(image1,TempPoint1,1,cv::Scalar(0x00,0x00,0xff));
		cv::putText(image1,PointIndex,TempPoint1,FONT_HERSHEY_SIMPLEX,0.5,cv::Scalar(255,0,255),1,8);
		cv::imshow("Cam1", image1);
		cv::circle(image2,TempPoint2,1,cv::Scalar(0x00,0x00,0xff));
		cv::putText(image2,PointIndex,TempPoint2,FONT_HERSHEY_SIMPLEX,0.5,cv::Scalar(255,0,255),1,8);
		cv::imshow("Cam2", image2);
		std::cout << "Inliers au imag0  " << TempPoint1<<std::endl;
		std::cout << "Inliers au imag1 " << TempPoint2<<std::endl;
		/*
		TempPoint1.x+=(*itMatches0).x;
		TempPoint1.y+=(*itMatches0).y;
		TempPoint2.x+=(*itMatches1).x;
		TempPoint2.y+=(*itMatches1).y;*/
	}
	for(;;) {
		if( cv::waitKey( 15 ) == 27 ) break;
  }
    return 0;
}

