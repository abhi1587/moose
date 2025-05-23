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
#include "NSEnums.h"

class INSADObjectTracker;

class INSADMaterial : public Material
{
public:
  static InputParameters validParams();

  INSADMaterial(const InputParameters & parameters);

  virtual void subdomainSetup() override;
  virtual void resolveOptionalProperties() override;

protected:
  virtual void computeQpProperties() override;

  /// velocity
  const ADVectorVariableValue & _velocity;

  /// gradient of velocity
  const ADVectorVariableGradient & _grad_velocity;

  /// gradient of the pressure
  const ADVariableGradient & _grad_p;

  /// viscosity
  const ADMaterialProperty<Real> & _mu;

  /// density
  const ADMaterialProperty<Real> & _rho;

  /// Time derivative of the velocity, e.g. the acceleration
  const ADVectorVariableValue * _velocity_dot;

  /// The strong residual of the mass continuity equation
  ADMaterialProperty<Real> & _mass_strong_residual;

  /// Strong residual corresponding to the momentum advective term
  ADMaterialProperty<RealVectorValue> & _advective_strong_residual;

  /// Strong residual corresponding to the momentum transient term
  ADMaterialProperty<RealVectorValue> & _td_strong_residual;

  /// Strong residual corresponding to the momentum gravity term
  ADMaterialProperty<RealVectorValue> & _gravity_strong_residual;

  /// Strong residual corresponding to the momentum boussinesq term
  ADMaterialProperty<RealVectorValue> & _boussinesq_strong_residual;

  /// Strong residual corresponding to coupled force term
  ADMaterialProperty<RealVectorValue> & _coupled_force_strong_residual;

  /// The mesh velocity
  ADMaterialProperty<RealVectorValue> & _mesh_velocity;

  /// The relative velocity, e.g. velocity - mesh_velocity
  ADMaterialProperty<RealVectorValue> & _relative_velocity;

  /// Strong residual corresponding to advected mesh term
  ADMaterialProperty<RealVectorValue> & _advected_mesh_strong_residual;

  // /// Future addition pending addition of INSADMMSKernel.
  // /// Strong residual corresponding to the mms function term
  // MaterialProperty<RealVectorValue> & _mms_function_strong_residual;

  /// Whether we are on the displaced mesh
  const bool _use_displaced_mesh;

  /// The quadrature points with potential partial derivatives with respect to displacement degrees
  /// of freedom
  const MooseArray<ADPoint> & _ad_q_point;

  /// The radial coordinate index for RZ coordinate systems
  const unsigned int _rz_radial_coord;

  /// The axial coordinate index for RZ coordinate systems
  const unsigned int _rz_axial_coord;

  /// A user object that consumes information from INSAD residual objects and feeds it into this
  /// material
  const INSADObjectTracker * _object_tracker;

  /// Whether the momentum equations are transient
  bool _has_transient;

  /// Whether there is a gravity force in the momentum equation
  bool _has_gravity;

  /// Whether natural convection forces via the Boussinesq approximation are added to the momentum
  /// equation
  bool _has_boussinesq;

  /// Whether there is a force from a coupled vector variable or vector function
  bool _has_coupled_force;

  /// The Boussinesq coefficient
  std::unordered_map<SubdomainID, const ADMaterialProperty<Real> *> _boussinesq_alphas;
  const ADMaterialProperty<Real> * _boussinesq_alpha;

  /// The temperature
  const ADVariableValue * _temperature;

  /// The reference temperature
  std::unordered_map<SubdomainID, const MaterialProperty<Real> *> _ref_temps;
  const MaterialProperty<Real> * _ref_temp;

  /// The viscous form of the equations. This is either "laplace" or "traction"
  NS::ViscousForm _viscous_form;

  /// The gravity vector
  RealVectorValue _gravity_vector;

  /// optionally copuled vector var(s)
  std::vector<const ADVectorVariableValue *> _coupled_force_var;

  /// optional vector function(s)
  std::vector<const Function *> _coupled_force_vector_function;

  /// Whether we have mesh convection
  bool _has_advected_mesh;

  /// The time derivative with respect to x-displacement
  const ADVariableValue * _disp_x_dot;

  /// The time derivative with respect to y-displacement
  const ADVariableValue * _disp_y_dot;

  /// The time derivative with respect to z-displacement
  const ADVariableValue * _disp_z_dot;

  unsigned int _disp_x_num = libMesh::invalid_uint;
  unsigned int _disp_y_num = libMesh::invalid_uint;
  unsigned int _disp_z_num = libMesh::invalid_uint;
  unsigned int _disp_x_sys_num = libMesh::invalid_uint;
  unsigned int _disp_y_sys_num = libMesh::invalid_uint;
  unsigned int _disp_z_sys_num = libMesh::invalid_uint;
};
