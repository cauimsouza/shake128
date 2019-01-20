#include <cstdio>
#include <vector>
#include <functional>
#include <algorithm>
#include "sponge.h"

using namespace std;

static vector<vector<bool>> split(const vector<bool> m, int r) {
	vector<vector<bool>> slices;
	for (int i = 0; i < m.size(); i += r)
		slices.push_back(vector<bool>(m.begin() + i, m.begin() + i + r));
	return slices;
}

vector<bool> Sponge::xof(const vector<bool>& s, int len) const {
	vector<bool> m(s);

	this->pad(m, this->r);

	auto slices = split(m, this->r);

	vector<bool> state(this->b, 0);

	for (auto slice : slices) {
		transform(slice.begin(), slice.end(), state.begin(), state.begin(), bit_xor<bool>());
		this->f(state);
	}

	vector<bool> output;
	while(output.size() < len) {
		output.insert(output.end(), state.begin(), state.begin() + this->r);
		this->f(state);
	}
	
	output.resize(len);
	return output;
}


Sponge::Sponge(void (*f)(std::vector<bool>&),
			   void (*pad)(std::vector<bool>&, int),
			   int b, int r) : f(f), pad(pad), b(b), r(r) { }
