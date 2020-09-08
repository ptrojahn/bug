#ifndef ELIGIBILITYOPT_H
#define ELIGIBILITYOPT_H

#include <torch/torch.h>

struct Trace {
	torch::Tensor param;
	torch::Tensor tensor;
};

class EligibilityOpt
{
public:
	EligibilityOpt(std::vector<torch::Tensor> params, double learningRate, double decay);
	void step(torch::Tensor tdError);
	void zero_grad();

private:
	std::vector<Trace> traces;
	double learningRate;
	double decay;
};

#endif // ELIGIBILITYOPT_H
