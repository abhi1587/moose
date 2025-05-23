//* This file is part of the MOOSE framework
//* https://mooseframework.inl.gov
//*
//* All rights reserved, see COPYRIGHT for full restrictions
//* https://github.com/idaholab/moose/blob/master/COPYRIGHT
//*
//* Licensed under LGPL 2.1, please see LICENSE for details
//* https://www.gnu.org/licenses/lgpl-2.1.html

#include "FunctionElementIntegral.h"
#include "Function.h"

registerMooseObject("MooseApp", FunctionElementIntegral);

InputParameters
FunctionElementIntegral::validParams()
{
  InputParameters params = ElementIntegralPostprocessor::validParams();

  params.addRequiredParam<FunctionName>("function", "Name of function to integrate");

  params.addClassDescription("Integrates a function over elements");

  return params;
}

FunctionElementIntegral::FunctionElementIntegral(const InputParameters & parameters)
  : ElementIntegralPostprocessor(parameters), _function(getFunction("function"))
{
}

Real
FunctionElementIntegral::computeQpIntegral()
{
  return _function.value(_t, _q_point[_qp]);
}
