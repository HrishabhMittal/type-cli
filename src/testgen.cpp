#include <fstream>
#include <string>
#include <vector>
#include <random>

double getRandom(double min, double max) {
    static std::random_device rd;
    static std::mt19937 gen(rd());
    std::uniform_real_distribution<> dist(min,max);
    return dist(gen);
}

namespace WordList {
    namespace {
        std::vector<std::string> words;
        std::string random(int size) {
            if (size>=words.size()) size=words.size();
            return words[getRandom(0,size)];
        }
    }
    void init(std::fstream file) {
        words={};
        std::string s;
        while (file>>s) {
            words.push_back(s);
        }
    }
    std::string genRandomText(int words=50,int size=500) {
        std::string text;
        while (words--) text+=random(size)+" ";
        text.pop_back();
        return text;
    }
};
