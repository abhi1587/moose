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

// Forward Declarations
class LotsOfRaysRayStudy;

class LotsOfRaysExpectedDistance : public GeneralPostprocessor
{
public:
  static InputParameters validParams();

  LotsOfRaysExpectedDistance(const InputParameters & parameters);

  virtual void initialize() override {}
  virtual void execute() override {}

  virtual Real getValue() const override;

protected:
  /// The LotsOfRaysRayStudy
  const LotsOfRaysRayStudy & _study;
};
