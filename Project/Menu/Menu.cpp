#include "Menu.hpp"

namespace Menu
{
	ImGuiWindowFlags m_iWindowFlags = ImGuiWindowFlags_None; // put your flags here
	bool m_bShowDemo; // optional to show the demo window

	void RenderMenu()
	{
		ImGui::SetNextWindowSize(ImVec2(200, 200), ImGuiCond_Once); // condition to set the size once
		ImGui::Begin("ImGui Menu", nullptr, m_iWindowFlags);
		{
			// content goes here

			ImGui::Text("Hello world!");
			ImGui::Checkbox("Show Demo Window", &m_bShowDemo);
		}
		ImGui::End();
	}
}