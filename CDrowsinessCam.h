#include <stdlib.h>     /* abs */
#include <unistd.h>

#include "/usr/include/opencv2/opencv.hpp"
#include "/usr/include/opencv2/face.hpp"

#include <iostream>
#include <fstream>


using namespace std;
using namespace cv;
using namespace cv::face;



class CDrowsinessCam
{
private:
    int drowLevel;
    //bool calculateDrowLevel();

    void drawPolyline( Mat &frame, const vector<Point2f> &landmarks, const int start, const int end, bool isClosed = false);
    void renderFace(cv::Mat &frame, vector<Point2f> &landmarks);

    CascadeClassifier faceDetector;
    Ptr<Facemark> facemark;
    Mat gray;

    //para calculo do ear
    double p2_p6_final,  p3_p5_final,  p1_p4_final;

    int i;
    double classInput[30];
    ofstream file;

    int next_class;

    std::chrono::steady_clock::time_point begin;
    std::chrono::steady_clock::time_point end;
    

public:
    CDrowsinessCam();
    ~CDrowsinessCam();


    
    bool processParameter(Mat frame);  //Obtain
    int EARcalculation();  //calculate the value of EAR

    bool checkDrowState();  //machine learning algorithm









};