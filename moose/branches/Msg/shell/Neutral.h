/**********************************************************************
** This program is part of 'MOOSE', the
** Messaging Object Oriented Simulation Environment.
**           Copyright (C) 2003-2009 Upinder S. Bhalla. and NCBS
** It is made available under the terms of the
** GNU Lesser General Public License version 2.1
** See the file COPYING.LIB for the full notice.
**********************************************************************/

#ifndef _NEUTRAL_H
#define _NEUTRAL_H

class Neutral
{
	public:
		friend istream& operator >>( istream& s, Neutral& d );
		friend ostream& operator <<( ostream& s, const Neutral& d );
		Neutral();

		/////////////////////////////////////////////////////////////////
		// Field access functions
		/////////////////////////////////////////////////////////////////

		/**
		 * Field access functions for the entire object. For Neutrals 
		 * the setThis function is a dummy: it doesn't do anything because
		 * the Neutral has no data to set. However, the function name acts
		 * as a placeholder and derived objects can override the function
		 * so that the entire object can be accessed as a field and also
		 * for inter-node data transfer.
		 */
		void setThis( Neutral v );

		/**
		 * Field access functions for the entire object. For Neutrals 
		 * the getThis function does return the Neutral object, but it
		 * has no data to set. However, the function name acts
		 * as a placeholder and derived objects can override the function
		 * so that the entire object can be accessed as a field and also
		 * used for inter-node data transfer.
		 */
		Neutral getThis() const;

		/**
		 * Field access functions for the name of the Element/Neutral
		 */
		void setName( Eref e, const Qinfo* q, string name );
		string getName( Eref e, const Qinfo* q ) const;

		/**
		 * Simply returns own fullId
		 */
		FullId getFullId( Eref e, const Qinfo* q ) const;

		/**
		 * Looks up the full Id info for the parent of the current Element
		 */
		FullId getParent( Eref e, const Qinfo* q ) const;

		/**
		 * Looks up all the Element children of the current Element
		 */
		vector< Id > getChildren( Eref e, const Qinfo* q ) const;

		/**
		 * Builds a vector of all descendants of e
		 */
		unsigned int buildTree( Eref e, const Qinfo* q, 
			vector< Id >& tree ) const;

		/**
		 * Traverses to root, building path.
		 */
		string getPath( Eref e, const Qinfo* q ) const;

		/**
		 * Looks up the Class name of the current Element
		 */
		string getClass( Eref e, const Qinfo* q ) const;

		/**
		 * Destroys Element and all children
		 */
		void destroy( Eref e, const Qinfo* q, int stage );
		
		////////////////////////////////////////////////////////////

		/**
		 * Finds specific named child
		 */
		static Id child( const Eref& e, const string& name );

		/**
		 * Looks up all the Element children of the current Element
		 */
		static vector< Id > children( Eref e );

		static FullId parent( const Eref& e );

		/**
		 * Checks if 'me' is a descendant of 'ancestor'
		 */
		static bool isDescendant( Id me, Id ancestor );

		/**
		 * Standard initialization function, used whenever we want to
		 * look up the class Cinfo
		 */
		static const Cinfo* initCinfo();

		/**
		 * return ids of all the children in ret.
		 */
		static void children( Eref e, vector< Id >& ret );

		/**
		 * Finds the path of element e
		 */
		static string path( const Eref& e );

	private:
		// string name_;
};

#endif // _NEUTRAL_H
