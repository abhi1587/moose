//* This file is part of the MOOSE framework
//* https://mooseframework.inl.gov
//*
//* All rights reserved, see COPYRIGHT for full restrictions
//* https://github.com/idaholab/moose/blob/master/COPYRIGHT
//*
//* Licensed under LGPL 2.1, please see LICENSE for details
//* https://www.gnu.org/licenses/lgpl-2.1.html

#pragma once

#include "Material.h"

/**
 * A material that couples a material property
 */
class ADCoupledMaterial : public Material
{
public:
  static InputParameters validParams();

  ADCoupledMaterial(const InputParameters & parameters);

protected:
  virtual void computeQpProperties();

  ADMaterialProperty<Real> & _ad_mat_prop;
  MaterialProperty<Real> & _regular_mat_prop;

  const ADVariableValue & _coupled_var;
};
