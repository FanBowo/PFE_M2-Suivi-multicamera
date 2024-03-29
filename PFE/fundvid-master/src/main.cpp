/*
 * main.cpp
 *
 *  Created on: 26 ago 2016
 *      Author: Nicola Pellicanò
 */

/** @file main.cpp
 *
 *  This file contains the demo's
 *  main() function.
 *
 */
#include <gvars3/instances.h>

#include <iostream>
#include <cstdlib>
#include <regex>
#include <memory>
#include "fundamentalmat_solver.h"
#include <src/density_estimator/kernel_density_estimator.h>
#include <src/kernels/histogram_kernel.h>
#include <src/sigma_functions/sigma_step_function.h>
#include <src/kernels/epanechnikov_kernel.h>
#include <src/sigma_functions/sigma_sigmoid_function.h>

#include "reader/imagesreader.h"
#include "reader/boostreader.h"
#include "matchers/sift_opencv_matcher.h"
#include "estimation/orsa_estimator.h"
#include "estimation/lm_fundmat_optimizer.h"


#include "exceptions/estimation_error_exception.h"
#include "exceptions/optimization_exception.h"

#include <fstream>

/**
 * Main function.
 * List of input arguments:
 * 	- argv[1] config file
 * 	- argv[2] output folder
 *  - argv[3] experiment number (debug mode only)
 *  - argv[4] debug folder
 *	- argv[5]  Init Inliers file  
 */
//#define ENABLE_DEBUG

 #ifdef ENABLE_DEBUG 
extern bool bFlagWithInitialInliers;
extern std::vector<cv::Point2f> InitInliersCam1;
extern std::vector<cv::Point2f> InitInliersCam2;
 #endif
 
int main(int argc, char **argv) {

	GVars3::GUI.LoadFile(argv[1]);
    if(argc<3){
        std::cerr<< "Wrong number of input arguments :" << std::endl;
		std::cerr<< "1. configuration file path" << std::endl;
		std::cerr<< "2. output folder" << std::endl;
#ifdef ENABLE_DEBUG
        std::cerr<< "3. experiment number (DEBUG MODE ONLY)" << std::endl;
        std::cerr << "4. debug folder (DEBUG MODE ONLY)" << std::endl;
		std::cerr << "5. Init Inliers (DEBUG MODE ONLY)" << std::endl;
#endif
        return 1;
	}
	std::string im0f=GVars3::GV3::get<std::string>("images0_folder");
	std::string im1f=GVars3::GV3::get<std::string>("images1_folder");
	//std::string logf=GVars3::GV3::get<std::string>("log_path");

    std::string fname1 = GVars3::GV3::get<std::string>("first_img_name0");
    if (!std::regex_match(fname1.c_str(), std::regex(R"([A-Za-z_\d]+[\-]*+(\d+)[.]{1}\w+)"))) {
        std::cerr << "Image file name 0 formatting error" << std::endl;
        return 2;
    }


    std::string fname2 = GVars3::GV3::get<std::string>("first_img_name1");
    if (!std::regex_match(fname2.c_str(), std::regex(R"([A-Za-z_\d]+[\-]*+(\d+)[.]{1}\w+)"))) {
        std::cerr << "Image file name 1 formatting error" << std::endl;
        return 2;
    }

    int dt = GVars3::GV3::get<int>("dt");
    if(dt<=0){
        std::cerr << "Error in the dt parameter: good values are greater than 0." << std::endl;
        return 3;
    }

//	std::smatch match;
//    std::regex_search(fname,match,std::regex("([A-Za-z_]+)\\d+[.]{1}\\w+"));
//	std::string base=match[1].str();
//	std::smatch match2;
//    std::regex_search(fname,match2,std::regex("[A-Za-z_]+\\d+[.]{1}(\\w+)"));
//	std::string ext=match2[1].str();

    std::string base = GVars3::GV3::get<std::string>("base");
    std::string ext = GVars3::GV3::get<std::string>("extension");

	double alpha=GVars3::GV3::get<double>("alpha");
	if(alpha<=0 || alpha>=1) {
		std::cerr << "Error in the alpha parameter: good values are in the interval ]0,1[." << std::endl;
        return 4;
	}

	int depth=GVars3::GV3::get<int>("k_depth");
	if(depth<=0){
		std::cerr << "Error in the k_depth parameter: good values are greater than 0." << std::endl;
        return 5;
	}
	double th=GVars3::GV3::get<double>("SIFT_th");
	if(th<=0 || th>=1) {
		std::cerr << "Error in the SIFT_th parameter: good values are in the interval ]0,1[." << std::endl;
        return 6;
	}
	int num_iter=GVars3::GV3::get<int>("number_iterations");
	if(num_iter<=0) {
		std::cerr << "Error in the number_iterations parameter: good values are greater than 0." << std::endl;
        return 7;
	}
		int num_pts=GVars3::GV3::get<int>("num_pts");
	if(num_pts<=0){
		std::cerr << "Error in the num_pts parameter: good values are greater than 0." << std::endl;
        return 8;
	}
	float epsilon=GVars3::GV3::get<float>("epsilon");
	if(epsilon<=0){
		std::cerr << "Error in the epsilon parameter: good values are greater than 0." << std::endl;
        return 9;
	}
	double sigma_high=GVars3::GV3::get<double>("sigma_high");
	if(sigma_high<=0){
		std::cerr << "Error in the sigma_high parameter: good values are greater than 0." << std::endl;
        return 10;
	}
	double sigma_low=GVars3::GV3::get<double>("sigma_low");
	if(sigma_low<=0){
		std::cerr << "Error in the sigma_low parameter: good values are greater than 0." << std::endl;
        return 11;
	}


    std::string kernel = GVars3::GV3::get<std::string>("kernel");
	int last_frame=GVars3::GV3::get<int>("last_frame");
	float precision=GVars3::GV3::get<float>("precision");


	//ImagesReader* r1=new ImagesReader(im0f,base,ext,logf);
	//ImagesReader* r2=new ImagesReader(im1f,base,ext,logf);

    std::shared_ptr<FramesReader> r1(new BoostReader(im0f, ext, base, fname1, dt));
    std::shared_ptr<FramesReader> r2(new BoostReader(im1f, ext, base, fname1, dt));
	cv::Mat image0,image1;
	retry:
	if(!(r1->getNextFrame(image0) && r2->getNextFrame(image1) && (last_frame<0 || r2->getFrameIndex()<=last_frame))){
		std::cout << "All figures have been estimated, but non valide" << std::endl;
		return 0;
	}
	
    std::shared_ptr<FVMatcher::AbstractMatcher> in_matcher(
            new FVMatcher::SIFTOpenCVMatcher(image0, image1, 2, true, true, th));
	int height=image0.rows;
	int width=image1.cols;
    std::shared_ptr<AbstractEstimator> orsa(new ORSAEStimator(precision, num_iter, height, width));
    std::shared_ptr<FundMatOptimizer> optimizer(new LMFundMatOptimizer(true));

    std::unique_ptr<Kernel> kernel_ptr(new HistogramKernel(epsilon));
    if (kernel == "e")kernel_ptr.reset(new EpanechnikovKernel(epsilon));

#ifdef ENABLE_DEBUG
    kernel_ptr->declare();
#endif


    std::shared_ptr<DensityEstimator> densityEstimator(new KernelDensityEstimator(std::move(kernel_ptr), epsilon));

    //estimate crosspoint
    std::vector<std::pair<double, double>> fakePoints;
    for (int i = 0; i < num_pts; i++)fakePoints.emplace_back(0.0, epsilon / 2);
    densityEstimator->initEstimator(fakePoints);
    double crosspoint = densityEstimator->estimateDensity(std::make_pair(0.0, 0.0));
    std::cout << "Cross point density: " << crosspoint << std::endl;

    //std::shared_ptr<SigmaFunction> sigmaFunction(new SigmaStepFunction(sigma_low,sigma_high,crosspoint));
    double steepness = (2.0 / crosspoint) * log(0.99 / 0.01);
    std::shared_ptr<SigmaFunction> sigmaFunction(
            new SigmaSigmoidFunction(sigma_low, sigma_high, crosspoint, steepness));
    auto solver = new FundamentalMatSolver(r1, r2, in_matcher, orsa, optimizer, densityEstimator, sigmaFunction, alpha,
                                           depth, th, last_frame);
#ifdef ENABLE_DEBUG
    if (argc >= 4)solver->setExperimentNumber(atoi(argv[3])); else solver->setExperimentNumber(0);
    if (argc >= 5)solver->setDebugFolder(argv[4]); else solver->setDebugFolder("debug");
	if(argc>=6){
		cv::FileStorage fs(argv[5],cv::FileStorage::READ);
		if (!fs.isOpened())
		{
			std::cout<<"erreur ouverture fichier InitIliers"<<std::endl;
			exit(1);
		}
		bFlagWithInitialInliers=true;									
		fs["Points0"]>>InitInliersCam1;
		fs["Points1"]>>InitInliersCam2;
		fs.release();  
	}

#endif

    try{

        //cv::FileStorage fs("/mnt/dfb2ead7-9641-4658-8517-aee3c9acb560/Hamlyn-undistorted/F.xml", cv::FileStorage::READ);
        //cv::FileStorage fs("/mnt/dfb2ead7-9641-4658-8517-aee3c9acb560/Hamlyn-undistorted/F.xml", cv::FileStorage::READ);
        //cv::Mat Finit;
        //fs["Fundamental"] >> Finit;
        //fs.release();

        int estimation_mode = GVars3::GV3::get<int>("mode");
        cv::Mat Fsol;
        if (estimation_mode > 0) {
            std::string finit_path = GVars3::GV3::get<std::string>("finit");
            std::ifstream fff(finit_path);
            if (!fff.is_open()) {
                std::cout << "File not found (Finit)" << std::endl;
                return 1;
            }
            double a, b, c, d, e, f, g, h, i;
            fff >> a >> b >> c >> d >> e >> f >> g >> h >> i;
            cv::Mat Finit(3, 3, CV_64FC1);
            Finit.at<double>(0, 0) = a;
            Finit.at<double>(0, 1) = b;
            Finit.at<double>(0, 2) = c;
            Finit.at<double>(1, 0) = d;
            Finit.at<double>(1, 1) = e;
            Finit.at<double>(1, 2) = f;
            Finit.at<double>(2, 0) = g;
            Finit.at<double>(2, 1) = h;
            Finit.at<double>(2, 2) = i;
            fff.close();
			//#ifdef ENABLE_DEBUG
			std::cout<<"Begin in solve(Finit)"<<std::endl;
			//#endif
            Fsol = solver->solve(Finit);
        } else {
			//No Finit given 
			//#ifdef ENABLE_DEBUG
			std::cout<<"Begin in solver->solve()"<<std::endl;
			//#endif
            Fsol = solver->solve();
        }
		
		std::cout<<"Succeded in solve()"<<std::endl;
		
		std::ofstream out;
		std::string outfolder(argv[2]);
		std::string outfundamental=outfolder+"/f.txt";
		out.open(outfundamental.c_str());
		outfundamental=outfolder+"/Fmatrix.xml";
		cv::FileStorage fs(outfundamental.c_str(),cv::FileStorage::WRITE);
		
		out << Fsol.at<double>(0,0) << " " << Fsol.at<double>(0,1) << " " << Fsol.at<double>(0,2) << std::endl;
		out << Fsol.at<double>(1,0) << " " << Fsol.at<double>(1,1) << " " << Fsol.at<double>(1,2) << std::endl;
		out << Fsol.at<double>(2,0) << " " << Fsol.at<double>(2,1) << " " << Fsol.at<double>(2,2) << std::endl;
		out.close();
		
		fs <<"Fmatrix"<<Fsol;
		fs.release();
	
		std::cout << "Output fundamental matrix:\n" << std::endl;
		std::cout << Fsol << std::endl;
		std::vector<cv::Point2f> inl0=solver->getInliersImage0();
		std::vector<cv::Point2f> inl1=solver->getInliersImage1();
		std::string outinl0=outfolder+"/inliers0.txt";
		out.open(outinl0.c_str());
		for (size_t i = 0; i < inl0.size(); ++i) {
			out << inl0[i].x << " "<<inl0[i].y << std::endl;
		}
		out.close();
		std::string outinl1=outfolder+"/inliers1.txt";
		out.open(outinl1.c_str());
		for (size_t i = 0; i < inl1.size(); ++i) {
			out << inl1[i].x << " "<<inl1[i].y << std::endl;
		}
		out.close();

	}catch(EstimationErrorException & e){
		std::cerr << e.what() << std::endl;
		goto retry;
	}catch(OptimizationException & e){
		std::cerr << e.what() << std::endl;
	}



	delete solver;

}

