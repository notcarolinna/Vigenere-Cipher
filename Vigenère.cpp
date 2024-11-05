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

// função que calcula a frequência de divisores de padrões de tamanho patternLength = 4
int divisorFrequencies(const std::string& cipher, int patternLength) {
    std::unordered_map<std::string, std::vector<int>> patternPositions; // dicionário de padrões e posições
    std::map<int, int> divisorFrequency; // dicionário de frequência de divisores

    for (size_t i = 0; i <= cipher.size() - patternLength; i++) { // para cada padrão de tamanho patternLength
        std::string pattern = cipher.substr(i, patternLength); 
        patternPositions[pattern].push_back(i); // adiciona a posição do padrão no texto
    }

    for (const auto& pair : patternPositions) { // para cada padrão
        const std::vector<int>& positions = pair.second; // pega as posições do padrão
        if (positions.size() > 1) { // se o padrão ocorre mais de uma vez
            for (size_t i = 1; i < positions.size(); i++) {
                int distance = positions[i] - positions[i - 1];
                std::set<int> divisors = getDivisors(distance); // calcula os divisores da distância
                for (int divisor : divisors) { // para cada divisor
                    divisorFrequency[divisor]++; // incrementa a frequência do divisor
                }
            }
        }
    }

    // ordena os divisores por frequência decrescente
    std::vector<std::pair<int, int>> sortedDivisors(divisorFrequency.begin(), divisorFrequency.end());
    std::sort(sortedDivisors.begin(), sortedDivisors.end(), [](const auto& a, const auto& b) {
        return a.second > b.second;
    });

    if (sortedDivisors.size() >= 4) {
        return sortedDivisors[3].first;  // retorna o 3º divisor mais frequente para ser o comprimento da chave
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

// função que calcula a frequência de cada caractere 
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

// função que calcula o deslocamento entre duas letras:
// se a letra mais frequente do grupo for 'S' e a letra mais frequente do alfabeto for 'A', o deslocamento é 18
int calculateShift(char from, char to) {
    return (to - from + 26) % 26; 
}

// função que calcula o deslocamento entre as duas letras no alfabeto:
// se o deslocamento for 18, a letra 'A' deslocada 4 posições é 'S'
char getLetterFromShift(int shift) {
    return 'A' + shift; 
}

void Kasiski(const std::string& cipher, size_t keywordLength, std::vector<int>& shifts) {
    std::vector<std::string> groups;
    std::vector<std::string> lettersByPosition(keywordLength); // posição das letras no grupo
    // No caso 42994, o comprimento da chave é 6, então o texto cifrado é dividido em grupos de 6 letras

    for (size_t i = 0; i < cipher.size(); i += keywordLength) {
        groups.push_back(cipher.substr(i, keywordLength));
    }

    for (const auto& group : groups) {
        auto frequencyCount = getFrequencies(group); // calcula a frequência de cada letra no grupo

        for (size_t pos = 0; pos < group.length(); ++pos) { 
            if (pos < keywordLength) { // se a posição for menor que o comprimento da chave
                lettersByPosition[pos] += group[pos]; // adiciona a letra na posição 
            }
        }
    }

    for (size_t i = 0; i < lettersByPosition.size(); ++i) { // para cada posição
        auto frequencyCount = getFrequencies(lettersByPosition[i]); // calcula a frequência de cada letra no grupo
        auto sortedFrequencies = sortFrequencies(frequencyCount); // ordena as frequências

        if (!sortedFrequencies.empty()) { 
            char mostFrequentLetter = sortedFrequencies[0].first; // pega a letra mais frequente
            int shift = calculateShift('A', mostFrequentLetter); // calcula o deslocamento
            char letterFromShift = getLetterFromShift(shift); // pega a letra do deslocamento no alfabeto
            shifts.push_back(shift); // adiciona o deslocamento ao vetor de deslocamentos
        }
    }
}

std::string decryptVigenere(const std::string& cipher, const std::string& key) {
    std::string decrypted;
    size_t keyIndex = 0;

    for (char ch : cipher) { // para cada caractere do texto cifrado
        if (isalpha(ch)) { // se o caractere for uma letra
            // o offset é 'A' se o caractere for maiúsculo e 'a' se for minúsculo
            char offset = isupper(ch) ? 'A' : 'a'; 
            char keyChar = key[keyIndex % key.length()]; // pega o caractere da chave na posição keyIndex

            char decryptedChar = (ch - keyChar + 26) % 26 + offset;  // calcula o caractere descriptografado 
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
