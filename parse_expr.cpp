#include <iostream>
#include <string>

#include <boost/fusion/include/adapt_struct.hpp>
#include <boost/spirit/include/phoenix_core.hpp>
#include <boost/spirit/include/phoenix_object.hpp>
#include <boost/spirit/include/phoenix_operator.hpp>
#include <boost/spirit/include/qi.hpp>

#include "ast.hpp"

namespace ascii 	= boost::spirit::ascii;
namespace fusion	= boost::fusion;
namespace phoenix 	= boost::phoenix;
namespace spirit	= boost::spirit;
namespace qi 		= boost::spirit::qi;

/*
BOOST_FUSION_ADAPT_STRUCT(
	ast::binary_operation_node,
	(ast::expression_node, lhs)
	(ast::binary_operation, op)
	(ast::expression_node, rhs)
)
*/

namespace parser
{

template <typename Iterator>
struct expression_grammar : public qi::grammar<Iterator, ast::expression_node(), ascii::space_type>
{
	expression_grammar()
		: expression_grammar::base_type(expression)
	{
		using phoenix::construct;

		using spirit::_1;
		using spirit::_a;
		using spirit::_val;

		using qi::attr;
		using qi::uint_;
		using qi::lit;
		using qi::real_parser;
		using qi::strict_ureal_policies;

		// the operator tokens

		// binary operators
		multiply_op %= lit('*') >> attr(ast::MULTIPLY);
		divide_op %= lit('/') >> attr(ast::DIVIDE);
		modulo_op %= lit('%') >> attr(ast::MODULO);
		add_op %= lit('+') >> attr(ast::ADD);
		subtract_op %= lit('-') >> attr(ast::SUBTRACT);
		sequence_op %= lit(',') >> attr(ast::SEQUENCE);

		// unary operators
		post_inc_op %= lit("++") >> attr(ast::POST_INCREMENT);
		post_dec_op %= lit("--") >> attr(ast::POST_DECREMENT);
		pre_inc_op %= lit("++") >> attr(ast::PRE_INCREMENT);
		pre_dec_op %= lit("--") >> attr(ast::PRE_DECREMENT);
		positive_op %= lit('+') >> attr(ast::POSITIVE);
		negative_op %= lit('-') >> attr(ast::NEGATIVE);
		invert_op %= lit('~') >> attr(ast::INVERT);
		not_op %= lit('!') >> attr(ast::NOT);

		// primary expressions

		primary %= constant_double | constant_integer;
		constant_double %= real_parser< double, strict_ureal_policies<double> >();
		constant_integer %= uint_;

		// list operators in decreasing order of precedence

		parenthetical %= lit('(') >> expression >> lit(')') | primary;

		post_inc_dec = parenthetical [_val = _1] >> *( (post_inc_op | post_dec_op) [ _val = construct<ast::unary_operation_node>(_1, _val) ] );

		unary =	*( (pre_inc_op | pre_dec_op | positive_op | negative_op | invert_op | not_op) ) >> post_inc_dec [_val = _1];

		multiplicative = unary [_val = _1] >>
			*(
				(multiply_op | divide_op | modulo_op) [_a = _1] >>
				unary [_val = construct<ast::binary_operation_node>(_a, _val, _1)]
			);

		additive = multiplicative [_val = _1] >>
			*(
				(add_op | subtract_op) [_a = _1] >>
				multiplicative [_val = construct<ast::binary_operation_node>(_a, _val, _1)]
			);

		sequence = additive [_val = _1] >>
			*(
				sequence_op [_a = _1] >>
				additive [_val = construct<ast::binary_operation_node>(_a, _val, _1)]
			);

		// an expression starts trying to match the lowest precedence operators

		expression %= sequence;
	}

	typedef qi::rule<Iterator, ast::expression_node(), ascii::space_type>
		expression_node_rule_type;

	typedef qi::rule<Iterator, ast::binary_operation(), ascii::space_type>
		binary_operation_rule_type;

	typedef qi::rule<Iterator, ast::expression_node(), qi::locals<ast::binary_operation>, ascii::space_type>
		binary_operation_node_rule_type;

	typedef qi::rule<Iterator, ast::unary_operation(), ascii::space_type>
		unary_operation_rule_type;

	typedef qi::rule<Iterator, ast::expression_node(), qi::locals<ast::unary_operation>, ascii::space_type>
		unary_operation_node_rule_type;

	expression_node_rule_type 		expression;
	expression_node_rule_type 		primary;
	expression_node_rule_type 		parenthetical;

	qi::rule<Iterator, ast::constant_double_node(), ascii::space_type>	constant_double;
	qi::rule<Iterator, ast::constant_integer_node(), ascii::space_type> 	constant_integer;

	binary_operation_rule_type 		multiply_op;
	binary_operation_rule_type 		divide_op;
	binary_operation_rule_type 		modulo_op;
	binary_operation_rule_type 		add_op;
	binary_operation_rule_type 		subtract_op;
	binary_operation_rule_type 		sequence_op;

	binary_operation_node_rule_type 	multiplicative;
	binary_operation_node_rule_type 	additive;
	binary_operation_node_rule_type 	sequence;

	unary_operation_rule_type 		post_inc_op;
	unary_operation_rule_type 		post_dec_op;
	unary_operation_rule_type 		pre_inc_op;
	unary_operation_rule_type 		pre_dec_op;
	unary_operation_rule_type 		positive_op;
	unary_operation_rule_type 		negative_op;
	unary_operation_rule_type 		invert_op;
	unary_operation_rule_type 		not_op;

	unary_operation_node_rule_type		post_inc_dec;
	unary_operation_node_rule_type		unary;
};

}

int main(int argc, char** argv)
{
	std::string input;

	for(int i=1; i<argc; ++i)
	{
		if(i != 1)
			input += " ";
		input += argv[i];
	}

	std::cout << "input: '" << input << "'" << std::endl;

	parser::expression_grammar<std::string::const_iterator> expression;
	std::string::const_iterator first(input.begin());
	std::string::const_iterator last(input.end());
	ast::expression_node expr;
	bool r = qi::phrase_parse(first, last, expression, ascii::space, expr);

	if(first != last)
	{
		std::cerr << "not all input consumed." << std::endl;
		return 1;
	}

	if(!r)
	{
		std::cerr << "parsing failed." << std::endl;
		return 1;
	}

	ast::ast_printer print;
	std::cout << "output:" << std::endl;
	print(expr);

	return 0;
}
