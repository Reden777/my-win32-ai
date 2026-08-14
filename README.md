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

## Build

From an MSYS2/MinGW or Clang64 shell:

```sh
make
./native-ai.exe
```

The app uses only Windows system UI libraries and WinHTTP; no third-party runtime is required.
This project began as a hardfork of Apex-master/chatgpt-cpp, moving away from Dear ImGui to achieve a pure native Windows implementation.
Original base inspiration: chatgpt-cpp
Network layer: (lib)curl, possibly WinHTTP with wolfssl.
OS target: Minimum Vista

<b>New Screenshots:</b>
![screenshot](https://github.com/Reden777/chatgpt-cpp/blob/main/screenshot3.png)

<b>Old Screenshots (to be updated):</b>
![screenshot](https://github.com/Reden777/chatgpt-cpp/blob/main/screenshot1.png)
![screenshot](https://github.com/Reden777/chatgpt-cpp/blob/main/screenshot2.png)
