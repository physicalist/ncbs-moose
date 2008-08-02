/**********************************************************************
** This program is part of 'MOOSE', the
** Messaging Object Oriented Simulation Environment,
** also known as GENESIS 3 base code.
**           copyright (C) 2003-2006 Upinder S. Bhalla. and NCBS
** It is made available under the terms of the
** GNU Lesser General Public License version 2.1
** See the file COPYING.LIB for the full notice.
**********************************************************************/

#include "moose.h"
#include "IdManager.h"
#include "ThisFinfo.h"
#include "shell/Shell.h"
// #include "ArrayWrapperElement.h"
#ifdef USE_MPI
const Cinfo* initPostMasterCinfo();
#endif

const unsigned int UNKNOWN_NODE = UINT_MAX;
// const unsigned int MIN_NODE = 1;
// const unsigned int MAX_NODE = 65536; // Dream on.
const unsigned int IdManager::numScratch = 1000;
const unsigned int IdManager::blockSize = 1000;
const unsigned int BAD_NODE = UINT_MAX;

IdManager::IdManager()
	: loadThresh_( 2000.0 ),
	scratchIndex_( 3 ), mainIndex_( 3 )
	// Start at 2 because root is 0 and shell is 1 and postmaster is 2.
{
	elementList_.resize( blockSize + numScratch );
}

/*
void IdManager::setNodes( unsigned int myNode, unsigned int numNodes )
{
	myNode_ = myNode;
	numNodes_ = numNodes;
	elementList_[0] = Enode( Element::root(), Shell::myNode() );
	if ( numNodes > 1 ) {
		if ( myNode == 0 )
			nodeLoad.resize( numNodes );
		elementList_.resize( numScratch + blockSize );
		mainIndex_ = numScratch;
	}
}
*/

/**
 * Returns the next available id and allocates space for it.
 * Later can be refined to mop up freed ids. 
 * Don't bother with the scratch space if we are on master node.
 */
unsigned int IdManager::scratchId()
{
	if ( Shell::numNodes() <= 1 ) {
		lastId_ = mainIndex_;
		mainIndex_++;
		if ( mainIndex_ >= elementList_.size() )
			elementList_.resize( mainIndex_ * 2 );
		return lastId_;
	} else {
		if ( scratchIndex_ < numScratch ) {
			lastId_ = scratchIndex_;
			++scratchIndex_;
			return lastId_;
		} else {
			regularizeScratch();
			return ( lastId_ = scratchIndex_ );
		}
	} 
	return lastId_;
}

// Should only be called on master node, and parent should have been checked
unsigned int IdManager::childId( unsigned int parent )
{
#ifdef USE_MPI
	assert( Shell::myNode() == 0 );
	if ( parent < mainIndex_ ) {
		Enode& pa = elementList_[ parent ];
		if ( mainIndex_ >= elementList_.size() )
			elementList_.resize( elementList_.size() * 2 );
		lastId_ = mainIndex_;
		mainIndex_++;
		if ( pa.node() == Id::GlobalNode ) {
			// Make the object on node 0.
			elementList_[ lastId_ ] = Enode( 0, Shell::myNode() );
		} else if ( pa.node() != Shell::myNode() ) {
			// Put object on parent node.
			elementList_[ lastId_ ] = Enode( 0, pa.node() );
		} else { // Parent is also on master
			// Crude load balancing calculation here, better will come.
			unsigned int targetNode = 
				static_cast< unsigned int >( mainIndex_ / loadThresh_ ) %
				Shell::numNodes();
			elementList_[ lastId_ ] = Enode( 0, targetNode );
		}
		return lastId_;
	}
	assert( 0 );
#else
	lastId_ = mainIndex_;
	mainIndex_++;
	if ( mainIndex_ >= elementList_.size() )
		elementList_.resize( mainIndex_ * 2, 0 );
	return lastId_;
#endif
}

/**
 * This variant of childId forces creation of object on specified node,
 * provided that we are in parallel mode. Otherwise it ignores the node
 * specification.  
 * Should only be called on master node, and parent should have been checked
 * In single-node mode it is equivalent to the scratchId and childId ops.
 * \todo Need to add facility to create object globally.
 *
 */
unsigned int IdManager::makeIdOnNode( unsigned int childNode )
{
	lastId_ = mainIndex_;
	mainIndex_++;
#ifdef USE_MPI
	assert( Shell::myNode() == 0 );
	assert( childNode < Shell::numNodes() );
	elementList_[ lastId_ ] = Enode( 0, childNode );
#endif
	if ( mainIndex_ >= elementList_.size() )
		elementList_.resize( mainIndex_ * 2 );
	return lastId_;
}

Element* IdManager::getElement( const Id& id ) const
{
	if ( id.id() < mainIndex_ ) {
		const Enode& ret = elementList_[ id.id() ];
#ifdef USE_MPI
		if ( ret.node() == UNKNOWN_NODE ) {
			// don't know how to handle this yet. It should trigger
			// a request to the master node to update the elist.
			// We then get into managing how many entries are unknown...
			assert( 0 );
			return 0;
		} else if ( ret.node() == Shell::myNode() || 
			ret.node() == Id::GlobalNode ) {
			return ret.e();
		} else {
			return 0;
		}
#endif
		return ret.e();
	}
	return 0;
}

/// \todo: This needs additional work for node safety.
bool IdManager::setElement( unsigned int index, Element* e )
{
	if ( index >= elementList_.size() )
		elementList_.resize( ( 1 + index / blockSize ) * blockSize );

	if ( index < mainIndex_ ) {
		Enode& old = elementList_[ index ];
		if ( old.node() == UNKNOWN_NODE || old.e() == 0 ) {
			elementList_[ index ] = Enode( e, Shell::myNode() );
			return 1;
		} else if ( e == 0 ) {
			// Here we are presumably clearing out an element. Permit it.
			elementList_[ index ] = Enode( 0, Shell::myNode() );
			/// \todo: We could add this element to a list for reuse here.
			return 1;
		} else if ( index == 0 ) {
			// Here it is the wrapper for off-node objects. Ignore it.
			return 1;
		} else { // attempt to overwrite element. Should I assert?
			assert( 0 );
			return 1;
		}
	} else {
		// Here we have been told by the master node to make a child 
		// at a specific index before the elementList has been
		// expanded to that index. Just expand it to fit.
		elementList_[ index ] = Enode( e, Shell::myNode() );
		mainIndex_ = index + 1;
		return 1;
	}
}

#ifdef USE_MPI
unsigned int IdManager::findNode( unsigned int index ) const 
{
	const Enode& e = elementList_[ index ];
	if ( e.node() == UNKNOWN_NODE )
		return BAD_NODE;
	return e.node();
	
}

// Do not permit op if parent is not global
void IdManager::setGlobal( unsigned int index )
{
	Enode& e = elementList_[ index ];
	e.setGlobal();
}

bool IdManager::isGlobal( unsigned int index ) const 
{
	const Enode& e = elementList_[ index ];
	return ( e.node() == Id::GlobalNode );
}

#else
unsigned int IdManager::findNode( unsigned int index ) const 
{
	return 0;
}

bool IdManager::isGlobal( unsigned int index ) const 
{
	return 0;
}

void IdManager::setGlobal( unsigned int index )
{
	;
}
#endif


/**
 * Returns the most recently created object.
 */
unsigned int IdManager::lastId()
{
	return lastId_;
}

bool IdManager::outOfRange( unsigned int index ) const
{
	return index >= mainIndex_;
}

bool IdManager::isScratch( unsigned int index ) const
{
#ifdef USE_MPI
	return ( Shell::myNode() > 0 && index > 0 && index < scratchIndex_ );
#else
	return 0;
#endif
	
}

/// \todo: Need to put in some grungy code to deal with this.
void IdManager::regularizeScratch()
{
	;
}
