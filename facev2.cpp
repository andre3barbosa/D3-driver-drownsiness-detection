#include "/usr/include/opencv2/opencv.hpp"
#include "/usr/include/opencv2/face.hpp"


using namespace std;
using namespace cv;
using namespace cv::face;
#include <stdlib.h>     /* abs */
#include <unistd.h>
#include<iostream>

#define IMAGE_PATH  ("/etc/frame.jpg")

#define FRAME_W 		640
#define FRAME_H 		480

#define CAM_BRIGHTNESS	0.4  //-> 20
#define CAM_CONTRAST	0.50 //-> 0
#define CAM_SATURATION	0.6  //-> 10

//#define COLOR Scalar(255, 0,0)
#define deviceID 0

// drawPolyLine draws a poly line by joining 
// successive points between the start and end indices. 
void drawPolyline( Mat &image, const vector<Point2f> &landmarks,
  const int start,
  const int end,
  bool isClosed = false
)
{
    // Gather all points between the start and end indices
    vector <Point> points;

    for (int i = start; i <= end; i++){
        points.push_back(cv::Point(landmarks[i].x, landmarks[i].y));
    }
    // Draw polylines. 
    //polylines(image, points, isClosed, COLOR, 3, 16);


  

     //EAR (eye aspect ratio)

        //p2-p6
        double p2_p6_x = landmarks[37].x  - landmarks[41].x; 
        double p2_p6_y=  landmarks[37].y -landmarks[41].y ;

        double p2_p6_final=sqrt((p2_p6_x * p2_p6_x) + (p2_p6_y * p2_p6_y));

        //p3-p5
        double p3_p5_x = landmarks[38].x - landmarks[40].x;
        double p3_p5_y = landmarks[38].y - landmarks[40].y;

        double p3_p5_final=sqrt((p3_p5_x * p3_p5_x) + (p3_p5_y * p3_p5_y));


        //p1-p4
        double p1_p4_x = landmarks[36].x - landmarks[39].x;  
        double p1_p4_y = landmarks[36].y - landmarks[39].y;

        double p1_p4_final=sqrt((p1_p4_x * p1_p4_x) + (p1_p4_y * p1_p4_y));



        double EAR= (p2_p6_final +  p3_p5_final)/(2* p1_p4_final);

        // cout << EAR << endl;

        double i = 0.25; //0.17

        if (EAR > i )
            cout << "OPEN" <<  endl;
        else 
            cout << "CLOSE" << endl;


        cout << "EAR value =  " << EAR << endl;
    
}


void renderFace(cv::Mat &image, vector<Point2f> &landmarks)
{
    // // Draw face for the 68-point model.
    // if (landmarks.size() == 68)
    // {
    //   drawPolyline(image, landmarks, 0, 16);           // Jaw line
    //   drawPolyline(image, landmarks, 17, 21);          // Left eyebrow
    //   drawPolyline(image, landmarks, 22, 26);          // Right eyebrow
    //   drawPolyline(image, landmarks, 27, 30);          // Nose bridge
    //   drawPolyline(image, landmarks, 30, 35, true);    // Lower nose
    //   drawPolyline(image, landmarks, 36, 41, true);    // Left eye
    //   drawPolyline(image, landmarks, 42, 47, true);    // Right Eye
    //   drawPolyline(image, landmarks, 48, 59, true);    // Outer lip
    //   drawPolyline(image, landmarks, 60, 67, true);    // Inner lip
    // }
    // else 
    // { // If the number of points is not 68, we do not know which 
    //   // points correspond to which facial features. So, we draw 
    //   // one dot per landamrk. 
    //   for(int i = 0; i < landmarks.size(); i++)
    //   {
    //     circle(image,landmarks[i],3, COLOR, FILLED);
    //   }
    // }

    //right eye

    drawPolyline(image, landmarks, 36, 36, true); //37  p1
    drawPolyline(image, landmarks, 37, 37, true); //38  p2
    drawPolyline(image, landmarks, 38, 38, true); //39  p3
    drawPolyline(image, landmarks, 39, 39, true); //40  p4
    drawPolyline(image, landmarks, 40, 40, true); //41  p5
    drawPolyline(image, landmarks, 41, 41, true); //42  p6

//left eye
    drawPolyline(image, landmarks, 42, 42, true); //43  
    drawPolyline(image, landmarks, 43, 43, true); //44
    drawPolyline(image, landmarks, 44, 44, true); //45
    drawPolyline(image, landmarks, 45, 45, true); //46
    drawPolyline(image, landmarks, 46, 46, true); //47
    drawPolyline(image, landmarks, 47, 47, true); //48 
}




int main(int argc,char** argv)
{
    // Load Face Detector
    CascadeClassifier faceDetector("/usr/share/OpenCV/haarcascades/haarcascade_frontalface_alt2.xml");

    // Create an instance of Facemark
    Ptr<Facemark> facemark = FacemarkLBF::create();

    // Load landmark detector
    // facemark->loadModel("/home/andre/Desktop/lbfmodel.yaml");
    facemark->loadModel("/usr/share/OpenCV/lbfmodel.yaml");

    // Set up webcam for video capture
    //VideoCapture cam(0);

    cv::VideoCapture cam;
    cam.open(deviceID, cv::CAP_V4L2);


     /******** Camera Configurations **********/    
	  cam.set(CAP_PROP_FRAME_WIDTH , FRAME_W);
    cam.set(CAP_PROP_FRAME_HEIGHT, FRAME_H);
    cam.set(CAP_PROP_BRIGHTNESS, CAM_BRIGHTNESS);
    cam.set(CAP_PROP_CONTRAST, CAM_CONTRAST);
    cam.set(CAP_PROP_SATURATION, CAM_SATURATION);

    system("v4l2-ctl --set-ctrl=auto_exposure=0");
    system("v4l2-ctl --set-ctrl=iso_sensitivity=4");
    system("v4l2-ctl --set-ctrl=auto_exposure_bias=18");
    system("v4l2-ctl --set-ctrl=exposure_time_absolute=1000");
    system("v4l2-ctl --set-ctrl=exposure_dynamic_framerate=1");
    system("v4l2-ctl --set-ctrl=white_balance_auto_preset=7");

    system("v4l2-ctl --set-ctrl=sharpness=30");

    // Variable to store a video frame and its grayscale 
    Mat frame, gray;
    
    // Read a frame
    while(cam.read(frame))
//    while(1)
    {
      
      if (frame.empty())
          printf("[Camera] blank frame captured.\n");

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
      
      // Run landmark detector
      bool success = facemark->fit(frame,faces,landmarks);
      
      if(success)
      {
        // If successful, render the landmarks on the face
        for(int i = 0; i < landmarks.size(); i++)
        {
          renderFace(frame, landmarks[i]);
        }
      }


    // write image to path
      imwrite(IMAGE_PATH, frame);
      // Display results 
      //imshow("Facial Landmark Detection", frame);
      // Exit loop if ESC is pressed
      //if (waitKey(1) == 27) break;

      cam.release();
      
    }
    return 0;
}

