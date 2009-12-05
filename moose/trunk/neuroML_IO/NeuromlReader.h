/*******************************************************************
 * File:            NeuromlReader.h
 * Description:      
 * Author:          Siji P George
 * E-mail:          siji.suresh@gmail.com
 ********************************************************************/
/**********************************************************************
** This program is part of 'MOOSE', the
** Messaging Object Oriented Simulation Environment,
** also known as GENESIS 3 base code.
**           copyright (C) 2003-2007 Upinder S. Bhalla. and NCBS
** It is made available under the terms of the
** GNU Lesser General Public License version 2.1
** See the file COPYING.LIB for the full notice.
**********************************************************************/
#ifndef _NEUROMLREADER_H
#define _NEUROMLREADER_H
#include <libneuroml/NCell.h>
#include <libneuroml/NBase.h>
class NeuromlReader
{
	public:
		NeuromlReader() {;}
		~NeuromlReader() {;}
		void  readModel(std::string filename,Id location);
		void pushtoVector(vector< double >&result,string expr_form,double r,double s,double m);
		double calcSurfaceArea(double length,double diameter);
		double calcVolume(double length,double diameter);
		void setupSynChannels(map< string,vector<string> > &,map< string,vector< string > > &);
		void setupChannels(map< string,vector<string> > &,map< string,vector< string > > &);
		void setupPools(map< string,vector<string> > &,map< string,vector< string > > &);
		
	private:
		Element* compt_;
 		Element* channel_;
		Element* gate_;
		Element* cable_;
		Element* synchannel_;
		Element* leak_;
		Element* ionPool_;
		static const double PI ;
		map< string,Id > segMap_;
		NCell* ncl_;
			
		
		
		

};
extern const Cinfo* initCompartmentCinfo();
extern const Cinfo* initHHChannelCinfo();
extern const Cinfo* initHHGateCinfo();
extern const Cinfo* initInterpolCinfo();
extern const Cinfo* initLeakageCinfo();
extern const Cinfo* initSynChanCinfo();
extern const Cinfo* initCaConcCinfo();
#endif // _NEUROMLREADER_H

