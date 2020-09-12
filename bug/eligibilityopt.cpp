#include "eligibilityopt.h"

EligibilityOpt::EligibilityOpt(std::vector<at::Tensor> params, double learningRate, double decay, double discount) : learningRate(learningRate), decay(decay), discount(discount) {
	for (torch::Tensor& paramTensor : params) {
		traces.push_back({.param = paramTensor, .trace = torch::zeros(paramTensor.sizes())});
	}
}

void EligibilityOpt::step(torch::Tensor tdError) {
	torch::NoGradGuard noGrad;
	for (Trace& trace : traces) {
		// tdError positive => realized is smaller than what we expected; increase through trace
		// tdError negative => realized is larger than what we expected; decrease through trace
		trace.param += learningRate * (torch::sigmoid(tdError) * 2 - 1) * trace.trace; // sigmoiding it prevents explosion
		// Update trace by decaying old values and adding current gradient
		trace.trace = discount * decay * trace.trace + trace.param.grad();
	}
	//std::cout << "sumGrad: " << sumGrad << " sumTrace: " << sumTrace << std::endl;
}

void EligibilityOpt::zero_grad() {
	for (Trace& trace : traces) {
		if (trace.param.grad().defined())
			trace.param.grad().zero_();
	}
}
