//* This file is part of the MOOSE framework
//* https://mooseframework.inl.gov
//*
//* All rights reserved, see COPYRIGHT for full restrictions
//* https://github.com/idaholab/moose/blob/master/COPYRIGHT
//*
//* Licensed under LGPL 2.1, please see LICENSE for details
//* https://www.gnu.org/licenses/lgpl-2.1.html

#pragma once

#include "GeneralPostprocessor.h"

/**
 * Compute the stats of VPP; this is needed for partition testing only; application developers
 * should use Statistics VPP in stochastic tools module.
 */
class StatVector : public GeneralPostprocessor
{
public:
  static InputParameters validParams();
  StatVector(const InputParameters & parameters);
  virtual void execute() override;
  virtual Real getValue() const override;
  virtual void initialize() final{};
  virtual void finalize() final{};

protected:
  const MooseEnum & _stat;
  const VectorPostprocessorValue & _vpp;
  Real _value = 0.;
};
