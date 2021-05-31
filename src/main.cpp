#include "imgui.h" // necessary for ImGui::*, imgui-SFML.h doesn't include imgui.h

#include "imgui-SFML.h" // for ImGui::SFML::* functions and SFML-specific overloads

#include "imgui-node-editor/imgui_node_editor.h"

#include "fsm-editor/editor.hpp"
#include "fsm-editor/nodes/nodes.hpp"
#include "fsm-editor/widgets/boolexprinput.hpp"

#include <SFML/Graphics/CircleShape.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/System/Clock.hpp>
#include <SFML/Window/Event.hpp>

int main()
{
	sf::ContextSettings settings;
	//settings.antialiasingLevel = 16;
	sf::RenderWindow window(sf::VideoMode(2560, 1440), "centauri-imgui-test", sf::Style::Default, settings);
	window.setFramerateLimit(75);
	ImGui::SFML::Init(window);

	auto& IO = ImGui::GetIO();
	IO.Fonts->Clear(); // clear fonts if you loaded some before (even if only default one was loaded)
	// IO.Fonts->AddFontDefault(); // this will load default font as well
	IO.Fonts->AddFontFromFileTTF("CascadiaCode-Regular.ttf", 16.f);
	IO.Fonts->AddFontFromFileTTF("CascadiaCode-Bold.ttf", 16.f);

	ImGui::SFML::UpdateFontTexture(); // important call: updates font texture

	fsme::FsmEditor editor;

	fsme::BoolExpressionAutocomplete autocomplete;

	{
		autocomplete.add_option("Keys", {"space", "self.inputs:check(InputKey.Space)"});
		autocomplete.add_option("Keys", {"lmb", "self.inputs:check(InputKey.MouseLeft)"});
		autocomplete.add_option("Keys", {"down", "self.inputs:check(InputKey.Down)"});

		autocomplete.add_option("Collision", {"on ground", "self.on_ground"});
	}

	editor.set_autocomplete_provider(&autocomplete);

	sf::Clock deltaClock;
	while (window.isOpen()) {
		sf::Event event;
		while (window.pollEvent(event)) {
			ImGui::SFML::ProcessEvent(event);

			if (event.type == sf::Event::Closed) {
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
