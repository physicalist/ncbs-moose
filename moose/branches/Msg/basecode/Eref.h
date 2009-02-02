class Eref
{
	public:
		Eref( Element* e, unsigned int index );

		/**
		 * returns the sum of all valid incoming entries
		 */
		double sumBuf( Slot slot );

		/**
		 * Returns the product of all valid incoming entries
		 * with v. If there are no entries, returns v
		 */
		double prdBuf( Slot slot, double v );

		/**
		 * Returns the single specified entry
		 */
		double oneBuf( Slot slot );

		/**
		 * Returns the memory location specified by slot.
		 * Used for sends.
		 */
		double* getBufPtr( Slot slot );
	private:
		Element* e_;
		unsigned int i_;
};
