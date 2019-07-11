/*
 * fundamentalmat_solver.cpp
 *
 *  Created on: 31 ago 2016
 *      Author: Nicola Pellicanò
 */

#include "fundamentalmat_solver.h"

#include <ctime>
#include <cmath>
#include <regex>
#include <opencv2/nonfree/nonfree.hpp>
#include <opencv2/core/eigen.hpp>
#include <fstream>
#include <src/sigma_functions/sigma_step_function.h>

#include "src/estimation/f_uncertainty.h"
#include "src/matchers/sift_guided_matching.h"

#include "src/exceptions/estimation_error_exception.h"
#include "src/exceptions/optimization_exception.h"


void rawToPoint2f(const std::vector<FVMatcher::FVMatch> & init_matches,std::vector<cv::Point2f> & out0,std::vector<cv::Point2f> & out1){
	for (const auto &init_match : init_matches) {
		cv::Point2f p0(static_cast<float>(init_match.p0.first), static_cast<float>(init_match.p0.second));
		cv::Point2f p1(static_cast<float>(init_match.p1.first), static_cast<float>(init_match.p1.second));
		out0.push_back(p0);
		out1.push_back(p1);
	}
}

void FundamentalMatSolver::refinement(const cv::vector<cv::Point2f> & inl0,const cv::vector<cv::Point2f> & inl1,Eigen::Matrix3d & eF){
	double Farr[3][3];
	Farr[0][0]=eF(0,0);Farr[0][1]=eF(0,1);Farr[0][2]=eF(0,2);
	Farr[1][0]=eF(1,0);Farr[1][1]=eF(1,1);Farr[1][2]=eF(1,2);
	Farr[2][0]=eF(2,0);Farr[2][1]=eF(2,1);Farr[2][2]=eF(2,2);
	auto points0 = new double *[inl0.size()];
	auto points1 = new double *[inl1.size()];
	for (size_t i = 0; i < inl0.size(); ++i) {
		points0[i]=new double[2];
		points1[i]=new double[2];
	}
	for (size_t i = 0; i < inl0.size(); ++i) {
		points0[i][0]=inl0[i].x;
		points0[i][1]=inl0[i].y;
		points1[i][0]=inl1[i].x;
		points1[i][1]=inl1[i].y;
	}

	try{
		optimizer->optFundMat(Farr, points0, points1, static_cast<int>(inl0.size()));
	}catch(OptimizationException & e){
		throw e;
	}

	for (size_t i = 0; i < inl0.size(); ++i) {
		delete[] points0[i];
		delete[] points1[i];
	}
	delete[] points0;
	delete[] points1;

	eF(0,0)=Farr[0][0];eF(0,1)=Farr[0][1];eF(0,2)=Farr[0][2];
	eF(1,0)=Farr[1][0];eF(1,1)=Farr[1][1];eF(1,2)=Farr[1][2];
	eF(2,0)=Farr[2][0];eF(2,1)=Farr[2][1];eF(2,2)=Farr[2][2];

}


void FundamentalMatSolver::labelCorePoints(const std::vector<cv::Point2f> & matches,std::vector<bool> & is_core){

	for (size_t i = 0; i < matches.size(); ++i) {
			cv::Point2f testp=matches[i];
			int count=0;
			is_core[i]=false;
			for (size_t j = 0; j < matches.size(); ++j) {
				if(i==j) continue;
				cv::Point2f checkp=matches[j];
				if (((testp.x - checkp.x) * (testp.x - checkp.x) + (testp.y - checkp.y) * (testp.y - checkp.y)) <=
					100 * 100) {
					count++;
				}
				if (count >= 5) {
					is_core[i]=true;
					break;
				}
			}
	}

}

void permuteCov(const Eigen::Matrix<double, 9, 9> &inCov, const std::vector<int> &permutation,
				Eigen::Matrix<double, 9, 9> &outCov) {
	for (int i = 0; i < 9; ++i) {
		for (int j = 0; j < 9; ++j) {
			outCov(i,j)=inCov(permutation[i],permutation[j]);
		}
	}
}

void saveFxml(const cv::Mat &F, int index, int expnum, const std::string &debug_folder) {
	std::string path = debug_folder + "/fLM";
    path+=std::to_string(index)+"-"+std::to_string(expnum)+".xml";
    cv::FileStorage fout(path,cv::FileStorage::WRITE);
    if(fout.isOpened()){
        fout << "Fundamental" << F;
        fout.release();
    }
}

void saveBeginInliersxml(std::vector<cv::Point2f> &matches0, std::vector<cv::Point2f> &matches1, int index, int expnum,
					const std::string &debug_folder) {
	std::string path = debug_folder + "/_0_BeginInliers";
    path+=std::to_string(index)+"-"+std::to_string(expnum)+".xml";
     cv::FileStorage fout(path,cv::FileStorage::WRITE);
     if(fout.isOpened()){
         fout << "Points0" << matches0 << "Points1" << matches1;
         fout.release();
     }
}


void saveInliersxml(std::vector<cv::Point2f> &matches0, std::vector<cv::Point2f> &matches1, int index, int expnum,
					const std::string &debug_folder) {
	std::string path = debug_folder + "/inliers";
     path+=std::to_string(index)+"-"+std::to_string(expnum)+".xml";
     cv::FileStorage fout(path,cv::FileStorage::WRITE);
     if(fout.isOpened()){
         fout << "Points0" << matches0 << "Points1" << matches1;
         fout.release();
     }

}

void saveInliersPercentages(std::vector<double> percs, int expnum, const std::string &debug_folder) {
	std::string path = debug_folder + "/inlperc";
	path += "-" + std::to_string(expnum) + ".txt";
	std::ofstream writer(path);
	for (auto perc : percs) {
		writer << perc << std::endl;
	}
	writer.close();
}

std::vector<double> CalculeVarianceOfDisAmonginliers(std::vector<cv::Point2f> &matches0, std::vector<cv::Point2f> &matches1){
	cv::Point2f PointCentreIm0(0,0);
	cv::Point2f PointCentreIm1(0,0);
	std::vector<double> Dis0;
	std::vector<double>Dis1;
	std::vector<double> DisTwoIm;
	std::vector<cv::Point2f>::iterator itMatches0= matches0.begin();
	std::vector<cv::Point2f>::iterator itMatches1= matches1.begin();
	
	for(;itMatches0!=matches0.end()&&itMatches1!=matches1.end();itMatches0++,itMatches1++){
		PointCentreIm0.x+=(*itMatches0).x;
		PointCentreIm0.y+=(*itMatches0).y;
		PointCentreIm1.x+=(*itMatches1).x;
		PointCentreIm1.y+=(*itMatches1).y;
	}
	PointCentreIm0.x=PointCentreIm0.x/(matches0.size());
	PointCentreIm0.y=PointCentreIm0.y/(matches0.size());
	PointCentreIm1.x=PointCentreIm1.x/(matches1.size());
	PointCentreIm1.y=PointCentreIm1.y/(matches1.size());
	
	for(itMatches0= matches0.begin(),itMatches1= matches1.begin();\
			itMatches0!=matches0.end()&&itMatches1!=matches1.end();\
			itMatches0++,itMatches1++){
		Dis0.push_back(std::hypot(((*itMatches0).x-PointCentreIm0.x),\
								((*itMatches0).y-PointCentreIm0.y)));
		Dis1.push_back(std::hypot(((*itMatches1).x-PointCentreIm1.x),\
								((*itMatches1).y-PointCentreIm1.y)));
	}
	
	std::vector<double>::iterator itDis0= Dis0.begin();
	std::vector<double>::iterator itDis1= Dis1.begin();
	double TempDis0=0;
	double TempDis1=0;
	for(;itDis0!=Dis0.end()&&itDis1!=Dis1.end();itDis0++,itDis1++){
		TempDis0+=*(itDis0);
		TempDis1+=*(itDis1);
	}
	DisTwoIm.push_back(TempDis0/Dis0.size());
	DisTwoIm.push_back(TempDis1/Dis1.size());
	return DisTwoIm;
}

std::vector<unsigned int> CalculeNumberOfInliersSimplyfied(std::vector<cv::Point2f> &matches0, std::vector<cv::Point2f> &matches1){
	
	std::vector<cv::Point2f>::iterator itMatches0= matches0.begin();
	std::vector<cv::Point2f>::iterator itMatches0_1= matches0.begin();
	std::vector<cv::Point2f>::iterator itMatches1= matches1.begin();
	std::vector<cv::Point2f>::iterator itMatches1_1= matches1.begin();
	
	double DisMin=42.6;
	unsigned int InliersCounter0=0;
	unsigned int InliersCounter1=0;
	std::vector<unsigned int> InliersCountTwoIm;
	bool NewInlierflag=true;
	for(;(itMatches0+1)!=matches0.end();itMatches0++){
		NewInlierflag=true;
			for(itMatches0_1=itMatches0+1;itMatches0_1!=matches0.end();itMatches0_1++){
				if(std::hypot(((*itMatches0).x-(*itMatches0_1).x),((*itMatches0).y-(*itMatches0_1).y))<DisMin){
					NewInlierflag=false;
					break;
				}
			}
			if(NewInlierflag){
				InliersCounter0++;
			}
	}
	
	for(;(itMatches1+1)!=matches1.end();itMatches1++){
		NewInlierflag=true;
			for(itMatches1_1=itMatches1+1;itMatches1_1!=matches1.end();itMatches1_1++){
				if(std::hypot(((*itMatches1).x-(*itMatches1_1).x),((*itMatches1).y-(*itMatches1_1).y))<DisMin){
					NewInlierflag=false;
					break;
				}
			}
			if(NewInlierflag){
				InliersCounter1++;
			}
	}
	InliersCountTwoIm.push_back(InliersCounter0);
	InliersCountTwoIm.push_back(InliersCounter1);
	return InliersCountTwoIm;
}

bool bFlagWithInitialInliers=false;
std::vector<cv::Point2f> InitInliersCam1;
std::vector<cv::Point2f> InitInliersCam2;

cv::Mat FundamentalMatSolver::solve(const cv::Mat &Finit) {

	
	cv::initModule_nonfree();
	srand((unsigned int) time(nullptr));

	double k_squared=-2*log(1.0f-alpha);

	std::vector<FVMatcher::FVMatch> init_matches=init_matcher->computeMatches();
	//We use ptt0 and ptt1 to save inliers when initilization, when estimation, it's  currentMatches0 and currentMatches1 to save them 
	//modified by FZZ, change definition of 
	/*
	std::vector<cv::Point2f> ptt0;
	std::vector<cv::Point2f> ptt1;
	rawToPoint2f(init_matches,ptt0,ptt1);
	*/
	/*************************/
	static  std::vector<cv::Point2f> ptt0;
	static std::vector<cv::Point2f> ptt1;
	std::vector<cv::Point2f> new_p0;
    std::vector<cv::Point2f> new_p1;
	rawToPoint2f(init_matches,new_p0,new_p1);
	for (size_t i = 0; i < new_p0.size(); ++i) {
        ptt0.push_back(new_p0[i]);
        ptt1.push_back(new_p1[i]);
    }
	std::vector<double> DisTwoIm;
	std::vector<unsigned int> InliersCountTwoIm;
	DisTwoIm=CalculeVarianceOfDisAmonginliers(ptt0,ptt1);//Calcule the dispersion of features found
	InliersCountTwoIm=CalculeNumberOfInliersSimplyfied(ptt0,ptt1);//Calcule the nomber of features, consider those features close to be one feature
	
	std::cout <<"Dis0: "<<DisTwoIm[0]<< std::endl;
	std::cout <<"Dis1: "<<DisTwoIm[1]<<std::endl;
	std::cout <<"InliersNum: "<<ptt0.size()<<std::endl;
	std::cout <<"CountInliersIm0 simplyfied: "<<InliersCountTwoIm[0]<< std::endl;
	std::cout <<"CountInliersIm1 simplyfied:  "<<InliersCountTwoIm[1]<< std::endl;
	double ThresholdDis=340.5;//Threshold of  dispersion of features found
	//unsigned int ThresholdNumInliers=60;
	unsigned int ThresholdNumNewInliers=60;//Threshold of nomber of features found
	unsigned int MAX_Accummulation=20;//Thershold of maximum nomber of pictures can be accumulated when initialisation
	//unsigned int MIN_Accummulation=10;
	static unsigned int AccummulationCount=0;
	if(AccummulationCount<MAX_Accummulation&&\
		( DisTwoIm[0]<ThresholdDis||\
		DisTwoIm[1]<ThresholdDis||\
		InliersCountTwoIm[0]<ThresholdNumNewInliers||\
		InliersCountTwoIm[1]<ThresholdNumNewInliers
		)){
		std::cout << "Inliers not sufficient for estimation " << std::endl;
		AccummulationCount=AccummulationCount+1;
		throw EstimationErrorException();
	}
	#ifdef ENABLE_DEBUG
	saveBeginInliersxml(ptt0, ptt1, r1->getFrameIndex(), 0, debug_folder);
	#endif
	AccummulationCount=0;
	/********************************/
	
	std::vector<int> permutation(9);
	permutation[0]=0;permutation[1]=3;permutation[2]=6;
	permutation[3]=1;permutation[4]=4;permutation[5]=7;
	permutation[6]=2;permutation[7]=5;permutation[8]=8;

	//Initial estimation of the F matrix
	std::cout << "Estimation frame "<< r1->getFrameIndex() << " in progress..." << std::endl;
    if(r1->getFrameIndex()!=r2->getFrameIndex()){
        std::cerr << "ERROR: Frames are misaligned. Check that your dataset is symmetric for the two cameras. Exiting." << std::endl;
        exit(1);
    }
	cv::Mat F;
    Eigen::Matrix3d eF;
	F=cv::Mat::zeros(3,3,cv::DataType<double>::type);
    Eigen::Matrix<double, 9, 9> ecurrentCov;
    cv::Mat image0, image1;
	
    if (cv::norm(Finit) > 0) {
		//When Finit existes
        size_t target = ptt0.size() * 5;
        ptt0.clear();
        ptt1.clear();
#ifdef ENABLE_DEBUG
        std::cout << "Estimating with init F: target number of matches is " << target << std::endl;
#endif
        F = Finit;
        cv2eigen(F, eF);
        ecurrentCov = Eigen::Matrix<double, 9, 9>::Zero();
        std::vector<std::pair<double, double>> empty;
        densityEstimator->initEstimator(empty);
        while (ptt0.size() < target && r1->getNextFrame(image0) && r2->getNextFrame(image1)) {
            std::shared_ptr<FUncertainty> func(new FUncertainty(eF, ecurrentCov, image0.cols));
            Eigen::Matrix3d eFoldtransposed = eF.transpose() * 1;
            Eigen::Matrix<double, 9, 9> eCovoldtransposed;
            permuteCov(ecurrentCov, permutation, eCovoldtransposed);
            std::shared_ptr<FUncertainty> func2(new FUncertainty(eFoldtransposed, eCovoldtransposed, image0.cols));
            FVMatcher::SIFTGuidedMatching guided = FVMatcher::SIFTGuidedMatching(func, func2, image0, image1,
                                                                                 ptt0, ptt1,
                                                                                 std::vector<bool>(ptt0.size(), false),
                                                                                 std::vector<bool>(ptt0.size(), false),
                                                                                 densityEstimator, sigmaFunction,
                                                                                 depth, th, k_squared);
            std::vector<FVMatcher::FVMatch> new_matches = guided.computeMatches();
            std::vector<cv::Point2f> new_p0;
            std::vector<cv::Point2f> new_p1;
            rawToPoint2f(new_matches, new_p0, new_p1);
            for (size_t i = 0; i < new_p0.size(); ++i) {
                ptt0.push_back(new_p0[i]);
                ptt1.push_back(new_p1[i]);
            }
        }
		
    }
    


	std::vector<bool> mask(ptt0.size());
	/*In this part, we should judge that are those inliers enough to estime, if not, continue to accumulate*/
	try{
		estimator->estimateF(ptt0,ptt1,F,mask);
	}catch(EstimationErrorException & e){	
		throw e;
	}

	std::cout << "No errors in initilization process: " << std::endl;
	
    cv2eigen(F, eF);

	//Select only inliers as current matches set
	std::vector<cv::Point2f> currentMatches0;
	std::vector<cv::Point2f> currentMatches1;
    
  
    
	int numinl = 0;
	for (size_t i = 0; i < mask.size(); ++i) {
		if(mask[i]){
			currentMatches0.push_back(ptt0[i]);
			currentMatches1.push_back(ptt1[i]);
			numinl++;
		}
	}
 #ifdef ENABLE_DEBUG 
    if(bFlagWithInitialInliers){
		std::vector<cv::Point2f>::iterator itInitInliersCam1= InitInliersCam1.begin();
		std::vector<cv::Point2f>::iterator itInitInliersCam2= InitInliersCam2.begin();
		std::vector<cv::Point2f>::iterator itcurrentMatches0= currentMatches0.begin();
		std::vector<cv::Point2f>::iterator itcurrentMatches1= currentMatches1.begin();
		
		bool bInliersNotExist=true;
		for(;itInitInliersCam1!=InitInliersCam1.end()\
			&&itInitInliersCam2!=InitInliersCam2.end();\
			itInitInliersCam1++,itInitInliersCam2++){
				bInliersNotExist=true;
				for(itcurrentMatches0= currentMatches0.begin(),itcurrentMatches1= currentMatches1.begin();\
					itcurrentMatches0!=currentMatches0.end()&&\
					itcurrentMatches1!=currentMatches1.end();\
					itcurrentMatches0++,itcurrentMatches1++
				){
					if((std::hypot(((*itcurrentMatches0).x-(*itInitInliersCam1).x),\
						((*itcurrentMatches0).y-(*itInitInliersCam1).y))<1.0)&&\
						(std::hypot(((*itcurrentMatches1).x-(*itInitInliersCam2).x),\
						((*itcurrentMatches1).y-(*itInitInliersCam2).y))<1.0)
					){
					bInliersNotExist=false;
						break;
				}
			
			}
			if(bInliersNotExist){
				currentMatches0.push_back(*(itInitInliersCam1));
				currentMatches1.push_back(*(itInitInliersCam2));
				numinl++;
			}
		}
    }     
#endif
    
	std::vector<double> inlsperc;
	inlsperc.push_back(1.0 * numinl / mask.size());

	refinement(currentMatches0, currentMatches1, eF);
	eigen2cv(eF,F);


#ifdef ENABLE_DEBUG
	saveFxml(F, r1->getFrameIndex(), experiment_number, debug_folder);
	saveInliersxml(currentMatches0, currentMatches1, r1->getFrameIndex(), experiment_number, debug_folder);
#endif


    double raw_covar[9][9];
	optimizer->getCovarF(raw_covar);
	for (size_t i = 0; i < 9; ++i) {
		for (size_t j = 0; j < 9; ++j) {
			ecurrentCov(i,j)=raw_covar[i][j];
		}
	}
	cv::Mat currentCov=cv::Mat(9,9, CV_64FC1);
	eigen2cv(ecurrentCov,currentCov);


    inliers_image0.clear();
	inliers_image1.clear();
	for (size_t i = 0; i < currentMatches0.size(); ++i) {
		inliers_image0.push_back(currentMatches0[i]);
		inliers_image1.push_back(currentMatches1[i]);
	}


	while(r1->getNextFrame(image0) && r2->getNextFrame(image1) && (last_frame<0 || r2->getFrameIndex()<=last_frame)){



		std::cout << "Estimation frame "<< r1->getFrameIndex() << " in progress..." << std::endl;

		//Detect core points
		std::vector<bool> is_core0(currentMatches0.size());
		labelCorePoints(currentMatches0,is_core0);
		std::vector<bool> is_core1(currentMatches1.size());
		labelCorePoints(currentMatches1,is_core1);

		std::shared_ptr<FUncertainty> func(new FUncertainty(eF, ecurrentCov, image0.cols));
		Eigen::Matrix3d eFoldtransposed=eF.transpose()*1;
		Eigen::Matrix<double,9,9> eCovoldtransposed;
		permuteCov(ecurrentCov,permutation,eCovoldtransposed);
		std::shared_ptr<FUncertainty> func2(new FUncertainty(eFoldtransposed, eCovoldtransposed, image0.cols));

		int tot=0;
		for (auto &&i : is_core0) {
			if (i)tot++;
		}
		//init the density estimator with the current set of inliers
		std::vector<std::pair<double, double>> mtc;
		for (auto match : currentMatches0) {
			mtc.push_back(std::make_pair(match.x, match.y));
		}
		densityEstimator->initEstimator(mtc);

		FVMatcher::SIFTGuidedMatching guided = FVMatcher::SIFTGuidedMatching(func, func2, image0, image1,
																			 currentMatches0, currentMatches1, is_core0,
																			 is_core1, densityEstimator, sigmaFunction,
																			 depth, th, k_squared);

		std::vector<FVMatcher::FVMatch> new_matches=guided.computeMatches();

		std::vector<cv::Point2f> new_p0;
		std::vector<cv::Point2f> new_p1;
		rawToPoint2f(new_matches,new_p0,new_p1);

#ifdef ENABLE_DEBUG
      std::cout << " Estimating new F with " << currentMatches0.size() << " + " << new_p0.size() << std::endl;
#endif
		std::vector<cv::Point2f> tot0;
		std::vector<cv::Point2f> tot1;
		for (size_t i = 0; i < currentMatches0.size(); ++i) {
			tot0.push_back(currentMatches0[i]);
			tot1.push_back(currentMatches1[i]);
		}
		for (size_t i = 0; i < new_p0.size(); ++i) {
			tot0.push_back(new_p0[i]);
			tot1.push_back(new_p1[i]);
		}

		std::vector<bool> mask2(tot0.size());
		F=cv::Mat::zeros(3,3,cv::DataType<double>::type);

		try{
			estimator->estimateF(tot0,tot1,F,mask2);
		}catch(EstimationErrorException & e){
			throw e;
		}
		cv2eigen(F,eF);

		currentMatches0.clear();
		currentMatches1.clear();
		numinl = 0;
		for (size_t i = 0; i < mask2.size(); ++i) {
			if(mask2[i]){
				currentMatches0.push_back(tot0[i]);
				currentMatches1.push_back(tot1[i]);
				numinl++;
			}
		}
		
 #ifdef ENABLE_DEBUG 
	if(bFlagWithInitialInliers){
		std::vector<cv::Point2f>::iterator itInitInliersCam1= InitInliersCam1.begin();
		std::vector<cv::Point2f>::iterator itInitInliersCam2= InitInliersCam2.begin();
		std::vector<cv::Point2f>::iterator itcurrentMatches0= currentMatches0.begin();
		std::vector<cv::Point2f>::iterator itcurrentMatches1= currentMatches1.begin();
		bool bInliersNotExist=true;
		
		for(itInitInliersCam1= InitInliersCam1.begin(),itInitInliersCam2= InitInliersCam2.begin()\
			;itInitInliersCam1!=InitInliersCam1.end()\
			&&itInitInliersCam2!=InitInliersCam2.end();\
			itInitInliersCam1++,itInitInliersCam2++){
				bInliersNotExist=true;
				for(itcurrentMatches0= currentMatches0.begin(),itcurrentMatches1= currentMatches1.begin();\
					itcurrentMatches0!=currentMatches0.end()&&\
					itcurrentMatches1!=currentMatches1.end();\
					itcurrentMatches0++,itcurrentMatches1++
				){
					if((std::hypot(((*itcurrentMatches0).x-(*itInitInliersCam1).x),\
						((*itcurrentMatches0).y-(*itInitInliersCam1).y))<1.0)&&\
						(std::hypot(((*itcurrentMatches1).x-(*itInitInliersCam2).x),\
						((*itcurrentMatches1).y-(*itInitInliersCam2).y))<1.0)
					){
					bInliersNotExist=false;
						break;
				}
			
			}
			if(bInliersNotExist){
				currentMatches0.push_back(*(itInitInliersCam1));
				currentMatches1.push_back(*(itInitInliersCam2));
				numinl++;
			}
		}
	}	
#endif
		
		inlsperc.push_back(1.0 * numinl / mask2.size());


#ifdef ENABLE_DEBUG
      std::cout << "Ending up with " << currentMatches0.size() << " inliers"<<std::endl;
#endif

		inliers_image0.clear();
		inliers_image1.clear();
		for (size_t i = 0; i < currentMatches0.size(); ++i) {
			inliers_image0.push_back(currentMatches0[i]);
			inliers_image1.push_back(currentMatches1[i]);
		}

		refinement(currentMatches0,currentMatches1,eF);
		eigen2cv(eF,F);
#ifdef ENABLE_DEBUG
		saveFxml(F, r1->getFrameIndex(), experiment_number, debug_folder);
		saveInliersxml(currentMatches0, currentMatches1, r1->getFrameIndex(), experiment_number, debug_folder);
		//saveInliersxml(tot0,tot1,r1->getFrameIndex(),experiment_number);
#endif
		optimizer->getCovarF(raw_covar);
		for (size_t i = 0; i < 9; ++i) {
			for (size_t j = 0; j < 9; ++j) {
				ecurrentCov(i,j)=raw_covar[i][j];
			}
		}
		eigen2cv(ecurrentCov,currentCov);
	}
#ifdef ENABLE_DEBUG
	std::cout << "Saving inliers percentages " << std::endl;
	saveInliersPercentages(inlsperc, experiment_number, debug_folder);
#endif
	return F;
}
