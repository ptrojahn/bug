#include "reinforcement.hpp"

#include <torch/torch.h>
#include <functional>
#include <cmath>

#include "state.hpp"
#include "eligibilityopt.h"

struct Replay {
	State pre;
	Action action;
	int reward;
	State post;
};

struct QNet : torch::nn::Module {
	torch::nn::Linear lin0 = nullptr; // TODO what does nullptr do?
	torch::nn::Linear lin1 = nullptr;
	torch::nn::Linear lin2 = nullptr;

	QNet() : lin0(BUG_RESOLUTION * 3, 32), lin1(32, 16), lin2(16, 3) {
		register_module("lin0", lin0);
		register_module("lin1", lin1);
		register_module("lin2", lin2);
	}

	torch::Tensor forward(torch::Tensor x) {
		x = torch::relu(lin0->forward(x));
		x = torch::relu(lin1->forward(x));
		return lin2->forward(x);
	}
};

Action qargmax(QNet& net, State& state) {
	return (Action)net.forward(state.getFeatures()).argmax().item<int>();
}
torch::Tensor qargmaxVal(QNet& net, State& state) {
	return net.forward(state.getFeatures()).max().unsqueeze(0);
}
torch::Tensor qVal(QNet& net, State& state, Action& action) {
	return net.forward(state.getFeatures())[action].unsqueeze(0);
}

Action egreedy(Action action, double epsilon) {
	double random = torch::rand(1).item<float>();
	if (random < epsilon) // Explore
		return (Action)torch::randint(0, 3, 1).item<int>();
	else { // Exploit
		return action;
	}
}

/* DQN learning implementation with eligibility traces
 * Q-learning is off-policy because it updates the policy with respect to the greedy argmax
 * and not the e-greedy it uses as policy.*/
void reinforcement_train() {
	const int episodes = 512;
	const int episodeLength = 200;
	const double epsilonDecay = pow(0.1, 1 / (double)episodes);
	const double learningRate = 0.00001;
	const double discount = 0.8;
	const double traceDecay = 0.5;
	std::shared_ptr<QNet> qNet = std::make_shared<QNet>();
	EligibilityOpt optimizer(qNet->parameters(), learningRate, traceDecay, discount);

	double epsilon = 1.;

	for (int episode = 1; episode <= episodes; episode++) {
		QNet qNetFixed(*qNet);
		State state;
		int timeAlive = 0;
		epsilon = epsilon * epsilonDecay;

		int score = 0;

		while (timeAlive < episodeLength) {
			// Make step
			timeAlive++;
			State pre = state;
			Action action = egreedy(qargmax(*qNet, state), epsilon);
			int reward = state.evaluate(action);
			optimizer.zero_grad();
			torch::Tensor valueExpected = reward + discount*qargmaxVal(qNetFixed, state).detach();
			torch::Tensor valueRealized = qVal(*qNet, pre, action);
			torch::Tensor tdError = valueExpected - valueRealized;
			torch::Tensor gradient = qVal(*qNet, pre, action);
			gradient.backward();
			for (torch::Tensor& param : qNet->parameters()) {
				param.grad().data().clamp_(-1, 1);
			}

			optimizer.step(tdError);

			if (reward > 0)
				score++;
			state.updateWindow();
		}
		std::cout << "Episode: " << episode << " Score: " << score << " Epsilon: " << epsilon << std::endl;
	}
	torch::save(qNet, "qNet.pt");
	std::cout << "Done training!" << std::endl;

	State initial;
	auto policyFun = std::function([&](State state) {
		return qargmax(*qNet, state);
	});
	initial.visual(policyFun);
}

void reinforcement_run() {
	std::shared_ptr<QNet> net = std::make_shared<QNet>();
	torch::load(net, "qNet.pt");
	State initial;
	auto policyFun = std::function([&](State state) {
		return qargmax(*net, state);
	});
	initial.visual(policyFun);
}
