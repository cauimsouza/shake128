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

vector<bool> Sponge::xof(const vector<bool>& m, int len) const {
	auto slices = split(this->pad(m), this->r);
	vector<bool> state(this->b, 0);

	for (auto slice : slices) {
		transform(slice.begin(), slice.end(), state.begin(), state.begin(), bit_xor<bool>());
		state = this->f(state);
	}

	vector<bool> output;
	while(output.size() < len) {
		output.insert(output.end(), state.begin(), state.begin() + this->r);
		state = this->f(state);
	}
	
	output.resize(len);
	return output;
}


Sponge::Sponge(std::vector<bool> (*f)(const std::vector<bool>&),
			   std::vector<bool> (*pad)(const std::vector<bool>&),
			   int b, int r) : f(f), pad(pad), b(b), r(r) { }

int main() {
	vector<bool> vet{1, 0, 1, 1};
	vector<bool> vet1{1, 0, 1, 1};
	vector<bool> vet2{0, 1, 1, 0, 0, 0, 1, 1};
	vector<vector<bool>> res = split(vet, 2);

	for (vector<bool> v : res) {
		for (int i = 0; i < 2; i++)
			printf("%d ", v[i] == true);
		printf("\n");
	}

	transform(vet1.begin(), vet1.end(), vet2.begin(), vet2.begin(), bit_xor<bool>());
	for (int i : vet2)
		printf("%d ", i);

	return 0;
}
