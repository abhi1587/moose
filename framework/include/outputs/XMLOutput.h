//* This file is part of the MOOSE framework
//* https://mooseframework.inl.gov
//*
//* All rights reserved, see COPYRIGHT for full restrictions
//* https://github.com/idaholab/moose/blob/master/COPYRIGHT
//*
//* Licensed under LGPL 2.1, please see LICENSE for details
//* https://www.gnu.org/licenses/lgpl-2.1.html

#pragma once

#include "AdvancedOutput.h"
#include "pugixml.h"

class XMLOutput : public AdvancedOutput
{
public:
  static InputParameters validParams();
  XMLOutput(const InputParameters & parameters);

protected:
  virtual void output() override;

  virtual std::string filename() override;

  virtual void outputVectorPostprocessors() override;

private:
  pugi::xml_document _xml_doc;

  bool _distributed = false;
};
