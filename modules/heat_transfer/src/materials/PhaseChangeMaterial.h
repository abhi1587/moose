//* This file is part of the MOOSE framework
//* https://www.mooseframework.org
//*
//* All rights reserved, see COPYRIGHT for full restrictions
//* https://github.com/idaholab/moose/blob/master/COPYRIGHT
//*
//* Licensed under LGPL 2.1, please see LICENSE for details
//* https://www.gnu.org/licenses/lgpl-2.1.html

#pragma once

#include "Material.h"
#include "RankTwoTensor.h"

/**
 * Declares a variable dependent function material property of type RankTwoTensor.
 */
template <bool is_ad>
class PhaseChangeMaterialTempl : public Material
{
public:
  static InputParameters validParams();

  PhaseChangeMaterialTempl(const InputParameters & parameters);

protected:
  virtual void computeQpProperties() override;

  GenericMaterialProperty<Real, is_ad> & _prop;

  const GenericVariableValue<is_ad> & _variable;
  const MaterialProperty<Real> & _Ts;
  const MaterialProperty<Real> & _Te;

  // const unsigned int _num_functions;
  // std::vector<const Function *> _functions;  
};

typedef PhaseChangeMaterialTempl<false> PhaseChangeMaterial;
typedef PhaseChangeMaterialTempl<true> ADPhaseChangeMaterial;


