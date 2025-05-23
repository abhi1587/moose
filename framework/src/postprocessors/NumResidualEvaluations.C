//* This file is part of the MOOSE framework
//* https://mooseframework.inl.gov
//*
//* All rights reserved, see COPYRIGHT for full restrictions
//* https://github.com/idaholab/moose/blob/master/COPYRIGHT
//*
//* Licensed under LGPL 2.1, please see LICENSE for details
//* https://www.gnu.org/licenses/lgpl-2.1.html

// MOOSE includes
#include "NumResidualEvaluations.h"
#include "FEProblem.h"
#include "SubProblem.h"
#include "NonlinearSystem.h"

registerMooseObject("MooseApp", NumResidualEvaluations);

InputParameters
NumResidualEvaluations::validParams()
{
  InputParameters params = GeneralPostprocessor::validParams();
  params.addClassDescription("Returns the total number of residual evaluations performed.");
  return params;
}

NumResidualEvaluations::NumResidualEvaluations(const InputParameters & parameters)
  : GeneralPostprocessor(parameters)
{
}

Real
NumResidualEvaluations::getValue() const
{
  return _fe_problem.getNonlinearSystemBase(_sys.number()).nResidualEvaluations();
}
