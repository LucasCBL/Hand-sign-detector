
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <stdio.h>
#include <iostream>
using namespace cv;

int main(int argc, char** argv)
{
	char* imageName = argv[1];

	Mat image;
	image = imread(imageName, 1);

	if (argc != 2 || !image.data)
	{
		printf(" No image data \n ");
		return -1;
	}

	Mat gray_image;
	
	// convertimos la imagen en color a escala de grises
	cvtColor(image, gray_image, CV_BGR2GRAY);

	//guardamos la imagen
	imwrite("Gray_Image.jpg", gray_image);

	//creamos las ventanas para mostrar las imágenes
	namedWindow(imageName, CV_WINDOW_AUTOSIZE);
	namedWindow("Gray image", CV_WINDOW_AUTOSIZE);

	//mostramos las imagenes
	imshow(imageName, image);
	imshow("Gray image", gray_image);
	
	//esperamos a que el usuario pulse una tecla
	waitKey(0);

	return 0;

}