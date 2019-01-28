#include <cstdlib>
#include <cstdio>
#include <cstdint>
#include <cassert>
#include <unistd.h>
#include <iostream>
#include <string>
#include "shake.h"

using namespace std;

static vector<bool> get_input() {
	freopen(nullptr, "rb", stdin);

	vector<bool> vet;

	unsigned char c;
	while (read(STDIN_FILENO, &c, 1) > 0) {
		for (int i = 7; i >= 0; i--)
			vet.push_back(c & (1 << i));
	}

	return vet;
}

static void print_binary_as_ascii(vector<bool>& output) {
	for (int i = 0; i < output.size(); i += 8) {
		unsigned c = 0;
		for (int j = 0; j < 8; j++)
			if (output[i + j]) c |= (1 << j);
		printf("%02X ", c);
	}

	printf("\n");
}

int main(int argc, char *argv[]) {
	if (argc < 2) {
		printf("Usage: %s NBYTES\n", argv[0]);
		return 1;
	}

	int nbits = 8 * atoi(argv[1]);

	auto message = get_input();
	
	Shake128 shake;
	auto digest = shake.xof(message, nbits);

	assert(digest.size() == nbits);

	print_binary_as_ascii(digest);

	return 0;
}
