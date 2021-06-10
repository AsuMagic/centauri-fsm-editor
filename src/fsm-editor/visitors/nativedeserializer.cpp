#include "nativedeserializer.hpp"

#include "../editor.hpp"
#include "../nodes/nodes.hpp"
#include "../widgets/boolexprinput.hpp"

// This place is not a place of honor.
// No highly esteemed deed is commemorated here.
// Nothing valued is here.

// The danger is unleashed only if you substantially disturb this place physically.
// This place is best shunned and left uninhabited.

namespace fsme
{
namespace visitors
{

void NativeDeserializer::deserialize(FsmEditor& editor, std::istream& input)
{
	NativeDeserializer deserializer(editor, input);

	editor.clear();
	auto& state = editor.m_state;

	deserializer.expect_magic(native_format::magic_header);

	state.last_allocated_id = deserializer.read_memcpy<std::uint64_t>();

	deserializer.expect_magic(native_format::pins_magic);
	deserializer.read_container(state.pins, [&] {
		ed::PinId pin_id = deserializer.read_memcpy<std::uint64_t>();

		PinInfo pin;

		deserializer.read_container(pin.links, [&] {
			LinkInfo link;
			link.id = deserializer.read_memcpy<std::uint64_t>();
			link.pins.from = deserializer.read_memcpy<std::uint64_t>();
			link.pins.to = deserializer.read_memcpy<std::uint64_t>();
			pin.links.emplace_back(link);
		});

		pin.node_id = deserializer.read_memcpy<std::uint64_t>();
		state.pins.emplace(std::make_pair(pin_id, pin));
	});

	deserializer.expect_magic(native_format::links_magic);
	deserializer.read_container(state.links, [&] {
		ed::LinkId link_id = deserializer.read_memcpy<std::uint64_t>();

		PinPair pins;
		pins.from = deserializer.read_memcpy<std::uint64_t>();
		pins.to = deserializer.read_memcpy<std::uint64_t>();

		state.links.emplace(std::make_pair(link_id, pins));
	});

	deserializer.expect_magic(native_format::nodes_magic);
	deserializer.read_container(state.nodes, [&] {
		const auto node_type = deserializer.read_memcpy<native_format::NodeType>();
		ed::NodeId node_id = deserializer.read_memcpy<std::uint64_t>();

		auto node = deserializer.make_node_from_type(node_id, node_type);

		// HACK: the node may have created pins when constructed, but we're hard loading them in the native format
		//       this needs to be known by the node implementations or it might break things; this is not a problem here
		node->resize_pins(node->m_inputs, 0);
		node->resize_pins(node->m_outputs, 0);

		deserializer.read_container(node->m_inputs, [&] {
			node->m_inputs.emplace_back(deserializer.read_memcpy<std::uint64_t>());
		});

		deserializer.read_container(node->m_outputs, [&] {
			node->m_outputs.emplace_back(deserializer.read_memcpy<std::uint64_t>());
		});

		node->accept(deserializer);

		state.nodes.emplace(std::make_pair(node_id, std::move(node)));
	});
}

void NativeDeserializer::visit(nodes::CondNode& node)
{
	for (const auto& output : node.outputs())
	{
		read(node.get_expression(output));
	}
}

void NativeDeserializer::visit(nodes::IfNode& node)
{
	read(node.get_expression());
}

void NativeDeserializer::visit(nodes::StateNode& node)
{
	read_char_array(node.get_name_input().get_buffer());
}

NativeDeserializer::NativeDeserializer(FsmEditor& editor, std::istream& input) :
	m_editor(&editor),
	m_in(&input)
{}

void NativeDeserializer::read(widgets::BoolExpressionInput& expression)
{
	expression.set_input_type(widgets::ExpressionInputType(read_memcpy<std::uint8_t>()));
	read_char_array(expression.get_raw_lua_input().text_buffer);

	auto& options = expression.get_raw_simple_expression_input().options;
	read_container(options, [&] {
		auto* autocomplete = m_editor->get_autocomplete_provider();

		if (autocomplete == nullptr)
		{
			return;
		}

		auto* option = autocomplete->find_by_shorthand(read_string());

		if (option != nullptr)
		{
			options.insert(option);
		}
	});
}

void NativeDeserializer::expect_magic(std::uint32_t magic)
{
	const auto obtained = read_memcpy<std::uint32_t>();
	if (obtained != magic)
	{
		throw std::runtime_error("Unexpected magic value obtained");
	}
}

std::string NativeDeserializer::read_string()
{
	const auto size = read_memcpy<std::uint64_t>();
	std::string ret(size, '\0');
	m_in->read(&ret[0], size);
	return ret;
}

std::unique_ptr<Node> NativeDeserializer::make_node_from_type(ed::NodeId id, native_format::NodeType node_type)
{
	switch (node_type)
	{
	case native_format::NodeType::STATE: return std::make_unique<nodes::StateNode>(*m_editor, id);
	case native_format::NodeType::COND: return std::make_unique<nodes::CondNode>(*m_editor, id);
	case native_format::NodeType::IF: return std::make_unique<nodes::IfNode>(*m_editor, id);
	}

	throw std::runtime_error("Incorrect node type found while deserializing");
}

}
}
