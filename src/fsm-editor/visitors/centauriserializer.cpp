#include "centauriserializer.hpp"

#include "../editor.hpp"
#include "../nodes/nodes.hpp"
#include "../widgets/boolexprinput.hpp"

namespace fsme
{

bool CentauriSerializer::serialize(std::ostream& output, Node& node)
{
	CentauriSerializer serializer(output);
	node.accept(serializer);
	return serializer.m_visited_root;
}

void CentauriSerializer::visit(CondNode& node)
{
	if (!m_visited_root)
	{
		return;
	}

	const auto& editor = node.editor();

	auto id = node.node_id();
	for (long i = 0; i < node.outputs().size(); ++i)
	{
		const ed::PinId& pin = node.outputs()[i];
		const auto& expr = node.get_expression(pin);
		const std::uint32_t next_expr_id = (i + 1) < node.outputs().size()
			? node.get_expression(node.outputs()[i + 1]).get_id()
			: -1;

		emit_branch(
			std::uintptr_t(id),
			expr.as_lua_expression(),
			get_node_id_for_pin(editor, pin),
			next_expr_id
		);

		id = next_expr_id;
	}

	visit_outputs(node);
}

void CentauriSerializer::visit(IfNode& node)
{
	if (!m_visited_root)
	{
		return;
	}

	const auto& editor = node.editor();

	emit_branch(
		std::uintptr_t(node.node_id()),
		node.get_expression().as_lua_expression(),
		get_node_id_for_pin(editor, node.outputs()[0]),
		get_node_id_for_pin(editor, node.outputs()[1])
	);

	visit_outputs(node);
}

void CentauriSerializer::visit(StateNode& node)
{
	if (!m_visited_root)
	{
		m_visited_root = true;
		visit_outputs(node);
	}
	else
	{
		emit_state(std::uintptr_t(node.node_id()), node.get_name_input().get_text());
	}
}

CentauriSerializer::CentauriSerializer(std::ostream& output) :
	m_visited_root(false),
	m_out(&output)
{}

void CentauriSerializer::visit_outputs(Node& node)
{
	for (const ed::PinId& output : node.outputs())
	{
		for (const auto& link : node.editor().get_pin_info(output)->links)
		{
			node.editor().get_node_by_pin_id(link.pins.end)->accept(*this);
		}
	}
}

void CentauriSerializer::emit_state(uint32_t id, const std::string& name)
{
	*m_out << id << " state " << name << '\n';
}

void CentauriSerializer::emit_branch(uint32_t id, const std::string& lua_expression, uint32_t on_true, uint32_t on_false)
{
	*m_out << id << " expr " << lua_expression << ' ' << on_true << ' ' << on_false << '\n';
}

std::uint32_t CentauriSerializer::get_node_id_for_pin(const FsmEditor& editor, ed::PinId pin)
{
	const PinInfo* pin_info = editor.get_pin_info(pin);
	if (pin_info == nullptr)
	{
		return -1;
	}

	if (pin_info->links.empty())
	{
		return -1;
	}

	if (pin_info->links.size() > 1)
	{
		throw std::runtime_error("sorry, more than one output not supported");
	}

	const PinPair& pair = pin_info->links[0].pins;

	return std::uintptr_t(editor.get_node_by_pin_id(pin != pair.start ? pair.start : pair.end)->node_id());
}

}
