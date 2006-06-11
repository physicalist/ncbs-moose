#ifndef _Molecule_h
#define _Molecule_h
class Molecule
{
	friend class MoleculeWrapper;
	public:
		Molecule()
		{
			nInit_ = 0.0;
			volumeScale_ = 1.0;
			n_ = 0.0;
			mode_ = 0;
		}

	private:
		double nInit_;
		double volumeScale_;
		double n_;
		int mode_;
		double total_;
		double A_;
		double B_;
		static const double EPSILON;
};
#endif // _Molecule_h
