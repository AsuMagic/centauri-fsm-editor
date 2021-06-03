#include "editor.hpp"

#include "nodes/nodes.hpp"
#include "visitors/centauriserializer.hpp"
#include "visitors/nodemenurenderer.hpp"
#include "visitors/linkverifier.hpp"

#include <sstream>

namespace fsme
{

FsmEditor::FsmEditor() :
	m_context(ed::CreateEditor()),
	m_autocomplete_provider(nullptr),
	m_last_allocated_id(0)
{
	ed::SetCurrentEditor(m_context);

	auto& style = ed::GetStyle();
	style.LinkStrength = 500.0f;
}

FsmEditor::~FsmEditor()
{
	ed::DestroyEditor(m_context);
}

void FsmEditor::render()
{
	if (ImGui::Begin("FSM editor"))
	{
		ed::SetCurrentEditor(m_context);
		ed::Begin("My Editor");

		for (auto& p : m_nodes)
		{
			ImGui::PushID(p.second.get());
			p.second->accept(m_node_renderer);

			if (is_node_selected(p.second->node_id()))
			{
				ed::Suspend();
				std::ostringstream ss;
				const bool was_serializable = visitors::CentauriSerializer::serialize(ss, *p.second);
				ImGui::SetTooltip("Serializable? %d\n%s", was_serializable, ss.str().c_str());
				ed::Resume();
			}

			ImGui::PopID();
		}

		handle_item_creation();
		handle_item_deletion();

		render_links();

		render_popups();

		refresh_selected_objects();

		ed::End();
	}

	ImGui::End();
}

void FsmEditor::destroy_node(ed::NodeId id)
{
	detail::map_erase(m_nodes, id);
}

ed::PinId FsmEditor::create_pin(ed::NodeId node)
{
	const ed::PinId id = new_unique_id();
	m_pin_infos.emplace(std::make_pair(id, PinInfo{node, {}}));
	return id;
}

void FsmEditor::destroy_pin(ed::PinId pin)
{
	destroy_links_involving(pin);
	detail::map_erase(m_pin_infos, pin);
}

const PinInfo* FsmEditor::get_pin_info(ed::PinId pin) const
{
	const auto it = m_pin_infos.find(pin);
	return (it != m_pin_infos.end()) ? &it->second : nullptr;
}

ed::LinkId FsmEditor::create_link(PinPair pin_pair)
{
	const ed::LinkId id = new_unique_id();

	m_link_infos.emplace(std::make_pair(id, pin_pair));

	LinkInfo link{id, pin_pair};
	m_pin_infos.at(pin_pair.from).links.push_back(link);
	m_pin_infos.at(pin_pair.to).links.push_back(link);

	return id;
}

void FsmEditor::destroy_link(ed::LinkId link)
{
	const auto it = m_link_infos.find(link);
	if (it != m_link_infos.end())
	{
		const PinPair pin_pair = it->second;
		m_link_infos.erase(it);

		detail::erase(m_pin_infos.at(pin_pair.from).links, pin_pair);
		detail::erase(m_pin_infos.at(pin_pair.to).links, pin_pair);
	}
}

const PinPair* FsmEditor::get_link_info(ed::LinkId link) const
{
	const auto it = m_link_infos.find(link);
	return (it != m_link_infos.end()) ? &it->second : nullptr;
}

void FsmEditor::destroy_links_involving(ed::PinId pin)
{
	const auto it = m_pin_infos.find(pin);
	if (it != m_pin_infos.end())
	{
		const auto& pin_infos = it->second;
		while (!pin_infos.links.empty())
		{
			// TODO: could be more efficient, but better
			destroy_link(pin_infos.links.back().id);
		}
	}
}

Node* FsmEditor::get_node_by_id(ed::NodeId id) const
{
	const auto it = m_nodes.find(id);
	return (it != m_nodes.end()) ? &*it->second : nullptr;
}

Node* FsmEditor::get_node_by_pin_id(ed::PinId pin) const
{
	const auto it = m_pin_infos.find(pin);
	return (it != m_pin_infos.end()) ? get_node_by_id(it->second.node_id) : nullptr;
}

bool FsmEditor::is_link_selected(ed::LinkId link) const
{
	return std::find(m_selected_links.begin(), m_selected_links.end(), link) != m_selected_links.end();
}

bool FsmEditor::is_node_selected(ed::NodeId node) const
{
	return std::find(m_selected_nodes.begin(), m_selected_nodes.end(), node) != m_selected_nodes.end();
}

void FsmEditor::handle_item_creation()
{
	if (ed::BeginCreate())
	{
		ed::PinId from;
		ed::PinId to;
		if (ed::QueryNewLink(&from, &to))
		{
			visitors::LinkFeasibility feasibility;
			const auto try_create_link = [&] {
				feasibility = visitors::LinkVerifier::verify(*this, from, to);
				if (feasibility == visitors::LinkFeasibility::FEASIBLE && ed::AcceptNewItem())
				{
					create_link({from, to});
				}

				return feasibility;
			};

			if (try_create_link() != visitors::LinkFeasibility::FEASIBLE)
			{
				ed::RejectNewItem();
				ImGui::SetTooltip("%s", get_feasibility_string(feasibility));
			}
		}
	}
	ed::EndCreate();
}

void FsmEditor::handle_item_deletion()
{
	if (ed::BeginDelete())
	{
		ed::LinkId deleted_link;
		while (ed::QueryDeletedLink(&deleted_link))
		{
			if (ed::AcceptDeletedItem())
			{
				destroy_link(deleted_link);
			}
		}

		ed::NodeId deleted_node;
		while (ed::QueryDeletedNode(&deleted_node))
		{
			if (ed::AcceptDeletedItem())
			{
				destroy_node(deleted_node);
			}
		}
	}
	ed::EndDelete();
}

void FsmEditor::refresh_selected_objects()
{
	m_selected_links.resize(ed::GetSelectedObjectCount());
	m_selected_links.resize(ed::GetSelectedLinks(m_selected_links.data(), m_selected_links.size()));

	m_selected_nodes.resize(ed::GetSelectedObjectCount());
	m_selected_nodes.resize(ed::GetSelectedNodes(m_selected_nodes.data(), m_selected_nodes.size()));
}

void FsmEditor::render_links()
{
	for (const auto& p : m_link_infos)
	{
		const ed::LinkId id = p.first;
		const PinPair& pins = p.second;

		ed::Link(id, pins.from, pins.to);

		if (is_node_selected(get_node_by_pin_id(pins.from)->node_id())
		 || is_node_selected(get_node_by_pin_id(pins.to)->node_id()))
		{
			ed::PushStyleVar(ed::StyleVar_FlowSpeed, 20.0f);
			ed::PushStyleVar(ed::StyleVar_FlowDuration, 0.2f);
			ed::PushStyleVar(ed::StyleVar_FlowMarkerDistance, 20.0f);
			ed::Flow(id);
			ed::PopStyleVar(3);
		}
	}
}

void FsmEditor::render_popups()
{
	const auto origin = ImGui::GetMousePos();

	ed::Suspend();

	if (ed::ShowNodeContextMenu(&m_context_menu_node))
	{
		ImGui::OpenPopup("Node context menu");
	}

	if (ed::ShowPinContextMenu(&m_context_menu_pin))
	{
		ImGui::OpenPopup("Pin context menu");
	}

	if (ed::ShowLinkContextMenu(&m_context_menu_link))
	{
		ImGui::OpenPopup("Link context menu");
	}

	if (ed::ShowBackgroundContextMenu())
	{
		ImGui::OpenPopup("Create new node");
	}

	if (ImGui::BeginPopup("Node context menu"))
	{
		Node* node = get_node_by_id(m_context_menu_node);

		if (node != nullptr)
		{
			node->accept(m_node_menu_renderer);
		}
		else
		{
			ImGui::CloseCurrentPopup();
		}

		ImGui::EndPopup();
	}

	if (ImGui::BeginPopup("Pin context menu"))
	{
		const PinInfo* info = get_pin_info(m_context_menu_pin);

		if (info != nullptr)
		{
			ImGui::Text("Pin has %d links", int(info->links.size()));

			if (ImGui::Button("Unlink"))
			{
				destroy_links_involving(m_context_menu_pin);
				ImGui::CloseCurrentPopup();
			}
		}
		else
		{
			ImGui::CloseCurrentPopup();
		}

		ImGui::EndPopup();
	}

	if (ImGui::BeginPopup("Link context menu"))
	{
		const PinPair* info = get_link_info(m_context_menu_link);

		if (info != nullptr)
		{
			if (ImGui::Button("Delete"))
			{
				destroy_link(m_context_menu_link);
				ImGui::CloseCurrentPopup();
			}

			ImGui::Text("Select node:");

			const ed::NodeId source_node_id = get_node_by_pin_id(info->from)->node_id();
			const ed::NodeId target_node_id = get_node_by_pin_id(info->to)->node_id();

			ImGui::SameLine();
			if (ImGui::Button("source"))
			{
				ed::SelectNode(source_node_id);
				ImGui::CloseCurrentPopup();
			}
			else if (ImGui::IsItemHovered())
			{
				ed::SelectNode(source_node_id);
			}
			else
			{
				ed::DeselectNode(source_node_id);
			}

			ImGui::SameLine();
			if (ImGui::Button("target"))
			{
				ed::SelectNode(target_node_id);
				ImGui::CloseCurrentPopup();
			}
			else if (ImGui::IsItemHovered())
			{
				ed::SelectNode(target_node_id);
			}
			else
			{
				ed::DeselectNode(target_node_id);
			}
		}
		else
		{
			ImGui::CloseCurrentPopup();
		}

		ImGui::EndPopup();
	}

	if (ImGui::BeginPopup("Create new node"))
	{
		ImGui::TextColored(ImVec4(1.0, 1.0, 1.0, 0.5), "Create a new node");
		ImGui::Separator();

		Node* created_node = nullptr;

		if (ImGui::Selectable("State node"))
		{
			created_node = &make_node<nodes::StateNode>();
			ImGui::CloseCurrentPopup();
		}

		if (ImGui::IsItemHovered())
		{
			//ImGui::SetTooltip("");
		}

		if (ImGui::Selectable("If node"))
		{
			created_node = &make_node<nodes::IfNode>();
			ImGui::CloseCurrentPopup();
		}

		if (ImGui::IsItemHovered())
		{
			ImGui::SetTooltip(
				"Creates an If node, which are useful to implement simple conditions.\n"
				"If nodes accept Lua expressions as well as pre-defined checks (e.g. keyboard input)."
			);
		}

		if (ImGui::Selectable("Conditional block node"))
		{
			created_node = &make_node<nodes::CondNode>();
			ImGui::CloseCurrentPopup();
		}

		if (ImGui::IsItemHovered())
		{
			ImGui::SetTooltip(
				"Creates a conditional block node.\n"
				"Has an arbitrary number of output pins.\n"
				"This is roughly equivalent to having a lot of If nodes in parallel."
			);
		}

		if (created_node != nullptr)
		{
			ed::SetNodePosition(created_node->node_id(), origin);
			ed::SelectNode(created_node->node_id());
		}

		ImGui::EndPopup();
	}

	ed::Resume();
}

}
