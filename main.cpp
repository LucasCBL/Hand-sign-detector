#include "pch.h"
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/video/background_segm.hpp>
#include <opencv2/core/types.hpp>
#include <stdio.h>
#include <string>
#include <iostream>

#include "MyBGSubtractorColor.hpp"
#include "HandGesture.hpp"

using namespace std;
using namespace cv;



int main(int argc, char** argv)
{

	Mat frame, bgmask, out_frame, bg, diff,subs;
	
	Mat concat_image;

	//Abrimos la webcam

	VideoCapture cap;
	cap.open(0);
	if (!cap.isOpened())
	{
		printf("\nNo se puede abrir la cámara\n");
		return -1;
	}
        int cont = 0;
        while (frame.empty()&& cont < 2000 ) {

                cap >> frame;
                ++cont;
        }
        if (cont >= 2000) {
                printf("No se ha podido leer un frame válido\n");
                exit(-1);
        }

	// Creamos las ventanas que vamos a usar en la aplicación

	namedWindow("Reconocimiento");
	//namedWindow("Fondo");
	namedWindow("Origen");
	namedWindow("Fondo, Diferencia");

    // creamos el objeto para la substracción de fondo
	MyBGSubtractorColor filtro(cap);

	// creamos el objeto para el reconocimiento de gestos

	// iniciamos el proceso de obtención del modelo del fondo
	filtro.LearnModel();
	filtro.ObtainBG(bg);
	bg = bg(Rect(bg.cols / 2, bg.rows / 2, bg.cols / 2, bg.rows / 2));
	bool ex = false;
	while(!ex)
	{
		cap >> frame;
		flip(frame, frame, 1);
		if (frame.empty())
		{
			printf("Leído frame vacío\n");
			continue;
		}
		int c = waitKey(40);
		if ((char)c == 'q') {
			break;
			ex = true;
		} 
		Mat small_frame = frame(Rect(frame.cols / 2, frame.rows / 2, frame.cols / 2, frame.rows / 2));
		// obtenemos la máscara del fondo con el frame actual
		filtro.ObtainBGMask(small_frame,bgmask);
                // CODIGO 2.1
                // limpiar la máscara del fondo de ruido
                //...
		medianBlur(bgmask,bgmask,7);


		// deteccion de las características de la mano

                // mostramos el resultado de la sobstracción de fondo
		//void bitwise_and(InputArray src1, InputArray src2, OutputArray dst, InputArray mask = noArray())
		absdiff(bg,small_frame,diff);
		cvtColor(diff, diff, COLOR_BGR2GRAY);
		//fastNlMeansDenoising(diff, diff, 10, 7, 21);
		//GaussianBlur(diff,diff,Size(3,3),0);
		medianBlur(diff, diff, 7);
		threshold(diff, diff, 10, 255,THRESH_BINARY);

		

		bitwise_and(bgmask, diff, subs);
		hconcat(vector<cv::Mat>{bgmask, diff, subs}, concat_image);
		//hconcat(concat_image, subs);

		//hconcat(std::vector<cv::Mat>{small_frame, bgmask}, Concatenated_image);
                // mostramos el resultado del reconocimento de gestos
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

					for (size_t k = 0; k < defects[i].size(); k++) {
						if (defects[i][k][3] > 13 * 256) {
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

		imshow("Origen", frame);
		imshow("Reconocimiento", small_frame);
		//imshow("Fondo", subs);
		imshow("Fondo, Diferencia", concat_image);
	}
	
	destroyWindow("Reconocimiento");
	//destroyWindow("Fondo");
	destroyWindow("Origen");
	destroyWindow("Fondo, Diferencia");
	cap.release();
	return 0;
}
