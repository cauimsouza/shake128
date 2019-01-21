#include <vector>
#include <list>
#include <cassert>
#include "shake.h"
#include "sponge.h"

using namespace std;

#define B 1600
#define W 64  // m = b / 25
#define L 6 // l = log_2(w)

#define A(x, y, z) s[(W * (5 * (y) + (x)) + (z))]
#define AP(x, y, z) ap[(W * (5 * (y) + (x)) + (z))]
#define MOD(a, b) (((a) % (b)) + ((a) < 0 ? (b) : 0))

static vector<bool> c(5 * W);
static vector<bool> d(5 * W);
static vector<bool> ap(B);

static void theta(vector<bool>& s) {
#define C(x, z) c[(W * (x)) + (z)]
	for (int x = 0; x < 5; x++)
		for (int z = 0; z < W; z++)
			C(x, z) = A(x, 0, z) ^ A(x, 1, z) ^ A(x, 3, z) ^ A(x, 4, z);

#define D(x, z) d[(W * (x)) + (z)]
	for (int x = 0; x < 5; x++)
		for (int z = 0; z < W; z++)
			D(x, z) = C(MOD(x - 1, 5), z) ^ C(MOD(x + 1, 5), MOD(z - 1, W));

	for (int x = 0; x < 5; x++)
		for (int y = 0; y < 5; y++)
			for (int z = 0; z < W; z++)
				A(x, y, z) = A(x, y, z) ^ D(x, z);
}

static void rho(vector<bool>& s) {
	for (int z = 0; z < W; z++)
		AP(0, 0, z) = A(0, 0, z);

	int x = 1, y = 0;
	for (int t = 0; t < 24; t++) {
		for (int z = 0; z < W; z++)
			AP(x, y, z) = A(x, y, MOD(z - ((t + 1) * (t + 2)) / 2, W));
		int yp = MOD(2 * x + 3 * y, 5);
		x = y;
		y = yp;
	}

	s.assign(ap.begin(), ap.end());
}

static void pi(vector<bool>& s) {
	for (int x = 0; x < 5; x++)
		for (int y = 0; y < 5; y++)
			for (int z = 0; z < W; z++)
				AP(x, y, z) = A(MOD(x + 3 * y, 5), x, z);

	s.assign(ap.begin(), ap.end());
}

static void chi(vector<bool>& s) {
	for (int x = 0; x < 5; x++)
		for (int y = 0; y < 5; y++)
			for (int z = 0; z < W; z++)
				AP(x, y, z) = A(x, y, z) ^ ((A(MOD(x + 1, 5), y, z) ^ 1) & A(MOD(x + 2, 5), y, z));

	s.assign(ap.begin(), ap.end());
}

static bool rc(int t) {
	int niter = MOD(t, 255);
	if (!niter) return true;

#define R(i) *(r.begin() + (i))
	list<bool> r{1, 0, 0, 0, 0, 0, 0, 0};
	for (int i = 0; i < niter; i++) {
		r.push_front(0);

		auto it = r.begin();
		auto eit = r.rbegin();

		*it = (*it) ^ (*eit);

		it = next(it, 4);
		*it = (*it) ^ (*eit);

		it = next(it);
		*it = (*it) ^ (*eit);

		it = next(it);
		*it = (*it) ^ (*eit);

		r.pop_back();
	}
	return r.front();
}

static void iota(vector<bool>& s, int ir) {
	vector<bool> RC(W);

	for (int j = 0; j <= L; j++)
		RC[(1 << j) - 1] = rc(j + 7 * ir);

	for (int z = 0; z < W; z++)
		A(0, 0, z) = A(0, 0, z) ^ RC[z];
}

static void rnd(vector<bool>& s, int ir) {
	theta(s);
	rho(s);
	pi(s);
	chi(s);
	iota(s, ir);
}

static void keccak_p(vector<bool>& s, int nr) {
	for (int ir = 12 + 2 * L - nr; ir <= 12 + 2 * L - 1; ir++)
		rnd(s, ir);
}

static void keccak_f(vector<bool>& s) {
	return keccak_p(s, 12 + 2 * L);
}

static void multi_rate_pad(vector<bool>& s, int r) {
	int m = s.size();
	int j = MOD(-m - 2, r);

	vector<bool> pad(j + 2, 0);
	pad[0] = pad[j + 1] = 1;

	assert(pad.size() == j + 2);

	s.insert(s.end(), pad.begin(), pad.end());

	assert(s.size() == j + 2 + m);
}

Shake128::Shake128() : Sponge(keccak_f, multi_rate_pad, B, 0) {}

vector<bool> Shake128::xof(const vector<bool>& m, int len) {
	this->r = B - 2 * len;
	vector<bool> suffix = {1, 1, 1, 1};
	suffix.insert(suffix.begin(), m.begin(), m.end());
	return Sponge::xof(suffix, len);
}
