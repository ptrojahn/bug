#include "geometry.hpp"

#include <iostream>

sf::Vector2i rotateAround(sf::Vector2i value, sf::Vector2i center, float angle) {
	sf::Transform rot;
	rot.rotate(angle);
	sf::Transform translate;
	translate.translate(sf::Vector2f(center));
	return sf::Vector2i(translate * rot * translate.getInverse() * sf::Vector2f(value));
}

sf::Vector2i rotate(sf::Vector2i value, float angle) {
	return rotateAround(value, sf::Vector2i(0, 0), angle);
}

float cross(sf::Vector2i first, sf::Vector2i second) {
	return first.x*second.y - first.y*second.x;
}


Ray::Ray(sf::Vector2i start, sf::Vector2i direction) {
	this->start = start;
	this->direction = direction;
}

std::optional<float> Ray::intersection(Shape* shape) {
	if (Line* other = dynamic_cast<Line*>(shape)) {
		if (direction == (other->start - other->end) || direction == (other->end - other->start)) {
			return std::nullopt;
		} else {
			sf::Vector2i dirOther = other->end - other->start;
			float facThis = (float)cross((other->start - start), dirOther) / cross(direction, dirOther);
			float facOther = (float)cross((start - other->start), direction) / cross(dirOther, direction);
			if (facThis >= 0 && facOther >= 0 && facOther <= 1) {
				return facThis; // TODO How does the conversion work
			} else {
				return std::nullopt;
			}
		}
	} else if (Rectangle* other = dynamic_cast<Rectangle*>(shape)) {
		Line linesOther[] = {
			Line(other->pos, other->pos + sf::Vector2i(other->size.x, 0)),
			Line(other->pos, other->pos + sf::Vector2i(0, other->size.y)),
			Line(other->pos + sf::Vector2i(other->size.x, 0), other->pos + sf::Vector2i(other->size.x, other->size.y)),
			Line(other->pos + sf::Vector2i(0, other->size.y), other->pos + sf::Vector2i(other->size.x, other->size.y))
		};
		std::optional<float> nearest = std::nullopt; // TODO How does this work
		for (auto lOther : linesOther) {
			std::optional<float> res = this->intersection(&lOther);
			if (res && (!nearest.has_value() || res < nearest)) { // TODO How does < work
				nearest = res;
			}
		}
		return nearest;
	} else if (Circle* other = dynamic_cast<Circle*>(shape)) {
		float a = pow(direction.x, 2) + pow(direction.y, 2);
		float b = 2*(start.x*direction.x - direction.x*other->pos.x + direction.y * start.y - direction.y * other->pos.y);
		float c = pow(start.x, 2) + pow(other->pos.x, 2) + pow(start.y, 2) + pow(other->pos.y, 2) - pow(other->radius, 2) - 2*(start.x * other->pos.x + start.y * other->pos.y);
		float discriminant = pow(b, 2) - 4*a*c;
		if (discriminant >= 0) {
			float root = sqrt(discriminant);
			float val = (-b - root) / (2*a);
			return val > 0 ? std::optional<float>(val) : std::nullopt;
		} else {
			return std::nullopt;
		}
	} else {
		return std::nullopt;
	}
}

Line::Line(sf::Vector2i start, sf::Vector2i end) {
	this->start = start;
	this->end = end;
}

bool Line::intersection(Shape* shape) {
	if (Line* other = dynamic_cast<Line*>(shape)) {
		Ray ray(start, end - start);
		std::optional<float> point = ray.intersection(other);
		return point && point <= 1;
	} else if (Rectangle* other = dynamic_cast<Rectangle*>(shape)) {
		Line linesOther[] = {
			Line(other->pos, other->pos + sf::Vector2i(other->size.x, 0)),
			Line(other->pos, other->pos + sf::Vector2i(0, other->size.y)),
			Line(other->pos + sf::Vector2i(other->size.x, 0), other->pos + sf::Vector2i(other->size.x, other->size.y)),
			Line(other->pos + sf::Vector2i(0, other->size.y), other->pos + sf::Vector2i(other->size.x, other->size.y))
		};
		for (auto lOther : linesOther) {
			if (lOther.intersection(this)) {
				return true;
			}
		}
		return false;
	} else if (Circle* other = dynamic_cast<Circle*>(shape)) {
		Ray ray(start, end - start);
		std::optional<float> point = ray.intersection(other);
		return point && point <= 1;
	} else {
		return false;
	}
}

Rectangle::Rectangle(sf::Vector2i pos, sf::Vector2i size) {
	this->pos = pos;
	this->size = size;
}

bool Rectangle::intersection(Shape* shape) {
	if (Line* other = dynamic_cast<Line*>(shape)) {
		return other->intersection(this);
	} else if (Rectangle* other = dynamic_cast<Rectangle*>(shape)) {
		Line linesThis[] = {
			Line(pos, pos + sf::Vector2i(size.x, 0)),
			Line(pos, pos + sf::Vector2i(0, size.y)),
			Line(pos + sf::Vector2i(size.x, 0), pos + sf::Vector2i(size.x, size.y)),
			Line(pos + sf::Vector2i(0, size.y), pos + sf::Vector2i(size.x, size.y))
		};
		Line linesOther[] = {
			Line(other->pos, other->pos + sf::Vector2i(other->size.x, 0)),
			Line(other->pos, other->pos + sf::Vector2i(0, other->size.y)),
			Line(other->pos + sf::Vector2i(other->size.x, 0), other->pos + sf::Vector2i(other->size.x, other->size.y)),
			Line(other->pos + sf::Vector2i(0, size.y), other->pos + sf::Vector2i(other->size.x, other->size.y))
		};
		for (auto lThis : linesThis) {
			for (auto lOther : linesOther) {
				if (lThis.intersection(&lOther))
					return true;
			}
		}
		return false;
	} else if (Circle* other = dynamic_cast<Circle*>(shape)) {
		Line linesThis[] = {
			Line(pos, pos + sf::Vector2i(size.x, 0)),
			Line(pos, pos + sf::Vector2i(0, size.y)),
			Line(pos + sf::Vector2i(size.x, 0), pos + sf::Vector2i(size.x, size.y)),
			Line(pos + sf::Vector2i(0, size.y), pos + sf::Vector2i(size.x, size.y))
		};
		for (auto lThis : linesThis) {
			if (lThis.intersection(other)) {
				return true;
			}
		}
		return false;
	} else {
		return false;
	}
}

Circle::Circle(sf::Vector2i pos, int radius) {
	this->pos = pos;
	this->radius = radius;
}

bool Circle::intersection(Shape* shape) {
	if (Line* other = dynamic_cast<Line*>(shape)) {
		return other->intersection(this);
	} else if (Rectangle* other = dynamic_cast<Rectangle*>(shape)) {
		return other->intersection(this);
	} else if (Circle* other = dynamic_cast<Circle*>(shape)) {
		sf::Vector2i dist = other->pos - this->pos;
		return sqrt(dist.x*dist.x + dist.y*dist.y) < this->radius + other->radius;
	} else {
		return false;
	}
}
