//* This file is part of the MOOSE framework
//* https://mooseframework.inl.gov
//*
//* All rights reserved, see COPYRIGHT for full restrictions
//* https://github.com/idaholab/moose/blob/master/COPYRIGHT
//*
//* Licensed under LGPL 2.1, please see LICENSE for details
//* https://www.gnu.org/licenses/lgpl-2.1.html

#include "FileMesh.h"
#include "Parser.h"
#include "MooseUtils.h"
#include "Moose.h"
#include "MooseApp.h"
#include "FileMeshGenerator.h"

#include "libmesh/exodusII_io.h"
#include "libmesh/mesh_tools.h"
#include "libmesh/nemesis_io.h"
#include "libmesh/parallel_mesh.h"

using libMesh::ExodusII_IO;
using libMesh::Nemesis_IO;

registerMooseObject("MooseApp", FileMesh);

InputParameters
FileMesh::validParams()
{
  InputParameters params = MooseMesh::validParams();
  params.addRequiredParam<MeshFileName>("file", "The name of the mesh file to read");
  MooseEnum dims("1=1 2 3", "1");
  params.addParam<MooseEnum>("dim",
                             dims,
                             "This is only required for certain mesh formats where "
                             "the dimension of the mesh cannot be autodetected. "
                             "In particular you must supply this for GMSH meshes. "
                             "Note: This is completely ignored for ExodusII meshes!");
  params.addParam<bool>("clear_spline_nodes",
                        false,
                        "If clear_spline_nodes=true, IsoGeometric Analyis spline nodes "
                        "and constraints are removed from an IGA mesh, after which only "
                        "C^0 Rational-Bernstein-Bezier elements will remain.");
  params.addClassDescription("Read a mesh from a file.");
  return params;
}

FileMesh::FileMesh(const InputParameters & parameters)
  : MooseMesh(parameters), _file_name(getParam<MeshFileName>("file"))
{
}

FileMesh::FileMesh(const FileMesh & other_mesh)
  : MooseMesh(other_mesh), _file_name(other_mesh._file_name)
{
}

FileMesh::~FileMesh() {}

std::unique_ptr<MooseMesh>
FileMesh::safeClone() const
{
  return _app.getFactory().copyConstruct(*this);
}

void
FileMesh::buildMesh()
{
  TIME_SECTION("buildMesh", 2, "Reading Mesh");

  // This dimension should get overridden if the mesh reader can determine the dimension
  getMesh().set_mesh_dimension(getParam<MooseEnum>("dim"));

  if (_is_nemesis)
  {
    // Nemesis_IO only takes a reference to DistributedMesh, so we can't be quite so short here.
    DistributedMesh & pmesh = cast_ref<DistributedMesh &>(getMesh());
    Nemesis_IO(pmesh).read(_file_name);

    getMesh().allow_renumbering(false);

    // Even if we want repartitioning when load balancing later, we'll
    // begin with the default partitioning defined by the Nemesis
    // file.
    bool skip_partitioning_later = getMesh().skip_partitioning();
    getMesh().skip_partitioning(true);
    getMesh().prepare_for_use();
    getMesh().skip_partitioning(skip_partitioning_later);
  }
  else // not reading Nemesis files
  {
    // See if the user has requested reading a solution from the file.  If so, we'll need to read
    // the mesh with the exodus reader instead of using mesh.read().  This will read the mesh on
    // every processor

    if (_app.getExodusFileRestart() && (_file_name.rfind(".exd") < _file_name.size() ||
                                        _file_name.rfind(".e") < _file_name.size()))
    {
      MooseUtils::checkFileReadable(_file_name);

      auto exreader = std::make_shared<ExodusII_IO>(getMesh());
      _app.setExReaderForRestart(std::move(exreader));
      exreader->read(_file_name);

      getMesh().allow_renumbering(false);
      getMesh().prepare_for_use();
    }
    else
    {
      _file_name = FileMeshGenerator::deduceCheckpointPath(*this, _file_name);

      // If we are reading a mesh while restarting, then we might have
      // a solution file that relies on that mesh partitioning and/or
      // numbering.  In that case, we need to turn off repartitioning
      // and renumbering, at least at first.
      bool restarting = _file_name.rfind(".cpa.gz") < _file_name.size();

      const bool skip_partitioning_later = restarting && getMesh().skip_partitioning();
      const bool allow_renumbering_later = restarting && getMesh().allow_renumbering();

      if (restarting)
      {
        getMesh().skip_partitioning(true);
        getMesh().allow_renumbering(false);
      }

      MooseUtils::checkFileReadable(_file_name);
      getMesh().read(_file_name);

      if (getParam<bool>("clear_spline_nodes"))
        MeshTools::clear_spline_nodes(getMesh());

      _app.possiblyLoadRestartableMetaData(MooseApp::MESH_META_DATA, _file_name);

      if (restarting)
      {
        getMesh().allow_renumbering(allow_renumbering_later);
        getMesh().skip_partitioning(skip_partitioning_later);
      }
    }
  }
}

void
FileMesh::read(const std::string & file_name)
{
  if (dynamic_cast<DistributedMesh *>(&getMesh()) && !_is_nemesis)
    getMesh().read(file_name, /*mesh_data=*/NULL, /*skip_renumber=*/false);
  else
    getMesh().read(file_name, /*mesh_data=*/NULL, /*skip_renumber=*/true);
}
