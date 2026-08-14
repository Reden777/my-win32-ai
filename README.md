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

OLD BELOW:

# chatgpt-cpp
chatgpt-cpp is a conversational AI software application built in C++ using OpenAI GPT to generate natural language responses to user inputs. It features a user-friendly interface powered by Dear ImGui library, with multiple settings to modify API requests.

* Playground UI (chatgpt-cpp/playground.h)
* OpenAI API (chatgpt-cpp/openai_api_client.h)

## Main Features

* Uses the OpenAI GPT model for generating responses.
* Provides a user-friendly interface powered by Dear ImGui library.
* Multiple settings to modify API requests, such as temperature and max_tokens values.
* Multiple AI styles that implement prompts before the user's question to personalize the conversation.

<b>Screenshots:</b>
![screenshot](https://github.com/Apex-master/chatgpt-cpp/blob/main/screenshot1.png)
![screenshot](https://github.com/Apex-master/chatgpt-cpp/blob/main/screenshot2.png)

## How it works
It uses the OpenAI GPT model to generate responses to user inputs. The application sends the user input to the GPT model API, which returns a sequence of tokens.

To improve the quality and relevance of the responses, ChatGPT-CPP includes multiple AI styles that implement prompts before the user's question. These prompts provide additional context for the GPT model to generate a more relevant and appropriate response.

The application provides a user-friendly interface powered by the Dear ImGui library, which allows users to interact with the chatbot and modify API requests. The user can adjust settings such as the temperature and presencePenalty values, which control the randomness and creativity of the model's responses. These settings can be fine-tuned to improve the quality of the conversation and tailor the chatbot's responses to the user's preferences.

## To do
* Multi-turn conversations (no sessions available yet for OpenAI API)

## Credits:
* [OpenAI](https://openai.com)
* [imgui](https://github.com/ocornut/imgui)
* [ImGuiColorTextEdit](https://github.com/BalazsJako/ImGuiColorTextEdit)
* [curl](https://github.com/curl/curl)
