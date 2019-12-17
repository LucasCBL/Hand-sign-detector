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

	Mat frame,small_frame, bgmask, out_frame, bg, diff,subs;
	
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
	//namedWindow("Origen");
	namedWindow("Fondo, Diferencia");
	namedWindow("Canvas");

    // creamos el objeto para la substracción de fondo
	MyBGSubtractorColor filtro(cap);

	// creamos el objeto para el reconocimiento de gestos

	// iniciamos el proceso de obtención del modelo del fondo
	filtro.LearnModel();
	filtro.ObtainBG(bg);
	bg = bg(Rect(bg.cols*0.5 / 3, bg.rows *0.5/ 3, bg.cols *2/ 3, bg.rows *2/ 3));
	Mat canvas(bg.rows, bg.cols , CV_8UC3, Scalar(0, 0, 0));
	bool ex = false;
	HandGesture hand;
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
		small_frame = frame(Rect(frame.cols *0.5/ 3, frame.rows *0.5/ 3, frame.cols *2/ 3, frame.rows*2 / 3));
		
		//elemento para posteriores eliminaciones de ruido
		Mat element = getStructuringElement(MORPH_RECT, Size(2 * 4 + 1, 2 * 4 + 1), Point(4, 4));

		// obtenemos la máscara del fondo con el frame actual
		filtro.ObtainBGMask(small_frame,bgmask);      
		medianBlur(bgmask,bgmask,7);
		dilate(bgmask, bgmask, element);
		erode(bgmask, bgmask, element);

		//SUBSTACCION DE FONDO
        // mostramos el resultado de la sobstracción de fondo
		absdiff(bg,small_frame,diff);
		cvtColor(diff, diff, COLOR_BGR2GRAY);

		medianBlur(diff, diff, 7);
		threshold(diff, diff, 10, 255,THRESH_BINARY);

		
		// and de las dos máscaras
		bitwise_and(bgmask, diff, subs);
		dilate(subs, subs, element);
		erode(subs, subs, element);


		char colortitle[] = "Filtrado por colores";
		char difftitle[] = "Filtrado por diferencia en fondo";
		char andtitle[] = "Combinacion and de ambos filtrados";
		putText(bgmask, colortitle, Point(10, 10), FONT_HERSHEY_SIMPLEX, 0.5, Scalar(255, 0, 0), 2, 8, false);
		putText(diff, difftitle, Point(10, 10), FONT_HERSHEY_SIMPLEX, 0.5, Scalar(255, 0, 0), 2, 8, false);
		putText(subs, andtitle, Point(10, 10), FONT_HERSHEY_SIMPLEX, 0.5, Scalar(255, 0, 0), 2, 8, false);

		hconcat(vector<cv::Mat>{bgmask, diff, subs}, concat_image);
		//hconcat(concat_image, subs);

		//hconcat(std::vector<cv::Mat>{small_frame, bgmask}, Concatenated_image);
                // mostramos el resultado del reconocimento de gestos
		Point pencil;
		pencil=hand.FeaturesDetection(small_frame, subs);
		if (pencil != Point(0, 0)) {
			circle(canvas, pencil, 2, Scalar(255, 255, 255), 2);
		}
		//imshow("Origen", frame);
		imshow("Reconocimiento", small_frame);
		//imshow("Fondo", subs);
		imshow("Fondo, Diferencia", concat_image);
		imshow("Canvas",canvas );
	}
	
	destroyWindow("Reconocimiento");
	//destroyWindow("Fondo");
	destroyWindow("Origen");
	destroyWindow("Canvas");
	destroyWindow("Fondo, Diferencia");
	cap.release();
	return 0;
}
