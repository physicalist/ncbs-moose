/**********************************************************************
** This program is part of 'MOOSE', the
** Messaging Object Oriented Simulation Environment,
** also known as GENESIS 3 base code.
**           copyright (C) 2003-2006 Upinder S. Bhalla. and NCBS
** It is made available under the terms of the
** GNU Lesser General Public License version 2.1
** See the file COPYING.LIB for the full notice.
**********************************************************************/

#ifndef _SIMPLE_ELEMENT_H
#define _SIMPLE_ELEMENT_H

/**
 * The SimpleElement class implements Element functionality in the
 * most common vanilla way. It manages a set of vectors and pointers
 * that keep track of messaging and field information.
 */
class SimpleElement: public Element
{
	public:
#ifdef DO_UNIT_TESTS
			friend void cinfoTest(); // Need to look at src_ and dest_
			friend void msgSrcTest(); // Need to look at src_ and dest_
			friend void msgFinfoTest(); // Need to look at src_ 
			friend void finfoLookupTest(); // to do these tests
			static int numInstances;
#endif
		SimpleElement(
				Id id,
				const std::string& name, 
				void* data = 0
		);

		/**
		 * Copies over the name of the SimpleElement, and assigns a
		 * scratch id.
		 * Does not copy the data or the Finfos.
		 * Those are done in the 'innerCopy'
		 * The messages are still more complicated and are done
		 * in other stages of the Copy command.
		 */
		SimpleElement( const SimpleElement* orig );

		/// This cleans up the data_ and finfo_ if needed.
		~SimpleElement();

		const std::string& name( ) const {
				return name_;
		}

		void setName( const std::string& name ) {
				name_ = name;
		}

		const std::string& className( ) const;

		const Cinfo* cinfo() const;


		/////////////////////////////////////////////////////////////
		// Msg handling functions
		/////////////////////////////////////////////////////////////

		/**
		 * Returns a pointer to the specified msg.
		 */
		const Msg* msg( unsigned int msgNum ) const;
		Msg* varMsg( unsigned int msgNum );

		void checkMsgAlloc( unsigned int num );

		/////////////////////////////////////////////////////////////
		// Information functions
		/////////////////////////////////////////////////////////////
		
		/// Computes the memory use by the Element, data and its messages.
		unsigned int getTotalMem() const;

		/// Computes the memory use by the messages.
		unsigned int getMsgMem() const;

		/**
		 * Reports if this element is going to be deleted.
		 */
		bool isMarkedForDeletion() const;

		/**
		 * Reports if this element is Global, i.e., should not be copied
		 */
		bool isGlobal() const;

		/**
		 * Puts the death mark on this element.
		 */
		void prepareForDeletion( bool stage );


		/**
		 * Returns data contents of SimpleElement
		 */
		void* data( unsigned int eIndex ) const {
			return data_;
		}

		/**
		 * Returns size of data array. For SimpleElement it is always 1.
		 */
		unsigned int numEntries() const {
			return 1;
		}

		/////////////////////////////////////////////////////////////
		// Finfo functions
		/////////////////////////////////////////////////////////////


		/**
		 * Regular lookup for Finfo from its name.
		 */
		const Finfo* findFinfo( const string& name );

		/**
		 * Special const lookup for Finfo from its name, where the returned
		 * Finfo is limited to the ones already defined in the class
		 * and cannot be an array or other dynamic finfo
		 */
		const Finfo* constFindFinfo( const string& name ) const;

		/**
		 * Returns finfo ptr associated with specified ConnTainer.
		 */
		const Finfo* findFinfo( const ConnTainer* c ) const;

		/**
		 * Local finfo_ lookup.
		 */
		const Finfo* localFinfo( unsigned int index ) const;

		/**
		 * Finds all the Finfos associated with this Element,
		 * starting from the local ones and then going to the 
		 * core class ones.
		 * Returns number of Finfos found.
		 */
		unsigned int listFinfos( vector< const Finfo* >& flist ) const;

		/**
		 * Finds the local Finfos associated with this Element.
		 * Note that these are variable. Typically they are Dynamic
		 * Finfos.
		 * Returns number of Finfos found.
		 */
		unsigned int listLocalFinfos( vector< Finfo* >& flist );

		void addExtFinfo( Finfo* f );
		void addFinfo( Finfo* f );
		bool dropFinfo( const Finfo* f );
		void setThisFinfo( Finfo* f );
		const Finfo* getThisFinfo( ) const;

		///////////////////////////////////////////////////////////////
		// Functions for the copy operation. All 5 are virtual
		///////////////////////////////////////////////////////////////
		Element* copy( Element* parent, const string& newName ) const;
		Element* copyIntoArray( Element* parent, const string& newName, int n ) const;
		bool isDescendant( const Element* ancestor ) const;

		Element* innerDeepCopy(
						map< const Element*, Element* >& tree ) const;
		Element* innerDeepCopy(
						map< const Element*, Element* >& tree, int n ) const;
		
		/*
		void replaceCopyPointers(
					map< const Element*, Element* >& tree,
					vector< pair< Element*, unsigned int > >& delConns );
		void copyMsg( map< const Element*, Element* >& tree );
		*/

		/**
 		* Copies messages from current element to duplicate provided dest is
 		* also on tree.
 		*/
		void copyMessages( Element* dup, 
			map< const Element*, Element* >& origDup ) const;

		///////////////////////////////////////////////////////////////
		// Debugging function
		///////////////////////////////////////////////////////////////
		void dumpMsgInfo() const;

		bool innerCopyMsg( const Conn* c, const Element* orig, Element* dup );
	protected:
		Element* innerCopy() const;
		Element* innerCopy(int n) const;

	private:
		string name_;

		vector< Finfo* > finfo_;

		void* data_;
		/**
		 * The Msg manages messages . The Msg are
		 * pre-allocated and at least the initial set are hard-coded
		 * to refer to specific message groups.
		 */
		vector< Msg > msg_;
};

#endif // _SIMPLE_ELEMENT_H
