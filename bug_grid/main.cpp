#include <tuple>

const char* world =
		"####################"
		"#                 X#"
		"#          >>>>>>  #"
		"#    >>>>>         #"
		"#              X   #"
		"####################";
const width = 20;
const height = 6;
const int startX, startY = 1;
const Vec2 actionLut[] = {Vec2(0, -1), Vec2(1, 0), Vec2(0, 1), Vec2(-1, 0)};
enum Action {
	Up,
	Right,
	Down,
	Left
};

struct Vec2 {
	int x, y;

	Vec2(int x, int y) {
		this->x = x;
		this->y = y;
	}

	Vec2 operator+(Vec2 other) {
		return Vec2(x + other.x, y + other.y);
	}
};

char world_val(Vec2 pos) {
	return world[pos.x + pos.y * width];
}

std::tuple<Vec2, int> evaluate(Vec2 state, int action) {
	Vec2 newState = state + actionLut[action];
	while (world_val(newState) == '>') {
		newState += actionLut[Right];
	}
	int reward = -1;
	if (world_val(newState) == 'X')
		reward = 10;
	return std::make_tuple(newState, -1);
}

int main() {
	return 0;
}
