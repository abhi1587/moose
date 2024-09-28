//* This file is part of the MOOSE framework
//* https://www.mooseframework.org
//*
//* All rights reserved, see COPYRIGHT for full restrictions
//* https://github.com/idaholab/moose/blob/master/COPYRIGHT
//*
//* Licensed under LGPL 2.1, please see LICENSE for details
//* https://www.gnu.org/licenses/lgpl-2.1.html

#include "TimeConstantFunction.h"

registerMooseObject("ThermalHydraulicsApp", TimeConstantFunction);

InputParameters
TimeConstantFunction::validParams()
{
  InputParameters params = Function::validParams();

  params.addRequiredParam<FunctionName>("initial_value", "Initial value");
  params.addRequiredParam<FunctionName>("final_value", "Final value");
  params.addRequiredParam<Real>("ramp_duration", "Duration, in seconds, of the ramp");
  params.addParam<Real>("initial_time", 0, "Initial time (necessary if not equal to zero)");

  params.addClassDescription("Gives a constant output or Ramps up to a value from another value over time.");
  params.declareControllable("initial_value");
  params.declareControllable("final_value");
  params.addParam<bool>("add_ramp", true, "Whether to add the heat transfer kernels");

  return params;
}

TimeConstantFunction::TimeConstantFunction(const InputParameters & parameters)
  : Function(parameters),FunctionInterface(this),

    _initial_value(getParam<FunctionName>("initial_value")),
    _final_value(getParam<FunctionName>("final_value")),
    _ramp_duration(getParam<Real>("ramp_duration")),
    _initial_time(getParam<Real>("initial_time")),
    _ramp_end_time(_initial_time + _ramp_duration),
    _add_ramp(getParam<bool>("add_ramp")),
    _init_func(),
    _final_func()
    // _ramp_slope((_final_value - _initial_value) / _ramp_duration)    
{
  _init_func = &getFunctionByName(_initial_value);
  _final_func = &getFunctionByName(_final_value);
}

Real
TimeConstantFunction::value(Real t, const Point & p) const
{
  const Real elapsed_time = t - _initial_time;
  const Real init_value = _init_func->value(t,p);
  const Real fin_value = _final_func->value(t,p);

  // std::cout << "initial_value =========================================== " << _initial_value << std::endl;
  if (t < _initial_time)
    return 1;
  else if (t > _ramp_end_time)
    return 1;
  else{     
      if (_add_ramp)
        return init_value + ((fin_value - init_value) / _ramp_duration) * elapsed_time;
      else
        return init_value;
  }      
}

RealVectorValue
TimeConstantFunction::gradient(Real /*t*/, const Point & /*p*/) const
{
  mooseError("TimeConstantFunction::gradient() is not implemented!");
}
