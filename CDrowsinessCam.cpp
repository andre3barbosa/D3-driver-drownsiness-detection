#include "CDrowsinessCam.h"


#include <stdlib.h>     /* abs */
#include <unistd.h>
#include <iostream>
#include <list>
#include <chrono> //time

using namespace std;
using namespace cv;
using namespace cv::face;
using namespace cv::ml;

CDrowsinessCam::CDrowsinessCam()
    :faceDetector("/usr/share/OpenCV/haarcascades/haarcascade_frontalface_alt2.xml"),
    gray(),
    p2_p6_final(0.0), 
    p3_p5_final(0.0),
    p1_p4_final(0.0)
{

    this->alertNumber = 0;


    next_class = NUM_NEXT_CLASS;


    for(i=0;i<MAX_FEAT;i++)
        classInput[i]=0.0;


    long_blinks = 0;
    total_blinks = 180;
    short_blinks = 180;
    consecutive_long_blinks = 0;
    cout << "begin" << endl;

}
CDrowsinessCam::~CDrowsinessCam()
{

}

void CDrowsinessCam::init(){

    // Load landmark detector
    facemark = FacemarkLBF::create();
    facemark->loadModel("/usr/share/OpenCV/lbfmodel.yaml");
    
    //Load the trained SVM model
    svm = SVM::load("/root/svm.xml");

}

bool CDrowsinessCam::processParameter(Mat frame)
{

        if (frame.empty())
          cerr  << ("[Camera] blank frame captured.\n");
        
        // Find face
        vector<Rect> faces;
        // Convert frame to grayscale because
        // faceDetector requires grayscale image.
        cvtColor(frame, gray, COLOR_BGR2GRAY);

        // Detect faces
        faceDetector.detectMultiScale(gray, faces);
        
        // Variable for landmarks. 
        // Landmarks for one face is a vector of points
        // There can be more than one face in the image. Hence, we 
        // use a vector of vector of points. 
        vector< vector<Point2f> > landmarks;
        //vector<Point2f> landmarks;
        // Run landmark detector
        bool success = facemark->fit(frame,faces,landmarks);
                
        if(success)
        //{   
            // If successful, render the landmarks on the face
            for(int i = 0; i < landmarks.size(); i++)
            {
                this->renderFace(frame, landmarks[i]);
            }
        //}  

}




float CDrowsinessCam::EARcalculation()
{     
    double EAR = (p2_p6_final +  p3_p5_final)/(2* p1_p4_final);  
    
    return (float)EAR; 
}

bool CDrowsinessCam::checkDrowState(float* input)
{
    Mat input_x = Mat(1, MAX_FEAT, CV_32F, input);
    int output_y = svm->predict(input_x);
    
    // takes true value for alarm, and false value doesn't alarm
    bool alert_var = 0;

    cout << output_y << endl;

    //rule verification
    // Update blink count
        if (output_y == 2) {
            long_blinks++;
            consecutive_long_blinks++;
        }else if(output_y == 1){
            short_blinks++;
            consecutive_long_blinks = 0;
        }else{
            consecutive_long_blinks = 0;
        }
        total_blinks++;

        // Add blink to the queue
        blink_queue.push_back(output_y);

        // Check if queue size is greater than 60
        if (blink_queue.size() > 180) {
            // Remove oldest blink from the queue
            int oldest_blink = blink_queue.front();
            blink_queue.pop_front();

            // Check if oldest blink was a long blink
            if (oldest_blink == 2) {
                long_blinks--;
                consecutive_long_blinks--;
            }else if(oldest_blink == 1){
                short_blinks--;
            }
            total_blinks--;
        }

        // Calculate proportion of long blinks
        double proportion = (double)long_blinks / total_blinks;

        // Check if proportion is higher than 25%
        //in the first 60 seconds don't made

        if (proportion > 0.25 ) {
            std::cout << "sleep" << std::endl;
            alert_var = true;
            alertNumber++;
        }else{
            //std::cout << "wake" << std::endl;
            alert_var = false;
        }

        if (consecutive_long_blinks >= 4) {
            std::cout << "ALERTA 5 or more consecutive long blinks in the last 60 values." << std::endl;
            alert_var = true;
            alertNumber++;
        }

    return alert_var;
}

void CDrowsinessCam::renderFace(cv::Mat &frame, vector<Point2f> &landmarks)
{
    double p2_p6_x = landmarks[37].x  - landmarks[41].x; 
    double p2_p6_y=  landmarks[37].y -landmarks[41].y ;

    this->p2_p6_final=sqrt((p2_p6_x * p2_p6_x) + (p2_p6_y * p2_p6_y));

    //p3-p5
    double p3_p5_x = landmarks[38].x - landmarks[40].x;
    double p3_p5_y = landmarks[38].y - landmarks[40].y;

    this->p3_p5_final=sqrt((p3_p5_x * p3_p5_x) + (p3_p5_y * p3_p5_y));


    //p1-p4
    double p1_p4_x = landmarks[36].x - landmarks[39].x;  
    double p1_p4_y = landmarks[36].y - landmarks[39].y;

    this->p1_p4_final=sqrt((p1_p4_x * p1_p4_x) + (p1_p4_y * p1_p4_y));

}
int CDrowsinessCam::getAlertNumber() const
{
    return this->alertNumber;
}
