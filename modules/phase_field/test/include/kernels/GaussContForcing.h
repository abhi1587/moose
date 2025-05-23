//* This file is part of the MOOSE framework
//* https://mooseframework.inl.gov
//*
//* All rights reserved, see COPYRIGHT for full restrictions
//* https://github.com/idaholab/moose/blob/master/COPYRIGHT
//*
//* Licensed under LGPL 2.1, please see LICENSE for details
//* https://www.gnu.org/licenses/lgpl-2.1.html

#pragma once

#include "Kernel.h"

/**
 * Note: This class is duplicated from moose_test.  It is useful for testing
 */
class GaussContForcing : public Kernel
{
public:
  static InputParameters validParams();

  GaussContForcing(const InputParameters & parameters);

protected:
  virtual Real computeQpResidual();

  const Real _amplitude;
  const Real _x_center;
  const Real _y_center;
  const Real _z_center;

  const Real _x_spread;
  const Real _y_spread;
  const Real _z_spread;

  const Real _x_min;
  const Real _x_max;
  const Real _y_min;
  const Real _y_max;
  const Real _z_min;
  const Real _z_max;
};
