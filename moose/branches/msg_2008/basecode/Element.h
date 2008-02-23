/**********************************************************************
** This program is part of 'MOOSE', the
** Messaging Object Oriented Simulation Environment,
** also known as GENESIS 3 base code.
**           copyright (C) 2003-2006 Upinder S. Bhalla. and NCBS
** It is made available under the terms of the
** GNU Lesser General Public License version 2.1
** See the file COPYING.LIB for the full notice.
**********************************************************************/

#ifndef _ELEMENT_H
#define _ELEMENT_H

#include "header.h"
/**
 * The Element class handles all the MOOSE infrastructure: messages,
 * field information and class information. It manages the actual
 * data class through a generic char* pointer.
 * Here we start out with a generic base class Element, which will
 * be specialized as needed for arrays and other things.
 */

class Cinfo;
class Id;

class Element
{
	public:
		Element( Id id );
		Element( bool ignoreId );

		virtual ~Element();

		/// Returns the name of the element
		virtual const std::string& name( ) const = 0;

		/// Sets the name of the element.
		virtual void setName( const std::string& name ) = 0;

		/// Returns the name of the element class
		virtual const std::string& className( ) const = 0;

		/// Returns the class info of the element class.
		virtual const Cinfo* cinfo( ) const = 0;

		/**
		 * Returns the identified Msg
		 */
		virtual const Msg* msg( unsigned int msgNum ) const = 0;
		virtual Msg* varMsg( unsigned int msgNum ) = 0;

		/**
		 * Ensures that the requested msg is allocated
		 */
		virtual void checkMsgAlloc( unsigned int num ) = 0;

		///////////////////////////////////////////////////////////////
		// Information functions
		///////////////////////////////////////////////////////////////
		
		/// True if element is marked for deletion.
		virtual bool isMarkedForDeletion() const = 0;

		/// True if element is global, that is, should not be copied.
		virtual bool isGlobal() const = 0;

		/// Before actual delete, mark all victims for message cleanup.
		virtual void prepareForDeletion( bool stage ) = 0;

		/**
		 * Returns a pointer to the data stored on this Element.
		 */
		virtual void* data( unsigned int eIndex ) const = 0;

		/**
		 * Returns number of entries in the data. 1 for SimpleElement,
		 * any value for ArrayElement
		 */
		virtual unsigned int numEntries() const = 0;
		
		/** Returns a Finfo that matches the path given by 'name'.
		 * This can be a general path including field indirection
		 * and indices. If necessary the function will generate
		 * a dynamic Finfo to handle the request. For this reason
		 * it cannot be a const function of the Element.
		 */
		virtual const Finfo* findFinfo( const string& name ) = 0;

		/**
		 * Returns finfo ptr associated with specified ConnTainer.
		 */
		virtual const Finfo* findFinfo( const ConnTainer* c ) const = 0;

		/**
		 * Returns a Finfo as above, except that it cannot handle any
		 * dynamic Finfo thus limiting it to predefined finfos. Has the
		 * merit that it is a const function
		 */
		virtual const Finfo* constFindFinfo( const string& name ) const = 0;

		/**
		 * Returns finfo ptr on local vector of Finfos. 0 if we are out
		 * of range. Most of these Finfos should be DynamicFinfos.
		 */
		virtual const Finfo* localFinfo( unsigned int index )
				const = 0;

		/**
		 * Checks that specified finfo does in fact come from this
		 * Element. Used for paranoia checks in some functions,
		 * though perhaps this can later be phased out by using
		 * encapsulation of finfos and elements into a Field object.
		 */
		virtual unsigned int listFinfos(
			vector<	const Finfo* >& flist ) const = 0;

		/**
		 * Finds the local Finfos associated with this Element.
		 * Note that these are variable. Typically they are Dynamic
		 * Finfos.
		 * Returns number of Finfos found.
		 */
		virtual unsigned int listLocalFinfos( vector< Finfo* >& flist )
				= 0;
		
		/**
		 * For adding ExtFieldFinfo for addfield command
		 */
		
		virtual void addExtFinfo( Finfo* f ) = 0;
		
		/**
		 * Appends a new Finfo onto the Element. Typically this new
		 * Finfo is a Dynamic Finfo used to store messages that are
		 * not precompiled, and therefore need to be allocated on the
		 * fly. It is also used once at startup to insert the 
		 * main Finfo for the object, typically ThisFinfo, that holds
		 * the class information.
		 */
		virtual void addFinfo( Finfo* f ) = 0;

		/**
 		* This function cleans up the finfo f. It removes its messages,
 		* deletes it, and removes its entry from the finfo list. Returns
 		* true if the finfo was found and removed. It does NOT
 		* permit deleting the ThisFinfo at index 0.
 		*/
		virtual bool dropFinfo( const Finfo* f ) = 0;

		/**
		 * Assigns the zeroth finfo, which is the one that 
		 * holds all the common info for the object. Normally this
		 * is assigned at object creation time, but when an object
		 * is taken over (zombified) by a solver, its functions
		 * have to be overridden. At that time the 'ThisFinfo' is
		 * reassigned by this function.
		 */
		virtual void setThisFinfo( Finfo* f ) = 0;

		/**
		 * Accesses the zeroth finfo.
		 */
		virtual const Finfo* getThisFinfo() const = 0;


		/**
		 * Returns the root element. This function is declared
		 * in Neutral.cpp, because that is the data type of the 
		 * root Element.
		 */
		static Element* root();

		virtual Id id() const {
			return id_;
		}

		///////////////////////////////////////////////////////////////
		// Element Id management functions
		///////////////////////////////////////////////////////////////

		// static Element* element( unsigned int id );

		// Deprecated
		// static Element* lastElement();

		// static unsigned int numElements();

		// static unsigned int nextId();
		// static unsigned int lastId();

		///////////////////////////////////////////////////////////////
		// Functions for the copy operation
		///////////////////////////////////////////////////////////////
		/**
		 * This function does a deep copy of the current element 
		 * including all messages. Returns the base of the copied tree.
		 * It attaches the copied element tree to the parent.
		 */
		virtual Element* copy( Element* parent, const string& newName )
				const = 0;
				
		/**
		 * This function takes a prototype element and creates an array
		 * of elements. Only the data part gets duplicated and Finfo and
		 * Cinfo details, etc remain common. This will greatly increase
		 * the space efficiency and reduce the number of messages.
		 * \param parent the element to which the generated parent to be 
		   added to
		   \param newName the name of the copied element
		   \param n number of copies
		   \return An Element * pointing to a ArrayElement object
		*/
		virtual Element* copyIntoArray( Element* parent, const string& newName, 
			int n ) const = 0;
		/**
		 * True if current element descends from the specified ancestor.
		 */
		virtual bool isDescendant( const Element* ancestor ) const = 0;

		/**
		 * This function fills up the map with current element and
		 * all its descendants. Returns the root element of the
		 * copied tree. The first entry in the map is the original
		 * The second entry in the map is the copy.
		 * The function does NOT fix up the messages.
		 */
		virtual Element* innerDeepCopy( 
				map< const Element*, Element* >& tree )
				const = 0;
				
		virtual Element* innerDeepCopy( 
				map< const Element*, Element* >& tree, int n )
				const = 0;
#if 0
		/**
		 * This function replaces Element* pointers in the conn_ vector
		 * with corresponding ones from the copied tree.
		 * It fills the delConns vector with entries where the conns
		 * have to be deleted. These include conns
		 * to global objects; they will later be reconstructed using
		 * the CopyMsg function.
		 */
		virtual void replaceCopyPointers(
				map< const Element*, Element* >& tree,
				vector< pair < Element*, unsigned int > >& delConns ) = 0;

		/**
		 * This function takes all the messages between this
		 * element and the key (original) portion of the tree,
		 * and makes duplicates of the messages to go between the 
		 * current element and the data (copied) portion of the tree.
		 * Excludes child messages.
		 */
		virtual void copyMsg( map< const Element*, Element* >& tree ) = 0;
#endif

		/**
 		* Copies messages from current element to duplicate provided dest is
 		* also on tree.
 		*/
		virtual void copyMessages( Element* dup, 
			map< const Element*, Element* >& origDup ) const = 0;
		
		/**
		 * Returns the memory use of the Element and its messages, 
		 * excluding the data part.
		 */
		virtual unsigned int getMsgMem() const = 0;

		/**
		 * Debugging function: prints out all the messaging info
		 * for this element.
		 */
		virtual void dumpMsgInfo() const = 0;
		virtual bool innerCopyMsg(
				const Conn* c, const Element* orig, Element* dup ) = 0;
	protected:
		/**
		 * This function copies the element, its data and its
		 * dynamic Finfos. What it does not do is to replace
		 * any pointers to other elements in the Conn array.
		 * It does not do anything about the element hierarchy
		 * either, because that is also handled through messages,
		 * ie., the Conn array.
		 * The returned Element is dangling in memory: No parent
		 * or child.
		 */
		virtual Element* innerCopy() const = 0;
		virtual Element* innerCopy(int n) const = 0;


	private:
		// static vector< Element* >& elementList();
		// static vector< Element* > elementList;
		Id id_;
};

#endif // _ELEMENT_H
