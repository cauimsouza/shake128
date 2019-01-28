#include <vector>
#include <cstdint>
#include <algorithm>
#include <list>
#include <cassert>
#include <cstdio>
#include <cstdlib>
#include <iostream>
#include "sponge.h"
#include "shake.h"

using namespace std;

#define B 1600
#define W 64  // w = b / 25
#define L 6 // l = log_2(w)
#define CAPACITY 256

bool A[5][5][W];
bool AP[5][5][W];
bool C[5][W];
bool D[5][W];

static int mod(int a, int b) {
	int ans = a % b;
	if (a > 0) return ans;
	return (ans + b) % b;
}

static void to_state(vector<bool> &s) {
	for (int x = 0; x < 5; x++)
		for (int y = 0; y < 5; y++)
			for (int z = 0; z < W; z++)
				A[x][y][z] = s[W * (5 * y + x) + z];
}

static void from_state(vector<bool> &s) {
	int i = 0;
	for (int y = 0; y < 5; y++)
		for (int x = 0; x < 5; x++)
			for (int z = 0; z < W; z++)
				s[i++] = A[x][y][z];
}

void print() {
	for (int y = 0; y < 5; y++) {
		for (int x = 0; x < 5; x++) {
			for (int z = W - 8; z >= 0; z -= 8) {
					unsigned char byte = 0;
					for (int j = 0; j < 8; j++) {
						if (A[x][y][z + j])	byte |= (1 << (j));
					}
					printf("%02x", byte);
			}
			printf(" ");
		}
		printf("\n");
	}
}

static void copy_AP_to_A() {
	for (int x = 0; x < 5; x++)
		for (int y = 0; y < 5; y++)
			for (int z = 0; z < W; z++)
				A[x][y][z] = AP[x][y][z];
}

static void theta() {
	for (int x = 0; x < 5; x++)
		for (int z = 0; z < W; z++)
			C[x][z] = A[x][0][z] ^ A[x][1][z] ^ A[x][2][z] ^ A[x][3][z] ^ A[x][4][z];

	for (int x = 0; x < 5; x++)
		for (int z = 0; z < W; z++) {
			D[x][z] = C[mod(x - 1 + 5, 5)][z] ^ C[mod(x + 1, 5)][mod(z - 1 + W, W)];
		}

	for (int x = 0; x < 5; x++)
		for (int y = 0; y < 5; y++)
			for (int z = 0; z < W; z++)
				A[x][y][z] = A[x][y][z] ^ D[x][z];

	// printf("After theta:\n"); print();printf("\n");
}

static void rho() {
	for (int z = 0; z < W; z++)
		AP[0][0][z] = A[0][0][z];

	int x = 1, y = 0;
	for (int t = 0; t < 24; t++) {
		for (int z = 0; z < W; z++) {
			AP[x][y][z] = A[x][y][mod(z - ((t + 1) * (t + 2)) / 2 + W, W)];
		}
		int yp = mod(2 * x + 3 * y, 5);
		x = y;
		y = yp;
	}

	copy_AP_to_A();

	// printf("After rho:\n"); print(); printf("\n");
}

static void pi() {
	for (int x = 0; x < 5; x++)
		for (int y = 0; y < 5; y++)
			for (int z = 0; z < W; z++)
				AP[x][y][z] = A[mod(x + 3 * y, 5)][x][z];

	copy_AP_to_A();

	// printf("After pi:\n"); print(); printf("\n");
}

static void chi() {
	for (int x = 0; x < 5; x++)
		for (int y = 0; y < 5; y++)
			for (int z = 0; z < W; z++)
				AP[x][y][z] = A[x][y][z] ^ ((A[mod(x + 1, 5)][y][z] ^ 1) & A[mod(x + 2, 5)][y][z]);

	copy_AP_to_A();

	// printf("After chi:\n"); print(); printf("\n");
} 

static bool rc(int ir) {
	int loops = mod(ir, 255);
	if (loops == 0) return 1;

	vector<bool> r = {1, 0, 0, 0, 0, 0, 0, 0};
	for (int i = 0; i < loops; i++) {
		r.insert(r.begin(), 0);
		r[0] = r[0] ^ r[8];
		r[4] = r[4] ^ r[8];
		r[5] = r[5] ^ r[8];
		r[6] = r[6] ^ r[8];
		r.resize(8);
	}

	return r[0];
}

static void iota(int ir) {
	vector<bool> r(W, false);
	for (int j = 0; j <= L; j++)
		r[(1 << j) - 1] = rc(j + 7 * ir);

	for (int z = 0; z < W; z++) {
		A[0][0][z] ^= r[z];
	}

	// printf("After iota:\n"); print(); printf("\n");
}

static void rnd(int ir) {
	theta();
	rho();
	pi();
	chi();
	iota(ir);
}

static void print_binary_as_ascii(vector<bool>& output) {
	for (int i = 0; i < output.size(); i += 8) {
		uint64_t c = 0;
		for (int j = 0; j < 8; j++)
			if (output[i + j]) c |= (1 << j);
		printf("%02X ", c);
	}

	printf("\n");
}

static void keccak_p(vector<bool>& s, int nr) {
	to_state(s);
	for (int ir = 12 + 2 * L - nr; ir <= 12 + 2 * L - 1; ir++)
		rnd(ir);
	from_state(s);
}

static void keccak_f(vector<bool>& s) {
	return keccak_p(s, 12 + 2 * L);
}

static void multi_rate_pad(vector<bool>& s, int r) {
	int m = s.size();
	int j = mod(-m - 2, r);

	assert(j >= 0);
	assert(j + 2 < 2 * r);
	assert(mod(j + 2 + m, r) == 0);

	vector<bool> pad(j + 2, 0);
	pad[0] = pad[j + 1] = 1;

	assert(pad.size() == j + 2);

	s.insert(s.end(), pad.begin(), pad.end());

	assert(s.size() == j + 2 + m);
}

Shake128::Shake128() : Sponge(keccak_f, multi_rate_pad, B, B - CAPACITY) {}

vector<bool> Shake128::xof(const vector<bool>& m, int len) {
	vector<bool> suffix = {1, 1, 1, 1};
	// suffix.insert(suffix.begin(), m.begin(), m.end());
	return Sponge::xof(suffix, len);
}

static void test_mod() {
	assert(mod(5, 3) == 2);
	assert(mod(3, 3) == 0);
	assert(mod(1, 3) == 1);
	assert(mod(7, 3) == 1);
	assert(mod(8, 3) == 2);
	assert(mod(0, 3) == 0);
	assert(mod(-1, 3) == 2);
	assert(mod(-6, 3) == 0);
	assert(mod(-8, 3) == 1);
	assert(mod(-1, 5) == 4);
	assert(mod(6, 5) == 1);
	assert(mod(5, 5) == 0);
	assert(mod(0, 5) == 0);
}

// int main() {
// 	test_mod();
// 	vector<bool> s(B, false);
// 	keccak_f(s);
// 	keccak_f(s);
// 	print_binary_as_ascii(s);
//
// 	return 0;
// }
