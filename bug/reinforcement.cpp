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

/* DQN learning implementation
 * Q-learning is off-policy because it updates the policy with respect to the greedy argmax
 * and not the e-greedy it uses as policy.*/
void reinforcement_train() {
	const int episodes = 32;
	const int episodeLength = 200;
	const double epsilonDecay = pow(0.1, 1 / (double)episodes);
	const int replaysMax = episodeLength*16;
	const int replayBatch = 64;
	const float discount = 0.9;
	std::shared_ptr<QNet> qNet = std::make_shared<QNet>();
	torch::optim::SGD optimizer(qNet->parameters(), 0.001);

	double epsilon = 1.;
	int replayIndex = 0;
	std::vector<Replay> replays;

	for (int episode = 1; episode <= episodes; episode++) {
		QNet qNetFixed(*qNet);
		State state;
		int timeAlive = 0;
		epsilon = epsilon * epsilonDecay;

		float debugLoss = 0;
		int debugLossCount = 0;
		torch::Tensor debugRes = torch::zeros(3);
		torch::Tensor debugValue = torch::zeros(1);

		while (timeAlive < episodeLength) {
			// Make step
			timeAlive++;
			State pre = state;
			Action action = egreedy(qargmax(*qNet, state), epsilon);
			debugRes += qNet->forward(state.getFeatures());
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
				for (int i = 0; i < replayBatch; i++) {
					optimizer.zero_grad();
					Replay& replay = replays[torch::randint(0, replays.size(), 1).item<int>()];
					torch::Tensor valueExpected = replay.reward + discount*qargmaxVal(qNetFixed, replay.post).detach();
					torch::Tensor valueNext = qVal(*qNet, replay.pre, replay.action);

					auto loss = torch::mse_loss(valueNext, valueExpected);
					debugLoss += loss.item<float>();
					debugValue += valueNext.item<float>();
					debugLossCount++;
					loss.backward();
					// Otherwise large gradients can blow parameters up to inf TODO: why .data().
					for (torch::Tensor& param : qNet->parameters()) {
						param.grad().data().clamp_(-1, 1);
					}
					optimizer.step();
				}
			}
			state.updateWindow();
		}
		std::cout << "Episode: " << episode << std::endl;
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
		return egreedy(qargmax(*net, state), 0.1);
	});
	initial.visual(policyFun);
}
