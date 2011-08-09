project(spirit-playground CXX)
cmake_minimum_required(VERSION 2.8)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall")

enable_testing()

find_package(Boost)
include_directories(${Boost_INCLUDE_DIR})

add_executable(parse_expr
	ast.cpp
	parse_expr.cpp
)

# The header outputted before the result
set(eval_to "Evaluated:")

# The null expression should fail to parse.
add_test(NullExpression parse_expr)
set_tests_properties(NullExpression PROPERTIES WILL_FAIL YES)

add_test(ConstantExpression parse_expr "3")
add_test(ParentheticalExpression1 parse_expr "(3)")
add_test(ParentheticalExpression2 parse_expr "( 3 )")
set_tests_properties(
	ConstantExpression ParentheticalExpression1 ParentheticalExpression2
	PROPERTIES
	PASS_REGULAR_EXPRESSION "${eval_to} 3, integer")

add_test(MultiplicativeExpression1 parse_expr "3 * 4")
set_tests_properties(MultiplicativeExpression1
	PROPERTIES PASS_REGULAR_EXPRESSION "${eval_to} 12, integer")

add_test(MultiplicativeExpression2 parse_expr "3 * 4 / 2")
set_tests_properties(MultiplicativeExpression2
	PROPERTIES PASS_REGULAR_EXPRESSION "${eval_to} 6, integer")

add_test(MultiplicativeExpression3 parse_expr "(3 * (4)) * 2")
add_test(MultiplicativeExpression4 parse_expr "3 * (4 * 2)")
set_tests_properties(
	MultiplicativeExpression3 MultiplicativeExpression4
	PROPERTIES PASS_REGULAR_EXPRESSION "${eval_to} 24, integer")

add_test(AdditiveExpression1 parse_expr "1+2+3+4")
add_test(AdditiveExpression2 parse_expr "1+(2+3)+4")
set_tests_properties(
	AdditiveExpression1 AdditiveExpression2
	PROPERTIES PASS_REGULAR_EXPRESSION "${eval_to} 10, integer")

add_test(AdditiveExpression3 parse_expr "1+(4)")
set_tests_properties(
	AdditiveExpression3
	PROPERTIES PASS_REGULAR_EXPRESSION "${eval_to} 5, integer")

add_test(AdditiveExpression4 parse_expr "1+5")
set_tests_properties(
	AdditiveExpression4
	PROPERTIES PASS_REGULAR_EXPRESSION "${eval_to} 6, integer")

add_test(MulAddExpression1 parse_expr "1+2*3")
add_test(MulAddExpression2 parse_expr "1+(2*3)")
set_tests_properties(
	MulAddExpression1 MulAddExpression2
	PROPERTIES PASS_REGULAR_EXPRESSION "${eval_to} 7, integer")

add_test(MulAddExpression3 parse_expr "(1+2)*2.5")
set_tests_properties(
	MulAddExpression3
	PROPERTIES PASS_REGULAR_EXPRESSION "${eval_to} 7.5, double")

add_test(MulAddExpression4 parse_expr "3*4+2")
set_tests_properties(
	MulAddExpression4
	PROPERTIES PASS_REGULAR_EXPRESSION "${eval_to} 14, integer")

add_test(MulAddExpression5 parse_expr "3*(4+2)")
set_tests_properties(
	MulAddExpression5
	PROPERTIES PASS_REGULAR_EXPRESSION "${eval_to} 18, integer")

add_test(SequenceExpression1 parse_expr "1+2*3, 4*5, 3/2.0")
set_tests_properties(
	SequenceExpression1
	PROPERTIES PASS_REGULAR_EXPRESSION "${eval_to} 1.5, double")

add_test(UnaryExpression1 parse_expr "-5")
set_tests_properties(
	UnaryExpression1
	PROPERTIES PASS_REGULAR_EXPRESSION "${eval_to} -5, integer")

add_test(UnaryExpression2 parse_expr "++--5")
set_tests_properties(
	UnaryExpression2
	PROPERTIES PASS_REGULAR_EXPRESSION "${eval_to} 5, integer")

add_test(UnaryExpression3 parse_expr "-+5")
set_tests_properties(
	UnaryExpression3
	PROPERTIES PASS_REGULAR_EXPRESSION "${eval_to} -5, integer")

add_test(UnaryExpression4 parse_expr "3*-6")
set_tests_properties(
	UnaryExpression4
	PROPERTIES PASS_REGULAR_EXPRESSION "${eval_to} -18, integer")

add_test(UnaryExpression5 parse_expr "2*4.++--++")
set_tests_properties(
	UnaryExpression5
	PROPERTIES PASS_REGULAR_EXPRESSION "${eval_to} 8, double")

add_test(UnaryExpression6 parse_expr "-- --5")
set_tests_properties(
	UnaryExpression6
	PROPERTIES PASS_REGULAR_EXPRESSION "${eval_to} 3, integer")

add_test(UnaryExpression7 parse_expr "-- 5")
set_tests_properties(
	UnaryExpression7
	PROPERTIES PASS_REGULAR_EXPRESSION "${eval_to} 4, integer")
