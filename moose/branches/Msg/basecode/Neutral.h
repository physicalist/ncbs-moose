/**********************************************************************
** This program is part of 'MOOSE', the
** Messaging Object Oriented Simulation Environment.
**           Copyright (C) 2003-2009 Upinder S. Bhalla. and NCBS
** It is made available under the terms of the
** GNU Lesser General Public License version 2.1
** See the file COPYING.LIB for the full notice.
**********************************************************************/

class Neutral: public Data
{
	public:
		Neutral();
		void process( const ProcInfo* p, Eref e );
		void setName( const string& name );
		const string& getName() const;
		static const Cinfo* initCinfo();
		void destroy();

	private:
		string name_;
};
