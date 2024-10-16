#include <iostream>
#include <string>
#include <fstream>
#include <unordered_map>
#include <vector>
#include <algorithm> 

int main() {
    std::ifstream file("./resources/cipher.txt");
    std::string line;
    std::unordered_map<std::string, int> patternCount; 
    if (!file) {
        std::cerr << "Unable to open file" << std::endl;   
        return 1;
    }

    while (std::getline(file, line)) {
        for (size_t i = 0; i <= line.length() - 3; ++i) {
            std::string pattern = line.substr(i, 3); 

            if (pattern.find_first_not_of("ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz") == std::string::npos) {
                patternCount[pattern]++; 
            }
        }
    }

    std::vector<std::pair<std::string, int>> sortedPatterns(patternCount.begin(), patternCount.end());

    std::sort(sortedPatterns.begin(), sortedPatterns.end(),
              [](const std::pair<std::string, int>& a, const std::pair<std::string, int>& b) {
                  return a.second > b.second; 
              });

    for (size_t i = 0; i < 3 && i < sortedPatterns.size(); ++i) {
        std::cout << sortedPatterns[i].first << " (repetido " << sortedPatterns[i].second << " vezes)" << std::endl;
    }

    return 0;
}
