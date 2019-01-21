#include <cstdlib>
#include <cstdio>
#include <cassert>
#include <iostream>
#include <string>
#include "shake.h"

using namespace std;

#define RATE 1000

vector<bool> get_input() {
	vector<bool> vet;
	unsigned char c;
	while (!feof(stdin)) {
		c = getchar();
		for (int i = 7; i >= 0; i--)
			vet.push_back(c & (1 << i));
	}

	return vet;
}

void print_binary_as_ascii(vector<bool>& output) {
	for (int i = 0; i < output.size(); i += 8) {
		unsigned char c = 0;
		for (int j = 7; j >= 0; j--)
			if (output[i + j]) c |= (1 << j);
		printf("%02x ", c);
	}

	printf("\n");
}

int main(int argc, char *argv[]) {
	if (argc < 2) {
		printf("Usage: %s NBYTES\n", argv[0]);
		return 1;
	}

	int nbits = 8 * atoi(argv[1]);

	freopen(nullptr, "rb", stdin);

	Shake128 shake(RATE);

	auto message = get_input();
	auto digest = shake.xof(message, nbits);

	assert(digest.size() == nbits);

	print_binary_as_ascii(digest);

	return 0;
}
