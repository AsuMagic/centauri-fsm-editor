#include "editor.hpp"

#include "nodes/nodes.hpp"
#include "visitors/nodemenurenderer.hpp"
#include "visitors/linkverifier.hpp"
#include "visitors/nodeduplicator.hpp"
#include "visitors/nativeserializer.hpp"
#include "visitors/nativedeserializer.hpp"
#include "util/erase.hpp"

#include <sstream>

namespace fsme
{

FsmEditor::FsmEditor(sf::RenderTarget& target) :
	m_target(&target),
	m_context(create_context()),
	m_autocomplete_provider(nullptr)
{}

FsmEditor::~FsmEditor()
{
	ed::DestroyEditor(m_context);
}

void FsmEditor::clear()
{
	m_state = {};
	m_volatile = {};

	ed::DestroyEditor(m_context);
	m_context = create_context();
}

void FsmEditor::render()
{
	ImGui::SetNextWindowPos(ImVec2(0.0, 0.0), ImGuiCond_Always);
	ImGui::SetNextWindowSize(ImVec2(m_target->getSize().x, m_target->getSize().y), ImGuiCond_Always);

	ImGui::Begin("FSM editor", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_MenuBar);

	render_menu_bar();

	ImGui::BeginChild("##loadsidebar", ImVec2(120.0f, ImGui::GetContentRegionAvail().y));
/*
	ImGui::Selectable("test");
	ImGui::Selectable("test2");*/

	ImGui::EndChild();
	ImGui::SameLine();
	ImGui::BeginChild("##canvas");

	render_canvas();

	ImGui::EndChild();

	ImGui::End();
}

void FsmEditor::destroy_node(ed::NodeId id)
{
	detail::map_erase(m_state.nodes, id);
}

ed::PinId FsmEditor::create_pin(ed::NodeId node)
{
	const ed::PinId id = new_unique_id();
	m_state.pins.emplace(std::make_pair(id, PinInfo{node, {}}));
	return id;
}

void FsmEditor::destroy_pin(ed::PinId pin)
{
	destroy_links_involving(pin);
	detail::map_erase(m_state.pins, pin);
}

const PinInfo* FsmEditor::get_pin_info(ed::PinId pin) const
{
	const auto it = m_state.pins.find(pin);
	return (it != m_state.pins.end()) ? &it->second : nullptr;
}

ed::LinkId FsmEditor::create_link(PinPair pin_pair)
{
	const ed::LinkId id = new_unique_id();

	m_state.links.emplace(std::make_pair(id, pin_pair));

	LinkInfo link{id, pin_pair};
	m_state.pins.at(pin_pair.from).links.push_back(link);
	m_state.pins.at(pin_pair.to).links.push_back(link);

	return id;
}

void FsmEditor::destroy_link(ed::LinkId link)
{
	const auto it = m_state.links.find(link);
	if (it != m_state.links.end())
	{
		const PinPair pin_pair = it->second;
		m_state.links.erase(it);

		detail::erase(m_state.pins.at(pin_pair.from).links, pin_pair);
		detail::erase(m_state.pins.at(pin_pair.to).links, pin_pair);
	}
}

const PinPair* FsmEditor::get_link_info(ed::LinkId link) const
{
	const auto it = m_state.links.find(link);
	return (it != m_state.links.end()) ? &it->second : nullptr;
}

void FsmEditor::destroy_links_involving(ed::PinId pin)
{
	const auto it = m_state.pins.find(pin);
	if (it != m_state.pins.end())
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
	const auto it = m_state.nodes.find(id);
	return (it != m_state.nodes.end()) ? &*it->second : nullptr;
}

Node* FsmEditor::get_node_by_pin_id(ed::PinId pin) const
{
	const auto it = m_state.pins.find(pin);
	return (it != m_state.pins.end()) ? get_node_by_id(it->second.node_id) : nullptr;
}

bool FsmEditor::is_link_selected(ed::LinkId link) const
{
	return std::find(
		m_volatile.selection.links.begin(),
		m_volatile.selection.links.end(),
		link
	) != m_volatile.selection.links.end();
}

bool FsmEditor::is_node_selected(ed::NodeId node) const
{
	return std::find(
		m_volatile.selection.nodes.begin(),
		m_volatile.selection.nodes.end(),
		node
				) != m_volatile.selection.nodes.end();
}

ed::EditorContext* FsmEditor::create_context()
{
	ed::EditorContext* context = ed::CreateEditor();

	ed::SetCurrentEditor(context);

	auto& style = ed::GetStyle();
	style.LinkStrength = 500.0f;

	return context;
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
	m_volatile.selection.links.resize(ed::GetSelectedObjectCount());
	m_volatile.selection.links.resize(ed::GetSelectedLinks(m_volatile.selection.links.data(), m_volatile.selection.links.size()));

	m_volatile.selection.nodes.resize(ed::GetSelectedObjectCount());
	m_volatile.selection.nodes.resize(ed::GetSelectedNodes(m_volatile.selection.nodes.data(), m_volatile.selection.nodes.size()));
}

void FsmEditor::render_menu_bar()
{
	bool open_new = false;

	ImGui::PushID(this);
	if (ImGui::BeginMenuBar())
	{
		if (ImGui::BeginMenu("File"))
		{
			if (ImGui::MenuItem("New..."))
			{
				open_new = true;
			}

			if (ImGui::MenuItem("Save"))
			{
				std::stringstream ss;
				visitors::NativeSerializer::serialize(*this, ss);
				fprintf(stderr, "Size %d\n", int(ss.str().size()));
			}

			if (ImGui::MenuItem("Save as..."))
			{

			}

			if (ImGui::MenuItem("Export to Centauri format"))
			{
				// todo
			}

			if (ImGui::IsItemHovered())
			{
				ImGui::SetTooltip(
					"Export the FSM graph in a format that can be then translated into Lua source code.\n"
					"This is a lossy export, so you will not be able to reopen an exported graph into the editor!"
				);
			}

			ImGui::EndMenu();
		}

		if (ImGui::BeginMenu("Debug"))
		{
			ImGui::Text("Last ID: %d", int(m_state.last_allocated_id));

			ImGui::Text("Node count: %d", int(m_state.nodes.size()));
			ImGui::Text("Link count: %d", int(m_state.links.size()));
			ImGui::Text("Pin count: %d", int(m_state.pins.size()));

			ImGui::Separator();

			if (ImGui::MenuItem("Reload"))
			{
				std::stringstream ss;
				visitors::NativeSerializer::serialize(*this, ss);
				fprintf(stderr, "Serialized form is %d bytes\n", int(ss.str().size()));
				visitors::NativeDeserializer::deserialize(*this, ss);
			}

			ImGui::EndMenu();
		}

		ImGui::EndMenuBar();
	}

	if (open_new)
	{
		ImGui::OpenPopup("New file");
		m_shared_input.get_text().clear();
	}

	ImGui::SetNextWindowSize(ImVec2(400, 200), ImGuiCond_Always);
	if (ImGui::BeginPopupModal("New file", nullptr, ImGuiWindowFlags_NoResize))
	{
		ImGui::TextWrapped(
			"Specify the name for the new state machine.\n"
			"This can, for instance, be \"zombie\", and the files will be created based on this."
		);

		detail::imgui_set_default_keyboard_focus();
		ImGui::SetNextItemWidth(ImGui::GetContentRegionAvailWidth());
		m_shared_input.set_hint("FSM name");
		m_shared_input.render();

		if (ImGui::Button("Create"))
		{
			// TODO:
		}

		ImGui::SameLine();
		if (ImGui::Button("Cancel"))
		{
			ImGui::CloseCurrentPopup();
		}

		ImGui::EndPopup();
	}
	ImGui::PopID();
}

void FsmEditor::render_canvas()
{
	ed::SetCurrentEditor(m_context);
	ed::Begin("My Editor");

	for (auto& p : m_state.nodes)
	{
		ImGui::PushID(p.second.get());
		p.second->accept(m_node_renderer);
		ImGui::PopID();
	}

	handle_item_creation();
	handle_item_deletion();

	render_links();

	render_popups();

	refresh_selected_objects();

	if (m_volatile.selection.nodes.empty() && ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_C)))
	{
		ed::Suspend();
		ImGui::OpenPopup("Create new node");
		ed::Resume();
	}

	if (ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_Escape), false))
	{
		for (const ed::NodeId& id : m_volatile.selection.nodes)
		{
			ed::DeselectNode(id);
		}

		for (const ed::LinkId& id : m_volatile.selection.links)
		{
			ed::DeselectLink(id);
		}
	}

	ed::End();
}

void FsmEditor::render_links()
{
	for (const auto& p : m_state.links)
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

	if (ed::ShowNodeContextMenu(&m_volatile.context_menu_node))
	{
		ImGui::OpenPopup("Node context menu");
		ed::SelectNode(m_volatile.context_menu_node);
	}

	if (ed::ShowPinContextMenu(&m_volatile.context_menu_pin))
	{
		ImGui::OpenPopup("Pin context menu");
		ed::SelectNode(get_node_by_pin_id(m_volatile.context_menu_pin)->node_id());
	}

	if (ed::ShowLinkContextMenu(&m_volatile.context_menu_link))
	{
		ImGui::OpenPopup("Link context menu");
		ed::SelectLink(m_volatile.context_menu_link);
	}

	if (ed::ShowBackgroundContextMenu())
	{
		ImGui::OpenPopup("Create new node");
	}

	if (ImGui::BeginPopup("Node context menu"))
	{
		Node* node = get_node_by_id(m_volatile.context_menu_node);

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
		const PinInfo* info = get_pin_info(m_volatile.context_menu_pin);

		if (info != nullptr)
		{
			ImGui::Text("Pin has %d links", int(info->links.size()));

			if (ImGui::Button("Unlink"))
			{
				destroy_links_involving(m_volatile.context_menu_pin);
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
		const PinPair* info = get_link_info(m_volatile.context_menu_link);

		if (info != nullptr)
		{
			if (ImGui::Button("Delete"))
			{
				destroy_link(m_volatile.context_menu_link);
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
