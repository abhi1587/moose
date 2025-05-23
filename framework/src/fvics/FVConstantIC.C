//* This file is part of the MOOSE framework
//* https://mooseframework.inl.gov
//*
//* All rights reserved, see COPYRIGHT for full restrictions
//* https://github.com/idaholab/moose/blob/master/COPYRIGHT
//*
//* Licensed under LGPL 2.1, please see LICENSE for details
//* https://www.gnu.org/licenses/lgpl-2.1.html

#include "FVConstantIC.h"
#include "libmesh/point.h"

registerMooseObject("MooseApp", FVConstantIC);

InputParameters
FVConstantIC::validParams()
{
  InputParameters params = FVInitialCondition::validParams();
  params.addRequiredParam<Real>("value", "The value to be set in IC");
  params.addClassDescription("Sets a constant field value.");
  return params;
}

FVConstantIC::FVConstantIC(const InputParameters & parameters)
  : FVInitialCondition(parameters), _value(getParam<Real>("value"))
{
}

Real
FVConstantIC::value(const Point & /*p*/)
{
  return _value;
}
