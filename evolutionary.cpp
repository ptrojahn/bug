#include "evolutionary.hpp"

#include <iostream>
#include <thread>
#include <future>

#include <torch/torch.h>

#include "bug.hpp"
#include "food.hpp"

#define EYE_RESOLUTION 32
#define MUTATION 0.1
#define GENERATIONS 2
#define FIRSTGENERATION 1000
#define DESCENDANTS 100
#define MAXSIMLIMIT 1000
#define REWARD 400
#define HEALTHINIT 400

torch::nn::Linear mutate_linear(torch::nn::Linear linear, float amount) {
	linear->weight = torch::clamp(linear->weight + (2*torch::rand(linear->weight.sizes()) - 1) * amount, 0, 1);
	linear->bias = torch::clamp(linear->bias + (2*torch::rand(linear->bias.sizes()) - 1) * amount, 0, 1);
	return linear;
}
torch::nn::Linear init_linear(torch::nn::Linear linear) {
	linear->weight = torch::rand(linear->weight.sizes());
	linear->bias = torch::rand(linear->bias.sizes());
	return linear;
}

struct BugNet : torch::nn::Module {
	torch::nn::Linear lin0 = nullptr; // TODO what does nullptr do?
	torch::nn::Linear lin1 = nullptr;
	torch::nn::Linear lin2 = nullptr;

	BugNet() : lin0(EYE_RESOLUTION, EYE_RESOLUTION), lin1(EYE_RESOLUTION, EYE_RESOLUTION / 2), lin2(EYE_RESOLUTION / 2, 3) {
		register_module("lin0", lin0);
		register_module("lin1", lin1);
		register_module("lin2", lin2);
		lin0 = init_linear(lin0);
		lin1 = init_linear(lin1);
		lin2 = init_linear(lin2);
	}
	std::shared_ptr<BugNet> copy_fuzzy() {
		std::shared_ptr<BugNet> bugnet = std::make_shared<BugNet>();
		bugnet->lin0 = mutate_linear(lin0, MUTATION);
		bugnet->lin1 = mutate_linear(lin1, MUTATION);
		bugnet->lin2 = mutate_linear(lin2, MUTATION);
		return bugnet;
	}

	torch::Tensor forward(torch::Tensor x) {
		x = torch::sigmoid(lin0->forward(x));
		x = torch::sigmoid(lin1->forward(x));
		return torch::sigmoid(lin2->forward(x));
	}
};

void simulateVisual(BugNet &net, World world) {
	Bug *bug = world.getAll<Bug>()[0];
	std::vector<Food*> foods = world.getAll<Food>();

	bug->actionFunction = std::function([&](std::vector<sf::Color> vision) {
		std::vector<float> inputVec;
		inputVec.reserve(3 * vision.size());
		for (sf::Color color : vision) {
			inputVec.push_back(color.r);
			inputVec.push_back(color.g);
			inputVec.push_back(color.b);
		}
		torch::Tensor input = torch::from_blob(inputVec.data(), {(long)inputVec.size(), 1});
		torch::Tensor output = net.forward(input);
		return Direction(output[0].item<float>() > 0.5, output[1].item<float>() > 0.5, output[2].item<float>() > 0.5);
	});
	auto update = [&](World &world) {
		int health = HEALTHINIT;
		for(int i = 0; i < MAXSIMLIMIT; i++) {
			for (Food* food : foods) {
				if (food->shape->intersection(bug->shape)) {
					health += REWARD;
					torch::Tensor pos = torch::rand(2) * 700 + 50;
					food->setPosition(sf::Vector2i(pos[0].item<int>(), pos[1].item<int>()));
				}
			}
			health -= 1;
			if (health == 0)
				return i;
			world.step();
		}
		return MAXSIMLIMIT;
	};
	world.visual(std::function(update));
}

int simulate(BugNet &net, World world) {
	Bug *bug = world.getAll<Bug>()[0];
	std::vector<Food*> foods = world.getAll<Food>();

	bug->actionFunction = std::function([&](std::vector<sf::Color> vision) {
		std::vector<float> inputVec;
		inputVec.reserve(3 * vision.size());
		for (sf::Color color : vision) {
			inputVec.push_back(color.r);
			inputVec.push_back(color.g);
			inputVec.push_back(color.b);
		}
		torch::Tensor input = torch::from_blob(inputVec.data(), {(long)inputVec.size(), 1});
		torch::Tensor output = net.forward(input);
		return Direction(output[0].item<float>() > 0.5, output[1].item<float>() > 0.5, output[2].item<float>() > 0.5);
	});
	int health = HEALTHINIT;
	for(int i = 0; i < MAXSIMLIMIT; i++) {
		for (Food* food : foods) {
			if (food->shape->intersection(bug->shape)) {
				health += REWARD;
				torch::Tensor pos = torch::rand(2) * 700 + 50;
				food->setPosition(sf::Vector2i(pos[0].item<int>(), pos[1].item<int>()));
			}
		}
		health -= 1;
		if (health == 0)
			return i;
		world.step();
	}
	return MAXSIMLIMIT;
}

int simulateStep(BugNet &net, World world) {
	Bug *bug = world.getAll<Bug>()[0];
	std::vector<Food*> foods = world.getAll<Food>();

	bug->actionFunction = std::function([&](std::vector<sf::Color> vision) {
		std::vector<float> inputVec;
		inputVec.reserve(3 * vision.size());
		for (sf::Color color : vision) {
			inputVec.push_back(color.r);
			inputVec.push_back(color.g);
			inputVec.push_back(color.b);
		}
		torch::Tensor input = torch::from_blob(inputVec.data(), {(long)inputVec.size(), 1});
		torch::Tensor output = net.forward(input);
		return Direction(output[0].item<float>() > 0.5, output[1].item<float>() > 0.5, output[2].item<float>() > 0.5);
	});

	int health = HEALTHINIT;
	for(int i = 0; i < MAXSIMLIMIT; i++) {
		for (Food* food : foods) {
			if (food->shape->intersection(bug->shape)) {
				health += REWARD;
				torch::Tensor pos = torch::rand(2) * 700 + 50;
				food->setPosition(sf::Vector2i(pos[0].item<int>(), pos[1].item<int>()));
			}
		}
		health -= 1;
		if (health == 0)
			return i;
		world.step();
	}
	return MAXSIMLIMIT;
}

void evolutionary_train(World &world) {
	torch::manual_seed(42);
	int thread_count = std::thread::hardware_concurrency();

	std::shared_ptr<BugNet> survivor;
	int survivorScore = 0;
	auto func_initial = [&](int count) -> std::tuple<int, std::shared_ptr<BugNet>> {
		int threadmax = 0;
		std::shared_ptr<BugNet> threadnet;
		for (int j = 0; j < count; j++) {
			std::shared_ptr<BugNet> descendant = std::make_shared<BugNet>();
			int score = simulate(*descendant, world);
			if (score > threadmax) {
				threadnet = descendant;
				threadmax = score;
			}
		}
		return std::make_tuple(threadmax, threadnet);
	};
	std::vector<std::future<std::tuple<int, std::shared_ptr<BugNet>>>> futures;
	int chunk_size = FIRSTGENERATION / thread_count;
	for (int i = 0; i < thread_count; i++) {
		if (i != thread_count - 1)
			futures.push_back(std::async(std::launch::async, func_initial, chunk_size));
		else // Distribution to threads might not always be even
			futures.push_back(std::async(std::launch::async, func_initial, FIRSTGENERATION - chunk_size * thread_count));
	}
	for (auto &future : futures) {
		future.wait();
		std::tuple<int, std::shared_ptr<BugNet>> res = future.get();
		if (std::get<0>(res) > survivorScore) {
			survivor = std::get<1>(res); // TODO: How does get work
			survivorScore = std::get<0>(res);
		}
	}

	// Refine
	auto func_refine = [&](int count) -> std::tuple<int, std::shared_ptr<BugNet>> {
		int threadmax = 0;
		std::shared_ptr<BugNet> threadnet;
		for (int j = 0; j < count; j++) {
			std::shared_ptr<BugNet> descendant = survivor->copy_fuzzy();
			int score = simulate(*descendant, world);
			if (score > threadmax) {
				threadnet = descendant;
				threadmax = score;
			}
		}
		return std::make_tuple(threadmax, threadnet);
	};
	for (int i = 0; i < GENERATIONS; i++) {
		std::cout << "Generation: " << i << "Score: " << survivorScore << "\r";
		std::flush(std::cout);

		std::vector<std::future<std::tuple<int, std::shared_ptr<BugNet>>>> futures;
		int chunk_size = DESCENDANTS / thread_count;
		for (int i = 0; i < thread_count; i++) {
			if (i != thread_count - 1)
				futures.push_back(std::async(std::launch::async, func_refine, chunk_size));
			else // Distribution to threads might not always be even
				futures.push_back(std::async(std::launch::async, func_refine, DESCENDANTS - chunk_size * thread_count));
		}
		std::shared_ptr<BugNet> currentSurvivor = survivor;
		int currentScore = survivorScore;
		for (auto &future : futures) {
			future.wait();
			std::tuple<int, std::shared_ptr<BugNet>> res = future.get();
			if (std::get<0>(res) > currentScore) {
				currentSurvivor = std::get<1>(res); // TODO: How does get work
				currentScore = std::get<0>(res);
			}
		}
	}
	std::cout << "Done!" << std::endl;

	torch::save(survivor, "evolutionary.pt"); // TODO How does Module -> Value work
}

void evolutionary_run(World &world) {
	std::shared_ptr<BugNet> net = std::make_shared<BugNet>();
	torch::load(net, "evolutionary.pt");
	simulateVisual(*net, world);
}
