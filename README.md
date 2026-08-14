# Hardfork of the project.
## AI Chatroom/Playground in Win32/GDI/riched20.dll
Implemented:
1. [x] Multi turn history
2. [x] Full rebuilding into Win32/GDI/RichEdit
3. [x] Usage of system fonts only
4. [x] JSON chat export (following OpenRouter orpg-3.0 schema)
5. [ ] Hand drawn 32x32-128x128 icons replacing ugly flat fonts
6. [x] Editable system prompt
7. [x] Markdown chat export
8. [x] Editable OpenAI-compatible endpoint and model
9. [ ] No syntax highlighting necessary because AI does all the coding
10. [ ] Eventual file editing ability
11. [x] Multiple chats and ability to switch between them
12. [x] Chats save to disk automatically
13. [x] MinGW/MSYS Makefile
14. [ ] Ability to switch between preset OpenRouter, OpenAI, and ability to add custom api endpoints that get saved.
15. [x] Save the api key encrypted at rest with DGPAPI
16. [ ] The last used model ID gets remembered across sessions.
17. [ ] When the AI replies, rather than being Assistant
[reply], it's [Model name]
[reply], to give an example:
GPT-4.1-mini
[reply]
18. [ ] Ability to rename [Model name] in the chat log ui to whatever name you want. (Low priority)
19. [ ] Ability to poll the api endpoint for a full list of models available, to click on one of them to paste it immediately in the model box.
20. [ ] Reasoning slider control (none, minimal, low, medium, high, xhigh, max), as well as a toggle to disable reasoning fully. (Important)

## Build

From an MSYS2/MinGW or Clang64 shell:

```sh
make
./native-ai.exe
```

The app uses only Windows system UI libraries and WinHTTP; no third-party runtime is required.
The API key is automatically stored in `api-key.dat` using user-scoped Windows DPAPI encryption.
This project began as a hardfork of Apex-master/chatgpt-cpp, moving away from Dear ImGui to achieve a pure native Windows implementation.
Original base inspiration: chatgpt-cpp
Network layer: (lib)curl, possibly WinHTTP with wolfssl.
OS target: Minimum Vista

<b>New Screenshots:</b>
![screenshot](https://github.com/Reden777/chatgpt-cpp/blob/main/screenshot3.png)

<b>Old Screenshots (to be updated):</b>
![screenshot](https://github.com/Reden777/chatgpt-cpp/blob/main/screenshot1.png)
![screenshot](https://github.com/Reden777/chatgpt-cpp/blob/main/screenshot2.png)
