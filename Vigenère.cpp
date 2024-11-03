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
#include <filesystem>
#include <chrono>

namespace fs = std::filesystem;

std::set<int> getDivisors(int n) {
    std::set<int> divisors;
    for (int i = 1; i <= n; i++) {
        if (n % i == 0) {
            divisors.insert(i);
        }
    }
    return divisors;
}

int divisorFrequencies(const std::string& cipher, int patternLength) {
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

    std::cout << "\n\n10 divisores mais frequentes:\n";
    for (size_t i = 0; i < std::min(sortedDivisors.size(), size_t(10)); i++) {
        std::cout << "Divisor: " << sortedDivisors[i].first << " Frequency: " << sortedDivisors[i].second << std::endl;
    }

    if (sortedDivisors.size() >= 4) {
        return sortedDivisors[3].first;  
    } else {
        std::cerr << "Menos de 4 divisores encontrados.\n";
        return 1;  
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

void Kasiski(const std::string& cipher, size_t keywordLength, std::vector<int>& shifts) {
    std::vector<std::string> groups;
    std::vector<std::string> lettersByPosition(keywordLength);

    for (size_t i = 0; i < cipher.size(); i += keywordLength) {
        groups.push_back(cipher.substr(i, keywordLength));
    }

    for (const auto& group : groups) {
        auto frequencyCount = getFrequencies(group);

        for (size_t pos = 0; pos < group.length(); ++pos) {
            if (pos < keywordLength) {
                lettersByPosition[pos] += group[pos];
            }
        }
    }

    for (size_t i = 0; i < lettersByPosition.size(); ++i) {
        auto frequencyCount = getFrequencies(lettersByPosition[i]);
        auto sortedFrequencies = sortFrequencies(frequencyCount);

        if (!sortedFrequencies.empty()) {
            char mostFrequentLetter = sortedFrequencies[0].first;
            int shift = calculateShift('A', mostFrequentLetter);
            char letterFromShift = getLetterFromShift(shift);
            
            shifts.push_back(shift);
        }
    }
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

void listCiphertexts(const std::string& directoryPath) {
    std::cout << "Available ciphertexts for decryption:\n";
    for (const auto& entry : fs::directory_iterator(directoryPath)) {
        if (entry.is_regular_file()) {
            std::cout << entry.path().filename().string() << '\n';
        }
    }
}

int main() {
    std::cout << "\n\nVigenère Cipher Decryption\n" << std::endl;

    const std::string ciphertextsDir = "./resources/ciphertexts";
    listCiphertexts(ciphertextsDir);

    std::string selectedCiphertext; 
    std::cout << "Enter the name of the ciphertext to decrypt (e.g., 'cipher.txt'): ";
    std::cin >> selectedCiphertext;

    std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();

    std::string cipher = readFile(ciphertextsDir + "/" + selectedCiphertext);
    if (cipher.empty()) {
        return 1;
    }

    int patternLength = 4;
    int keywordLength = divisorFrequencies(cipher, patternLength); 

    std::vector<int> shifts;
    Kasiski(cipher, keywordLength, shifts);

    std::string key;
    for (int shift : shifts) {
        key += getLetterFromShift(shift);
    }

    std::string decryptedText = decryptVigenere(cipher, key);
    std::cout << "\nDecrypted Text: \n" << decryptedText << std::endl;

    std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
    std::cout << "\nKey: " << key << std::endl;
    std::cout << "Decryption took " << std::chrono::duration_cast<std::chrono::seconds>(end - begin).count() << " [s]" << std::endl;

    return 0;
}
