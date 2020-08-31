#include "reinforcement.hpp"

#include <torch/torch.h>
#include <functional>
#include <cmath>

#include "state.hpp"

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

	QNet() : lin0(BUG_RESOLUTION * 3, 64), lin1(64, 32), lin2(32, 3) {
		register_module("lin0", lin0);
		register_module("lin1", lin1);
		register_module("lin2", lin2);
	}

	torch::Tensor forward(torch::Tensor x) {
		x = torch::relu(lin0->forward(x));
		x = torch::relu(lin1->forward(x));
		x = torch::relu(lin2->forward(x));
		return x;
	}
};

Action qargmax(QNet& net, State& state) {
	return (Action)net.forward(state.getFeatures()).argmax().item<int>();
}
torch::Tensor qargmaxVal(QNet& net, State& state) {
	return net.forward(state.getFeatures()).max();
}
torch::Tensor qVal(QNet& net, State& state, Action& action) {
	return net.forward(state.getFeatures())[action];
}

Action egreedy(Action action, double epsilon) {
	double random = torch::rand(1).item<float>();
	if (epsilon < random) // Explore
		return (Action)torch::randint(0, 3, 1).item<int>();
	else { // Exploit
		return action;
	}
}

void reinforcement_train() {
	const int episodes = 10;
	const double epsilonDecay = 0.9;
	const int replaysMax = 10000;
	const int replayBatch = 32;
	std::shared_ptr<QNet> qNet = std::make_shared<QNet>();
	torch::optim::SGD optimizer(qNet->parameters(), 0.05);

	State state;
	double epsilon = 1.;
	int replayIndex = 0;
	std::vector<Replay> replays;

	for (int episode = 1; episode <= episodes; episode++) {
		QNet qNetFixed(*qNet);
		int timeAlive = 0;
		epsilon = epsilon * epsilonDecay;
		std::cout << episode << std::endl;

		while (timeAlive < 200) {
			// Make step
			timeAlive++;
			State pre = state;
			Action action = egreedy(qargmax(*qNet, state), epsilon);
			int reward = state.evaluate(action);
			State post = state;
			Replay replay = {.pre = pre, .action = action, .reward = reward, .post = post};
			if (replays.size() < replaysMax)
				replays.push_back(replay);
			else {
				replays[replayIndex] = replay;
				replayIndex %= replaysMax;
			}

			//Train from replay
			if (replays.size() >= replayBatch) { // TODO: How is a pointer a iterator?
				torch::Tensor valueExpected;
				torch::Tensor valueNext;
				for (int i = 0; i < replayBatch; i++) {
					Replay& replay = replays[torch::randint(0, replays.size(), 1).item<int>()];
					torch::Tensor valueExpectedR = replay.reward + qargmaxVal(qNetFixed, replay.post).unsqueeze(0).detach();
					torch::Tensor valueNextR = qVal(*qNet, replay.pre, replay.action).unsqueeze(0);
					valueExpected = valueExpected.defined() ? torch::cat({valueExpected, valueExpectedR}, 0) : valueExpectedR;
					valueNext = valueNext.defined() ? torch::cat({valueNext, valueNextR}, 0) : valueNextR;
				}
				optimizer.zero_grad();
				auto loss = torch::mse_loss(valueNext, valueExpected);
				loss.backward();
				optimizer.step();
			}
		}
	}
	torch::save(qNet, "qNet.pt");
}

void reinforcement_run() {
	std::shared_ptr<QNet> net = std::make_shared<QNet>();
	torch::load(net, "qNet.pt");
	State initial;
	auto policyFun = std::function([&](State state) {
		return qargmax(*net, state);
		//return Action::Forward;
	});
	initial.visual(policyFun);
}
