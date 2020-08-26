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
		this->data = new T[width*height];
		for (int i = 0; i < width * height; i++) {
			this->data[i] = init;
		}
	}
	void set(int x, int y, T val) {
		this->data[x + width * y] = val;
	}
	void set(Vec2 pos, T val) {
		set(pos.x, pos.y, val);
	}

	T get(int x, int y) const {
		return data[x + width * y];
	}
	T get(Vec2 pos) const {
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
		"........."
		">>>>>.<<O");

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

int egreedy(int policy, double epsilon) {
	if (probabilityDist(engine) < epsilon)
		return policy; // Exploit
	else
		return actionDist(engine); // Explore
}

Mat<Action> learn_mc() {
	Mat<Action> policy(width, height);
	Mat<int> counter(width, height);
	Mat<double> meanValue(width, height);

	for (int episode = 1; episode < 10000; episode++) {
		//### Run episode
		double epsilon = 0.8;
		std::vector<Step> history;
		Vec2 lastPos = Vec2(startX, startY);
		while (world.get(lastPos) != 'O') {
			int action = egreedy(policy.get(lastPos), epsilon);
			auto res = evaluate(lastPos, action);
			lastPos = std::get<0>(res);
			history.push_back(Step(Vec2(lastPos.x, lastPos.y), action, std::get<1>(res)));
		}
		std::cout << history.size() << std::endl;

		//### Policy evaluation: Calculate new values for V with our policy
		std::vector<int> localReturn(history.size());
		localReturn.back() = history.back().reward;
		for (int i = 0; i < localReturn.size() - 1; i++) {
			localReturn[localReturn.size() - 2 - i] = history[localReturn.size() - 2 - i].reward + localReturn[localReturn.size() - 1 - i];
		}
		for (int i = 0; i < localReturn.size(); i++) {
			counter.set(history[i].pos, counter.get(history[i].pos) + 1);
			meanValue.set(history[i].pos, meanValue.get(history[i].pos) + (1./counter.get(history[i].pos))*(localReturn[i] - meanValue.get(history[i].pos)));
		}

		//### Policy improvement: Update the policy with the newly gained knowledge of V
		// pi = maxarg a q(s, a)
		for (int x = 0; x < width; x++) {
			for (int y = 0; y < height; y++) {
				Vec2 state = Vec2(x, y);
				double newStateValues[] = {	meanValue.get(std::get<0>(evaluate(state, Up))),
											meanValue.get(std::get<0>(evaluate(state, Right))),
											meanValue.get(std::get<0>(evaluate(state, Down))),
											meanValue.get(std::get<0>(evaluate(state, Left)))};
				int maxAction = 0;
				for (int action = 1; action < 3; action++) {
					if (newStateValues[action] > newStateValues[maxAction])
						maxAction = action;
				}
				policy.set(state, (Action)maxAction);
			}
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
