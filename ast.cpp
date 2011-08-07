#include <iostream>

#include "ast.hpp"

namespace ast {

std::ostream& operator << (std::ostream& os, binary_operation op)
{
	switch(op)
	{
		case MULTIPLY:
			os << '*';
			break;
		case DIVIDE:
			os << '/';
			break;
		case MODULO:
			os << '%';
			break;
		case ADD:
			os << '+';
			break;
		case SUBTRACT:
			os << '-';
			break;
		case SEQUENCE:
			os << ',';
			break;
		default:
			os << "{err: unknown binary operation: " << (int)op << "}";
			break;
	}

	return os;
}

std::ostream& operator << (std::ostream& os, unary_operation op)
{
	switch(op)
	{
		case POST_INCREMENT:
			os << "++ (post)";
			break;
		case POST_DECREMENT:
			os << "-- (post)";
			break;
		case PRE_INCREMENT:
			os << "++ (pre)";
			break;
		case PRE_DECREMENT:
			os << "-- (pre)";
			break;
		case POSITIVE:
			os << '+';
			break;
		case NEGATIVE:
			os << '-';
			break;
		case INVERT:
			os << '~';
			break;
		case NOT:
			os << '!';
			break;
		default:
			os << "{err: unknown unary operation: " << (int)op << "}";
			break;
	}

	return os;
}

const int tabsize = 4;

void tab(int indent)
{
	for(int i=0; i < indent; ++i)
		std::cout << ' ';
}

struct ast_node_printer : boost::static_visitor<>
{
	ast_node_printer(int indent = 0)
		: indent(indent)
	{ }

	void operator() (const constant_integer_node& node) const
	{
		tab(indent);
		std::cout << "constant integer: " << node.val << std::endl;
	}

	void operator() (const constant_double_node& node) const
	{
		tab(indent);
		std::cout << "constant double: " << node.val << std::endl;
	}

	void operator() (const binary_operation_node& node) const
	{
		ast_printer print(indent + tabsize);

		tab(indent); std::cout << "operator: " << node.op << std::endl;
		tab(indent); std::cout << "`- lhs" << std::endl;
		print(node.lhs);
		tab(indent); std::cout << "`- rhs" << std::endl;
		print(node.rhs);
	}

	void operator() (const unary_operation_node& node) const
	{
		ast_printer print(indent + tabsize);

		tab(indent); std::cout << "operator: " << node.op << std::endl;
		tab(indent); std::cout << "`- expression" << std::endl;
		print(node.expr);
	}

	int indent;
};

void ast_printer::operator() (const expression_node& expr) const
{
	boost::apply_visitor(ast_node_printer(indent), expr);
}

}
