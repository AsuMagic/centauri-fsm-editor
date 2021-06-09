#include "nativeserializer.hpp"

#include "../editor.hpp"
#include "../nodes/nodes.hpp"
#include "../widgets/boolexprinput.hpp"

namespace fsme
{
namespace visitors
{

void NativeSerializer::visit(nodes::CondNode& node)
{
	serialize_node_header(node);
	for (const auto& output : node.outputs())
	{
		serialize_expression(node.get_expression(output));
	}
}

void NativeSerializer::visit(nodes::IfNode& node)
{
	serialize_node_header(node);
	serialize_expression(node.get_expression());
}

void NativeSerializer::visit(nodes::StateNode& node)
{
	serialize_node_header(node);
	serialize_char_array(node.get_name_input().get_text());
}

NativeSerializer::NativeSerializer(std::ostream& output) :
	m_out(&output)
{}

void NativeSerializer::serialize_node_header(const Node& node)
{
	serialize_memcpy(std::uint64_t(node.node_id()));

	serialize_memcpy(std::uint64_t(node.inputs().size()));
	for (const auto& input : node.inputs())
	{
		serialize_memcpy(std::uint64_t(input));
	}

	serialize_memcpy(std::uint64_t(node.outputs().size()));
	for (const auto& output : node.outputs())
	{
		serialize_memcpy(std::uint64_t(output));
	}
}

void NativeSerializer::serialize_expression(widgets::BoolExpressionInput& expression)
{
	serialize_memcpy(std::uint64_t(expression.get_id()));
	serialize_memcpy(std::uint8_t(expression.get_input_type()));
	serialize_char_array(expression.get_raw_lua_input().text_buffer);
	serialize_container(
		expression.get_raw_simple_expression_input().options,
		[this](const widgets::BoolExpressionOption* option) {
			serialize_char_array(option->shorthand);
		}
	);
}

}
}
