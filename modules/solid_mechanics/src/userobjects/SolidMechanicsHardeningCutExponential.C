//* This file is part of the MOOSE framework
//* https://mooseframework.inl.gov
//*
//* All rights reserved, see COPYRIGHT for full restrictions
//* https://github.com/idaholab/moose/blob/master/COPYRIGHT
//*
//* Licensed under LGPL 2.1, please see LICENSE for details
//* https://www.gnu.org/licenses/lgpl-2.1.html

#include "SolidMechanicsHardeningCutExponential.h"

registerMooseObject("SolidMechanicsApp", SolidMechanicsHardeningCutExponential);
registerMooseObjectRenamed("SolidMechanicsApp",
                           TensorMechanicsHardeningCutExponential,
                           "01/01/2025 00:00",
                           SolidMechanicsHardeningCutExponential);

InputParameters
SolidMechanicsHardeningCutExponential::validParams()
{
  InputParameters params = SolidMechanicsHardeningModel::validParams();
  params.addRequiredParam<Real>(
      "value_0", "The value of the parameter for all internal_parameter <= internal_0");
  params.addParam<Real>("value_residual",
                        "The value of the parameter for internal_parameter = "
                        "infinity.  Default = value_0, ie perfect plasticity");
  params.addParam<Real>("internal_0", 0, "The cutoff of internal parameter");
  params.addParam<Real>("rate",
                        0,
                        "Let p = internal_parameter.  Then value = value_0 for "
                        "p<internal_0, and otherwise, value = value_residual + (value_0 "
                        "- value_residual)*exp(-rate*(p - internal_0)");
  params.addClassDescription("Hardening is Cut-exponential");
  return params;
}

SolidMechanicsHardeningCutExponential::SolidMechanicsHardeningCutExponential(
    const InputParameters & parameters)
  : SolidMechanicsHardeningModel(parameters),
    _val_0(getParam<Real>("value_0")),
    _val_res(parameters.isParamValid("value_residual") ? getParam<Real>("value_residual") : _val_0),
    _intnl_0(getParam<Real>("internal_0")),
    _rate(getParam<Real>("rate"))
{
}

Real
SolidMechanicsHardeningCutExponential::value(Real intnl) const
{
  Real x = intnl - _intnl_0;
  if (x <= 0)
    return _val_0;
  else
    return _val_res + (_val_0 - _val_res) * std::exp(-_rate * x);
}

Real
SolidMechanicsHardeningCutExponential::derivative(Real intnl) const
{
  Real x = intnl - _intnl_0;
  if (x <= 0)
    return 0;
  else
    return -_rate * (_val_0 - _val_res) * std::exp(-_rate * x);
}

std::string
SolidMechanicsHardeningCutExponential::modelName() const
{
  return "CutExponential";
}
