#pragma once

#include <Windows.h>

class Point {
public:
	long x;
	long y;

	Point() { x = y = 0; }
	Point(const long newX, const long newY) { x = newX; y = newY; }
	Point(const Point& newPoint) { x = newPoint.x; y = newPoint.y; }
	Point(const Point* pNewPoint) { x = pNewPoint->x; y = pNewPoint->y; }
	Point(const POINT& newPoint) { x = newPoint.x; y = newPoint.y; }

	Point& operator=(const Point& newPoint) { x = newPoint.x; y = newPoint.y; return (*this); }
	Point& operator=(const Point* pNewPoint) { x = pNewPoint->x; y = pNewPoint->y; return (*this); }

	Point& operator+=(const Point& newPoint) { x += newPoint.x; y += newPoint.y; return (*this); }
	Point& operator-=(const Point& newPoint) { x -= newPoint.x; y -= newPoint.y; return (*this); }
	Point& operator+=(const Point* pNewPoint) { x += pNewPoint->x; y += pNewPoint->y; return (*this); }
	Point& operator-=(const Point* pNewPoint) { x -= pNewPoint->x; y -= pNewPoint->y; return (*this); }
	Point operator+(const Point& other) { Point temp(this); temp += other; return temp; }
	Point operator-(const Point& other) { Point temp(this); temp -= other; return temp; }

	bool operator==(const Point& other) const { return ((x == other.x) && (y == other.y)); }
	bool operator!=(const Point& other) const { return (!((x == other.x) && (y == other.y))); }

	long GetX() const { return x; }
	long GetY() const { return y; }
	void SetX(const long newX) { x = newX; }
	void SetY(const long newY) { y = newY; }
	void Set(const long newX, const long newY) { x = newX; y = newY; }
};

inline Point operator-(const Point& left, const Point& right) { Point temp(left); temp -= right; return temp; }