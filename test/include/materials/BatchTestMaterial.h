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
#include "BatchMaterialTest.h"
#include "RankTwoTensor.h"

/**
 * Test material that goes with BatchMaterialTest
 */
class BatchTestMaterial : public Material
{
public:
  static InputParameters validParams();

  BatchTestMaterial(const InputParameters & parameters);

protected:
  void computeProperties() override;

  // used only for checking the batch computation
  const VariableValue & _var1;
  const MaterialProperty<RankTwoTensor> & _prop1;
  const MaterialProperty<Real> & _prop2;
  const VariableValue & _var1_n;
  const MaterialProperty<RankTwoTensor> & _prop1_n;
  const MaterialProperty<Real> & _prop2_n;

  // output batch result as property
  MaterialProperty<Real> & _prop_out1;
  MaterialProperty<Real> & _prop_out2;

  // coupling the batch computation result
  const BatchMaterialTest & _batch_uo;
  const BatchMaterialTest::OutputVector & _output;
};
