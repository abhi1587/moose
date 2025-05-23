//* This file is part of the MOOSE framework
//* https://mooseframework.inl.gov
//*
//* All rights reserved, see COPYRIGHT for full restrictions
//* https://github.com/idaholab/moose/blob/master/COPYRIGHT
//*
//* Licensed under LGPL 2.1, please see LICENSE for details
//* https://www.gnu.org/licenses/lgpl-2.1.html

#include "LegacyTensorMechanicsAction.h"

registerMooseAction("SolidMechanicsApp", LegacyTensorMechanicsAction, "setup_mesh_complete");

registerMooseAction("SolidMechanicsApp",
                    LegacyTensorMechanicsAction,
                    "validate_coordinate_systems");

registerMooseAction("SolidMechanicsApp", LegacyTensorMechanicsAction, "add_kernel");

InputParameters
LegacyTensorMechanicsAction::validParams()
{
  InputParameters params = QuasiStaticSolidMechanicsPhysics::validParams();
  params.addParam<bool>(
      "use_displaced_mesh", false, "Whether to use displaced mesh in the kernels");
  return params;
}

LegacyTensorMechanicsAction::LegacyTensorMechanicsAction(const InputParameters & params)
  : QuasiStaticSolidMechanicsPhysics(params)
{
}

void
LegacyTensorMechanicsAction::act()
{
  if (_current_task == "add_kernel" || _current_task == "validate_coordinate_systems")
    QuasiStaticSolidMechanicsPhysics::act();
}
