#include <iostream>
#include <memory>
#include <opencv2/opencv.hpp>

#include "nnet/net.h"

using namespace cv;
using namespace std;
using namespace NNET;

struct BBox {
	float x1, y1, x2, y2, score;
	int label;
};

int main(int argc, char** argv)
{
	NNET::Net net;
	net.ncnn_param(argv[1]);
	net.load_model(argv[2]);
	Mat img = imread(argv[3]);
	
// VideoCapture cam(argv[3]);
// Mat img;
// while (1) {
// cam >> img;
// if (img.empty()) break;
	
	
	const float kScoreThreshold = 0.f;


	int height = img.rows;
	int width = img.cols;
	Mat imgResized;
	cv::resize(img, imgResized, Size(300, 300));

	int inputH = 300;
	int inputW = 300;
	Size input_geometry_ = cv::Size(inputH, inputW);
	
	std::vector<cv::Mat> input_channels;
	NNET::Blob* data = net.net_input_blobs_[0];
	float* input_data = data->mutable_cpu_data();
	for (int i = 0; i < data->CC(); ++i) {
		cv::Mat channel(inputH, inputW, CV_32FC1, input_data);
		input_channels.push_back(channel);
		input_data += inputH * inputW;
	}
	cv::Mat sample_resized;
	cv::resize(img, sample_resized, input_geometry_);
	
	cv::Mat sample_float;
	sample_resized.convertTo(sample_float, CV_32FC3);
	
	Mat mean_;
	//Scalar channel_mean = {104.007, 116.669, 122.679, 0};
	Scalar channel_mean;
	channel_mean[0] = 104.f;
	channel_mean[1] = 117.f;
	channel_mean[2] = 123.f;
	mean_ = cv::Mat(inputH, inputW, CV_32FC3, channel_mean);
	
	cv::Mat sample_normalized;
	cv::subtract(sample_float, mean_, sample_normalized);
	cv::split(sample_normalized, input_channels);
	
	LOG(INFO) << "start net...";
	net.Run();
	LOG(INFO) << "end.";

	// get output, shape is N x 7
	NNET::Blob* result = net.net_output_blobs_[0];
	const float* result_data = result->cpu_data();


	for (int i = 0; i < result->count(); ++i)
	{
		LOG(INFO) << result_data[i];
	}
	//LOG(INFO) << result->count() << "   " << result->height() << "********************";

	const int num_det = result->HH();
	vector<BBox> detections;
	for (int k = 0; k < num_det; ++k) {
		if(result_data[1] > kScoreThreshold) {
			// [image_id, label, score, xmin, ymin, xmax, ymax]
			BBox bbox;
			bbox.x1 = result_data[2] * width;
			bbox.y1 = result_data[3] * height;
			bbox.x2 = result_data[4] * width;
			bbox.y2 = result_data[5] * height;
			bbox.score = result_data[1];
			bbox.label = static_cast<int>(result_data[0]);
			detections.push_back(bbox);
			
			LOG(INFO) << bbox.x1 << " " << bbox.y1 << " " << bbox.x2-bbox.x1 << " " << bbox.y2-bbox.y1;
		}
		result_data += 6;
	}

	// draw
	for (auto& bbox : detections) {
		cv::Rect rect(bbox.x1, bbox.y1, bbox.x2 - bbox.x1 + 1, bbox.y2 - bbox.y1 + 1);
		cv::rectangle(img, rect, cv::Scalar(0, 0, 255), 2);
	}

	//cv::imwrite("faceboxs-result.jpg", img);
	cv::imshow("SSD", img);
	cv::waitKey();
//}

	return 0;
}