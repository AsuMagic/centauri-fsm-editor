#include "imgui.h"
#include "imgui-SFML.h"

#include "imgui-node-editor/imgui_node_editor.h"

#include "fsm-editor/editor.hpp"

#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/System/Clock.hpp>
#include <SFML/Window/Event.hpp>

/**
 * @mainpage Planet Centauri FSM editor
 *
 * This tool allows to edit FSMs that can be translated into Lua code using user-friendly transition graphs. The main
 * purpose of this is to ease definition of monster logic in a graphical way.
 *
 * Here are a few points of interest to get started in this codebase:
 * - fsme::FsmEditor is the class that handles UI and manages graph entities (nodes, links, etc.).
 * - fsme::nodes is the namespace containing all different node types, which inherit from fsme::Node.
 * - fsme::visitors is the namespace containing visitors, which inherit from fsme::NodeVisitor. This allows performing
 * operations on nodes and the graph without requiring to add logic to fsme::Node itself.
 * - fsme::widgets is the namespace containing several GUI widgets, such as text input abstraction (hiding away
 * allocation details and such).
 *
 * At the moment, dependencies are expected to be installed using vcpkg:
 * - `imgui`
 * - `imgui-sfml`
 * - `sfml`
 *
 * The `imgui-node-editor` is included within the source directly.
 */

int main()
{
	sf::RenderWindow window(sf::VideoMode(2560, 1440), "Centauri FSM editor");
	window.setFramerateLimit(75);
	ImGui::SFML::Init(window);

	auto& io = ImGui::GetIO();

	io.Fonts->Clear();
	io.Fonts->AddFontFromFileTTF("CascadiaCode-Regular.ttf", 16.f);
	io.Fonts->AddFontFromFileTTF("CascadiaCode-Bold.ttf", 16.f);
	ImGui::SFML::UpdateFontTexture();

	fsme::FsmEditor editor;

	fsme::widgets::BoolExpressionAutocomplete autocomplete;

	{
		autocomplete.add_option("Keys", {"space", "self.inputs:check(InputKey.Space)"});
		autocomplete.add_option("Keys", {"lmb", "self.inputs:check(InputKey.MouseLeft)"});
		autocomplete.add_option("Keys", {"down", "self.inputs:check(InputKey.Down)"});

		autocomplete.add_option("Collision", {"on ground", "self.on_ground"});
	}

	editor.set_autocomplete_provider(&autocomplete);

	sf::Clock deltaClock;
	while (window.isOpen())
	{
		for (sf::Event event{}; window.pollEvent(event);)
		{
			ImGui::SFML::ProcessEvent(event);

			if (event.type == sf::Event::Closed)
			{
				window.close();
			}
		}

		ImGui::SFML::Update(window, deltaClock.restart());

		editor.render();

		window.clear();
		ImGui::SFML::Render(window);
		window.display();
	}

	ImGui::SFML::Shutdown();

	return 0;
}
