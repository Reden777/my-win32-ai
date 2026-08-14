# Hardfork of the project.
## AI Chatroom/Playground in Win32/GDI/riched20.dll
Plans:
1. [ ] Multi turn history
2. [ ] Full rebuilding into Win32/GDI
3. [ ] Usage of system fonts only
4. [ ] Json chat export (following OpenRouter orpg-3.0 schema)
5. [ ] Hand drawn 32x32-128x128 icons replacing ugly flat fonts
6. [ ] Removal of premade system prompts, instead letting the user set everything
7. [ ] Markdown chat export & hopefully import
8. [ ] Removal of hardcoded AIs, instead polling OpenAI/OpenRouter directly
9. [ ] No syntax highlighting necessary because AI does all the coding
10. [ ] Eventual file editing ability
11. [ ] Multiple chats and ability to switch between them.
12. [ ] The current chat saves to disk automatically.
13. [ ] Remove visual studio build bloat. Instead, use MinGW/MSYS.
This project began as a hardfork of Apex-master/chatgpt-cpp, moving away from Dear ImGui to achieve a pure native Windows implementation.
Original base inspiration: chatgpt-cpp
Network layer: (lib)curl, possibly WinHTTP with wolfssl.
OS target: Minimum Vista

<b>Old Screenshots (to be updated):</b>
![screenshot](https://github.com/Reden777/chatgpt-cpp/blob/main/screenshot1.png)
![screenshot](https://github.com/Reden777/chatgpt-cpp/blob/main/screenshot2.png)
