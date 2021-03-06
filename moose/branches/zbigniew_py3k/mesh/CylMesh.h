/**********************************************************************
** This program is part of 'MOOSE', the
** Messaging Object Oriented Simulation Environment.
**           Copyright (C) 2011 Upinder S. Bhalla. and NCBS
** It is made available under the terms of the
** GNU Lesser General Public License version 2.1
** See the file COPYING.LIB for the full notice.
**********************************************************************/

#ifndef _CYL_MESH_H
#define _CYL_MESH_H

/**
 * The CylMesh represents a chemically identified compartment shaped
 * like an extended cylinder. This is psuedo-1 dimension: Only the
 * axial dimension is considered for diffusion and subdivisions.
 * Typically used in modelling small segments of dendrite
 */
class CylMesh: public MeshCompt
{
	public: 
		CylMesh();
		~CylMesh();
		//////////////////////////////////////////////////////////////////
		//  Utility func
		//////////////////////////////////////////////////////////////////
		/**
		 * Recomputes all local coordinate and meshing data following
		 * a change in any of the coord parameters
		 */

		void updateCoords();

		//////////////////////////////////////////////////////////////////
		// Field assignment stuff
		//////////////////////////////////////////////////////////////////

		void setX0( double v );
		double getX0() const;
		void setY0( double v );
		double getY0() const;
		void setZ0( double v );
		double getZ0() const;
		void setR0( double v );
		double getR0() const;

		void setX1( double v );
		double getX1() const;
		void setY1( double v );
		double getY1() const;
		void setZ1( double v );
		double getZ1() const;
		void setR1( double v );
		double getR1() const;

		void innerSetCoords( const vector< double >& v);
		void setCoords( const Eref& e, vector< double > v );
		vector< double > getCoords( const Eref& e ) const;

		void setLambda( double v );
		double getLambda() const;

		double getTotLength() const;

		unsigned int innerGetDimensions() const;

		//////////////////////////////////////////////////////////////////
		// FieldElement assignment stuff for MeshEntries
		//////////////////////////////////////////////////////////////////

		/// Virtual function to return MeshType of specified entry.
		unsigned int getMeshType( unsigned int fid ) const;
		/// Virtual function to return dimensions of specified entry.
		unsigned int getMeshDimensions( unsigned int fid ) const;
		/// Virtual function to return volume of mesh Entry.
		double getMeshEntryVolume( unsigned int fid ) const;
		/// Virtual function to return coords of mesh Entry.
		vector< double > getCoordinates( unsigned int fid ) const;
		/// Virtual function to return diffusion X-section area
		vector< double > getDiffusionArea( unsigned int fid ) const;
		/// Virtual function to return scale factor for diffusion. 1 here.
		vector< double > getDiffusionScaling( unsigned int fid ) const;
		/// Volume of mesh Entry including abutting diff-coupled voxels
		double extendedMeshEntryVolume( unsigned int fid ) const;

		//////////////////////////////////////////////////////////////////

		/**
		 * Inherited virtual func. Returns number of MeshEntry in array
		 */
		unsigned int innerGetNumEntries() const;
		/// Inherited virtual func.
		void innerSetNumEntries( unsigned int n );
		
		/// Inherited virtual, do nothing for now.
		vector< unsigned int > getParentVoxel() const;
		const vector< double >& getVoxelVolume() const;
		const vector< double >& getVoxelArea() const;
		const vector< double >& getVoxelLength() const;
		//////////////////////////////////////////////////////////////////
		// Dest funcs
		//////////////////////////////////////////////////////////////////

		/// Virtual func to make a mesh with specified Volume and numEntries
		void innerBuildDefaultMesh( const Eref& e,
			double volume, unsigned int numEntries );

		void innerHandleRequestMeshStats(
			const Eref& e,
			const SrcFinfo2< unsigned int, vector< double > >*
				meshStatsFinfo
		);

		void innerHandleNodeInfo(
			const Eref& e, 
			unsigned int numNodes, unsigned int numThreads );

		void transmitChange( const Eref& e );

		void buildStencil();
		
		//////////////////////////////////////////////////////////////////
		// inherited virtual funcs for Boundary
		//////////////////////////////////////////////////////////////////
		
		void matchMeshEntries( const ChemCompt* other, 
			vector< VoxelJunction > & ret ) const;

		double nearest( double x, double y, double z, 
						unsigned int& index ) const;
	
		double nearest( double x, double y, double z, 
						double& linePos, double& r ) const;
	
		void indexToSpace( unsigned int index, 
						double& x, double& y, double& z ) const;
		
		//////////////////////////////////////////////////////////////////
		// Inner specific functions needed by matchMeshEntries.
		//////////////////////////////////////////////////////////////////
		void matchCylMeshEntries( const CylMesh* other,
			vector< VoxelJunction >& ret ) const;
		void matchCubeMeshEntries( const CubeMesh* other,
			vector< VoxelJunction >& ret ) const;
		void matchNeuroMeshEntries( const NeuroMesh* other,
			vector< VoxelJunction >& ret ) const;

		// Selects a grid volume for generating intersection with CubeMesh.
		double selectGridVolume( double h ) const;

		//////////////////////////////////////////////////////////////////

		static const Cinfo* initCinfo();

	private:
		unsigned int numEntries_; // Number of subdivisions to use
		bool useCaps_; // Flag: Should the ends have hemispherical caps?
		bool isToroid_; // Flag: Should the ends loop around mathemagically?

		double x0_; /// coords
		double y0_; /// coords
		double z0_; /// coords

		double x1_; /// coords
		double y1_; /// coords
		double z1_; /// coords

		double r0_;	/// Radius at one end
		double r1_; /// Radius at other end

		double lambda_;	/// Length constant for diffusion. Equal to dx.

		/**
		 * surfaceGranularity_ decides how finely to subdivide lambda 
		 * or cubic mesh side, when computing surface area of intersections
		 * between them when diffusing. Defaults to 0.1
		 */
		double surfaceGranularity_; 

		double totLen_;	/// Utility value: Total length of cylinder
		double rSlope_;	/// Utility value: dr/dx
		double lenSlope_; /// Utility value: dlen/dx
};

#endif	// _CYL_MESH_H
