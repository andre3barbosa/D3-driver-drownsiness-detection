#include <stdlib.h>     /* abs */
#include <unistd.h>

#include "/usr/include/opencv2/opencv.hpp"
#include "/usr/include/opencv2/face.hpp"
#include "/usr/include/opencv2/ml.hpp"

#include <iostream>
#include <fstream>

#include <deque>


#define MAX_FEAT 15
#define NUM_NEXT_CLASS 4

using namespace std;
using namespace cv;
using namespace cv::face;
using namespace cv::ml;



class CDrowsinessCam
{
private:

    /*Image processing functions*/
    void drawPolyline( Mat &frame, const vector<Point2f> &landmarks, const int start, const int end, bool isClosed = false);
    void renderFace(cv::Mat &frame, vector<Point2f> &landmarks);

    /*Image processing variables*/
    CascadeClassifier faceDetector;
    Ptr<Facemark> facemark;
    Mat gray;
    Ptr<SVM> svm;

    //Rule variables
    int long_blinks;
    int total_blinks;
    int short_blinks;
    int consecutive_long_blinks;
    std::deque<int> blink_queue;


    //for EAR determination
    double p2_p6_final,  p3_p5_final,  p1_p4_final;

    //input variable of the SVM model
    float classInput[MAX_FEAT];

    //number of alerts
    int alertNumber;
    

public:
    CDrowsinessCam();
    ~CDrowsinessCam();

    void init();

    
    bool processParameter(Mat frame);   //obtain the parameters for EAR calculation
    float EARcalculation();             //calculate the value of EAR

    bool checkDrowState(float *);       //machine learning algorithm
    int getAlertNumber() const;         //get alert number

};