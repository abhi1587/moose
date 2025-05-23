//* This file is part of the MOOSE framework
//* https://mooseframework.inl.gov
//*
//* All rights reserved, see COPYRIGHT for full restrictions
//* https://github.com/idaholab/moose/blob/master/COPYRIGHT
//*
//* Licensed under LGPL 2.1, please see LICENSE for details
//* https://www.gnu.org/licenses/lgpl-2.1.html

#pragma once

#include "ADKernel.h"

/**
 * Tests the InputParameters::addDeprecatedCoupledVar method
 *
 */
class DeprecatedCoupledVarKernel : public ADKernel
{
public:
  static InputParameters validParams();

  DeprecatedCoupledVarKernel(const InputParameters & parameters);

protected:
  ADReal computeQpResidual() override;

  const ADVariableValue & _source;
};
