#ifndef SHAKE_H
#define SHAKE_H

#include <vector>
#include "sponge.h"

class Shake128 : private Sponge {
	public:
		Shake128();

		std::vector<bool> xof(const std::vector<bool>& m, int len);
};

#endif /* SHAKE_H */
