//
//  main.cpp
//  FacebookBasketBaller
//
//  Created by Bogdan Stoicescu on 10/08/2016.
//  Copyright © 2016 Bogdan Stoicescu. All rights reserved.
//

#include <iostream>
#include <opencv2/opencv.hpp>
#include "opencv2/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"
#include <chrono>
#include <thread>
#include <string>

using namespace cv;
using namespace std;
int lowR,lowG,lowB,highR,highG,highB;
Mat image;
Mat redImage;
int counter;
int delay20s = 1220;
int delay30s = 4900;
int delay40s = 4900;
int delayFrame =1777;

void getFeatures(int,void*) {
    Mat bgr[3];   //destination array
    split(image,bgr);//split source

    namedWindow("R-red",CV_GUI_EXPANDED);
    threshold(bgr[0], bgr[0], lowR, highR, CV_THRESH_BINARY);
    redImage =bgr[0](Rect(0,225,bgr[0].cols,1776-225));
    imshow("R-red",redImage);
}

void resetThrowsCallback(int state,void*){
    counter =0;
}
int getTargetLocation(Mat redImage){
    int start = -1;
    for(int i=340 ;i < redImage.rows/2;i++){
        for(int j=0; j< redImage.cols;j++)
            if( redImage.at<uchar>(i,j) ==0 && start == -1){
                start = j;
            }
            else if (start != -1 && redImage.at<uchar>(i,j) == 255){
                cout<<"Lenght of target is \t"<<j-start<<" found at \t"<<i<<" "<<j<<"\t Middle Target:"<<start+(j-start)/2<<endl;
                return start+(j-start)/2;
            }
    }
    return -1;
}

int getProjectileLocation(Mat redImage){
    int start=-1;
    for(int j=0; j< redImage.cols;j++)
        if(redImage.at<uchar>(1370,j) == 0 && start == -1)
            start = j;
        else if (redImage.at<uchar>(1370,j) == 255 && start != -1){
            cout<<"Lenght of project'l is \t"<<j-start<<" found at \t"<<1370<<" "<<j<<"\t Middle Ball:"<<start+(j-start)/2<<endl;
            return start+(j-start)/2;
        }
    return -1;

}

void setupThresholdGUI(){
    lowR=100;
    highR=255;

    createTrackbar("lowR", nullptr , &lowR, 255,getFeatures);
    createTrackbar("highR", nullptr , &highR, 255,getFeatures);
    createTrackbar("Counter", nullptr , &counter, 55,getFeatures);
    createTrackbar("20's delay", nullptr , &delay20s, 5000,getFeatures);
    createTrackbar("30's delay", nullptr , &delay30s, 25000,getFeatures);
    createTrackbar("40's delay", nullptr , &delay40s, 25000,getFeatures);

    createTrackbar("Interframe  delay", nullptr , &delayFrame, 5000,getFeatures);
    createButton("Reset throws",resetThrowsCallback,nullptr,CV_PUSH_BUTTON);

}

void fire(int startLocation,int endLocation){
    string command = "/usr/local/adb shell input swipe "+ std::to_string(startLocation) + " 1557 "+ to_string(endLocation) +" 600 ";
    system(command.data());
}

int main(int argc, const char * argv[]) {
//    system("/usr/local/adb shell screencap -p | perl -pe 's/\\x0D\\x0A/\\x0A/g' > /Users/stbogdan/dev/FacebookBasketBaller/screen.png ");
//    waitKey();
    fire(0,0);


    namedWindow("R-red",CV_GUI_EXPANDED);
    auto start = chrono::steady_clock::now();

    image = imread("/Users/stbogdan/dev/FacebookBasketBaller/screen.png", CV_LOAD_IMAGE_COLOR);

    auto end = chrono::steady_clock::now();
    auto diff = end - start;
    cout<< chrono::duration <double, milli> (diff).count()<<"\tto get screenshot" <<endl;


    setupThresholdGUI();

    counter =0;
    waitKey();
    string displayString = "";
    //Start firing loop
    while(true){
        //Get frame
        start = chrono::steady_clock::now();
        system("/usr/local/adb shell screencap -p /sdcard/screen.png");
        system("/usr/local/adb pull /sdcard/screen.png /Users/stbogdan/dev/FacebookBasketBaller/screen.png");
        image = imread("/Users/stbogdan/dev/FacebookBasketBaller/screen.png", CV_LOAD_IMAGE_COLOR);

        end = chrono::steady_clock::now();
        double diff =chrono::duration <double, milli> (end - start).count();
        cout<< (int) diff <<"\tto get frame" <<endl;

        //Threshold
        getFeatures(0,nullptr);

        //Determine ball and target
        int basket      =getTargetLocation(redImage);
        int ball        =getProjectileLocation(redImage);

        end = chrono::steady_clock::now();
        diff =chrono::duration <double, milli> (end - start).count();
        cout<< (int) diff <<"\tto get screenshow, features and points" <<endl;

        //Handle time variation by waiting a bit
        waitKey(1200-(int) diff);

        end = chrono::steady_clock::now();
        diff =chrono::duration <double, milli> (end - start).count();
        cout<< (int) diff <<"\tto prepare for fire" <<endl;
        //Fire if target found
        if(counter==10) {
            waitKey(4880);
            cout<<"Waiting for target"<<endl;
        }
        else if(counter> 10 && counter<20){
            waitKey(4880);
            cout<<"Used 10's delay"<<endl;

        }
        else if(counter>=20 && counter<30){
            waitKey(delay20s);
            cout<<"Used 20's delay"<<endl;

        }
        else if(counter>=30 && counter<=40){
            if(abs(basket-ball) > 100)
                {
                    displayString = "Target out of range (" + to_string(abs(basket-ball)) +"), aborting fire";
                    displayStatusBar("R-red", displayString) ;
                    basket =-1;
                }
            else
                displayStatusBar("R-red", "PRIMED FOR FIRE");
            waitKey(delay30s);
            cout<<"Used 30's delay"<<delay30s<<endl;

        }
        else if(counter>40){
            if(abs(basket-ball) > 100)
            {
                displayString = "Target out of range (" + to_string(abs(basket-ball)) +"), aborting fire";
                displayStatusBar("R-red", displayString) ;
                basket =-1;
            }
            else
                displayStatusBar("R-red", "PRIMED FOR FIRE");

            waitKey(delay40s);
            cout<<"Used 40's delay "<<delay40s<<endl;

        }
        if(ball!= -1 && basket != -1){
            fire(ball,basket);
            counter++;
            cout<<"Fired "<<counter<<endl;

        }
        else{
            cout<<endl<<"FIRE ABORDED TARGET NOT FOUND "<<counter<<endl<<endl;
            continue;
        }

        end = chrono::steady_clock::now();
        diff =chrono::duration <double, milli> (end - start).count();
        cout<< (int) diff <<"\tto fire" <<endl<<endl;

        //Wait for ball to respawn
         waitKey(delayFrame);
    }

    return 0;
}
