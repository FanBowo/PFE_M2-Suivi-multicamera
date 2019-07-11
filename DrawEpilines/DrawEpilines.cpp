#include <stdio.h>
#include <opencv2/opencv.hpp>
#include <fstream>

using namespace cv;

void my_mouse1_callback(int event, int x, int y, int flags, void* param);
void my_mouse2_callback(int event, int x, int y, int flags, void* param);
volatile bool UpdatePoint1 = false;
volatile bool UpdatePoint2= false;
Mat image1,image2;
cv::Point PointClick1;
cv::Point PointClick2;

cv::Mat Fmatrix ;

unsigned int PointIndex=0;

void DrawEpilines(int WhichPicToDraw);

int main(int argc, char** argv )
{
    	if(argc != 4){
		std::cout<<"Wrong number of command line parameters for "<< argv[0] << std::endl;
		std::cout<<"Para1:pic1\n Para2:pic2\n Para3:Fmatrix"<<std::endl;
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
										
	
	fs["Fmatrix"]>>Fmatrix;
	fs.release();
	
    cv::namedWindow("Cam1", WINDOW_AUTOSIZE );
	cv::namedWindow("Cam2", WINDOW_AUTOSIZE );
    cv::imshow("Cam1", image1);
	cv::imshow("Cam2", image2);
	
	cv::setMouseCallback( "Cam1",my_mouse1_callback,(void*)&PointClick1);
	cv::setMouseCallback( "Cam2",my_mouse2_callback,(void*)&PointClick2);
	 for(;;) {
		 if(UpdatePoint1){
				UpdatePoint1=false;
			 	DrawEpilines(2);
		 }
		 if(UpdatePoint2){
				UpdatePoint2=false;
			 	DrawEpilines(1);
		 }
		if( cv::waitKey( 15 ) == 27 ) break;
  }


    return 0;
}

void my_mouse1_callback(int event, int x, int y, int flags, void* param) 
{
  cv::Point & TempPoint = *(cv::Point*) param;
  switch( event ) {
	case cv::EVENT_LBUTTONDOWN: {
		PointIndex++;
		std::string sPointIndex= std::to_string(PointIndex);
      TempPoint.x=x;
      TempPoint.y=y;
      cv::circle(image1,TempPoint,1,cv::Scalar(0x00,0x00,0xff));
	  cv::putText(image1,sPointIndex,TempPoint,FONT_HERSHEY_SIMPLEX,0.5,cv::Scalar(255,0,255),2,8);
      cv::imshow("Cam1", image1);
      UpdatePoint1=true;
    }
    break;
  }
}

void my_mouse2_callback(int event, int x, int y, int flags, void* param) 
{
  cv::Point & TempPoint = *(cv::Point*) param;
  switch( event ) {
	case cv::EVENT_LBUTTONDOWN: {
		PointIndex++;
		std::string sPointIndex= std::to_string(PointIndex);
      TempPoint.x=x;
      TempPoint.y=y;
      cv::circle(image2,TempPoint,1,cv::Scalar(0x00,0x00,0xff));
	  cv::putText(image2,sPointIndex,TempPoint,FONT_HERSHEY_SIMPLEX,0.5,cv::Scalar(255,0,255),2,8);
      cv::imshow("Cam2", image2);
      UpdatePoint2=true;
		}
    break;
  }
}

void DrawEpilines(int WhichPicToDraw){
	Mat TempImage;
	cv::Point PointClick;
	int NoClick;
	switch (WhichPicToDraw){
		case 1: TempImage=image1; PointClick=PointClick2;NoClick=2;
					std::cout<<"ClickPic2 DrawPic1"<<std::endl;
					break;
		case 2: TempImage=image2;PointClick=PointClick1;
					std::cout<<"ClickPic1 DrawPic2"<<std::endl;NoClick=1;
					break;
		default:return;
	}
	std::cout<<"x: "<<PointClick.x<<"y: "<<PointClick.y<<std::endl;
	std::vector <cv::Point2f> PointsInFirstImag;
	std::vector<cv::Vec<float,3>> LineInSecondeImag;
	PointsInFirstImag.push_back(PointClick);
	cv::computeCorrespondEpilines(PointsInFirstImag,NoClick,Fmatrix,LineInSecondeImag);
	float aPic=LineInSecondeImag[0][0];
	float bPic=LineInSecondeImag[0][1];
	float cPic=LineInSecondeImag[0][2];
	std::cout<<" aPic :"<<aPic\
				<< " bPic :"<<bPic\
				<<" cPic :"<<cPic<< std::endl;

	cv::Size Image2Size(TempImage.size().width,TempImage.size().height);
	float Tempx1=TempImage.size().width;
	float Tempx2=0;		
	cv::Point StartPoint(Tempx1,((-cPic)-aPic*Tempx1)/bPic);
	cv::Point EndPoint(Tempx2,((-cPic)-aPic*Tempx2)/bPic);
				
	for(;;){
		if (clipLine(Image2Size,StartPoint,EndPoint)) break;
		else{
					Tempx1-=10;
					Tempx2+=10;
					StartPoint.x=Tempx1;
					StartPoint.y=((-cPic)-aPic*Tempx1)/bPic;
					EndPoint.x=Tempx2;
					EndPoint.y=((-cPic)-aPic*Tempx2)/bPic;
				}
	}
			
	int thickness = 1;
	int lineType = 8;
	cv::line( TempImage,
				StartPoint,
				EndPoint,
				cv::Scalar(0x00,0x00,0xff),    /* red */
				thickness,
				lineType );
	std::string sPointIndex= std::to_string(PointIndex);
	cv::Point PointMid;
	PointMid.x=(StartPoint.x+EndPoint.x)/2;
	PointMid.y=(StartPoint.y+EndPoint.y)/2;
	switch (WhichPicToDraw){
		case 1: 
		cv::putText(image1,sPointIndex,PointMid,FONT_HERSHEY_SIMPLEX,1,cv::Scalar(255,0,255),2,8);
		cv::imshow("Cam1", TempImage);
		break;
		case 2: 
		cv::putText(image2,sPointIndex,PointMid,FONT_HERSHEY_SIMPLEX,1,cv::Scalar(255,0,255),2,8);
		cv::imshow("Cam2", TempImage);
		break;
		default:return;
	}
	
}