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

using namespace cv;
using namespace std;
int lowR,lowG,lowB,highR,highG,highB;
Mat image;
Mat redImage;
int counter;

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
    for(int i=0 ;i < redImage.rows/2;i++){
        for(int j=0; j< redImage.cols;j++)
            if( redImage.at<uchar>(i,j) ==0 && start == -1){
                start = j;
            }
            else if (start != -1 && redImage.at<uchar>(i,j) == 255){
                cout<<"Lenght of target is"<<j-start<<" found at "<<i<<" "<<j<<endl;
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
            cout<<"Lenght of projectile is"<<j-start<<endl;
            return start+(j-start)/2;
        }
    return -1;

}

void setupThresholdGUI(){
    lowR=100;
    highR=255;
    
    createTrackbar("lowR", nullptr , &lowR, 255,getFeatures);
    createTrackbar("highR", nullptr , &highR, 255,getFeatures);
    createButton("Reset throws",resetThrowsCallback,nullptr,CV_PUSH_BUTTON);

}

void fire(int startLocation,int endLocation){
    string command = "/usr/local/adb shell input swipe "+ to_string(startLocation) + " 1557 "+ to_string(endLocation) +" 600 ";
    system(command.data());
}

int main(int argc, const char * argv[]) {
//    system("/usr/local/adb shell screencap -p | perl -pe 's/\\x0D\\x0A/\\x0A/g' > /Users/stbogdan/dev/FacebookBasketBaller/screen.png ");
//    waitKey();
    fire(0,0);
    
    
    namedWindow("Phone feed",CV_GUI_EXPANDED);
    auto start = chrono::steady_clock::now();
    
    image = imread("/Users/stbogdan/dev/FacebookBasketBaller/screen.png", CV_LOAD_IMAGE_COLOR);
    
    auto end = chrono::steady_clock::now();
    auto diff = end - start;
    cout<< chrono::duration <double, milli> (diff).count()<<"\tto get screenshot" <<endl;


    setupThresholdGUI();
    
    counter =0;
    while(true){
        //Get frame
        start = chrono::steady_clock::now();
        system("/usr/local/adb shell screencap -p /sdcard/screen.png");
        system("/usr/local/adb pull /sdcard/screen.png /Users/stbogdan/dev/FacebookBasketBaller/screen.png");
        image = imread("/Users/stbogdan/dev/FacebookBasketBaller/screen.png", CV_LOAD_IMAGE_COLOR);
        
        end = chrono::steady_clock::now();
        diff = end - start;
        start = chrono::steady_clock::now();
        cout<< chrono::duration <double, milli> (diff).count()<<"\tto get frame" <<endl;

        
        //Threshold
        getFeatures(0,nullptr);

        //Determine ball and target
        int basket      =getTargetLocation(redImage);
        int ball        =getProjectileLocation(redImage);
        
        end = chrono::steady_clock::now();
        diff = end - start;
        cout<< chrono::duration <double, milli> (diff).count()<<"\tto get features and points" <<endl;
        
        //Fire if target found
        if(counter==10) {
            waitKey(6650);
            cout<<endl<<"Waiting for target";
        }
        else if(counter> 10 && counter<=20){
            waitKey(4880);
        }
        else if(counter>20 && counter<=30){
            waitKey(3600);
        }
        else if(counter>30){
            return 0;
        }
        if(ball!= -1 && basket != -1){
            fire(ball,basket);
            counter++;
            cout<<endl<<"Fired "<<counter<<endl<<endl;
        }
        else{
            cout<<endl<<"FIRE ABORDED TARGET NOT FOUND "<<counter<<endl<<endl;
        }
        
        //Wait for ball to respawn
         waitKey(1500);
    }
    
    return 0;
}
