
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
void HandGesture::FeaturesDetection(Mat small_frame, Mat subs, Mat frame) {
	int count;
	char a[40];

	Mat element = getStructuringElement(MORPH_RECT, Size(2 * 4 + 1, 2 * 4 + 1), Point(4, 4));
	dilate(subs, subs, element);
	erode(subs, subs, element);

	///////////////////////////////CODIGO TEMPORAL/////////////////////////////
	vector<vector<Point>> contours;
	vector<Vec4i>hierarchy;

	findContours(subs, contours, hierarchy, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE, Point());


	size_t indexOfBiggestContour = -1;
	size_t sizeOfBiggestContour = 0;

	if (contours.size() > 0) {
		sizeOfBiggestContour = 0;
		indexOfBiggestContour = -1;

		for (size_t i = 0; i < contours.size(); i++) {
			if (contours[i].size() > sizeOfBiggestContour) {
				sizeOfBiggestContour = contours[i].size();
				indexOfBiggestContour = i;
			}
		}
	}
	drawContours(small_frame, contours, indexOfBiggestContour, cv::Scalar(255, 0, 0), 2, 8, vector<Vec4i>(), 0, Point());

	vector<vector<int> >hull(contours.size());
	vector<vector<Point> >hullPoint(contours.size());
	vector<vector<Vec4i> >defects(contours.size());
	vector<vector<Point> >defectPoint(contours.size());
	vector<vector<Point> >contours_poly(contours.size());
	Point2f rect_point[4];
	vector<RotatedRect>minRect(contours.size());
	vector<Rect> boundRect(contours.size());
	


	for (size_t i = 0; i < contours.size(); i++) {
		if (contourArea(contours[i]) > 5000) {
			convexHull(contours[i], hull[i], true);
			convexityDefects(contours[i], hull[i], defects[i]);
			if (indexOfBiggestContour == i) {
				minRect[i] = minAreaRect(contours[i]);
				for (size_t k = 0; k < hull[i].size(); k++) {
					int ind = hull[i][k];
					hullPoint[i].push_back(contours[i][ind]);
				}
				count = 0;
				Rect bounding_rect = boundingRect(Mat(hullPoint[i]));
				for (size_t k = 0; k < defects[i].size(); k++) {
					if (defects[i][k][3] > bounding_rect.height * 256 * 0.25 && defects[i][k][3] < bounding_rect.height * 256*0.8) {
						/*   int p_start=defects[i][k][0];   */
						int p_end = defects[i][k][1];
						int p_far = defects[i][k][2];
						defectPoint[i].push_back(contours[i][p_far]);
						circle(small_frame, contours[i][p_end], 3, Scalar(0, 255, 0), 2);
						count++;
					}
				}

				if (count == 1)
					strcpy_s(a, "ONE");

				else if (count == 2)
					strcpy_s(a, "TWO");
				else if (count == 3)
					strcpy_s(a, "THREE");
				else if (count == 4)
					strcpy_s(a, "FOUR");
				else if (count == 5)
					strcpy_s(a, "FIVE");
				else
					strcpy_s(a, "Welcome !!");

				putText(frame, a, Point(70, 70), FONT_HERSHEY_SIMPLEX, 3, Scalar(255, 0, 0), 2, 8, false);
				drawContours(subs, contours, i, Scalar(255, 255, 0), 2, 8, vector<Vec4i>(), 0, Point());
				drawContours(subs, hullPoint, i, Scalar(255, 255, 0), 1, 8, vector<Vec4i>(), 0, Point());
				drawContours(small_frame, hullPoint, i, Scalar(0, 0, 255), 2, 8, vector<Vec4i>(), 0, Point());
				approxPolyDP(contours[i], contours_poly[i], 3, false);
				boundRect[i] = boundingRect(contours_poly[i]);
				rectangle(small_frame, boundRect[i].tl(), boundRect[i].br(), Scalar(255, 0, 0), 2, 8, 0);
				minRect[i].points(rect_point);
				for (size_t k = 0; k < 4; k++) {
					line(small_frame, rect_point[k], rect_point[(k + 1) % 4], Scalar(0, 255, 0), 2, 8);
				}

			}
		}

	}
}

void print_with_finger(Mat small_frame, Mat subs, Mat frame) {

}
	
