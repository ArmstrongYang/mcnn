#include "nnet/run/input_layer.h"

namespace NNET
{

void InputLayer::SetUp(Tensor& Input, Tensor& Output) 
{
	YW = stoi(this->layer_param_[0]);
	YH = stoi(this->layer_param_[1]);
	YC = stoi(this->layer_param_[2]);
	
	Output[0]->Reshape(YC, YH, YW);
}

void InputLayer::L2Net(Net *net)
{
	net->_NH = YH;
	net->_NW = YW;
	net->_NC = YC;
}

void InputLayer::Reshape(Tensor& Input, Tensor& Output)
{
	YC = Output[0]->CC();
	YH = Output[0]->HH();
	YW = Output[0]->WW();
	
	Output[0]->Reshape(YC, YH, YW);
}

void InputLayer::Run(Tensor& Input, Tensor& Output)
{
	
}

}