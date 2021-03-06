/**********************************************************************
** This program is part of 'MOOSE', the
** Messaging Object Oriented Simulation Environment.
**           Copyright (C) 2003-2009 Upinder S. Bhalla. and NCBS
** It is made available under the terms of the
** GNU Lesser General Public License version 2.1
** See the file COPYING.LIB for the full notice.
**********************************************************************/

#ifndef _SINGLE_MSG_H
#define _SINGLE_MSG_H


class SingleMsg: public Msg
{
	friend void Msg::initMsgManagers(); // for initializing Id.
	public:
		SingleMsg( MsgId mid, Eref e1, Eref e2 );
		~SingleMsg();

		void exec( const char* arg, const ProcInfo* p) const;

		// bool isMsgHere( const Qinfo& q ) const;

		DataId i1() const;
		DataId i2() const;

		// returns the id of the managing Element.
		Id managerId() const;

		FullId findOtherEnd( FullId end ) const;

		Msg* copy( Id origSrc, Id newSrc, Id newTgt,
			FuncId fid, unsigned int b, unsigned int n ) const;
		
		void addToQ( const Element* src, Qinfo& q, 
			const ProcInfo* p, MsgFuncBinding i, const char* arg ) const;

		void setI1( DataId di );
		DataId getI1() const;

		void setI2( DataId di );
		DataId getI2() const;

		static const Cinfo* initCinfo();
	private:
		DataId i1_;
		DataId i2_;
		static Id managerId_;
};

#endif // _SINGLE_MSG_H
