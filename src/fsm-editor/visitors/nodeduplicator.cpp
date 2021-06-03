#include "nodeduplicator.hpp"

#include "../nodes/nodes.hpp"
#include "../editor.hpp"

namespace fsme
{
namespace visitors
{

void NodeDuplicator::visit(nodes::CondNode& node)
{
	auto& editor = node.editor();

	auto& clone = editor.make_node<nodes::CondNode>();
	setup_generic_clone(node, clone);

	const std::size_t output_count = node.outputs().size();
	clone.set_output_count(output_count);
	for (std::size_t i = 0; i < output_count; ++i)
	{
		clone.get_expression(clone.outputs()[i]) = node.get_expression(node.outputs()[i]);
	}
}

void NodeDuplicator::visit(nodes::IfNode& node)
{
	auto& editor = node.editor();

	auto& clone = editor.make_node<nodes::IfNode>();
	setup_generic_clone(node, clone);
	clone.get_expression() = node.get_expression();
}

void NodeDuplicator::visit(nodes::StateNode& node)
{
	auto& editor = node.editor();

	auto& clone = editor.make_node<nodes::StateNode>();
	setup_generic_clone(node, clone);
	clone.get_name_input().set_text(node.get_name_input().get_text() + " (new)");
}

void NodeDuplicator::setup_generic_clone(Node& original, Node& clone)
{
	ed::SelectNode(clone.node_id());
	place_node_under(clone, original);
}

void NodeDuplicator::place_node_under(Node& bottom, Node& top)
{
	const ImVec2 top_position = ed::GetNodePosition(top.node_id());
	const ImVec2 top_size = ed::GetNodeSize(top.node_id());

	const ImVec2 bottom_position(top_position.x, top_position.y + top_size.y + 8);

	ed::SetNodePosition(bottom.node_id(), bottom_position);
}

}
}
