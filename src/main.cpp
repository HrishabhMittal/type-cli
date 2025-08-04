#include "oui/src/elements.cpp"
#include "testgen.cpp"
#include "timer.cpp"
#include <cctype>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <memory>
#include <ostream>
#include <string>
#include <utility>
#include <sys/ioctl.h>
#include <unistd.h>

std::pair<int,int> termsize() {
    struct winsize win;
    if(ioctl(STDIN_FILENO, TIOCGWINSZ, &win) == -1) {
        perror("ioctl");
        exit(1);
    }
    return {win.ws_col,win.ws_row};
}
class TypeHandler {
    std::string& textarea;
    std::string grey, blue, red;
    std::string typed;
    std::string original;
    Timer t;
    bool timerstarted=false;
    bool running=true;
    void rebuildTextarea() {
        if (!running) return;
        std::string result;
        int ogp=0,typ=0;
        while (true) {
            result+=blue;
            while (ogp<original.size()&&typ<typed.size()&&(original[ogp]==typed[typ])) {
                result+=original[ogp];
                ogp++;
                typ++;
            }
            if (typ==typed.size()) {
                result+=grey;
                if (ogp==original.size()) {
                    t.stop();
                    running=false;
                    return;
                }
                while (ogp<original.size()) result+=original[ogp++];
            }
            if (ogp==original.size()) break;
            result+=red;
            while (typ<typed.size()&&ogp<original.size()&&typed[typ]!=' '&&original[ogp]!=' ') {
                if (typed[typ]!=original[ogp]) {
                    result+=original[ogp];
                    typ++;
                    ogp++;
                }
            }
            if (typ<typed.size()&&typed[typ]==' ') {
                while (ogp<original.size()&&original[ogp]!=' ') {
                    result+=original[ogp++];
                }
            } else if (ogp<original.size()&&original[ogp]==' ') {
                while (typ<typed.size()&&typed[typ]!=' ') {
                    result+=typed[typ++];
                }
            }
        }
        textarea=std::move(result);
    }
public:
    TypeHandler(std::string& out): textarea(out),
        grey(Terminal::FGColor_str({200,200,200})), blue(Terminal::FGColor_str({45,90,100})), red(Terminal::FGColor_str({90,45,75})) {
        textarea="";
        WordList::init(std::fstream("words.txt"));
        original=WordList::genRandomText(20,500);
        rebuildTextarea();
    }
    int curpos() {
        return typed.size();
    }
    int update() {
        if (!running) return true;
        int c=Input::handleInput();
        if ((typed.size()==0||typed.back()==' ') && c==' ') return false;
        if (typed.size()!=0 && c==BACKSPACE) {
            typed.pop_back();
        } else if (isprint(c)) {
            typed.push_back(c);
        } else return false;
        if (!timerstarted) t.reset_and_start(),timerstarted=true;
        rebuildTextarea();
        return true;
    }
};
struct TypingApp {
    Application* app;
    TypeHandler* th;
    bool running=true;
    float wpm=0;
    std::pair<int,int> lastsize=termsize();    
    TypingApp() {
        app=new Application(Rect{1, 1, 50, 12}, false);
            app->add(std::make_unique<Div>(Rect{},true),0.2);
            auto mainscreen=std::make_unique<Div>(Rect{},false);
                auto typescreen=std::make_unique<Div>(Rect{},true);
                    typescreen->add(std::make_unique<Div>(Rect{},true),0.2);
                    auto maintypingarea=std::make_unique<Label>("");
                        std::string& typingarea=maintypingarea->text;
                    typescreen->add(std::move(maintypingarea),0.6);
                mainscreen->add(std::move(typescreen),0.6);
            app->add(std::move(mainscreen),0.6);
        th=new TypeHandler(typingarea);
    }
    void fit_screen() {
        auto size=termsize();
        if (size!=lastsize) resize({1,1,size.first,size.second}),lastsize=size;
    }
    void resize(Rect r) {
        app->resize(r);
    }
    void update_and_render() {
        while (!th->update());
        app->render();
    }
};
int main() {
    Terminal::set_raw_mode();
    Terminal::OpenAltBuffer();
    Terminal::cursorInvisible();

    TypingApp typingapp;
    typingapp.fit_screen();
    typingapp.update_and_render();
    while (typingapp.running) {
        typingapp.fit_screen();
        typingapp.update_and_render();
    }
    Terminal::resetColor();
}
