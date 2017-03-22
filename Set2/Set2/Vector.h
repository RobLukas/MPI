#pragma once
#include <iostream>
#include <vector>

class Vector
{
public:
	Vector();
	Vector(float x, float y, float z);
	~Vector();
	float getX();
	float getY();
	float getZ();
	float getLength();


private:
	float x, y, z;

};

Vector::Vector()
{
	x = 0;
	y = 0;
	z = 0;
}

Vector::Vector(float x, float y, float z)
{
	this->x = x;
	this->y = y;
	this->z = z;
}

Vector::~Vector()
{
}

float Vector::getX() {
	return this->x;
}

float Vector::getY() {
	return this->y;
}

float Vector::getZ() {
	return this->z;
}

float Vector::getLength() {
	return sqrt(pow(x, 2) + pow(y, 2) + pow(z, 2));
}