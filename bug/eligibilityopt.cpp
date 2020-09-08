#include "eligibilityopt.h"

EligibilityOpt::EligibilityOpt(std::vector<at::Tensor> params, double learningRate, double decay) : learningRate(learningRate), decay(decay) {
	for (torch::Tensor& paramTensor : params) {
		traces.push_back({.param = paramTensor, .tensor = torch::zeros(paramTensor.sizes(), torch::TensorOptions().requires_grad(false))});
	}
}

void EligibilityOpt::step(torch::Tensor tdError) {
	torch::NoGradGuard noGrad;
	for (Trace& trace : traces) {
		trace.tensor = trace.tensor * decay + trace.param.grad() * learningRate;
		trace.param -= tdError * trace.tensor;
	}
}

void EligibilityOpt::zero_grad() {
	for (Trace& trace : traces) {
		if (trace.param.grad().defined())
			trace.param.grad().zero_();
	}
}
