//* This file is part of the MOOSE framework
//* https://mooseframework.inl.gov
//*
//* All rights reserved, see COPYRIGHT for full restrictions
//* https://github.com/idaholab/moose/blob/master/COPYRIGHT
//*
//* Licensed under LGPL 2.1, please see LICENSE for details
//* https://www.gnu.org/licenses/lgpl-2.1.html

#include "SolidPropertiesTestUtils.h"
#include "ThermalFunctionSolidPropertiesTest.h"

/**
 * Test that the thermal conductivity and its derivatives are
 * correctly computed.
 */
TEST_F(ThermalFunctionSolidPropertiesTest, k)
{
  Real T = 10.0;

  REL_TEST(_sp->k_from_T(T), 7.121, REL_TOL_SAVED_VALUE);
  DERIV_TEST(_sp->k_from_T, T, REL_TOL_DERIVATIVE);
}

/**
 * Test that the isobaric specific heat capacity and its derivatives are
 * correctly computed.
 */
TEST_F(ThermalFunctionSolidPropertiesTest, cp)
{
  Real T = 800.0;

  REL_TEST(_sp->cp_from_T(T), 1200.0, REL_TOL_SAVED_VALUE);
  DERIV_TEST(_sp->cp_from_T, T, REL_TOL_DERIVATIVE);
}

/**
 * Test that the specific internal energy and its derivatives are
 * correctly computed.
 */
TEST_F(ThermalFunctionSolidPropertiesTest, e)
{
  const Real T = 800.0;
  REL_TEST(_sp->e_from_T(T), 536850.0, REL_TOL_SAVED_VALUE);
  SPECIFIC_INTERNAL_ENERGY_TESTS(_sp, T, 1e-6, 1e-6);
}

/**
 * Test that the density and its derivatives are
 * correctly computed.
 */
TEST_F(ThermalFunctionSolidPropertiesTest, rho)
{
  Real T = 10.0;

  REL_TEST(_sp->rho_from_T(T), 110.0, REL_TOL_SAVED_VALUE);
  DERIV_TEST(_sp->rho_from_T, T, REL_TOL_DERIVATIVE);
}
