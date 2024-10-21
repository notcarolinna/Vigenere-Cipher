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

    for(const auto& pair : frequencyTable) {
        std::cout << pair.first << " - " << pair.second << '\n';
    }

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

std::unordered_map<char, int> getFrequencies(const std::string& str) {
    std::unordered_map<char, int> frequencyCount;
    for (char ch : str) {
        frequencyCount[ch]++;
    }
    return frequencyCount;
}

std::vector<std::pair<char, int>> sortFrequencies(const std::unordered_map<char, int>& frequencyCount) {
    std::vector<std::pair<char, int>> sortedFrequencies(frequencyCount.begin(), frequencyCount.end());
    std::sort(sortedFrequencies.begin(), sortedFrequencies.end(), [](const auto& a, const auto& b) {
        return a.second > b.second;
    });
    return sortedFrequencies;
}

int calculateShift(char from, char to) {
    return (to - from + 26) % 26;
}

char getLetterFromShift(int shift) {
    return 'A' + shift;
}

void Kasiski(const std::string& cipher, int keywordLength, const std::vector<std::pair<char, double>>& frequencyTable, std::vector<int>& shifts) {
    std::vector<std::string> groups;
    std::vector<std::string> lettersByPosition(keywordLength);

    for (size_t i = 0; i < cipher.size(); i += keywordLength) {
        groups.push_back(cipher.substr(i, keywordLength));
    }

    for (const auto& group : groups) {
        for (size_t pos = 0; pos < group.length(); ++pos) {
            if (pos < keywordLength) {
                lettersByPosition[pos] += group[pos];
            }
        }
    }

    std::cout << "\n\t\t***Letters by Position***\n\n";
    for (size_t i = 0; i < lettersByPosition.size(); ++i) {
        auto frequencyCount = getFrequencies(lettersByPosition[i]);
        auto sortedFrequencies = sortFrequencies(frequencyCount);

        if (!sortedFrequencies.empty()) {
            char mostFrequentLetter = sortedFrequencies[0].first;
            int shift = calculateShift('A', mostFrequentLetter);
            char letterFromShift = getLetterFromShift(shift);
            
            shifts.push_back(shift);

            std::cout << "STR" << (i + 1) << ": "  
                      << "\nMost frequent letter: " << mostFrequentLetter 
                      << " - Frequency: " << sortedFrequencies[0].second 
                      << " - Shift from E: " << shift 
                      << " - Letter from Shift: " << letterFromShift << "\n";
        }
    }
    std::cout << "\n**********************************************************\n\n";
}

std::string decryptVigenere(const std::string& cipher, const std::string& key) {
    std::string decrypted;
    size_t keyIndex = 0;

    for (char ch : cipher) {
        if (isalpha(ch)) {
            char offset = isupper(ch) ? 'A' : 'a';
            char keyChar = key[keyIndex % key.length()];

            char decryptedChar = (ch - keyChar + 26) % 26 + offset; 
            decrypted += decryptedChar;

            keyIndex++;
        } else {
            decrypted += ch;
        }
    }

    return decrypted;
}

int main() {
    std::string cipher = readFile("./resources/cipher.txt");
    if (cipher.empty()) {
        return 1;
    }

    std::vector<std::pair<char, double>> frequencyTable = frequencyReader("./resources/frequencia.txt");
    if (frequencyTable.empty()) {
        return 1;
    }

    int patternLength = 4;
    divisorFrequencies(cipher, patternLength);

    int keywordLength = 6;
    std::vector<int> shifts;
    Kasiski(cipher, keywordLength, frequencyTable, shifts);

    std::string key;
    for (int shift : shifts) {
        key += getLetterFromShift(shift);
    }

    std::string decryptedText = decryptVigenere(cipher, key);
    std::cout << "Decrypted Text: " << decryptedText << std::endl;

    return 0;
}