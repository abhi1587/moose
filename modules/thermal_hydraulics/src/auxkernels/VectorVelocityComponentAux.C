//* This file is part of the MOOSE framework
//* https://mooseframework.inl.gov
//*
//* All rights reserved, see COPYRIGHT for full restrictions
//* https://github.com/idaholab/moose/blob/master/COPYRIGHT
//*
//* Licensed under LGPL 2.1, please see LICENSE for details
//* https://www.gnu.org/licenses/lgpl-2.1.html

#include "VectorVelocityComponentAux.h"

registerMooseObject("ThermalHydraulicsApp", VectorVelocityComponentAux);

InputParameters
VectorVelocityComponentAux::validParams()
{
  InputParameters params = AuxKernel::validParams();
  params.addRequiredCoupledVar("arhoA", "alpha*rho*A");
  params.addRequiredCoupledVar("arhouA", "alpha*rho*u*A");
  params.addRequiredParam<MaterialPropertyName>("direction",
                                                "Directional vector of 1D elements in 3D space");
  params.addRequiredParam<unsigned int>("component", "The vector component of interest");
  params.addClassDescription("Computes the component of a vector-valued velocity field given by "
                             "its magnitude and direction.");
  return params;
}

VectorVelocityComponentAux::VectorVelocityComponentAux(const InputParameters & parameters)
  : AuxKernel(parameters),
    _arhoA(coupledValue("arhoA")),
    _arhouA(coupledValue("arhouA")),
    _dir(getMaterialProperty<RealVectorValue>("direction")),
    _component(getParam<unsigned int>("component"))
{
  if (isNodal())
    mooseError(name(), ": Does not support nodal variables.");
}

Real
VectorVelocityComponentAux::computeValue()
{
  mooseAssert(_arhoA[_qp] != 0, "alpha*rho*A is 0, unable to compute velocity");
  return _dir[_qp](_component) * _arhouA[_qp] / _arhoA[_qp];
}
