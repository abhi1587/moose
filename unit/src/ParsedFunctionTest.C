//* This file is part of the MOOSE framework
//* https://mooseframework.inl.gov
//*
//* All rights reserved, see COPYRIGHT for full restrictions
//* https://github.com/idaholab/moose/blob/master/COPYRIGHT
//*
//* Licensed under LGPL 2.1, please see LICENSE for details
//* https://www.gnu.org/licenses/lgpl-2.1.html

#include "ParsedFunctionTest.h"
#include "MathFVUtils.h"
#include "MooseParsedFunction.h"

#include "libmesh/fe_map.h"
#include "libmesh/quadrature_gauss.h"

using namespace libMesh;

ParsedFunction<Real> *
ParsedFunctionTest::fptr(MooseParsedFunction & f)
{
  return f._function_ptr->_function_ptr.get();
}

InputParameters
ParsedFunctionTest::getParams()
{
  InputParameters params = _factory.getValidParams("ParsedFunction");
  // test constructor with no additional variables
  params.set<FEProblem *>("_fe_problem") = _fe_problem.get();
  params.set<FEProblemBase *>("_fe_problem_base") = _fe_problem.get();
  return params;
}

MooseParsedFunction &
ParsedFunctionTest::buildFunction(InputParameters & params)
{
  const std::string name = "test" + std::to_string(function_index++);
  _fe_problem->addFunction("ParsedFunction", name, params);
  auto & f = _fe_problem->getFunction(name);
  auto parsed_f = dynamic_cast<MooseParsedFunction *>(&f);
  mooseAssert(parsed_f, "Failed to cast");
  return *parsed_f;
}

TEST_F(ParsedFunctionTest, basicConstructor)
{
  auto params = getParams();
  params.set<std::string>("value") = std::string("x + 1.5*y + 2 * z + t/4");
  auto & f = buildFunction(params);
  Moose::Functor<Real> f_wrapped(f);
  f.initialSetup();
  EXPECT_EQ(f.value(4, Point(1, 2, 3)), 11);
  EXPECT_EQ(f.value(4, 1, 2, 3), 11);

  //
  // Test the functor interfaces
  //

  const auto & lm_mesh = _mesh->getMesh();

  Real f_traditional(0);
  Real f_functor(0);
  RealVectorValue gradient_traditional;
  RealVectorValue gradient_functor;
  Real dot_traditional(0);
  Real dot_functor(0);

  auto test_eq = [&f_traditional,
                  &f_functor,
                  &gradient_traditional,
                  &gradient_functor,
                  &dot_traditional,
                  &dot_functor]()
  {
    EXPECT_EQ(f_traditional, f_functor);
    for (const auto i : make_range(unsigned(LIBMESH_DIM)))
      EXPECT_EQ(gradient_traditional(i), gradient_functor(i));
    EXPECT_EQ(dot_traditional, dot_functor);
  };

  // Test elem overloads
  const Elem * const elem = lm_mesh.elem_ptr(0);
  const auto elem_arg = Moose::ElemArg{elem, false};
  const Point vtx_average = elem->vertex_average();
  f_traditional = f.value(0, vtx_average);
  f_functor = f_wrapped(elem_arg, Moose::currentState());
  gradient_traditional = f.gradient(0, vtx_average);
  gradient_functor = f_wrapped.gradient(elem_arg, Moose::currentState());
  dot_traditional = f.timeDerivative(0, vtx_average);
  dot_functor = f_wrapped.dot(elem_arg, Moose::currentState());
  test_eq();

  unsigned int side = libMesh::invalid_uint;
  for (const auto s : elem->side_index_range())
    if (elem->neighbor_ptr(s))
    {
      side = s;
      break;
    }

  // Test face overloads
  _mesh->buildFiniteVolumeInfo();
  const FaceInfo * const fi = _mesh->faceInfo(elem, side);
  auto face = Moose::FaceArg(
      {fi, Moose::FV::LimiterType::CentralDifference, true, false, nullptr, nullptr});
  f_traditional = f.value(0, fi->faceCentroid());
  f_functor = f_wrapped(face, Moose::currentState());
  gradient_traditional = f.gradient(0, fi->faceCentroid());
  gradient_functor = f_wrapped.gradient(face, Moose::currentState());
  dot_traditional = f.timeDerivative(0, fi->faceCentroid());
  dot_functor = f_wrapped.dot(face, Moose::currentState());
  test_eq();

  // Test ElemQp overloads
  const FEFamily mapping_family = FEMap::map_fe_type(*elem);
  const FEType fe_type(elem->default_order(), mapping_family);
  std::unique_ptr<FEBase> fe(FEBase::build(elem->dim(), fe_type));
  const auto & xyz = fe->get_xyz();
  QGauss qrule(elem->dim(), fe_type.default_quadrature_order());
  fe->attach_quadrature_rule(&qrule);
  fe->reinit(elem);
  auto elem_qp = Moose::ElemQpArg({elem, 0, &qrule, xyz[0]});
  f_traditional = f.value(0, xyz[0]);
  f_functor = f_wrapped(elem_qp, Moose::currentState());
  gradient_traditional = f.gradient(0, xyz[0]);
  gradient_functor = f_wrapped.gradient(elem_qp, Moose::currentState());
  dot_traditional = f.timeDerivative(0, xyz[0]);
  dot_functor = f_wrapped.dot(elem_qp, Moose::currentState());
  test_eq();

  // Test ElemSideQp overloads
  QGauss qrule_face(elem->dim() - 1, fe_type.default_quadrature_order());
  fe->attach_quadrature_rule(&qrule_face);
  fe->reinit(elem, side);
  auto elem_side_qp = Moose::ElemSideQpArg({elem, side, 0, &qrule_face, xyz[0]});
  f_traditional = f.value(0, xyz[0]);
  f_functor = f_wrapped(elem_side_qp, Moose::currentState());
  gradient_traditional = f.gradient(0, xyz[0]);
  gradient_functor = f_wrapped.gradient(elem_side_qp, Moose::currentState());
  dot_traditional = f.timeDerivative(0, xyz[0]);
  dot_functor = f_wrapped.dot(elem_side_qp, Moose::currentState());
  test_eq();

  // Test elem_point overloads
  const Point test_point(0.5, 0.5, 0.5);
  const auto elem_point = Moose::ElemPointArg{elem, test_point, false};
  f_traditional = f.value(0, test_point);
  f_functor = f_wrapped(elem_point, Moose::currentState());
  gradient_traditional = f.gradient(0, test_point);
  gradient_functor = f_wrapped.gradient(elem_point, Moose::currentState());
  dot_traditional = f.timeDerivative(0, test_point);
  dot_functor = f_wrapped.dot(elem_point, Moose::currentState());
  test_eq();
}

TEST_F(ParsedFunctionTest, advancedConstructor)
{
  // test the constructor with one variable
  std::vector<std::string> one_var(1);
  one_var[0] = "q";

  auto params = getParams();
  params.set<std::string>("value") = "x + y + q";
  params.set<std::vector<std::string>>("vars") = one_var;
  params.set<std::vector<std::string>>("vals") =
      std::vector<std::string>(1, "-1"); // Dummy value, will be overwritten in test below

  auto & f = buildFunction(params);
  f.initialSetup();
  // Access address via pointer to MooseParsedFunctionWrapper that contains pointer to
  // libMesh::ParsedFunction
  fptr(f)->getVarAddress("q") = 4;
  EXPECT_EQ(f.value(0, Point(1, 2)), 7);
  EXPECT_EQ(f.value(0, 1, 2), 7);

  // test the constructor with three variables
  std::vector<std::string> three_vars(3);
  three_vars[0] = "q";
  three_vars[1] = "w";
  three_vars[2] = "r";

  auto params2 = getParams();
  params2.set<std::string>("value") = "r*x + y/w + q";
  params2.set<std::vector<std::string>>("vars") = three_vars;
  params2.set<std::vector<std::string>>("vals") =
      std::vector<std::string>(3, "-1"); // Dummy values, will be overwritten in test below

  auto & f2 = buildFunction(params2);
  f2.initialSetup();
  fptr(f2)->getVarAddress("q") = 4;
  fptr(f2)->getVarAddress("w") = 2;
  fptr(f2)->getVarAddress("r") = 1.5;
  EXPECT_EQ(f2.value(0, Point(2, 4)), 9);
  EXPECT_EQ(f2.value(0, 2, 4), 9);

  // test the constructor with one variable that's set
  std::vector<std::string> one_val(1);
  one_val[0] = "2.5";

  auto params3 = getParams();
  params3.set<std::string>("value") = "q*x";
  params3.set<std::vector<std::string>>("vars") = one_var;
  params3.set<std::vector<std::string>>("vals") = one_val;

  auto & f3 = buildFunction(params3);
  f3.initialSetup();
  EXPECT_EQ(f3.value(0, 2), 5);

  // test the constructor with three variables
  std::vector<std::string> three_vals(3);
  three_vals[0] = "1.5";
  three_vals[1] = "1";
  three_vals[2] = "0";

  auto params4 = getParams();
  params4.set<std::string>("value") = "q*x + y/r + w";
  params4.set<std::vector<std::string>>("vars") = three_vars;
  params4.set<std::vector<std::string>>("vals") = three_vals;

  auto & f4 = buildFunction(params4);
  f4.initialSetup();
  fptr(f4)->getVarAddress("r") = 2;
  EXPECT_EQ(f4.value(0, Point(2, 4)), 6);
  EXPECT_EQ(f4.value(0, 2, 4), 6);
  fptr(f4)->getVarAddress("r") = 4;
  EXPECT_EQ(f4.value(0, Point(2, 4)), 5);
  EXPECT_EQ(f4.value(0, 2, 4), 5);
}

TEST_F(ParsedFunctionTest, testVariables)
{
  // a lot of this functionality is tested in advancedConstructor as well
  // test one variable, make sure we can change it by the reference any time
  std::vector<std::string> one_var(1);
  one_var[0] = "q";

  auto params = getParams();
  params.set<std::string>("value") = "x + y + q";
  params.set<std::vector<std::string>>("vars") = one_var;
  params.set<std::vector<std::string>>("vals") =
      std::vector<std::string>(1, "-1"); // Dummy value, will be overwritten in test below

  auto & f = buildFunction(params);
  f.initialSetup();
  Real & q = fptr(f)->getVarAddress("q");
  q = 4;
  EXPECT_EQ(f.value(0, Point(1, 2)), 7);
  EXPECT_EQ(f.value(0, 1, 2), 7);
  q = 2;
  EXPECT_EQ(f.value(0, Point(1, 2)), 5);
  EXPECT_EQ(f.value(0, 1, 2), 5);
  q = -4;
  EXPECT_EQ(f.value(0, Point(1, 2)), -1);
  EXPECT_EQ(f.value(0, 1, 2), -1);

  // test three variables, test updating them randomly
  std::vector<std::string> three_vars(3);
  three_vars[0] = "q";
  three_vars[1] = "w";
  three_vars[2] = "r";

  auto params2 = getParams();
  params2.set<std::string>("value") = "r*x + y/w + q";
  params2.set<std::vector<std::string>>("vars") = three_vars;
  params2.set<std::vector<std::string>>("vals") =
      std::vector<std::string>(3, "-1"); // Dummy values, will be overwritten in test below

  auto & f2 = buildFunction(params2);
  f2.initialSetup();
  Real & q2 = fptr(f2)->getVarAddress("q");
  Real & w2 = fptr(f2)->getVarAddress("w");
  Real & r2 = fptr(f2)->getVarAddress("r");
  q2 = 4;
  w2 = 2;
  r2 = 1.5;
  EXPECT_EQ(f2.value(0, Point(2, 4)), 9);
  q2 = 1;
  w2 = 4;
  r2 = 2.5;
  EXPECT_EQ(f2.value(0, Point(2, 4)), 7);
  q2 = 2;
  EXPECT_EQ(f2.value(0, Point(2, 4)), 8);
  w2 = 3;
  EXPECT_EQ(f2.value(0, Point(2, 6)), 9);
}

TEST_F(ParsedFunctionTest, testConstants)
{
  // this functions tests that pi and e get correctly substituted
  // it also tests built in functions of the function parser
  auto params = getParams();
  params.set<std::string>("value") = "log(e) + x";

  auto & f = buildFunction(params);
  f.initialSetup();
  EXPECT_NEAR(2, f.value(0, 1), 0.0000001);

  auto params2 = getParams();
  params2.set<std::string>("value") = "sin(pi*x)";

  auto & f2 = buildFunction(params2);
  f2.initialSetup();
  EXPECT_NEAR(0, f2.value(0, 1), 0.0000001);
  EXPECT_NEAR(1, f2.value(0, 0.5), 0.0000001);
  EXPECT_NEAR(-1, f2.value(0, 1.5), 0.0000001);
}
