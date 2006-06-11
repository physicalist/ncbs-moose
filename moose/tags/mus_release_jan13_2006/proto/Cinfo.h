#ifndef _Cinfo_h
#define _Cinfo_h
/************************************************************************ This program is part of 'MOOSE', the** Messaging Object Oriented Simulation Environment,** also known as GENESIS 3 base code.**           copyright (C) 2003 Upinder S. Bhalla. and NCBS** It is made available under the terms of the** GNU Lesser Public License version 2.1** See the file COPYING.LIB for the full notice.**********************************************************************/class Cinfo {
		Cinfo(const string& name,
	friend class CinfoWrapper;
	public:
		Cinfo()
		{
		}
				const string& author,
				const string& description,
				const string& baseName,
				Finfo** fieldArray,
				const unsigned long nFields,
				Element* (*createWrapper)(const string&, Element*,
					const Element*)
			);
		~Cinfo() {
				;
		}
		const string& name() const {
				return name_;
		}
		static const Cinfo* find(const string& name);
		Element* create(const string& name,
			Element* parent, const Element* proto = 0) const;
		Field field( const string& name ) const;
		void listFields( vector< Finfo* >& ) const;
		const Finfo* findMsg( const Conn* conn, RecvFunc func ) const;
		Finfo* findRemoteMsg( Conn* c, RecvFunc func ) const;
		static void initialize();

	private:
		const string name_;
		const string author_;
		const string description_;
		const string baseName_;
		const vector < string > fields_;
		static map<string, Cinfo*>& lookup() {
			static map<string, Cinfo*> lookup_;
			return lookup_;
		}
		const string name_;
		string author_;
		string description_;
		string baseName_;
		Finfo** fieldArray_;
		const unsigned long nFields_;
		Element* (*createWrapper_)(
			const string& name, Element* parent, const Element* proto);
		const Cinfo* base_;
};
#endif // _Cinfo_h
