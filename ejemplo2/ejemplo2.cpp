#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"
#include <stdlib.h>
#include <stdio.h>

using namespace cv;


Mat src, src_gray;
Mat detected_edges;

int edgeThresh = 1;
int lowThreshold;
int const max_lowThreshold = 100;
int ratio = 3;
int kernel_size = 3;
char* window_name = "Bordes";


void CannyThreshold(int, void*)
{
	/// Detector de bordes Canny
	Canny(src_gray, detected_edges, lowThreshold, lowThreshold*ratio, kernel_size);

	imshow(window_name, detected_edges);
}


int main(int argc, char** argv)
{
	/// Cargar la imagen
	src = imread(argv[1]);

	if (!src.data)
	{
		return -1;
	}

	/// Convertir la imagen a escala de grises
	cvtColor(src, src_gray, CV_BGR2GRAY);

	/// Crear una ventana
	namedWindow(window_name, CV_WINDOW_AUTOSIZE);

	/// Crear un trackbar para el umbral del detector de bordes Canny
	createTrackbar("Umbral:", window_name, &lowThreshold, max_lowThreshold, CannyThreshold);

	/// Detectar los bordes y mostrar
	CannyThreshold(0, 0);

	/// Esperar a que el usuario presione una tecla
	waitKey(0);

	return 0;
}