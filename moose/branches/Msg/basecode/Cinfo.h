/**********************************************************************
** This program is part of 'MOOSE', the
** Messaging Object Oriented Simulation Environment,
** also known as GENESIS 3 base code.
**           copyright (C) 2003-2009 Upinder S. Bhalla. and NCBS
** It is made available under the terms of the
** GNU General Public License version 2
** See the file COPYING.LIB for the full notice.
**********************************************************************/
#ifndef _CINFO_H
#define _CINFO_H

class DinfoBase;

/**
 * Class to manage class information for all the other classes.
 */
class Cinfo
{
		public:
			/**
			 * The Cinfo intializer is used for static initialization
			 * of all the MOOSE Cinfos. Each MOOSE class must set up
			 * a function to build its Cinfo. This function must be
			 * called statically in the MOOSE class .cpp file. 
			 * Note how it takes the base *Cinfo as an argument. This
			 * lets us call the base Cinfo initializer when making
			 * each Cinfo class, thus ensuring the correct static
			 * initialization sequence, despite the somewhat loose
			 * semantics for this sequence in most C++ compilers.
			 */
			Cinfo( const std::string& name,
					const Cinfo* baseCinfo, // Base class
					Finfo** finfoArray,	// Field information array
					unsigned int nFinfos,
					DinfoBase* d,	// A handle to lots of utility functions for the Data class.
					struct SchedInfo* schedInfo = 0,
					unsigned int nSched = 0
			);

			~Cinfo();
//////////////////////////////////////////////////////////////////////////
			/**
			 * Initializes the Cinfo. Must be called exactly once for
			 * each Cinfo.
			 */
			void init( Finfo** finfoArray, unsigned int nFinfos );

			/**
			 * registerFinfo:
			 * Puts Finfo information into Cinfo, and updates fields on
			 * Finfo as necessary.
			 */
			void registerFinfo( Finfo* f );

			/**
			 * Registers the OpFunc, assigns it a FuncId and returns the
			 * FuncId.
			 */
			FuncId registerOpFunc( const OpFunc* f );

			/**
			 * Returns the next free value for BindIndex, and keeps track
			 * of the total number set up.
			 */
			BindIndex registerBindIndex();

			/**
			 * Handles any operations that must be done after an Element
			 * is created. Scans through all Finfos as they are the ones to
			 * manage such requests. Examples are to create FieldElements.
			 */
			void postCreationFunc( Id newId, Element* newElm ) const;

			/**
			 * Registers a finfo as needing post-creation work
			 */
			void registerPostCreationFinfo( const Finfo* f );

//////////////////////////////////////////////////////////////////////////

			const OpFunc* getOpFunc( FuncId fid ) const;
			// FuncId getOpFuncId( const string& funcName ) const;

			/**
			 * Return the name of the Cinfo
			 */
			const std::string& name() const;

			/**
			 * Finds the Cinfo with the specified name.
			 */
			static const Cinfo* find( const std::string& name );

			/**
			 * Finds Finfo by name in the list for this class, 
			 * ignoring any element-specific fields.
			 * Returns 0 on failure.
			 */
			const Finfo* findFinfo( const string& name) const;

			/**
			 * Finds the funcId by name. Returns 0 on failure.
			const FuncId findFuncId( const string& name) const;
			 */

			/**
			 * Number of SrcMsgs in total. Each has a unique number to
			 * bind to an entry in the msgBinding vector in the Element.
			 */
			unsigned int numBindIndex() const;

			/**
			 * Returns the map between name and field info
			 */
			const map< string, Finfo* >& finfoMap() const;

			/**
			 * Returns the Dinfo, which manages creation and destruction
			 * of the data, and knows about its size.
			 */
			const DinfoBase* dinfo() const;

			/**
			 * Returns true if the current Cinfo is derived from
			 * the ancestor
			 */
			bool isA( const string& ancestor ) const;

			/**
			 * Utility function for debugging
			 */
			void reportFids() const;

		private:
			const string name_;

			std::map< std::string, std::string > doc_;
			// const std::string author_;
			// const std::string description_;
			const Cinfo* baseCinfo_;
			const DinfoBase* dinfo_;

			BindIndex numBindIndex_;

			map< string, Finfo* > finfoMap_;

			/**
			 * These are special Finfos which have to be invoked
			 * after the Element is created, and their postCreationFuncs
			 * called. They are typically things like FieldElementFinfos.
			 */
			vector< const Finfo* > postCreationFinfos_;
			vector< const OpFunc* > funcs_;
//			map< string, FuncId > opFuncNames_;

			static map< string, Cinfo* >& cinfoMap();

			// Many opfuncs share same FuncId
			// static map< OpFunc*, FuncId >& funcMap();
};

#endif // _CINFO_H
