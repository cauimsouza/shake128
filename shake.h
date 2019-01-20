#ifndef SHAKE_H
#define SHAKE_H

#include <vector>
#include "sponge.h"

class Shake128 : public Sponge {
	public:
		Shake128(int r);

		std::vector<bool> xof(const std::vector<bool>& m, int len) const;
};

#endif /* SHAKE_H */
