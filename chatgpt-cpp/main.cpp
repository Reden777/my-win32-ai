#define UNICODE
#define _UNICODE
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <richedit.h>
#include <commdlg.h>
#include <winhttp.h>
#include <algorithm>
#include <fstream>
#include <sstream>
#include <string>
#include <thread>
#include <vector>
#include "json.h"

using json = nlohmann::json;

struct Message { std::string role, content; };
struct Chat { std::string title = "New chat"; std::vector<Message> messages; };

enum { ID_CHATS=100, ID_NEW, ID_DELETE, ID_TRANSCRIPT, ID_INPUT, ID_SEND,
       ID_ENDPOINT, ID_KEY, ID_MODEL, ID_SYSTEM, ID_REFRESH, ID_EXPORT_JSON,
       ID_EXPORT_MD, WM_RESULT=WM_APP+1, WM_MODELS=WM_APP+2 };

static HWND g_main, g_chats, g_transcript, g_input, g_send, g_endpoint, g_key,
            g_model, g_system, g_status;
static HFONT g_font;
static std::vector<Chat> g_data;
static int g_current = 0;
static const wchar_t* kStore = L"chats.json";

static std::wstring wide(const std::string& s) {
    if (s.empty()) return {};
    int n=MultiByteToWideChar(CP_UTF8,0,s.data(),(int)s.size(),nullptr,0);
    std::wstring w(n,0); MultiByteToWideChar(CP_UTF8,0,s.data(),(int)s.size(),w.data(),n); return w;
}
static std::string utf8(const std::wstring& w) {
    if (w.empty()) return {};
    int n=WideCharToMultiByte(CP_UTF8,0,w.data(),(int)w.size(),nullptr,0,nullptr,nullptr);
    std::string s(n,0); WideCharToMultiByte(CP_UTF8,0,w.data(),(int)w.size(),s.data(),n,nullptr,nullptr); return s;
}
static std::wstring text(HWND h) {
    int n=GetWindowTextLengthW(h); std::wstring s(n,0);
    if(n) GetWindowTextW(h,s.data(),n+1); return s;
}
static void status(const wchar_t* s) { SetWindowTextW(g_status,s); }

static void save() {
    json root; root["version"]="orpg-3.0"; root["chats"]=json::array();
    for (auto& c:g_data) { json jc; jc["title"]=c.title; jc["messages"]=json::array();
        for(auto& m:c.messages) jc["messages"].push_back({{"role",m.role},{"content",m.content}});
        root["chats"].push_back(jc); }
    std::ofstream f(kStore,std::ios::binary); f << root.dump(2);
}
static void load() {
    try { std::ifstream f(kStore,std::ios::binary); if(f) { json r; f>>r;
        for(auto& jc:r.value("chats",json::array())) { Chat c; c.title=jc.value("title","New chat");
            for(auto& jm:jc.value("messages",json::array())) c.messages.push_back({jm.value("role","user"),jm.value("content","")});
            g_data.push_back(std::move(c)); } } } catch(...) {}
    if(g_data.empty()) g_data.push_back({});
}
static void redraw_list() {
    SendMessageW(g_chats,LB_RESETCONTENT,0,0);
    for(auto& c:g_data) SendMessageW(g_chats,LB_ADDSTRING,0,(LPARAM)wide(c.title).c_str());
    SendMessageW(g_chats,LB_SETCURSEL,g_current,0);
}
static void redraw_chat() {
    std::wstring out;
    if(g_current>=0 && g_current<(int)g_data.size()) for(auto& m:g_data[g_current].messages) {
        out += m.role=="user" ? L"You\r\n" : m.role=="assistant" ? L"Assistant\r\n" : L"System\r\n";
        out += wide(m.content); out += L"\r\n\r\n";
    }
    SetWindowTextW(g_transcript,out.c_str());
    SendMessageW(g_transcript,EM_SETSEL,out.size(),out.size()); SendMessageW(g_transcript,EM_SCROLLCARET,0,0);
}

static bool request(const std::wstring& endpoint,const std::wstring& key,const std::string& body,std::string& reply) {
    URL_COMPONENTS u{}; u.dwStructSize=sizeof(u); u.dwSchemeLength=u.dwHostNameLength=u.dwUrlPathLength=u.dwExtraInfoLength=(DWORD)-1;
    if(!WinHttpCrackUrl(endpoint.c_str(),0,0,&u)) { reply="Invalid endpoint URL"; return false; }
    std::wstring host(u.lpszHostName,u.dwHostNameLength), path(u.lpszUrlPath,u.dwUrlPathLength);
    if(u.dwExtraInfoLength) path.append(u.lpszExtraInfo,u.dwExtraInfoLength);
    HINTERNET ses=WinHttpOpen(L"NativeAI/1.0",WINHTTP_ACCESS_TYPE_DEFAULT_PROXY,WINHTTP_NO_PROXY_NAME,WINHTTP_NO_PROXY_BYPASS,0);
    HINTERNET con=ses?WinHttpConnect(ses,host.c_str(),u.nPort,0):nullptr;
    HINTERNET req=con?WinHttpOpenRequest(con,L"POST",path.c_str(),nullptr,WINHTTP_NO_REFERER,WINHTTP_DEFAULT_ACCEPT_TYPES,u.nScheme==INTERNET_SCHEME_HTTPS?WINHTTP_FLAG_SECURE:0):nullptr;
    std::wstring auth=L"Content-Type: application/json\r\nAuthorization: Bearer "+key;
    BOOL ok=req && WinHttpSendRequest(req,auth.c_str(),-1L,(void*)body.data(),(DWORD)body.size(),(DWORD)body.size(),0) && WinHttpReceiveResponse(req,nullptr);
    if(ok) { DWORD avail=0; while(WinHttpQueryDataAvailable(req,&avail)&&avail) { std::string b(avail,0); DWORD got=0; if(!WinHttpReadData(req,b.data(),avail,&got)) break; reply.append(b.data(),got); } }
    if(!ok) reply="Network error "+std::to_string(GetLastError());
    if(req)WinHttpCloseHandle(req); if(con)WinHttpCloseHandle(con); if(ses)WinHttpCloseHandle(ses); return !!ok;
}
static void send_async() {
    std::string prompt=utf8(text(g_input)); if(prompt.empty()) return;
    std::wstring endpoint=text(g_endpoint), key=text(g_key); std::string model=utf8(text(g_model)), system=utf8(text(g_system));
    auto& c=g_data[g_current]; if(c.messages.empty()) c.title=prompt.substr(0,std::min<size_t>(40,prompt.size()));
    c.messages.push_back({"user",prompt}); SetWindowTextW(g_input,L""); save(); redraw_list(); redraw_chat(); EnableWindow(g_send,FALSE); status(L"Sending...");
    int index=g_current; std::vector<Message> messages=c.messages;
    std::thread([endpoint,key,model,system,messages,index]{
        json body={{"model",model},{"messages",json::array()}};
        if(!system.empty()) body["messages"].push_back({{"role","system"},{"content",system}});
        for(auto&m:messages) body["messages"].push_back({{"role",m.role},{"content",m.content}});
        std::string raw; bool ok=request(endpoint,key,body.dump(),raw); std::string answer;
        if(ok) try { auto r=json::parse(raw); if(r.contains("error")) answer="Error: "+r["error"].value("message",raw); else answer=r["choices"][0]["message"].value("content",""); } catch(...) { answer="Invalid server response: "+raw; }
        else answer=raw;
        auto payload=new std::pair<int,std::string>(index,answer); PostMessageW(g_main,WM_RESULT,0,(LPARAM)payload);
    }).detach();
}
static std::wstring pick_file(const wchar_t* filter,const wchar_t* ext) {
    wchar_t file[MAX_PATH]=L""; OPENFILENAMEW o{}; o.lStructSize=sizeof(o); o.hwndOwner=g_main; o.lpstrFilter=filter; o.lpstrFile=file; o.nMaxFile=MAX_PATH; o.lpstrDefExt=ext; o.Flags=OFN_OVERWRITEPROMPT;
    return GetSaveFileNameW(&o)?file:L"";
}
static void export_chat(bool markdown) {
    auto p=pick_file(markdown?L"Markdown\0*.md\0":L"JSON\0*.json\0",markdown?L"md":L"json"); if(p.empty())return;
    std::ofstream f(p.c_str(),std::ios::binary); auto& c=g_data[g_current];
    if(markdown) { f<<"# "<<c.title<<"\n\n"; for(auto&m:c.messages) f<<"## "<<m.role<<"\n\n"<<m.content<<"\n\n"; }
    else { json j={{"version","orpg-3.0"},{"title",c.title},{"messages",json::array()}}; for(auto&m:c.messages)j["messages"].push_back({{"role",m.role},{"content",m.content}}); f<<j.dump(2); }
    status(L"Exported.");
}
static HWND ctl(const wchar_t* cls,const wchar_t* value,DWORD style,int id) {
    HWND h=CreateWindowExW(WS_EX_CLIENTEDGE,cls,value,WS_CHILD|WS_VISIBLE|style,0,0,0,0,g_main,(HMENU)(INT_PTR)id,GetModuleHandleW(nullptr),nullptr);
    SendMessageW(h,WM_SETFONT,(WPARAM)g_font,TRUE); return h;
}
static void layout(int w,int h) {
    const int gap=8,left=190,right=300,top=8,bottom=105;
    MoveWindow(g_chats,gap,40,left-16,h-190,TRUE); MoveWindow(GetDlgItem(g_main,ID_NEW),gap,h-142,82,28,TRUE); MoveWindow(GetDlgItem(g_main,ID_DELETE),98,h-142,82,28,TRUE);
    MoveWindow(g_transcript,left,top,w-left-right-gap,h-bottom-top,TRUE); MoveWindow(g_input,left,h-bottom+gap,w-left-right-100,bottom-16,TRUE); MoveWindow(g_send,w-right-92,h-bottom+gap,84,bottom-16,TRUE);
    int x=w-right+8,y=26,ew=right-16; MoveWindow(g_endpoint,x,y,ew,24,TRUE); y+=48; MoveWindow(g_key,x,y,ew,24,TRUE); y+=48; MoveWindow(g_model,x,y,ew-92,24,TRUE); MoveWindow(GetDlgItem(g_main,ID_REFRESH),x+ew-86,y,86,24,TRUE); y+=48; MoveWindow(g_system,x,y,ew,110,TRUE);
    MoveWindow(GetDlgItem(g_main,ID_EXPORT_JSON),x,y+126,135,28,TRUE); MoveWindow(GetDlgItem(g_main,ID_EXPORT_MD),x+143,y+126,135,28,TRUE); MoveWindow(g_status,gap,h-32,w-2*gap,24,TRUE);
}
static LRESULT CALLBACK proc(HWND hwnd,UINT msg,WPARAM wp,LPARAM lp) {
    if(msg==WM_CREATE) {
        g_main=hwnd; g_font=(HFONT)GetStockObject(DEFAULT_GUI_FONT);
        g_chats=ctl(L"LISTBOX",L"",LBS_NOTIFY|WS_VSCROLL,ID_CHATS); ctl(L"BUTTON",L"New",0,ID_NEW); ctl(L"BUTTON",L"Delete",0,ID_DELETE);
        g_transcript=ctl(MSFTEDIT_CLASS,L"",ES_MULTILINE|ES_READONLY|ES_AUTOVSCROLL|WS_VSCROLL,ID_TRANSCRIPT);
        g_input=ctl(MSFTEDIT_CLASS,L"",ES_MULTILINE|ES_AUTOVSCROLL|WS_VSCROLL,ID_INPUT); g_send=ctl(L"BUTTON",L"Send",BS_DEFPUSHBUTTON,ID_SEND);
        g_endpoint=ctl(L"EDIT",L"https://api.openai.com/v1/chat/completions",ES_AUTOHSCROLL,ID_ENDPOINT); g_key=ctl(L"EDIT",L"",ES_PASSWORD|ES_AUTOHSCROLL,ID_KEY); g_model=ctl(L"EDIT",L"gpt-4.1-mini",ES_AUTOHSCROLL,ID_MODEL); ctl(L"BUTTON",L"Models",0,ID_REFRESH);
        g_system=ctl(MSFTEDIT_CLASS,L"You are a helpful assistant.",ES_MULTILINE|WS_VSCROLL,ID_SYSTEM); ctl(L"BUTTON",L"Export JSON",0,ID_EXPORT_JSON); ctl(L"BUTTON",L"Export Markdown",0,ID_EXPORT_MD); g_status=ctl(L"STATIC",L"Ready",0,900);
        load(); redraw_list(); redraw_chat(); return 0;
    }
    if(msg==WM_SIZE){layout(LOWORD(lp),HIWORD(lp));return 0;}
    if(msg==WM_COMMAND) { int id=LOWORD(wp);
        if(id==ID_SEND) send_async();
        else if(id==ID_NEW){g_data.push_back({});g_current=(int)g_data.size()-1;save();redraw_list();redraw_chat();}
        else if(id==ID_DELETE&&g_data.size()>1){g_data.erase(g_data.begin()+g_current);g_current=std::min(g_current,(int)g_data.size()-1);save();redraw_list();redraw_chat();}
        else if(id==ID_CHATS&&HIWORD(wp)==LBN_SELCHANGE){g_current=(int)SendMessageW(g_chats,LB_GETCURSEL,0,0);redraw_chat();}
        else if(id==ID_EXPORT_JSON)export_chat(false); else if(id==ID_EXPORT_MD)export_chat(true);
        else if(id==ID_REFRESH) status(L"Enter any model exposed by your OpenAI-compatible provider."); return 0;
    }
    if(msg==WM_RESULT){auto*p=(std::pair<int,std::string>*)lp;if(p->first>=0&&p->first<(int)g_data.size())g_data[p->first].messages.push_back({"assistant",p->second});delete p;save();redraw_chat();EnableWindow(g_send,TRUE);status(L"Ready");return 0;}
    if(msg==WM_CLOSE){save();DestroyWindow(hwnd);return 0;} if(msg==WM_DESTROY){PostQuitMessage(0);return 0;} return DefWindowProcW(hwnd,msg,wp,lp);
}
int WINAPI wWinMain(HINSTANCE hi,HINSTANCE,LPWSTR,int show) {
    LoadLibraryW(L"Msftedit.dll"); WNDCLASSEXW wc{sizeof(wc)}; wc.lpfnWndProc=proc;wc.hInstance=hi;wc.hCursor=LoadCursor(nullptr,IDC_ARROW);wc.hIcon=LoadIcon(nullptr,IDI_APPLICATION);wc.hbrBackground=(HBRUSH)(COLOR_BTNFACE+1);wc.lpszClassName=L"NativeAIWindow";RegisterClassExW(&wc);
    HWND w=CreateWindowExW(0,wc.lpszClassName,L"Native AI Chatroom",WS_OVERLAPPEDWINDOW|WS_CLIPCHILDREN,CW_USEDEFAULT,CW_USEDEFAULT,1200,760,nullptr,nullptr,hi,nullptr);ShowWindow(w,show);UpdateWindow(w);MSG m;while(GetMessageW(&m,nullptr,0,0)>0){TranslateMessage(&m);DispatchMessageW(&m);}return (int)m.wParam;
}
