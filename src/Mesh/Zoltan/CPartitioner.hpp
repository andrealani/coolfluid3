// Copyright (C) 2010 von Karman Institute for Fluid Dynamics, Belgium
//
// This software is distributed under the terms of the
// GNU Lesser General Public License version 3 (LGPLv3).
// See doc/lgpl.txt and doc/gpl.txt for the license text.

#ifndef CF_Mesh_Zoltan_CPartitioner_hpp
#define CF_Mesh_Zoltan_CPartitioner_hpp

////////////////////////////////////////////////////////////////////////////////

#include "Mesh/CMeshPartitioner.hpp"
#include "Mesh/Zoltan/LibZoltan.hpp"

namespace CF {
namespace Mesh {
namespace Zoltan {

////////////////////////////////////////////////////////////////////////////////

/// CMeshPartitioner component class
/// This class serves as a component that that will partition the mesh
/// @author Willem Deconinck
class Zoltan_API CPartitioner : public CMeshPartitioner {

public: // typedefs

  /// type of pointer to Component
  typedef boost::shared_ptr<CPartitioner> Ptr;
  /// type of pointer to constant Component
  typedef boost::shared_ptr<CPartitioner const> ConstPtr;

public: // functions

  /// Contructor
  /// @param name of the component
  CPartitioner ( const std::string& name );

  /// Virtual destructor
  virtual ~CPartitioner();

  /// Get the class name
  static std::string type_name () { return "CPartitioner"; }

  /// Partitioning functions

  virtual void build_graph() {}

  virtual void partition_graph();
	
	virtual void migrate();

private: // functions

  void set_partitioning_params();
  
  // query function
  static int  query_nb_of_objects(void *data, int *ierr);
  
  static void query_list_of_objects(void *data, int sizeGID, int sizeLID,
                                   ZOLTAN_ID_PTR globalID, ZOLTAN_ID_PTR localID,
                                   int wgt_dim, float *obj_wgts, int *ierr);

  static void query_nb_connected_objects(void *data, int sizeGID, int sizeLID, int num_obj,
                                         ZOLTAN_ID_PTR globalID, ZOLTAN_ID_PTR localID,
                                         int *numEdges, int *ierr);

  static void query_list_of_connected_objects(void *data, int sizeGID, int sizeLID, int num_obj, 
                                              ZOLTAN_ID_PTR globalID, ZOLTAN_ID_PTR localID,
                                              int *num_edges,
                                              ZOLTAN_ID_PTR nborGID, int *nborProc,
                                              int wgt_dim, float *ewgts, int *ierr);
	
	
	
	//////////////////////////////////////////////////////////////////////

	// Functions for migration
	
	static 	void get_elems_sizes(void *data, int gidSize, int lidSize, int num_ids,
															 ZOLTAN_ID_PTR globalIDs, ZOLTAN_ID_PTR localIDs, int *sizes, int *ierr);
	
	static 	void pack_elems_messages(void *data, int gidSize, int lidSize, int num_ids,
																	 ZOLTAN_ID_PTR globalIDs, ZOLTAN_ID_PTR localIDs, int *dests, int *sizes, int *idx, char *buf, int *ierr);
	
	static	void unpack_elems_messages(void *data, int gidSize, int num_ids,
																		 ZOLTAN_ID_PTR globalIDs, int *sizes, int *idx, char *buf, int *ierr);
		
	static 	void get_nodes_sizes(void *data, int gidSize, int lidSize, int num_ids,
															 ZOLTAN_ID_PTR globalIDs, ZOLTAN_ID_PTR localIDs, int *sizes, int *ierr);
	
	static 	void pack_nodes_messages(void *data, int gidSize, int lidSize, int num_ids,
																	 ZOLTAN_ID_PTR globalIDs, ZOLTAN_ID_PTR localIDs, int *dests, int *sizes, int *idx, char *buf, int *ierr);
	
	static	void unpack_nodes_messages(void *data, int gidSize, int num_ids,
																		 ZOLTAN_ID_PTR globalIDs, int *sizes, int *idx, char *buf, int *ierr);

  
private: // data

  ZoltanObject* m_zz;
	
	
// following data should be local to partitioning function, but is now global
// for access for temporary migration function
private: // data
	
	bool m_partitioned;
	int changes;
  int numGidEntries;
  int numLidEntries;
  int numImport;
  ZOLTAN_ID_PTR importGlobalIds;
  ZOLTAN_ID_PTR importLocalIds;
  int *importProcs;
  int *importToPart;
  int numExport;
  ZOLTAN_ID_PTR exportGlobalIds;
  ZOLTAN_ID_PTR exportLocalIds;
  int *exportProcs;
  int *exportToPart;
    
};

////////////////////////////////////////////////////////////////////////////////

} // Zoltan
} // Mesh
} // CF

////////////////////////////////////////////////////////////////////////////////

#endif // CF_Mesh_Zoltan_CPartitioner_hpp