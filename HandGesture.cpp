
#include "pch.h"
#include "HandGesture.hpp"

#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/video/background_segm.hpp>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <string>

using namespace cv;
using namespace std;

HandGesture::HandGesture() {
	
}


double HandGesture::getAngle(Point s, Point e, Point f) {
	
	double v1[2],v2[2];
	v1[0] = s.x - f.x;
	v1[1] = s.y - f.y;

	v2[0] = e.x - f.x;
	v2[1] = e.y - f.y;

	double ang1 = atan2(v1[1],v1[0]);
	double ang2 = atan2(v2[1],v2[0]);

	double angle = ang1 - ang2;
	if (angle > CV_PI) angle -= 2 * CV_PI;
	if (angle < -CV_PI) angle += 2 * CV_PI;
	return (angle * 180.0/CV_PI);
}
void HandGesture::FeaturesDetection(Mat mask, Mat output_img) {
	
	vector<vector<Point> > contours;
	Mat temp_mask;
	mask.copyTo(temp_mask);
	int index = -1;

        // CODIGO 3.1
        // detección del contorno de la mano y selección del contorno más largo
        //...

        // pintar el contorno
        //...
	

	//obtener el convex hull	
	vector<int> hull;
	convexHull(contours[index],hull);
	
	// pintar el convex hull
	Point pt0 = contours[index][hull[hull.size()-1]];
	for (int i = 0; i < hull.size(); i++)
	{
		Point pt = contours[index][hull[i]];
		line(output_img, pt0, pt, Scalar(0, 0, 255), 2, LINE_AA);
		pt0 = pt;
	}
	
        //obtener los defectos de convexidad
	vector<Vec4i> defects;
	convexityDefects(contours[index], hull, defects);
		
		
		int cont = 0;
		for (int i = 0; i < defects.size(); i++) {
			Point s = contours[index][defects[i][0]];
			Point e = contours[index][defects[i][1]];
			Point f = contours[index][defects[i][2]];
			float depth = (float)defects[i][3] / 256.0;
			double angle = getAngle(s, e, f);
		
                        // CODIGO 3.2
                        // filtrar y mostrar los defectos de convexidad
                        //...

                }
	
		
}
