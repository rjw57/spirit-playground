#include <iostream>
#include <sstream>
#include <stdexcept>

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
		std::cout << node.val << ", integer" << std::endl;
	}

	void operator() (const constant_double_node& node) const
	{
		tab(indent);
		std::cout << node.val << ", double" << std::endl;
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
	ast_printer::operator() (expr, indent); 
}

void ast_printer::operator() (const expression_node& expr, int specific_indent) const
{
	boost::apply_visitor(ast_node_printer(specific_indent), expr);
}

template <typename T>
struct type_traits
{
	static const bool has_modulo = false;
	static T modulo(const T& lhs, const T& rhs)
	{
		throw std::invalid_argument("Cannot take modulo of this type.");
	}
};

template<>
struct type_traits<int>
{
	static const bool has_modulo = true;
	static int modulo(const int& lhs, const int& rhs)
	{
		return lhs % rhs;
	}
};

struct binary_op_evaluator : boost::static_visitor<expression_node>
{
	binary_op_evaluator(binary_operation op)
		: op(op)
	{ }

	expression_node operator() (constant_integer_node lhs, constant_integer_node rhs) const
	{
		return integer_apply(lhs.val, rhs.val);
	}

	expression_node operator() (constant_double_node lhs, constant_double_node rhs) const
	{
		return double_apply(lhs.val, rhs.val);
	}

	expression_node operator() (constant_integer_node lhs, constant_double_node rhs) const
	{
		return double_apply(lhs.val, rhs.val);
	}

	expression_node operator() (constant_double_node lhs, constant_integer_node rhs) const
	{
		return double_apply(lhs.val, rhs.val);
	}

	// catch all error handler
	template <typename LeftType, typename RightType>
	expression_node operator() (LeftType lhs, RightType rhs) const
	{
		ast_printer print(0);
		std::cerr << "BinOp not implemented with" << std::endl;
		std::cerr << "`- LHS:" << std::endl; print(lhs, 4);
		std::cerr << "`- RHS:" << std::endl; print(rhs, 4);
		throw std::invalid_argument("Not implemented.");
	}

	template <typename NodeType, typename ValueType>
	NodeType apply(ValueType lhs, ValueType rhs) const
	{
		switch(op)
		{
			case MULTIPLY:
				return NodeType(lhs * rhs);
				break;
			case DIVIDE:
				return NodeType(lhs / rhs);
				break;
			case MODULO:
				return type_traits<ValueType>::modulo(lhs, rhs);
				break;
			case ADD:
				return NodeType(lhs + rhs);
				break;
			case SUBTRACT:
				return NodeType(lhs - rhs);
				break;
			case SEQUENCE:
				return NodeType(rhs);
				break;
		}

		std::stringstream ss;
		ss << "unhandled binary operation in evaluate: " << op;
		throw std::invalid_argument(ss.str());
	}

	expression_node integer_apply(int lhs, int rhs) const
	{
		return apply<constant_integer_node>(lhs, rhs);
	}

	expression_node double_apply(double lhs, double rhs) const
	{
		return apply<constant_double_node>(lhs, rhs);
	}

	binary_operation	op;
};

struct ast_node_evaluator : boost::static_visitor<expression_node>
{
	expression_node operator() (const constant_integer_node& node) const
	{
		return node;
	}

	expression_node operator() (const constant_double_node& node) const
	{
		return node;
	}

	expression_node operator() (const binary_operation_node& node) const
	{
		ast_evaluator eval;
		binary_op_evaluator bin_op_eval_visitor(node.op);
		expression_node eval_lhs(eval(node.lhs)), eval_rhs(eval(node.rhs));
		return boost::apply_visitor(bin_op_eval_visitor, eval_lhs, eval_rhs);
	}

	expression_node operator() (const unary_operation_node& node) const
	{
		return constant_integer_node(123);
	}
};

expression_node ast_evaluator::operator() (const expression_node& expr) const
{
	return expression_node(boost::apply_visitor(ast_node_evaluator(), expr));
}

}
