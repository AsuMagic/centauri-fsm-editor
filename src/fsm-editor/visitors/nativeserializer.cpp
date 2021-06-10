#include "nativeserializer.hpp"

#include "../editor.hpp"
#include "../nodes/nodes.hpp"
#include "../widgets/boolexprinput.hpp"

namespace fsme
{
namespace visitors
{

void NativeSerializer::serialize(FsmEditor& editor, std::ostream& output)
{
	NativeSerializer serializer(output);

	auto& state = editor.m_state;

	serializer.write_memcpy(native_format::magic_header);

	serializer.write_memcpy(std::uint64_t(state.last_allocated_id));

	serializer.write_memcpy(native_format::pins_magic);
	serializer.write_container(state.pins, [&](const auto& p) {
		const ed::PinId& pin_id = p.first;
		const PinInfo& pin = p.second;

		serializer.write_memcpy(std::uint64_t(pin_id));
		serializer.write_container(pin.links, [&](const LinkInfo& link) {
			serializer.write_memcpy(std::uint64_t(link.id));
			serializer.write_memcpy(std::uint64_t(link.pins.from));
			serializer.write_memcpy(std::uint64_t(link.pins.to));
		});
		serializer.write_memcpy(std::uint64_t(pin.node_id));
	});

	serializer.write_memcpy(native_format::links_magic);
	serializer.write_container(state.links, [&](const auto& p) {
		const ed::LinkId& link_id = p.first;
		const PinPair& pins = p.second;

		serializer.write_memcpy(std::uint64_t(link_id));
		serializer.write_memcpy(std::uint64_t(pins.from));
		serializer.write_memcpy(std::uint64_t(pins.to));
	});

	serializer.write_memcpy(native_format::nodes_magic);
	serializer.write_container(state.nodes, [&](const auto& p) {
		p.second->accept(serializer);
	});
}

void NativeSerializer::visit(nodes::CondNode& node)
{
	write_memcpy(native_format::NodeType::COND);
	write(node);

	for (const auto& output : node.outputs())
	{
		write(node.get_expression(output));
	}
}

void NativeSerializer::visit(nodes::IfNode& node)
{
	write_memcpy(native_format::NodeType::IF);
	write(node);
	write(node.get_expression());
}

void NativeSerializer::visit(nodes::StateNode& node)
{
	write_memcpy(native_format::NodeType::STATE);
	write(node);
	write_char_array(node.get_name_input().get_text());
}

NativeSerializer::NativeSerializer(std::ostream& output) :
	m_out(&output)
{}

void NativeSerializer::write(const Node& node)
{
	write_memcpy(std::uint64_t(node.node_id()));

	write_container(node.m_inputs, [&](ed::PinId input) {
		write_memcpy(std::uint64_t(input));
	});

	write_container(node.m_outputs, [&](ed::PinId output) {
		write_memcpy(std::uint64_t(output));
	});
}

void NativeSerializer::write(widgets::BoolExpressionInput& expression)
{
	write_memcpy(std::uint8_t(expression.get_input_type()));
	write_char_array(expression.get_raw_lua_input().text_buffer);
	write_container(
		expression.get_raw_simple_expression_input().options,
		[this](const widgets::BoolExpressionOption* option) {
			write_char_array(option->shorthand);
		}
	);
}

}
}
