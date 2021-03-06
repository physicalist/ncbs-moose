/**********************************************************************
** This program is part of 'MOOSE', the
** Messaging Object Oriented Simulation Environment,
** also known as GENESIS 3 base code.
**           copyright (C) 2003-2005 Upinder S. Bhalla. and NCBS
** It is made available under the terms of the
** GNU Lesser Public License version 2.1
** See the file COPYING.LIB for the full notice.
**********************************************************************/
#ifndef _ELEMENT_H
#define _ELEMENT_H

#include "header.h"

// Used in copying
// Defined in MsgFuncs.cpp
extern void duplicateMessagesOnTree(
	map<const Element*, Element*>& tree);

// Abstract base class for all Elements.
class Element {
	friend Element* lookupChildIn(const Conn *);
	// friend class Neutral;
	public:
		// Core API for Element
		Element(const string& name)
			: name_(name)
		{ ; }

		virtual ~Element() {
			;
		}

		const string& name() const {
			return name_;
		}

		////////////////////////////////////////////////////////////
		// Field and class stuff.
		////////////////////////////////////////////////////////////

		// New field-modifying relays are inserted near the front so
		// that searches for field matches are faster. 
		// Solvers are first.
		// virtual void AddRelay(const relay& r);

		static void setName( Conn* c, string name ) {
			static_cast< Element* >( c->parent() )->name_ = name;
		}

		static string getName( const Element* e ) {
			return e->name_;
		}

		////////////////////////////////////////////////////////////
		// Hierarchy and traversal stuff
		////////////////////////////////////////////////////////////

		static Element* root() {
			return root_;
		}
		static Element* classes() {
			return classes_;
		}

		virtual Element* parent() const;

		const string path() const;

		virtual Element* relativeFind( const string& n );

		static Element* find( const string& absolutePath ) {
			return root_->relativeFind( absolutePath );
		}

		// Adds child to child list. Done by Neutral, possibly we
		// may want to specialize somewhere, so it is a virtual.
		virtual bool adoptChild(Element *child) = 0;


		////////////////////////////////////////////////////////////
		// Stuff to do with relays
		////////////////////////////////////////////////////////////

		Field field( const string& name );

		void appendRelay( Finfo* f ) {
			relays_.push_back( f );
		}

		void dropRelay( Finfo* f ) {
			relays_.erase( std::find( relays_.begin(), relays_.end(), f ) );
		}

		Finfo* findVacantValueRelay( Finfo* valueFinfo, bool isSending);

		// looks for a Finfo that either uses (Dest) or calls (Src)
		// the specified RecvFunc. Used to traverse messages.
		Field lookupSrcField( Conn*, RecvFunc func );
		Field lookupDestField( Conn*, RecvFunc func );

///////////////////////////////////////////////////////////////////////
// These boilerplate functions are used to access the object.
///////////////////////////////////////////////////////////////////////
		virtual const Cinfo* cinfo() const {
			return &cinfo_;
		}

		static Element* create(
			const string& n, Element* pa, const Element* proto) {
			return 0;
		}

		virtual RecvFunc getDeleteFunc() {
			return deleteFunc;
		}

		// Seems paradoxical, but when the message from parent to 
		// child is triggered, it means that the child should delete
		// itself.
		static void deleteFunc( Conn* c ) {
			delete c->parent();
		}

		static Conn* getChildIn( Element* e ) {
			return &( e->childIn_ );
		}

		void listFields( vector< Finfo* >& );

///////////////////////////////////////////////////////////////////////
// Wildcarding Functions 
///////////////////////////////////////////////////////////////////////
		// These are defined in Wildcard.cpp
		static int startFind( const string& n, vector< Element* >& ret);
		Element* wildcardName(const string& n ) ;
        virtual int wildcardRelativeFind(
			const string& n, vector< Element* >& ret, int doublehash) ;
		static int wildcardFind(
			const string& path, vector< Element* >& ret );
		Element* wildcardFieldComparison( const string& line );

///////////////////////////////////////////////////////////////////////
// Functions involved in different kinds of copying
///////////////////////////////////////////////////////////////////////
		virtual Element* shallowCopy( Element* parent ) const;

		// Used to do any special operations, such as rebuild an
		// inner representation of other elements, after a copy.
		// Called on the copied object. Used for solvers.
		virtual void handlePostCopy( 	)
		{
			;
		}

		// This is the regular copy. Copies an entire element tree
		// including all messages within the tree. Returns the
		// copied Element
		Element* deepCopy( Element* pa ) const;

		// Checks if pa is anywhere in the ancestry of current element
		bool descendsFrom( const Element* pa ) const;

		// The final part of the copy routine.
		// This refers to each object class to traverse children.
		// Returns copied object
		virtual Element* internalDeepCopy( Element* pa, 
			map< const Element*, Element* >& tree ) const;
	protected:
		static string* nameValFunc(Element* e) {
			return &(e->name_);
		}

		// This does the actual work of the copying, using the tree
		// to handle the message copying
		// Returns copied object
		Element* deepTreeCopy( Element* pa, 
			map< const Element*, Element* >& tree ) const;


	private:
		string name_;
	//	UniConn< Element, Element::childIn_ > childIn_;
	//	UniConn< Element, childIn_ > childIn_;
	//	static Element* lookupChildIn(const Conn *);
		UniConn< lookupChildIn > childIn_;
		vector< Finfo* > relays_;
		static Element* root_;
		static Element* classes_;
		static Element* initializeRoot();
		static Finfo* fieldArray_[];
		static const Cinfo cinfo_;
};

#endif	// _ELEMENT_H
