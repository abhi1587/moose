//* This file is part of the MOOSE framework
//* https://mooseframework.inl.gov
//*
//* All rights reserved, see COPYRIGHT for full restrictions
//* https://github.com/idaholab/moose/blob/master/COPYRIGHT
//*
//* Licensed under LGPL 2.1, please see LICENSE for details
//* https://www.gnu.org/licenses/lgpl-2.1.html

#include "LeadBismuthFluidProperties.h"

registerMooseObject("FluidPropertiesApp", LeadBismuthFluidProperties);

InputParameters
LeadBismuthFluidProperties::validParams()
{
  InputParameters params = SinglePhaseFluidProperties::validParams();
  params.addClassDescription("Fluid properties for Lead Bismuth eutectic 2LiF-BeF2");

  return params;
}

LeadBismuthFluidProperties::LeadBismuthFluidProperties(const InputParameters & parameters)
  : SinglePhaseFluidProperties(parameters)
{
}

std::string
LeadBismuthFluidProperties::fluidName() const
{
  return "LeadBismuth";
}

Real
LeadBismuthFluidProperties::molarMass() const
{
  return 2.3399e-1;
}

Real
LeadBismuthFluidProperties::bulk_modulus_from_p_T(Real /*p*/, Real T) const
{
  // Isentropic bulk modulus
  return (38.02 - 1.296e-2 * T + 1.32e-6 * T * T) * 1e9;
}

Real
LeadBismuthFluidProperties::c_from_v_e(Real v, Real e) const
{
  Real T = T_from_v_e(v, e);
  return 1855 - 0.212 * T;
}

ADReal
LeadBismuthFluidProperties::c_from_v_e(const ADReal & v, const ADReal & e) const
{
  ADReal T = SinglePhaseFluidProperties::T_from_v_e(v, e);
  return 1855 - 0.212 * T;
}

Real
LeadBismuthFluidProperties::p_from_v_e(Real v, Real e) const
{
  Real h = h_from_v_e(v, e);
  return (h - e) / v;
}

void
LeadBismuthFluidProperties::p_from_v_e(Real v, Real e, Real & p, Real & dp_dv, Real & dp_de) const
{
  p = p_from_v_e(v, e);
  Real h, dh_dv, dh_de;
  h_from_v_e(v, e, h, dh_dv, dh_de);
  dp_dv = (v * dh_dv - h + e) / v / v;
  dp_de = (dh_de - 1) / v;
}

Real
LeadBismuthFluidProperties::T_from_v_e(Real v, Real /*e*/) const
{
  return (1 / v - 11065) / -1.293;
}

void
LeadBismuthFluidProperties::T_from_v_e(Real v, Real e, Real & T, Real & dT_dv, Real & dT_de) const
{
  T = T_from_v_e(v, e);
  dT_de = 0;
  dT_dv = 1 / v / v / 1.293;
}

Real
LeadBismuthFluidProperties::cp_from_v_e(Real v, Real e) const
{
  Real T = T_from_v_e(v, e);
  return 164.8 - 3.94e-2 * T + 1.25e-5 * T * T - 4.56e+5 / T / T;
}

void
LeadBismuthFluidProperties::cp_from_v_e(
    Real v, Real e, Real & cp, Real & dcp_dv, Real & dcp_de) const
{
  Real T, dT_dv, dT_de;
  T_from_v_e(v, e, T, dT_dv, dT_de);
  cp = cp_from_v_e(v, e);
  dcp_dv = -3.94e-2 * dT_dv + 2 * dT_dv * 1.25e-5 * T + 2 * dT_dv * 4.56e+5 / T / T / T;

  dcp_de = -3.94e-2 * dT_de + 2 * dT_de * 1.25e-5 * T + 2 * dT_de * 4.56e+5 / T / T / T;
}
Real
LeadBismuthFluidProperties::cv_from_v_e(Real v, Real e) const
{
  Real p = p_from_v_e(v, e);
  Real T = T_from_v_e(v, e);
  return cv_from_p_T(p, T);
}

void
LeadBismuthFluidProperties::cv_from_v_e(
    Real v, Real e, Real & cv, Real & dcv_dv, Real & dcv_de) const
{
  Real p, dp_dv, dp_de;
  p_from_v_e(v, e, p, dp_dv, dp_de);
  Real T, dT_dv, dT_de;
  T_from_v_e(v, e, T, dT_dv, dT_de);
  Real dcv_dp, dcv_dT;
  cv_from_p_T(p, T, cv, dcv_dp, dcv_dT);
  dcv_dv = dcv_dp * dp_dv + dcv_dT * dT_dv;
  dcv_de = dcv_dp * dp_de + dcv_dT * dT_de;
}

Real
LeadBismuthFluidProperties::cv_from_p_T(Real p, Real T) const
{
  Real rho, drho_dT, drho_dp;
  rho_from_p_T(p, T, rho, drho_dp, drho_dT);
  Real alpha = -drho_dT / rho;
  Real bulk_modulus = bulk_modulus_from_p_T(p, T);
  Real cp = cp_from_p_T(p, T);
  return cp / (1 + alpha * alpha * bulk_modulus * T / rho / cp);
}

void
LeadBismuthFluidProperties::cv_from_p_T(
    Real p, Real T, Real & cv, Real & dcv_dp, Real & dcv_dT) const
{
  Real cp, dcp_dp, dcp_dT;
  cp_from_p_T(p, T, cp, dcp_dp, dcp_dT);
  cv = cv_from_p_T(p, T);

  Real rho, drho_dT, drho_dp;
  rho_from_p_T(p, T, rho, drho_dp, drho_dT);
  Real alpha = -drho_dT / rho;
  Real alpha_2 = alpha * alpha, alpha_3 = alpha * alpha * alpha;
  Real dalpha_dT = drho_dT * drho_dT / rho / rho;
  Real bulk = bulk_modulus_from_p_T(p, T);
  Real dbulk_dT = (-1.296e-2 + 2 * 1.32e-6 * T) * 1e9;
  Real denominator = (1 + alpha * alpha * bulk * T / rho / cp);
  // no pressure dependence in alpha, T, bulk modulus, cp or rho
  dcv_dp = 0;
  dcv_dT = dcp_dT / denominator -
           cp / denominator / denominator *
               (2 * alpha * dalpha_dT * bulk * T / rho / cp + alpha_2 * dbulk_dT * T / rho / cp +
                alpha_2 * bulk / rho / cp + alpha_3 * bulk * T / rho / cp -
                dcp_dT * alpha_2 * bulk * T / rho / cp / cp);
}

Real
LeadBismuthFluidProperties::mu_from_v_e(Real v, Real e) const
{
  Real T = T_from_v_e(v, e);
  return 4.94e-4 * std::exp(754.1 / T);
}

void
LeadBismuthFluidProperties::mu_from_v_e(
    Real v, Real e, Real & mu, Real & dmu_dv, Real & dmu_de) const
{
  Real T, dT_dv, dT_de;
  T_from_v_e(v, e, T, dT_dv, dT_de);
  mu = mu_from_v_e(v, e);
  dmu_dv = dT_dv * -754.1 * 4.94e-4 * exp(754.1 / T) / T / T;
  dmu_de = dT_de * -754.1 * 4.94e-4 * exp(754.1 / T) / T / T;
}

Real
LeadBismuthFluidProperties::k_from_v_e(Real v, Real e) const
{
  Real T = T_from_v_e(v, e);
  return 3.284 + 1.617e-2 * T - 2.305e-6 * T * T;
}

void
LeadBismuthFluidProperties::k_from_v_e(Real v, Real e, Real & k, Real & dk_dv, Real & dk_de) const
{
  Real T, dT_dv, dT_de;
  T_from_v_e(v, e, T, dT_dv, dT_de);
  k = k_from_v_e(v, e);
  dk_dv = 1.617e-2 * dT_dv - 2 * 2.305e-6 * dT_dv * T;
  dk_de = 1.617e-2 * dT_de - 2 * 2.305e-6 * dT_de * T;
}

Real
LeadBismuthFluidProperties::rho_from_p_T(Real /*p*/, Real T) const
{
  return 11065 - 1.293 * T;
}

void
LeadBismuthFluidProperties::rho_from_p_T(
    Real p, Real T, Real & rho, Real & drho_dp, Real & drho_dT) const
{
  rho = rho_from_p_T(p, T);
  drho_dp = 0;
  drho_dT = -1.293;
}

void
LeadBismuthFluidProperties::rho_from_p_T(
    const ADReal & p, const ADReal & T, ADReal & rho, ADReal & drho_dp, ADReal & drho_dT) const
{
  rho = SinglePhaseFluidProperties::rho_from_p_T(p, T);
  drho_dp = 0;
  drho_dT = -1.293;
}

Real
LeadBismuthFluidProperties::v_from_p_T(Real p, Real T) const
{
  return 1.0 / rho_from_p_T(p, T);
}

void
LeadBismuthFluidProperties::v_from_p_T(Real p, Real T, Real & v, Real & dv_dp, Real & dv_dT) const
{
  v = v_from_p_T(p, T);
  dv_dp = 0;
  dv_dT = 1.293 / MathUtils::pow(11065 - 1.293 * T, 2);
}

Real
LeadBismuthFluidProperties::h_from_p_T(Real /*p*/, Real T) const
{
  // see 2.55 in 2005 NEA Lead Handbook
  // 4.167e-6 is replaced by 1.25e-5/3 for accuracy
  return 164.8 * (T - _T_mo) - 1.97e-2 * (T * T - _T_mo * _T_mo) +
         (1.25e-5 / 3) * (T * T * T - _T_mo * _T_mo * _T_mo) + 4.56e+5 * (1 / T - 1 / _T_mo);
}

void
LeadBismuthFluidProperties::h_from_p_T(Real p, Real T, Real & h, Real & dh_dp, Real & dh_dT) const
{
  h = h_from_p_T(p, T);
  dh_dp = 0;
  dh_dT = cp_from_p_T(p, T);
}

Real
LeadBismuthFluidProperties::h_from_v_e(Real v, Real e) const
{
  Real T = T_from_v_e(v, e);
  return 164.8 * (T - _T_mo) - 1.97e-2 * (T * T - _T_mo * _T_mo) +
         (1.25e-5 / 3) * (T * T * T - _T_mo * _T_mo * _T_mo) + 4.56e+5 * (1 / T - 1 / _T_mo);
}

void
LeadBismuthFluidProperties::h_from_v_e(Real v, Real e, Real & h, Real & dh_dv, Real & dh_de) const
{
  Real T, dT_dv, dT_de;
  T_from_v_e(v, e, T, dT_dv, dT_de);
  h = h_from_v_e(v, e);
  Real cp = cp_from_v_e(v, e);
  dh_dv = cp * dT_dv;
  dh_de = cp * dT_de;
}

Real
LeadBismuthFluidProperties::e_from_p_T(Real p, Real T) const
{
  // definition of h = e + p * v
  Real v = v_from_p_T(p, T);
  Real h = h_from_p_T(p, T);
  return h - p * v;
}

void
LeadBismuthFluidProperties::e_from_p_T(Real p, Real T, Real & e, Real & de_dp, Real & de_dT) const
{
  Real dh_dp, dv_dp, dh_dT, dv_dT, v, h;
  h_from_p_T(p, T, h, dh_dp, dh_dT);
  v_from_p_T(p, T, v, dv_dp, dv_dT);
  e = e_from_p_T(p, T);
  de_dp = dh_dp - v - dv_dp * p;
  de_dT = dh_dT - dv_dT * p;
}

Real
LeadBismuthFluidProperties::e_from_p_rho(Real p, Real rho) const
{
  return e_from_p_T(p, T_from_p_rho(p, rho));
}

void
LeadBismuthFluidProperties::e_from_p_rho(
    Real p, Real rho, Real & e, Real & de_dp, Real & de_drho) const
{
  Real T, dT_dp, dT_drho;
  T_from_p_rho(p, rho, T, dT_dp, dT_drho);
  Real de_dp_T, de_dT;
  e_from_p_T(p, T, e, de_dp_T, de_dT);
  de_dp = de_dp_T * 1 + de_dT * dT_dp;
  de_drho = de_dT * dT_drho;
}

Real
LeadBismuthFluidProperties::T_from_p_rho(Real /*p*/, Real rho) const
{
  return (rho - 11065) / -1.293;
}

void
LeadBismuthFluidProperties::T_from_p_rho(
    Real p, Real rho, Real & T, Real & dT_dp, Real & dT_drho) const
{
  T = T_from_p_rho(p, rho);
  dT_dp = 0;
  dT_drho = 1 / -1.293;
}

Real
LeadBismuthFluidProperties::T_from_p_h(Real /*p*/, Real h) const
{
  // Obtained from sympy solving h(T) with T as a symbol
  return -2279.2270619664 *
             std::sqrt(
                 -5.00288972505329e-15 * (283680000.0 * h - 112351848202732.0) /
                     std::pow(
                         5.58786624617756e-6 * h +
                             MathUtils::pow(1.0 - 3.69464146080841e-6 * h, 2) +
                             std::sqrt(0.755198459604694 *
                                           MathUtils::pow(2.52492508612867e-6 * h - 1.0, 3) +
                                       MathUtils::pow(
                                           -5.58786624617756e-6 * h -
                                               MathUtils::pow(1.0 - 3.69464146080841e-6 * h, 2) +
                                               0.0867667825136797,
                                           2)) -
                             0.0867667825136797,
                         1. / 3.) +
                 0.617230605772255 *
                     std::pow(
                         5.58786624617756e-6 * h +
                             MathUtils::pow(1.0 - 3.69464146080841e-6 * h, 2) +
                             std::sqrt(0.755198459604694 *
                                           MathUtils::pow(2.52492508612867e-6 * h - 1.0, 3) +
                                       MathUtils::pow(
                                           -5.58786624617756e-6 * h -
                                               MathUtils::pow(1.0 - 3.69464146080841e-6 * h, 2) +
                                               0.0867667825136797,
                                           2)) -
                             0.0867667825136797,
                         1. / 3.) -
                 1.0) +
         3223.31382276067 *
             std::sqrt(
                 5.27858159332216e-12 * (129917883803.256 - 480000.0 * h) /
                     std::sqrt(
                         -5.00288972505329e-15 * (283680000.0 * h - 112351848202732.0) /
                             std::pow(
                                 5.58786624617756e-6 * h +
                                     MathUtils::pow(1.0 - 3.69464146080841e-6 * h, 2) +
                                     std::sqrt(
                                         0.755198459604694 *
                                             MathUtils::pow(2.52492508612867e-6 * h - 1.0, 3) +
                                         MathUtils::pow(
                                             -5.58786624617756e-6 * h -
                                                 MathUtils::pow(1.0 - 3.69464146080841e-6 * h, 2) +
                                                 0.0867667825136797,
                                             2)) -
                                     0.0867667825136797,
                                 1. / 3.) +
                         0.617230605772255 *
                             std::pow(
                                 5.58786624617756e-6 * h +
                                     MathUtils::pow(1.0 - 3.69464146080841e-6 * h, 2) +
                                     std::sqrt(
                                         0.755198459604694 *
                                             MathUtils::pow(2.52492508612867e-6 * h - 1.0, 3) +
                                         MathUtils::pow(
                                             -5.58786624617756e-6 * h -
                                                 MathUtils::pow(1.0 - 3.69464146080841e-6 * h, 2) +
                                                 0.0867667825136797,
                                             2)) -
                                     0.0867667825136797,
                                 1. / 3.) -
                         1.0) +
                 2.50144486252665e-15 * (283680000.0 * h - 112351848202732.0) /
                     std::pow(
                         5.58786624617756e-6 * h +
                             MathUtils::pow(1.0 - 3.69464146080841e-6 * h, 2) +
                             std::sqrt(0.755198459604694 *
                                           MathUtils::pow(2.52492508612867e-6 * h - 1.0, 3) +
                                       MathUtils::pow(
                                           -5.58786624617756e-6 * h -
                                               MathUtils::pow(1.0 - 3.69464146080841e-6 * h, 2) +
                                               0.0867667825136797,
                                           2)) -
                             0.0867667825136797,
                         1. / 3.) -
                 0.308615302886127 *
                     std::pow(
                         5.58786624617756e-6 * h +
                             MathUtils::pow(1.0 - 3.69464146080841e-6 * h, 2) +
                             std::sqrt(0.755198459604694 *
                                           MathUtils::pow(2.52492508612867e-6 * h - 1.0, 3) +
                                       MathUtils::pow(
                                           -5.58786624617756e-6 * h -
                                               MathUtils::pow(1.0 - 3.69464146080841e-6 * h, 2) +
                                               0.0867667825136797,
                                           2)) -
                             0.0867667825136797,
                         1. / 3.) -
                 1.0) +
         1182.0;
}

void
LeadBismuthFluidProperties::T_from_p_h(Real p, Real h, Real & T, Real & dT_dp, Real & dT_dh) const
{
  T = T_from_p_h(p, h);
  dT_dp = 0;
  // using inverse relation
  Real h1, dh_dp, dh_dT;
  h_from_p_T(p, T, h1, dh_dp, dh_dT);
  dT_dh = 1 / dh_dT;
}

Real
LeadBismuthFluidProperties::cp_from_p_T(Real /*p*/, Real T) const
{
  return 164.8 - 3.94e-2 * T + 1.25e-5 * T * T - 4.56e+5 / T / T;
}

void
LeadBismuthFluidProperties::cp_from_p_T(
    Real p, Real T, Real & cp, Real & dcp_dp, Real & dcp_dT) const
{
  cp = cp_from_p_T(p, T);
  dcp_dp = 0;
  dcp_dT = -3.94e-2 + 2 * 1.25e-5 * T + 2 * 4.56e+5 / T / T / T;
}

Real
LeadBismuthFluidProperties::mu_from_p_T(Real /*p*/, Real T) const
{
  return 4.94e-4 * std::exp(754.1 / T);
}

void
LeadBismuthFluidProperties::mu_from_p_T(
    Real p, Real T, Real & mu, Real & dmu_dp, Real & dmu_dT) const
{
  mu = mu_from_p_T(p, T);
  dmu_dp = 0;
  dmu_dT = -754.1 * 4.94e-4 * exp(754.1 / T) / T / T;
}

Real
LeadBismuthFluidProperties::k_from_p_T(Real /*p*/, Real T) const
{
  return 3.284 + 1.617e-2 * T - 2.305e-6 * T * T;
}

void
LeadBismuthFluidProperties::k_from_p_T(Real p, Real T, Real & k, Real & dk_dp, Real & dk_dT) const
{
  k = k_from_p_T(p, T);
  dk_dp = 0;
  dk_dT = 1.617e-2 - 2 * 2.305e-6 * T;
}
