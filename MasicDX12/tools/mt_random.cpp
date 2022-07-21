#include "mt_random.h"

MTRandom::MTRandom() {
	m_engine.seed(m_seed);
}

unsigned int MTRandom::Random(unsigned int n) {
	if (n == 0) { return 0; }
	return m_int_distribution(m_engine, IntDistribution::param_type{ 0, n - 1 });
}

float MTRandom::Random() {
	return m_float_distribution(m_engine);
}

void MTRandom::SetRandomSeed(unsigned int n) {
	m_engine.seed(n);
	m_seed = n;
}

unsigned int MTRandom::GetRandomSeed() {
	return m_seed;
}

void MTRandom::Randomize() {
	SetRandomSeed(m_random_device());
}