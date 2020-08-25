#ifndef GEOMETRY_H
#define GEOMETRY_H

#include <optional>
#include <cmath>

#include <SFML/Graphics.hpp>

sf::Vector2i rotateAround(sf::Vector2i value, sf::Vector2i center, float angle);
sf::Vector2i rotate(sf::Vector2i value, float angle);

struct Line;
struct Rectangle;
struct Circle;

struct Shape {
	virtual bool intersection(Shape* other) = 0;
};

struct Line : public Shape {
	sf::Vector2i start;
	sf::Vector2i end;

	Line(sf::Vector2i start, sf::Vector2i end);
	bool intersection(Shape* other);
};

struct Rectangle : public Shape {
	sf::Vector2i pos;
	sf::Vector2i size;

	Rectangle(sf::Vector2i pos, sf::Vector2i size);
	bool intersection(Shape* other);
};

struct Circle : public Shape {
	sf::Vector2i pos;
	float radius;

	Circle(sf::Vector2i pos, int radius);
	bool intersection(Shape* other);
};

struct Ray {
	sf::Vector2i start;
	sf::Vector2i direction;

	Ray(sf::Vector2i start, sf::Vector2i direction);
	std::optional<float> intersection(Shape* other);
};

#endif
