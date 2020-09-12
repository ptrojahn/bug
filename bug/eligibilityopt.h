#ifndef ELIGIBILITYOPT_H
#define ELIGIBILITYOPT_H

#include <torch/torch.h>

struct Trace {
	torch::Tensor param;
	torch::Tensor trace;
};

class EligibilityOpt
{
public:
	EligibilityOpt(std::vector<torch::Tensor> params, double learningRate, double decay, double discount);
	void step(torch::Tensor tdError);
	void zero_grad();

private:
	std::vector<Trace> traces;
	double learningRate;
	double decay;
	double discount;
};

#endif // ELIGIBILITYOPT_H
