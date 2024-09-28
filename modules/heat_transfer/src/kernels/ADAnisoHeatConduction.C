//* This file is part of the MOOSE framework
//* https://www.mooseframework.org
//*
//* All rights reserved, see COPYRIGHT for full restrictions
//* https://github.com/idaholab/moose/blob/master/COPYRIGHT
//*
//* Licensed under LGPL 2.1, please see LICENSE for details
//* https://www.gnu.org/licenses/lgpl-2.1.html

#include "ADAnisoHeatConduction.h"

registerMooseObject("HeatTransferApp", ADAnisoHeatConduction);

InputParameters
ADAnisoHeatConduction::validParams()
{
  InputParameters params = ADKernel::validParams();
  params.addClassDescription(
      "Anisotropic HeatConduction kernel $\\nabla \\cdot -\\widetilde{k} \\nabla u$ "
      "with weak form given by $(\\nabla \\psi_i, \\widetilde{k} \\nabla u)$.");
  params.addParam<MaterialPropertyName>(
      "thermal_conductivity",
      "thermal_conductivity",
      "Material property providing thermal conductivity of the material.");

  return params;
}

ADAnisoHeatConduction::ADAnisoHeatConduction(const InputParameters & parameters)
  : ADKernel(parameters), _k(getADMaterialProperty<RankTwoTensor>("thermal_conductivity"))
{
}

ADReal
ADAnisoHeatConduction::computeQpResidual()
{
  return _k[_qp] * _grad_u[_qp] * _grad_test[_i][_qp];
}
