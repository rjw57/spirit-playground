#include <boost/variant.hpp>
#include <boost/variant/recursive_variant.hpp>

namespace ast
{

enum unary_operation
{
	POST_INCREMENT,
	POST_DECREMENT,
	PRE_INCREMENT,
	PRE_DECREMENT,
	POSITIVE,
	NEGATIVE,
	INVERT,
	NOT,
};

enum binary_operation
{
	MULTIPLY,
	DIVIDE,
	MODULO,
	ADD,
	SUBTRACT,
	SEQUENCE,
};

struct 		constant_integer_node;
struct		constant_double_node;
struct		binary_operation_node;
struct		unary_operation_node;

typedef boost::variant<
	constant_integer_node,
	constant_double_node,
	boost::recursive_wrapper<binary_operation_node>,
	boost::recursive_wrapper<unary_operation_node>
> 	expression_node;

struct constant_integer_node
{
	int val;

	constant_integer_node()
		: val(0)
	{ }

	constant_integer_node(const constant_integer_node& n)
		: val(n.val)
	{ }

	constant_integer_node(int val)
		: val(val)
	{ }

	constant_integer_node& operator = (int v) { val = v; return *this; }
};

struct constant_double_node
{
	double val;

	constant_double_node()
		: val(0.)
	{ }

	constant_double_node(const constant_double_node& n)
		: val(n.val)
	{ }

	constant_double_node(double val)
		: val(val)
	{ }

	constant_double_node& operator = (double v) { val = v; return *this; }
};

struct unary_operation_node
{
	unary_operation_node(unary_operation op, expression_node expr)
		: op(op), expr(expr)
	{ }

	unary_operation op;
	expression_node expr;
};

struct binary_operation_node
{
	binary_operation_node(binary_operation op, expression_node lhs, expression_node rhs)
		: op(op), lhs(lhs), rhs(rhs)
	{ }

	binary_operation op;
	expression_node lhs;
	expression_node rhs;
};

// print out the AST

struct ast_printer
{
	ast_printer(int indent = 0)
		: indent(indent)
	{ }

	void operator()(const expression_node& expr) const;
	void operator()(const expression_node& expr, int specific_indent) const;

	int indent;
};

struct ast_evaluator
{
	expression_node operator()(const expression_node& expr) const;
};

}
