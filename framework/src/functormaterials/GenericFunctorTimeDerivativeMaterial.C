//* This file is part of the MOOSE framework
//* https://mooseframework.inl.gov
//*
//* All rights reserved, see COPYRIGHT for full restrictions
//* https://github.com/idaholab/moose/blob/master/COPYRIGHT
//*
//* Licensed under LGPL 2.1, please see LICENSE for details
//* https://www.gnu.org/licenses/lgpl-2.1.html

#include "GenericFunctorTimeDerivativeMaterial.h"

registerMooseObject("MooseApp", GenericFunctorTimeDerivativeMaterial);
registerMooseObject("MooseApp", ADGenericFunctorTimeDerivativeMaterial);

template <bool is_ad>
InputParameters
GenericFunctorTimeDerivativeMaterialTempl<is_ad>::validParams()
{
  InputParameters params = FunctorMaterial::validParams();
  params.set<ExecFlagEnum>("execute_on") = {EXEC_ALWAYS};
  params.addClassDescription(
      "FunctorMaterial object for declaring properties that are populated by evaluation of "
      "time derivatives of Functors objects. (such as variables, constants, postprocessors). "
      "The time derivative is only returned if the 'dot' functor routine is implemented.");
  params.addParam<std::vector<std::string>>("prop_names",
                                            "The names of the properties this material will have");

  params.addParam<std::vector<MooseFunctorName>>("prop_values",
                                                 "The corresponding names of the "
                                                 "functors which gradient are going to provide "
                                                 "the values for the variables");
  return params;
}

template <bool is_ad>
GenericFunctorTimeDerivativeMaterialTempl<is_ad>::GenericFunctorTimeDerivativeMaterialTempl(
    const InputParameters & parameters)
  : FunctorMaterial(parameters),
    _prop_names(getParam<std::vector<std::string>>("prop_names")),
    _prop_values(getParam<std::vector<MooseFunctorName>>("prop_values")),
    _num_props(_prop_names.size())
{
  unsigned int num_values = _prop_values.size();

  if (_num_props != num_values)
    mooseError("Number of prop_names must match the number of prop_values for a "
               "GenericFunctorTimeDerivativeMaterial!");

  // Check that there is no name conflict, a common mistake with this object
  for (const auto i : make_range(_num_props))
    for (const auto j : make_range(num_values))
      if (_prop_names[i] == _prop_values[j])
        paramError("prop_names",
                   "prop_names should not be the same as any of the prop_values. They"
                   " can both be functors, and functors may not have the same name.");

  _functors.resize(_num_props);

  for (const auto i : make_range(_num_props))
  {
    if (_fe_problem.hasPostprocessor(_prop_values[i]))
      paramError("prop_names", "Postprocessors should not be used in this functor material");
    _functors[i] = &getFunctor<GenericReal<is_ad>>(_prop_values[i]);
  }

  const std::set<ExecFlagType> clearance_schedule(_execute_enum.begin(), _execute_enum.end());
  for (const auto i : make_range(_num_props))
    addFunctorProperty<GenericReal<is_ad>>(
        _prop_names[i],
        [this, i](const auto & r, const auto & t) -> GenericReal<is_ad>
        { return (*_functors[i]).dot(r, t); },
        clearance_schedule);
}

template class GenericFunctorTimeDerivativeMaterialTempl<false>;
template class GenericFunctorTimeDerivativeMaterialTempl<true>;
