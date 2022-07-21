#pragma once

#include <random>

class MTRandom {
private:
	unsigned int m_seed = 1;
	std::mt19937 m_engine;
	using IntDistribution = std::uniform_int_distribution<unsigned int>;
	using RealDistribution = std::uniform_real_distribution<float>;
	IntDistribution m_int_distribution;
	RealDistribution m_float_distribution = RealDistribution(0.0f, 1.0f);
	std::random_device m_random_device;

public:
	MTRandom();

	unsigned int Random(unsigned int n);
	float Random();
	void SetRandomSeed(unsigned int n);
	unsigned int GetRandomSeed();
	void Randomize();
};