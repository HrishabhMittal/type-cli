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
        std::string random() {
            return words[getRandom(0,words.size())];
        }
    }
    void init(std::fstream file) {
        words={};
        std::string s;
        while (file>>s) {
            words.push_back(s);
        }
    }
    std::string genRandomText(int words=50) {
        std::string text;
        while (words--) text+=random()+" ";
        text.pop_back();
        return text;
    }
};
