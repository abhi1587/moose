//* This file is part of the MOOSE framework
//* https://mooseframework.inl.gov
//*
//* All rights reserved, see COPYRIGHT for full restrictions
//* https://github.com/idaholab/moose/blob/master/COPYRIGHT
//*
//* Licensed under LGPL 2.1, please see LICENSE for details
//* https://www.gnu.org/licenses/lgpl-2.1.html

#include "ADBoundaryFlux3EqnGhostVelocityTemperature.h"
#include "THMIndicesVACE.h"
#include "SinglePhaseFluidProperties.h"
#include "Numerics.h"

registerMooseObject("ThermalHydraulicsApp", ADBoundaryFlux3EqnGhostVelocityTemperature);

InputParameters
ADBoundaryFlux3EqnGhostVelocityTemperature::validParams()
{
  InputParameters params = ADBoundaryFlux3EqnGhostBase::validParams();

  params.addClassDescription(
      "Computes a boundary flux from a specified velocity and temperature for the 1-D, "
      "1-phase, variable-area Euler equations using a ghost cell");

  params.addRequiredParam<Real>("vel", "Specified velocity");
  params.addRequiredParam<Real>("T", "Specified temperature");
  params.addParam<bool>("reversible", true, "True for reversible, false for pure inlet");

  params.addRequiredParam<UserObjectName>("fluid_properties",
                                          "Name of single-phase fluid properties user object");

  params.declareControllable("vel T");
  return params;
}

ADBoundaryFlux3EqnGhostVelocityTemperature::ADBoundaryFlux3EqnGhostVelocityTemperature(
    const InputParameters & parameters)
  : ADBoundaryFlux3EqnGhostBase(parameters),

    _vel(getParam<Real>("vel")),
    _T(getParam<Real>("T")),
    _reversible(getParam<bool>("reversible")),
    _fp(getUserObject<SinglePhaseFluidProperties>("fluid_properties"))
{
}

std::vector<ADReal>
ADBoundaryFlux3EqnGhostVelocityTemperature::getGhostCellSolution(
    const std::vector<ADReal> & U) const
{
  const ADReal rhoA = U[THMVACE1D::RHOA];
  const ADReal rhouA = U[THMVACE1D::RHOUA];
  const ADReal rhoEA = U[THMVACE1D::RHOEA];
  const ADReal A = U[THMVACE1D::AREA];

  const ADReal rho = rhoA / A;
  std::vector<ADReal> U_ghost(THMVACE1D::N_FLUX_INPUTS);
  if (!_reversible || THM::isInlet(_vel, _normal))
  {
    // Pressure is the only quantity coming from the interior
    const ADReal vel = rhouA / rhoA;
    const ADReal E = rhoEA / rhoA;
    const ADReal e = E - 0.5 * vel * vel;
    const ADReal p = _fp.p_from_v_e(1.0 / rho, e);

    const ADReal rho_b = _fp.rho_from_p_T(p, _T);
    const ADReal rhouA_b = rho_b * _vel * A;
    const ADReal e_b = _fp.e_from_p_rho(p, rho_b);
    const ADReal E_b = e_b + 0.5 * _vel * _vel;

    U_ghost[THMVACE1D::RHOA] = rho_b * A;
    U_ghost[THMVACE1D::RHOUA] = rhouA_b;
    U_ghost[THMVACE1D::RHOEA] = rho_b * E_b * A;
    U_ghost[THMVACE1D::AREA] = A;
  }
  else
  {
    U_ghost[THMVACE1D::RHOA] = rhoA;
    U_ghost[THMVACE1D::RHOUA] = rhoA * _vel;
    U_ghost[THMVACE1D::RHOEA] = rhoEA;
    U_ghost[THMVACE1D::AREA] = A;
  }

  return U_ghost;
}
