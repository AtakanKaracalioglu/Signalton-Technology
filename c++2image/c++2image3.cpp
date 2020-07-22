/* Motion and face detection using two consecutive images*/

#include <iostream>
#include <sstream>

#include "opencv2/core.hpp"
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/videoio.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/video.hpp>
#include <opencv2/objdetect.hpp>
#include <vector>
#include <stdio.h>
#include <stdlib.h>
#include "opencv2/opencv.hpp"

#include "mysql_connection.h"
#include <cppconn/driver.h>
#include <cppconn/exception.h>
#include <cppconn/resultset.h>
#include <cppconn/statement.h>

using namespace std;
using namespace cv;

int detectAndDisplay(Mat frame);
bool searchForMovement(Mat thresholdImage, Mat& cameraFeed, int pixcount, int& x, int& y);
string intToString(int number);

CascadeClassifier face_cascade;
CascadeClassifier eyes_cascade;

//default capture width and height
const int FRAME_WIDTH = 640;
const int FRAME_HEIGHT = 480;
//max number of objects to be detected in frame
const int MAX_NUM_OBJECTS = 20;
//minimum and maximum object area
const int MIN_OBJECT_AREA = 20 * 20;
const int MAX_OBJECT_AREA = FRAME_HEIGHT * FRAME_WIDTH / 1.5;
//our sensitivity value to be used in the absdiff() function
const static int SENSITIVITY_VALUE = 140;
//size of blur used to smooth the intensity image output from absdiff() function
const static int BLUR_SIZE = 4;
//we'll have just one object to search for
//and keep track of its position.
int theObject[2] = { 0,0 };
//bounding rectangle of the object, we will use the center of this as its position.
Rect objectBoundingRectangle = Rect(0, 0, 0, 0);


int main(int argc, const char** argv) {
	CommandLineParser parser(argc, argv,

			//#include "main.h"
			"{face_cascade|/usr/local/share/opencv4/haarcascades/haarcascade_frontalface_alt.xml|Path to face cascade.}"
			"{eyes_cascade|/usr/local/share/opencv4/haarcascades/haarcascade_eye_tree_eyeglasses.xml|Path to eyes cascade.}"
			"{camera|0|Camera device number.}");
	parser.about("\nThis program demonstrates using the cv::CascadeClassifier class to detect objects (Face + eyes) in a video stream.\n"
			"You can use Haar or LBP features.\n\n");
	//parser.printMessage();
	cout << "Start Visual Detection..." << endl;

	String face_cascade_name = samples::findFile(parser.get<String>("face_cascade"));
	String eyes_cascade_name = samples::findFile(parser.get<String>("eyes_cascade"));

	//-- 1. Load the cascades
	if (!face_cascade.load(face_cascade_name))
	{
		cout << "--(!)Error loading face cascade\n";
		return -1;
	};
	if (!eyes_cascade.load(eyes_cascade_name))
	{
		cout << "--(!)Error loading eyes cascade\n";
		return -1;
	};

	//some boolean variables for added functionality
	bool objectDetected = true;
	int x, y = 0;
	//Set true for background substraction
	bool debugMode = false;
	bool trackingEnabled = true;
	int pixcount = 0;

	//Reading database
	const char *DIR = "./images/";
	char *in_fname;


	Mat frame1, frame2;

	Mat differenceImage;
	Mat thresholdImage;

	int last_id;
	int face_det;
	bool activity_det;

	std::ostringstream stringStream;
	std::string query_str;

	sql::Driver *driver;
	sql::Connection *con;
	sql::Statement *stmt;
	sql::ResultSet *res;

	///////////////////////
	try{
		/* Create a connection */
		driver = get_driver_instance();
		con = driver->connect("tcp://127.0.0.1:3306", "root", "ghost");
		/* Connect to the MySQL test database */
		con->setSchema("mydb");
		stmt = con->createStatement();
	} catch (sql::SQLException &e) {
		cout << "# ERR: SQLException in " << __FILE__;
		//cout << "(" << EXAMPLE_FUNCTION << ") on line " << __LINE__ << endl;
		cout << "# ERR: " << e.what();
		cout << " (MySQL error code: " << e.getErrorCode();
		cout << ", SQLState: " << e.getSQLState() << " )" << endl;
	}
	last_id = -1;
	while(true){
		try {
//			if (last_id < 0){
//				res = stmt->executeQuery("SELECT * from mydb.jpg_test;");
//			}
			stringStream.str("");
			stringStream << "SELECT * from mydb.jpg_test where id > " << last_id << ";";
			res = stmt->executeQuery(stringStream.str());

		} catch (sql::SQLException &e) {
			cout << "# ERR: SQLException in " << __FILE__;
			//cout << "(" << EXAMPLE_FUNCTION << ") on line " << __LINE__ << endl;
			cout << "# ERR: " << e.what();
			cout << " (MySQL error code: " << e.getErrorCode();
			cout << ", SQLState: " << e.getSQLState() << " )" << endl;
		}

		while(res -> next()){
			frame1.deallocate();
			thresholdImage.deallocate();
			differenceImage.deallocate();
			//cout << "Processing new image..." << endl;
			//cout << "First? " << res->isFirst() << endl;
			if(res->isFirst() && last_id < 0){
				in_fname = new char[res->getString("name").length() + strlen(DIR) + 1];
				strcpy(in_fname, DIR);
				strcat(in_fname, res->getString("name").c_str());
				in_fname[res->getString("name").length() + strlen(DIR)] = '\0';
				//cout << "   " << in_fname << endl;

				frame2 = imread(in_fname, IMREAD_GRAYSCALE);
				face_det = detectAndDisplay(frame2);
				stringStream.str("");
				stringStream << "UPDATE mydb.jpg_test set face_detected=" << face_det << ", activity_detected=0, xpos=0, ypos=0 where id=" << res->getString("id") << ";";
				query_str = stringStream.str();
				cout << "In image file " << in_fname << " -> Faces detected: " << face_det << ", Activity detected: false" << endl;
				try
				{
					stmt->executeQuery(query_str);
				}
				catch(sql::SQLException &e)
				{
					2 == 2; // burayı değiştir
				}
				last_id = std::stoi(res->getString("id"));
				delete in_fname;
				continue;
			}

			//read frames
			frame2.copyTo(frame1);
			frame2.deallocate();


			in_fname = new char[res->getString("name").length() + strlen(DIR) + 1];
			strcpy(in_fname, DIR);
			strcat(in_fname, res->getString("name").c_str());
			in_fname[res->getString("name").length() + strlen(DIR)] = '\0';
			//cout << "   " << in_fname << endl;

			frame2 = imread(in_fname, IMREAD_GRAYSCALE);


			//perform frame differencing with the sequential images. This will output an "intensity image"
			cv::absdiff(frame1, frame2, differenceImage);

			//threshold intensity image at a given sensitivity value
			cv::threshold(differenceImage, thresholdImage, SENSITIVITY_VALUE, 255, THRESH_BINARY);
			for (int y = 0; y < thresholdImage.cols; y++)
			{
				for (int x = 0; x <thresholdImage.rows;x++ )
				{
					if (thresholdImage.at<uchar>(x, y) == 255) {
						pixcount++;
					}
				}
			}
			if (debugMode == true) {
				//show the difference image and threshold image
				cv::imshow("Difference Image", differenceImage);
				cv::imshow("Threshold Image", thresholdImage);
				waitKey(0);
			}

			//blur the image to get rid of the noise. This will output an intensity image
			cv::blur(thresholdImage, thresholdImage, cv::Size(BLUR_SIZE, BLUR_SIZE));
			//threshold again to obtain binary image from blur output
			cv::threshold(thresholdImage, thresholdImage, SENSITIVITY_VALUE, 255, THRESH_BINARY);

			if (debugMode == true)
			{
				//show the threshold image after it's been "blurred"
				imshow("Final Threshold Image", thresholdImage);
			}

			//if tracking enabled, search for contours in our thresholded image
			if (trackingEnabled) {
				int xpos = 0;
				int ypos = 0;
				face_det = detectAndDisplay(frame2);
				activity_det = searchForMovement(thresholdImage, frame2, pixcount, xpos, ypos);

				stringStream.str("");
				stringStream << "UPDATE mydb.jpg_test set face_detected=" << face_det << ", activity_detected =" << int(activity_det) << \
						", xpos=" << xpos << ", ypos=" << ypos << " where id=" << res->getString("id") << ";";
				query_str = stringStream.str();
				cout << "In image file " << in_fname << " -> Faces detected: " << face_det << ", Activity detected: " << (activity_det > 0 ? "true" : "false") << endl;

				try
				{
					stmt->executeQuery(query_str);
				}
				catch(sql::SQLException &e)
				{
					2 == 2; // burayı değiştir
				}
			}
			last_id = std::stoi(res->getString("id"));
			delete in_fname;
		}
		delete res;
	}
	//delete res;
	delete stmt;
	delete con;

	return 0;
}


string intToString(int number)
{
	//this function has a number input and string output
	std::stringstream ss;
	ss << number;
	return ss.str();
}

bool searchForMovement(Mat thresholdImage, Mat& cameraFeed_in, int pixcount, int& x, int& y) {
	bool objectDetected = false;
	Mat temp;
	Mat cameraFeed;
	cameraFeed_in.copyTo(cameraFeed);
	thresholdImage.copyTo(temp);
	//these two vectors needed for output of findContours
	vector< vector<Point> > contours;
	vector<Vec4i> hierarchy;
	int largest_area = 0;
	int largest_contour_index = 0;
	Rect objectBoundingRectangle;
	//find contours of filtered image using openCV findContours function
	findContours(temp, contours, hierarchy, RETR_CCOMP, CHAIN_APPROX_SIMPLE);// retrieves all contours
	//findContours(temp, contours, hierarchy, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);// retrieves external contours
	for (size_t i = 0; i < contours.size(); i++) // iterate through each contour.
		{
			double area = contourArea(contours[i]);  //  Find the area of contour

			if (area > largest_area)
			{
				largest_area = area;
				largest_contour_index = i;               //Store the index of largest contour
				objectBoundingRectangle = boundingRect(contours[i]); // Find the bounding rectangle for biggest contour
			}
		}


	//if contours vector is not empty, we have found some objects
	if (contours.size() > 0 && pixcount > 0)objectDetected = true;
	else objectDetected = false;

	if (objectDetected) {
		//we will simply assume that the biggest contour is the object we are looking for.
		//make a bounding rectangle around the largest contour then find its centroid
		//this will be the object's final estimated position.
		int xpos = objectBoundingRectangle.x + objectBoundingRectangle.width / 2;
		int ypos = objectBoundingRectangle.y + objectBoundingRectangle.height / 2;

		//update the objects positions by changing the 'theObject' array values
		theObject[0] = xpos, theObject[1] = ypos;
		//make some temp x and y variables so we dont have to type out so much

		x = theObject[0];
		y = theObject[1];

		//draw some crosshairs around the object
		circle(cameraFeed, Point(x, y), 50, Scalar(0, 0, 255), 2);
		if (y - 25 > 0)
			line(cameraFeed, Point(x, y), Point(x, y - 25), Scalar(0, 0, 255), 2);
		else line(cameraFeed, Point(x, y), Point(x, 0), Scalar(0, 0, 255), 2);
		if (y + 25 < FRAME_HEIGHT)
			line(cameraFeed, Point(x, y), Point(x, y + 25), Scalar(0, 0, 255), 2);
		else line(cameraFeed, Point(x, y), Point(x, FRAME_HEIGHT), Scalar(0, 0, 255), 2);
		if (x - 25 > 0)
			line(cameraFeed, Point(x, y), Point(x - 25, y), Scalar(0, 0, 255), 2);
		else line(cameraFeed, Point(x, y), Point(0, y), Scalar(0, 0, 255), 2);
		if (x + 25 < FRAME_WIDTH)
			line(cameraFeed, Point(x, y), Point(x + 25, y), Scalar(0, 0, 255), 2);
		else line(cameraFeed, Point(x, y), Point(FRAME_WIDTH, y), Scalar(0, 0, 255), 2);


		//putText(cameraFeed, intToString(x) + "," + intToString(y), Point(x, y + 30), 1, 1, Scalar(0, 0, 255), 2);
		//putText(cameraFeed, "Motion Detected", Point(400, 50), 1, 1, Scalar(0, 0, 255), 3);
		string strMytestString("& Movement detected at  "+intToString(x) + "," + intToString(y));
		//cout << strMytestString;
		//imshow("MotionDetect", cameraFeed);

	}
	else
	{
		string strMytestString("& No movement detected  ");
		//cout << strMytestString;
		//putText(cameraFeed, "No Motion Detected", Point(400, 50), 1, 1, Scalar(0, 0, 255), 3);
		//imshow("MotionDetect", cameraFeed);

	}
	return objectDetected;

}


int detectAndDisplay(Mat frame_in)
{
	Mat frame;
	frame_in.copyTo(frame);
	Mat frame_gray;
	frame_in.copyTo(frame_gray);
	//cvtColor(frame, frame_gray, COLOR_BGR2GRAY);
	equalizeHist(frame_gray, frame_gray);
	//-- Detect faces
	std::vector<Rect> faces;
	face_cascade.detectMultiScale(frame_gray, faces);

	if (faces.size() > 0)
	{
		for (size_t i = 0; i < faces.size(); i++)
		{
			Point center(faces[i].x + faces[i].width / 2, faces[i].y + faces[i].height / 2);
			ellipse(frame, center, Size(faces[i].width / 2, faces[i].height / 2), 0, 0, 360, Scalar(255, 0, 0), 2);
			Mat faceROI = frame_gray(faces[i]);

		}
		string strMytestString("   Faces Detected: ");
		//cout << strMytestString << faces.size() << endl;
		//imshow("detected", frame_gray);
	}
	else
	{
		string strMytestString(" No face detected ");
		//cout << strMytestString;
	}
	return faces.size();
}
