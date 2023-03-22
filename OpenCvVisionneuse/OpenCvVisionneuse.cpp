#include<opencv2/opencv.hpp>
#include<iostream>
#include <Windows.h>
#include <stdlib.h>
#include <mmsystem.h>

using namespace cv;
using namespace std;


Point computeCentroid(const Mat& mask) {
    Moments m = moments(mask, true);
    Point center(m.m10 / m.m00, m.m01 / m.m00);
    return center;
}


bool inRange(unsigned low, unsigned high, unsigned x) {
    return (low <= x && x <= high);
}


vector<Mat> sliderImages(vector<string> imgs, string basePath, int smollOrHq) {
    vector<Mat> imgsArray;
    
    // int smollOrHq :
    // 0 : for small images like the navbar.
    // 1 : for hq resolution like the image show
    if (smollOrHq == 0) {

        for (int i = 0; i < imgs.size(); i++) {

            Mat tmp = imread((basePath + imgs[i]), IMREAD_COLOR);

            // 900 * number of images to make them small for the navigation bar :
            resize(tmp, tmp, Size(ceil(900 / imgs.size()), 200));
            imgsArray.push_back(tmp);
        }

    }
    else if (smollOrHq == 1) {
        for (int i = 0; i < imgs.size(); i++) {

            Mat tmp = imread((basePath + imgs[i]), IMREAD_COLOR);

            //Hq resolution :
            resize(tmp, tmp, Size(1920, 1080));
            imgsArray.push_back(tmp);
        }

    }
    else {
        cout << "please chose the type of resize : 0 or 1 " << endl;
    }

    return imgsArray;
}

int main() {

    Mat myImage;//Declaring a matrix to load the frames
    VideoCapture cap(0);//Declaring an object to capture stream of frames
    if (!cap.isOpened()) { //This section prompt an error message if no video stream is found
        cout << "No video stream detected" << endl;
        system("pause");
        return-1;
    }

    /**
    //POUR LOUVERTURE DES IMAGES :
    vector<Mat> images;
    Mat horizontalStack;
    size_t count = fnImages.size(); //number of png files in images folder
    for (int i = 0; i < count; i++) {
        images.push_back(imread(fnImages[i]));
        hconcat(images[i], images.size(), horizontalStack);

    }
    vector<String> fnImages;
    glob("/*.png", fnImages, false);


    vector<Mat> imagesArray = concatImages(fnImages);
    **/


    try {
        string basePath = "C:\\Users\\Ce PC\\source\\repos\\OpenCvVisionneuse\\OpenCvVisionneuse\\images\\";
        vector<string> imgs = { "image1.jpg", "image2.jpg", "image3.jpg", "image4.jpg", "image5.jpg", "image6.jpg" };

        vector<Mat> smallImagesArray = sliderImages(imgs, basePath, 0);
        vector<Mat> hQImagesArray = sliderImages(imgs, basePath, 1);

        int currentImage = 0;
        Mat resultatImageSelectionner = hQImagesArray[currentImage];

        while (true) { //Taking an everlasting loop to show the video//

            cap >> myImage;

            if (myImage.empty()) { //Breaking the loop if no video frame is detected//
                break;
            }

            //Clone de l'image original pour pouvoir comparer visuellement
            Mat resultat = myImage.clone();
            Mat concatedImages;

            hconcat(smallImagesArray, concatedImages);

            //Conversion de l'image RGB en HSV
            Mat resultatHsv;
            cvtColor(resultat, resultatHsv, COLOR_BGR2HSV);

            Mat1b blueMask;
            inRange(resultatHsv, Scalar(78, 158, 124), Scalar(138, 255, 255), blueMask);
            
            //Trouve le contours avec labycentre
            vector<vector<Point>> contourMaskBlue;
            vector<Vec4i> paletteBleue;
            findContours(blueMask, contourMaskBlue, paletteBleue, RETR_TREE, CHAIN_APPROX_NONE);
            Point blueMaskCentroidPoints = computeCentroid(blueMask);
            int xPos = blueMaskCentroidPoints.x * 2; //*2 : Augmenter la vitesse de deplacement

            circle(concatedImages, Point(xPos, concatedImages.rows / 2), 10, Scalar(0, 0, 255), -1);
            
            cout << "x : " << blueMaskCentroidPoints.x << " | y : " << blueMaskCentroidPoints.y << endl;

            //Affiche l'image selon la position :
            if (blueMaskCentroidPoints.x < 75) {
                currentImage = 0;
            }else if (blueMaskCentroidPoints.x >= 75 && blueMaskCentroidPoints.x < 150) {
                currentImage = 1;
            }
            else if (blueMaskCentroidPoints.x >= 150 && blueMaskCentroidPoints.x < 225) {
                currentImage = 2;
            }
            else if (blueMaskCentroidPoints.x >= 225 && blueMaskCentroidPoints.x < 295) {
                currentImage = 3;
            }
            else if (blueMaskCentroidPoints.x >= 295 && blueMaskCentroidPoints.x < 365) {
                currentImage = 4;
            }
            else if (blueMaskCentroidPoints.x >= 365 && blueMaskCentroidPoints.x < 435) {
                currentImage = 5;
            }
            resultatImageSelectionner = hQImagesArray[currentImage];

            namedWindow("result Image", WINDOW_NORMAL);
            resizeWindow("result Image", 960, 540);
            imshow("result Image", resultatImageSelectionner);

            namedWindow("Video Player - blue mask", WINDOW_NORMAL);
            resizeWindow("Video Player - blue mask", 540, 320);
            imshow("Video Player - blue mask", blueMask);

            namedWindow("Video Player - result", WINDOW_NORMAL);
            resizeWindow("Video Player - result", 960, 200);
            imshow("Video Player - result", concatedImages);


            char c = (char)waitKey(25);//Allowing 25 milliseconds frame processing time and initiating break condition//
            if (c == 27) { //If 'Esc' is entered break the loop//
                break;
            }
        }

        cap.release();//Releasing the buffer memory//
        return 0;
    }

    catch (const Exception& e) {
        const char* err_msg = e.what();
        cout << "error : " << err_msg << endl;
    }
}