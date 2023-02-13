#include <opencv2/ml.hpp>
#include <fstream>
#include <string>

using namespace std;
using namespace cv;
using namespace cv::ml;

int main(){


    //Create an instance of the SVM class
    Ptr<SVM> svm = SVM::create();

    //Read the CSV file into a matrix and a vector. 
    ifstream file("/root/test.csv");

    vector <vector<double>> data;
    vector <double> labels;
    string line;
    string label;   //classification
    string feature; //ear values

    while(getline(file,line,'#'))
    {
            getline(file,label,'\n');

            stringstream inputLine(line);
            vector <double> row;

            double value, label2;    

            while(getline(inputLine,feature,','))
            {
                value = stod(feature);
                row.push_back(value);                
            }           
            data.push_back(row);
            row.clear();
            label2=stod(label);
            labels.push_back(label2);
    }


    //Set the parameters for the SVM, such as the kernel type (e.g. linear, polynomial, RBF) and the values for any relevant kernel parameters
    svm->setType(SVM::C_SVC);
    svm->setKernel(SVM::RBF);
    svm->setGamma(0.4); //related to the localization and flexibility for the RBF kernel
    svm->setC(10); //the penalization factor for wrongly classified data


    //Convert the data and labels to the right format for OpenCV
    Mat trainingData(data.size(), 15, CV_32FC1);
    Mat trainingLabels(labels.size(), 1, CV_32SC1);
    
    for(int i=0; i<data.size(); i++) {
        for(int j=0; j<15; j++) {
            trainingData.at<float>(i,j) = data[i][j];
        }
        trainingLabels.at<int>(i,0) = labels[i];
    }

    //Train the SVM using the training data
    Ptr<TrainData> trainData = TrainData::create(trainingData, ROW_SAMPLE, trainingLabels);
    svm->trainAuto(trainData);


   //Save the trained SVM model to a file:
    svm->save("/root/svm.xml");

}
