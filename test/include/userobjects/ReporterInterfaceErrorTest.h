//* This file is part of the MOOSE framework
//* https://mooseframework.inl.gov
//*
//* All rights reserved, see COPYRIGHT for full restrictions
//* https://github.com/idaholab/moose/blob/master/COPYRIGHT
//*
//* Licensed under LGPL 2.1, please see LICENSE for details
//* https://www.gnu.org/licenses/lgpl-2.1.html

#pragma once

#include "GeneralUserObject.h"

/**
 * A UserObject that tests errors produced by the ReporterInterface.
 */
class ReporterInterfaceErrorTest : public GeneralUserObject
{
public:
  static InputParameters validParams();

  ReporterInterfaceErrorTest(const InputParameters & params);

  void initialSetup() override;

  void initialize() override{};
  void execute() override{};
  void finalize() override{};
};
