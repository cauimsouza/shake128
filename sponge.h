#ifndef SPONGE_H
#define SPONGE_H

class Sponge {
	private:
		/* permutation function */
		std::vector<bool> (*const f)(const std::vector<bool>&);
		/* padding rule */
		std::vector<bool> (*const pad)(const std::vector<bool>&);
		/* width of the permutation */
		const int b;
		/* rate */
		const int r;
	public:
		std::vector<bool> xof(const std::vector<bool>& m, int len) const;

		Sponge(std::vector<bool> (*f)(const std::vector<bool>&),
			   std::vector<bool> (*pad)(const std::vector<bool>&),
			   int b, int r);
};

#endif /* SPONGE_H */
