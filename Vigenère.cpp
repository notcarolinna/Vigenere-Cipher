#include <iostream>
#include <string>
#include <fstream>
#include <unordered_map>
#include <vector>
#include <algorithm> 
#include <set>
#include <map>

std::set<int> getDivisors(int n) {
    std::set<int> divisors;
    for (int i = 1; i <= n; i++) {
        if (n % i == 0) {
            divisors.insert(i);
        }
    }
    return divisors;
}

std::map<int, int> findDivisorFrequencies(const std::string& cipher, int patternLength) {
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

    return divisorFrequency;
}

int main() {
    std::ifstream cipherFile("./resources/teste-eng.txt");
    std::string cipher;

    if (!cipherFile) {
        std::cerr << "Unable to open cipherFile" << std::endl;
        return 1;
    }

    std::string line;
    while (std::getline(cipherFile, line)) {
        cipher += line;
    }

    int patternLength = 4;
    std::map<int, int> divisorFrequency = findDivisorFrequencies(cipher, patternLength);

    std::vector<std::pair<int, int>> sortedDivisors(divisorFrequency.begin(), divisorFrequency.end());
    std::sort(sortedDivisors.begin(), sortedDivisors.end(), [](const auto& a, const auto& b) {
        return a.second > b.second;
    });

    std::cout << "\n\t\t***Most frequent divisors***\n\n";
    int count = 0;
    for (size_t i = 1; i < sortedDivisors.size() && count < 10; ++i) {
        std::cout << "Divisor: " << sortedDivisors[i].first << " - Frequency: " << sortedDivisors[i].second << '\n';
        count++;
    }
    std::cout << "\n**********************************************************\n";

    return 0;
}