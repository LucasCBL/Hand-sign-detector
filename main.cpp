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
		hand.FeaturesDetection(small_frame, subs, frame);

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
