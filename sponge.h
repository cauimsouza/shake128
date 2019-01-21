#ifndef SPONGE_H
#define SPONGE_H

class Sponge {
	private:
		/* permutation function */
		void (*const f)(std::vector<bool>&);
		/* padding rule */
		void (*const pad)(std::vector<bool>&, int);
		/* width of the permutation */
		const int b;
	protected:
		/* rate */
		int r;
	public:
		std::vector<bool> xof(const std::vector<bool>& m, int len) const;

		Sponge(void (*f)(std::vector<bool>&),
			   void (*pad)(std::vector<bool>&, int),
			   int b, int r);
};

#endif /* SPONGE_H */
