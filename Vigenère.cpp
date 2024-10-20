#include <iostream>
#include <string>
#include <fstream>
#include <unordered_map>
#include <vector>
#include <algorithm>
#include <set>
#include <map>
#include <sstream>
#include <cctype>

std::vector<std::pair<char, double>> frequencyReader(const std::string& filePath) {
    std::ifstream file(filePath);
    std::vector<std::pair<char, double>> frequencyTable;

    if (!file) {
        std::cerr << "Unable to open file: " << filePath << std::endl;
        return frequencyTable;
    }

    std::string line;
    while (std::getline(file, line)) {
        char letter;
        double frequency;
        std::string trash;

        std::istringstream iss(line);
        iss >> letter >> trash >> frequency;

        frequencyTable.push_back({letter, frequency});
    }

    std::sort(frequencyTable.begin(), frequencyTable.end(), [](const auto& a, const auto& b) {
        return a.second > b.second;
    });

    return frequencyTable;
}

std::set<int> getDivisors(int n) {
    std::set<int> divisors;
    for (int i = 1; i <= n; i++) {
        if (n % i == 0) {
            divisors.insert(i);
        }
    }
    return divisors;
}

void divisorFrequencies(const std::string& cipher, int patternLength) {
    std::unordered_map<std::string, std::vector<int>> patternPositions;
    std::map<int, int> divisorFrequency;

    for (size_t i = 0; i <= cipher.size() - patternLength; i++) {
        std::string pattern = cipher.substr(i, patternLength);
        patternPositions[pattern].push_back(i);
    }

    for (const auto& pair : patternPositions) {
        const std::vector<int>& positions = pair.second;
        if (positions.size() > 1) {
            for (size_t i = 1; i < positions.size(); i++) {
                int distance = positions[i] - positions[i - 1];
                std::set<int> divisors = getDivisors(distance);
                for (int divisor : divisors) {
                    divisorFrequency[divisor]++;
                }
            }
        }
    }

    std::vector<std::pair<int, int>> sortedDivisors(divisorFrequency.begin(), divisorFrequency.end());
    std::sort(sortedDivisors.begin(), sortedDivisors.end(), [](const auto& a, const auto& b) {
        return a.second > b.second;
    });

    std::cout << "\n\t\t***Most frequent divisors***\n\n";
    int count = 0;
    for (size_t i = 0; i < sortedDivisors.size() && count < 10; ++i) {
        std::cout << "Divisor: " << sortedDivisors[i].first << " - Frequency: " << sortedDivisors[i].second << '\n';
        count++;
    }
}

std::string readFile(const std::string& filePath) {
    std::ifstream file(filePath);
    std::string content;

    if (!file) {
        std::cerr << "Unable to open file: " << filePath << std::endl;
        return "";
    }

    std::string line;
    while (std::getline(file, line)) {
        content += line;
    }

    return content;
}

std::string shiftSubstring(const std::string& group, const std::vector<std::pair<char, double>>& frequencyTable) {
    std::unordered_map<char, int> frequencyCount;
    std::vector<std::pair<char, int>> appearanceOrder;

    for (char ch : group) {
        frequencyCount[ch]++;
    }

    for (char ch : group) {
        appearanceOrder.emplace_back(ch, frequencyCount[ch]);
    }

    std::sort(appearanceOrder.begin(), appearanceOrder.end(), [&frequencyCount](const auto& a, const auto& b) {
        return a.second > b.second;
    });

    std::string shifted(group.length(), ' ');
    size_t i = 0;
    std::unordered_map<char, char> mappedLetters;

    for (const auto& pair : appearanceOrder) {
        char cipherLetter = pair.first;
        if (mappedLetters.find(cipherLetter) == mappedLetters.end()) {
            mappedLetters[cipherLetter] = frequencyTable[i].first;
            i++;
        }
    }

    for (size_t j = 0; j < group.length(); ++j) {
        shifted[j] = std::toupper(mappedLetters[group[j]]);
    }

    return shifted;
}

void Kasiski(const std::string& cipher, int keywordLength, const std::vector<std::pair<char, double>>& frequencyTable) {
    std::vector<std::string> groups;

    std::cout << "\n\t\t***Kasiski Method***\n";

    for (size_t i = 0; i < cipher.size(); i += keywordLength) {
        groups.push_back(cipher.substr(i, keywordLength));
    }

    for (const auto& group : groups) {
        std::unordered_map<char, int> frequencyCount;

        for (char ch : group) {
            frequencyCount[ch]++;
        }

        std::cout << std::endl; 

        std::cout << "Substring: " << group << "\nFrequencies: ";
        for (const auto& pair : frequencyCount) {
            std::cout << pair.first << ":" << pair.second << " ";
        }
        std::cout << "\n";

        std::string shiftedGroup = shiftSubstring(group, frequencyTable);
        std::cout << "Shifted Substring: " << shiftedGroup << "\n";
    }
    std::cout << "\n**********************************************************\n\n";
}

int main() {
    std::string cipher = readFile("./resources/teste-eng.txt");
    if (cipher.empty()) {
        return 1;
    }

    std::vector<std::pair<char, double>> frequencyTable = frequencyReader("./resources/frequency.txt");
    if (frequencyTable.empty()) {
        return 1;
    }

    int patternLength = 4;
    divisorFrequencies(cipher, patternLength);

    int keywordLength = 6;
    Kasiski(cipher, keywordLength, frequencyTable);

    return 0;
}