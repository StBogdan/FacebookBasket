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

using namespace cv;
using namespace std;

int lowR,highR;
Mat image;
Mat redImage;
int counter;

int delay10s = 4880;
int delay20s = 1220;
int delay30s = 4900;
int delay40s = 4900;

int delayFrame =1777;
string adbPath = "/usr/local/adb";

void getFeatures(int,void*) {
    Mat bgr[3];         //destination array
    split(image,bgr);   //split source

    namedWindow("Thresholded",CV_GUI_EXPANDED);
    
    threshold(bgr[0], bgr[0], lowR, highR, CV_THRESH_BINARY);
    redImage =bgr[0](Rect(0,225,bgr[0].cols,1776-225));
    
    imshow("Thresholded",redImage);
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
    //Default thresholding values
    lowR=100;
    highR=255;

    //Create trackbars
    createTrackbar("lowR", nullptr , &lowR, 255,getFeatures);
    createTrackbar("highR", nullptr , &highR, 255,getFeatures);
    createTrackbar("Counter", nullptr , &counter, 55,getFeatures);
    createTrackbar("20's delay", nullptr , &delay20s, 10000,getFeatures);
    createTrackbar("20's delay", nullptr , &delay20s, 10000,getFeatures);
    createTrackbar("30's delay", nullptr , &delay30s, 10000,getFeatures);
    createTrackbar("40's delay", nullptr , &delay40s, 10000,getFeatures);
    
    createTrackbar("Interframe  delay", nullptr , &delayFrame, 5000,getFeatures);
    createButton("Reset throws",resetThrowsCallback,nullptr,CV_PUSH_BUTTON);

}

void fire(int startLocation,int endLocation){
    string command = adbPath +" shell input swipe "+ std::to_string(startLocation) + " 1557 "+ to_string(endLocation) +" 600 ";
    system(command.data());
}

int main(int argc, const char * argv[]) {
    if(argv[1] != nullptr){
        adbPath = argv[1];
    }
    namedWindow("Thresholded",CV_GUI_EXPANDED);
    setupThresholdGUI();
    counter =0;
    waitKey();
    string displayString ="";
    string commandString ="";

    //Start firing loop
    while(true){
        //Get frame
        auto start = chrono::steady_clock::now();
        
        commandString = adbPath + " shell screencap -p /sdcard/screen.png";
        system(commandString.data());
        
        commandString = adbPath + " pull /sdcard/screen.png screenShot.png";
        system(commandString.data());
        
        image = imread("screenShot.png", CV_LOAD_IMAGE_COLOR);

        auto end = chrono::steady_clock::now();
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
        if(counter>= 10 && counter<20){
            waitKey(delay10s);
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
                    displayStatusBar("Thresholded", displayString) ;
                    basket =-1; //This prevents the swipe from occuring
                }
            else
                displayStatusBar("Thresholded", "Ready to fire, waiting for delay");
            waitKey(delay30s);
            cout<<"Used 30's delay"<<delay30s<<endl;

        }
        else if(counter>40){
            if(abs(basket-ball) > 100)
            {
                displayString = "Target out of range (" + to_string(abs(basket-ball)) +"), aborting fire";
                displayStatusBar("Thresholded", displayString) ;
                basket =-1; //This prevents the swipe from occuring
            }
            else
                displayStatusBar("Thresholded", "Ready to fire, waiting for delay");
            waitKey(delay40s);
            cout<<"Used 40's delay "<<delay40s<<endl;

        }
        if(ball!= -1 && basket != -1){
            fire(ball,basket);
            counter++;
            cout<<"Fired for "<<counter<<endl;

        }
        else{
            cout<<endl<<"Target not found, retrying for "<<counter<<endl;
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
