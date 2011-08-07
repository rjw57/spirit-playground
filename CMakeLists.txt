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

# The null expression should fail to parse.
add_test(NullExpression parse_expr)
set_tests_properties(NullExpression PROPERTIES WILL_FAIL YES)

# A constant should parse correctly.
add_test(ConstantExpression parse_expr "3")

add_test(ParentheticalExpression1 parse_expr "(3)")

add_test(ParentheticalExpression2 parse_expr "( 3 )")

add_test(MultiplicativeExpression1 parse_expr "3 * 4")
add_test(MultiplicativeExpression2 parse_expr "3 * 4 / 5")
add_test(MultiplicativeExpression3 parse_expr "(3 * (4)) * 5")
add_test(MultiplicativeExpression4 parse_expr "3 * (4 * 5)")

add_test(AdditiveExpression1 parse_expr "1+2+3+4")
add_test(AdditiveExpression2 parse_expr "1+(2+3)+4")
add_test(AdditiveExpression3 parse_expr "1+(4)")
add_test(AdditiveExpression4 parse_expr "1+5")

add_test(MulAddExpression1 parse_expr "1+2*3")
add_test(MulAddExpression2 parse_expr "1+(2*3)")
add_test(MulAddExpression3 parse_expr "(1+2)*3")
add_test(MulAddExpression4 parse_expr "3*4+2")
add_test(MulAddExpression5 parse_expr "3*(4+2)")

add_test(SequenceExpression1 parse_expr "1+2*3, 4*5, 3")

add_test(UnaryExpression1 parse_expr "-5")
add_test(UnaryExpression2 parse_expr "++--5")
add_test(UnaryExpression3 parse_expr "-+5")
add_test(UnaryExpression4 parse_expr "3*-6")
add_test(UnaryExpression5 parse_expr "2*4.++--")
