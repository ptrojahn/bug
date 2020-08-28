#include <tuple>
#include <string.h>
#include <vector>
#include <random>
#include <iostream>

struct Vec2 {
	int x, y;

	Vec2() {
		x = 0;
		y = 0;
	}

	Vec2(int x, int y) {
		this->x = x;
		this->y = y;
	}

	Vec2 operator+(Vec2 other) {
		return Vec2(x + other.x, y + other.y);
	}
	void operator+=(Vec2 other) {
		this->x += other.x;
		this->y += other.y;
	}
	bool operator==(Vec2 other) {
		return x == other.x && y == other.y;
	}
};

std::ostream &operator<< (std::ostream &os, Vec2 &v) {
	return os << "(" << v.x << ", " << v.y << ")";
}

template<typename T> struct Mat {
	int width, height;
	std::vector<T> data;

	Mat(int width, int height, T* data = nullptr) {
		this->width = width;
		this->height = height;
		this->data = std::vector<T>(width*height);
		if (data != nullptr) {
			memcpy(&this->data[0], data, width * height);
		}
	}
	Mat(int width, int height, T init) {
		this->width = width;
		this->height = height;
		this->data = std::vector<T>(width*height, init);
		for (int i = 0; i < width * height; i++) {
			this->data[i] = init;
		}
	}

	T& get(int x, int y) {
		return data[x + width * y];
	}
	T& get(Vec2 pos) {
		return get(pos.x, pos.y);
	}
	const T& get(int x, int y) const {
		return data[x + width * y];
	}
	const T& get(Vec2 pos) const {
		return get(pos.x, pos.y);
	}
};

struct Step {
	Vec2 pos;
	int action;
	int reward;
	Step(Vec2 pos, int action, int reward) {
		this->pos = pos;
		this->action = action;
		this->reward = reward;
	}
};

const int width = 9;
const int height = 3;
const Mat<char> world = Mat<char>(width, height,
		"....<<..."
		".<<<....."
		">>>>>.<<O");
		//"....<<..."
		//".......<."
		//"....<<<<O");

const int startX = 0, startY = 0;
const Vec2 actionLut[] = {Vec2(0, -1), Vec2(1, 0), Vec2(0, 1), Vec2(-1, 0)};
enum Action {
	Up,
	Right,
	Down,
	Left
};

std::mt19937 engine(1337); // TODO Why not std::mersenne_twister_engine?
std::uniform_real_distribution<double> probabilityDist(0, 1);
std::uniform_int_distribution<int> actionDist(0, 3);

std::tuple<Vec2, int> evaluate(Vec2 state, int action) {
	Vec2 newState = state + actionLut[action];
	if (newState.x < 0 || newState.x > width - 1 || newState.y < 0 || newState.y > height - 1)
		return std::make_tuple(state, -1);
	while (world.get(newState) == '>') {
		newState += actionLut[Right];
	}
	while (world.get(newState) == '<') {
		newState += actionLut[Left];
	}
	int reward = -1;
	if (world.get(newState) == 'O')
		reward = 1000;
	return std::make_tuple(newState, reward);
}

Action egreedy(Action policy, double epsilon) {
	if (probabilityDist(engine) < epsilon)
		return (Action)actionDist(engine); // Explore
	else
		return policy; // Exploit
}

void printMat(Mat<Action> mat) {
	std::cout << "START" << std::endl;
	for (int y = 0; y < mat.height; y++) {
		for (int x = 0; x < mat.width; x++) {
			std::cout << mat.get(x, y);
		}
		std::cout << std::endl;
	}
	std::cout << "END" << std::endl;
}
//############################
// TD(0) learning
//############################

Mat<Action> learn_sarsa() {
	Mat<Action> policy(width, height);
	Mat<std::array<double, 4>> qs(width, height);

	const int episodes = 10000;

	int debugSteps = 0;
	const int debugPrint = 500;

	double epsilon = 1;
	const double epsilonDecay = 0.95;
	const double discountFactor = 0.95;
	const double alpha = 0.2;

	for (int episode = 1; episode <= episodes; episode++) {
		epsilon = epsilon * epsilonDecay;
		Vec2 lastState = Vec2(startX, startY);
		while (world.get(lastState) != 'O') {
			debugSteps++;
			Action action = egreedy(policy.get(lastState), epsilon);
			auto res = evaluate(lastState, action);
			Action action2 = egreedy(policy.get(std::get<0>(res)), epsilon);
			// We add a small amount of the difference of the bellman equation and our current value to our current value
			qs.get(lastState)[action] += alpha * (std::get<1>(res) + discountFactor*qs.get(std::get<0>(res))[action2] - qs.get(lastState)[action]);
		}

		if (episode % debugPrint == 0) {
			std::cout << "Episode: " << episode << " Average steps: " << debugSteps / (double)debugPrint << std::endl;
			printMat(policy);
			debugSteps = 0;
			std::cout << std::endl;
		}
	}
	return policy;
}

//############################
// Monte-Carlo learning
//############################

Mat<Action> learn_mc() {
	Mat<Action> policy(width, height);
	Mat<std::array<int, 4>> counter(width, height);
	Mat<std::array<double, 4>> meanValue(width, height);

	const int episodes = 10000;

	int debugSteps = 0;
	const int debugPrint = 500;

	double epsilon = 1;
	const double epsilonDecay = 0.95;
	for (int episode = 1; episode <= episodes; episode++) {
		//### Policy evaluation: Calculate new values for V with our policy
		// Run episode
		epsilon = epsilon * epsilonDecay;
		std::vector<Step> history;
		Vec2 lastPos = Vec2(startX, startY);
		while (world.get(lastPos) != 'O') {
			int action = egreedy(policy.get(lastPos), epsilon);
			auto res = evaluate(lastPos, action);
			history.push_back(Step(Vec2(lastPos.x, lastPos.y), action, std::get<1>(res)));
			lastPos = std::get<0>(res);
		}
		//std::cout << episode << std::endl;

		// Calculate new values for V with our policy
		std::vector<int> localReturn(history.size());
		localReturn.back() = history.back().reward;
		for (int i = 0; i < localReturn.size() - 1; i++) {
			localReturn[localReturn.size() - 2 - i] = history[localReturn.size() - 2 - i].reward + localReturn[localReturn.size() - 1 - i];
		}
		for (int i = 0; i < localReturn.size(); i++) {
			bool found = false;
			for (int j = 0; j < i; j++) {
				if (history[i].pos == history[j].pos && history[i].action == history[j].action) {
					found = true;
					break;
				}
			}
			if (!found) {
				counter.get(history[i].pos)[history[i].action] = counter.get(history[i].pos)[history[i].action] + 1;
				meanValue.get(history[i].pos)[history[i].action] = meanValue.get(history[i].pos)[history[i].action] + (1./counter.get(history[i].pos)[history[i].action])*(localReturn[i] - meanValue.get(history[i].pos)[history[i].action]);
			}
		}

		//### Policy improvement: Update the policy with the newly gained knowledge of V
		// pi = maxarg a q(s, a)
		for (int x = 0; x < width; x++) {
			for (int y = 0; y < height; y++) {
				Vec2 state = Vec2(x, y);
				int maxAction = 0;
				for (int action = 1; action < 3; action++) {
					if (meanValue.get(state)[action] > meanValue.get(state)[maxAction])
						maxAction = action;
				}
				policy.get(state) = (Action)maxAction;
			}
		}

		debugSteps += history.size();
		if (episode % debugPrint == 0) {
			std::cout << "Episode: " << episode << " Average steps: " << debugSteps / (double)debugPrint << std::endl;
			printMat(policy);
			debugSteps = 0;
		}
	}
	return policy;
}

int main() {
	Mat<Action> policy = learn_mc();

	// Print optimized solution
	Vec2 state = Vec2(startX, startY);
	int reward = 0;
	const char* actionStringLut[] = {"Up", "Right", "Down", "Left"};
	std::vector<Action> actions;
	while (world.get(state) != 'O') {
		actions.push_back(policy.get(state));
		auto res = evaluate(state, policy.get(state));
		reward += std::get<1>(res);
		state = std::get<0>(res);
		if (actions.size() > 100) {
			std::cout << "Policy isn't useful yet when greedy" << std::endl;
			return 0;
		}
	}
	for (Action action : actions) {
		std::cout << actionStringLut[action] << " ";
	}
	std::cout << std::endl;
	return 0;
}
