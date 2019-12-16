#pragma once

#include <opencv2/imgproc/imgproc.hpp>
#include<opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/video/background_segm.hpp>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <string>

class HandGesture {
private:
	cv::Point last_center;//para comparar el mopvimiento del centro
public:
	HandGesture();
	void FeaturesDetection(cv::Mat mask, cv::Mat subs);
	void print_with_finger(cv::Mat, cv::Mat);
	

private:
	double getAngle(cv::Point s, cv::Point e, cv::Point f);
	
	

};
