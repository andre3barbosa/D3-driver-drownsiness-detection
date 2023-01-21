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
   // Load landmark detector
    facemark = FacemarkLBF::create();
    facemark->loadModel("/usr/share/OpenCV/lbfmodel.yaml");
    //svm = SVM::load("/root/svm.xml");
    //float input = {0.0};
    //Mat input_x = Mat(1, 30, CV_32F, input);
    //svm->predict(input_x);
    //this-> begin = std::chrono::steady_clock::now();
    // for(i=0;i<30;i++)
    //     classInput[i]=0.0;

    if(!file){
        file.open("ear.csv",ios_base::out);
        file << "\n";
        file.close();
    }

    next_class = NUM_NEXT_CLASS;
    // cout << "begin" << endl;
    // this-> begin = std::chrono::steady_clock::now();
    

    for(i=0;i<MAX_FEAT;i++)
        classInput[i]=0.0;

    cout << "begin" << endl;
    this-> begin = std::chrono::steady_clock::now();
    //file.open("ear.txt",ios_base::app);

}
CDrowsinessCam::~CDrowsinessCam()
{

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
         
    double EAR= (p2_p6_final +  p3_p5_final)/(2* p1_p4_final);
    
    cout<<EAR<<endl;
    
        for(int i = (MAX_FEAT - 1); i > 0; i--)
            classInput[i] = classInput[i-1];
        classInput[0] = EAR;    
        //     // file.open("ear.csv",ios_base::out | ios_base::app);
        
        
        if((next_class--)==0){
                this-> end = std::chrono::steady_clock::now();

                file.open("ear.csv",ios_base::app);
                
                file << std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count() << ", ";


                for (int i = 0; i < MAX_FEAT; i++) {
                //cout << classInput[i];
                    
                    file << classInput[i] << " , ";
                    //cout << classInput[i] << " , ";
                }
                //cout << "\n";
                file << "\n";
                file.close();

                next_class = NUM_NEXT_CLASS;
        }
            
        // file << "\n";
        // file.close();  
        

   

                /*for (int i = 0; i < 30; i++) {
                //cout << classInput[i];
                    
                     cout << classInput[i] << " , ";
                    Mat input_x = (Mat_<float>(1,30) << classInput[i]);
                    output_y = svm->predict(input_x);

                }*/

        //cout << EAR << endl;
        
        //file << EAR << endl;        
        /*this-> end = std::chrono::steady_clock::now();
        file << "[Time] Value: "<<std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count() << endl;
        this-> begin = std::chrono::steady_clock::now();
        file << EAR << endl;
        return (float)EAR;*/
    
}

int CDrowsinessCam::checkDrowState(float* input)
{
    //this-> begin = std::chrono::steady_clock::now();
    Mat input_x = Mat(1, MAX_FEAT, CV_32F, input);
    file << input_x << endl;
    int output_y = svm->predict(input_x);
    //this-> end = std::chrono::steady_clock::now();

    //file << "[Time] Value: "<< std::chrono::duration_cast<std::chrono::microseconds>(end - begin).count() <<"[Output] Value: " << output_y <<endl;

    //cout << output_y << endl;

    //file << "[checkDrowState] Value: " << output_y << endl;


    return output_y;

}



// drawPolyLine draws a poly line by joining 
// successive points between the start and end indices. 
void CDrowsinessCam::drawPolyline( Mat &frame, const vector<Point2f> &landmarks,
  const int start,
  const int end,
  bool isClosed
)
{ 
    // Gather all points between the start and end indices
    // vector <Point> points;

    // for (int i = start; i <= end; i++){
    //     points.push_back(cv::Point(landmarks[i].x, landmarks[i].y));
    // }
    // Draw polylines. 
    //polylines(image, points, isClosed, COLOR, 3, 16);    

    //Face landmarks

    //EAR (eye aspect ratio)

        //p2-p6
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

        //this->EARcalculation(); 
        //double EAR= (p2_p6_final +  p3_p5_final)/(2* p1_p4_final);
        //file << EAR << endl; 
}


void CDrowsinessCam::renderFace(cv::Mat &frame, vector<Point2f> &landmarks)
{
    
// //right eye
//     drawPolyline(frame, landmarks, 36, 36, true); //37  p1
//     drawPolyline(frame, landmarks, 37, 37, true); //38  p2
//     drawPolyline(frame, landmarks, 38, 38, true); //39  p3
//     drawPolyline(frame, landmarks, 39, 39, true); //40  p4
//     drawPolyline(frame, landmarks, 40, 40, true); //41  p5
//     drawPolyline(frame, landmarks, 41, 41, true); //42  p6

// //left eye
//     drawPolyline(frame, landmarks, 42, 42, true); //43  
//     drawPolyline(frame, landmarks, 43, 43, true); //44
//     drawPolyline(frame, landmarks, 44, 44, true); //45
//     drawPolyline(frame, landmarks, 45, 45, true); //46
//     drawPolyline(frame, landmarks, 46, 46, true); //47
//     drawPolyline(frame, landmarks, 47, 47, true); //48 

//right eye
    //drawPolyline(frame, landmarks, 36, 41, true); //37  p1


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

