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

class TimestepSize : public GeneralPostprocessor
{
public:
  static InputParameters validParams();

  TimestepSize(const InputParameters & parameters);

  virtual void initialize() override {}
  virtual void execute() override {}

  /**
   * This will return the current time step size.
   */
  virtual Real getValue() const override;

protected:
  FEProblemBase & _feproblem;
};
