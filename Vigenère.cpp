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
        // se i eh um divisor de n
        if (n % i == 0) {
            divisors.insert(i);
        }
    }
    return divisors;
}

// funcao que calcula a frequencia de divisores de padroes 
int divisorFrequencies(const std::string& cipher, int patternLength) {
    // dicionario de padroes de posicoes e dicionario de frequencia de divisores
    std::unordered_map<std::string, std::vector<int>> patternPositions;
    std::map<int, int> divisorFrequency;

    // para cada padrao de tamanho patternLength, adiciona a posicao do padrao no texto
    for (size_t i = 0; i <= cipher.size() - patternLength; i++) {
        std::string pattern = cipher.substr(i, patternLength); 
        patternPositions[pattern].push_back(i);
    }

    // para cada padrao, pega as posicoes do padrao
    for (const auto& pair : patternPositions) {
        const std::vector<int>& positions = pair.second;
        // se o padrao ocorre mais de uma vez
        if (positions.size() > 1) {
            for (size_t i = 1; i < positions.size(); i++) {
                int distance = positions[i] - positions[i - 1];
                std::set<int> divisors = getDivisors(distance);
                // para cada divisor, incrementa a frequencia do divisor
                for (int divisor : divisors) {
                    divisorFrequency[divisor]++;
                }
            }
        }
    }

    // ordena os divisores por >frequencia< decrescente
    std::vector<std::pair<int, int>> sortedDivisors(divisorFrequency.begin(), divisorFrequency.end());
    std::sort(sortedDivisors.begin(), sortedDivisors.end(), [](const auto& a, const auto& b) {
        return a.second > b.second;
    });

    if (sortedDivisors.size() >= 4) {
        // retorna o 3o divisor mais frequente para ser o comprimento da chave
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

// funcao que calcula a frequencia de cada caractere 
std::unordered_map<char, int> getFrequencies(const std::string& str) {
    std::unordered_map<char, int> frequencyCount;
    for (char ch : str) {
        frequencyCount[ch]++;
    }
    return frequencyCount;
}

// funcao que ordena as frequencias de caracteres em ordem decrescente
std::vector<std::pair<char, int>> sortFrequencies(const std::unordered_map<char, int>& frequencyCount) {
    std::vector<std::pair<char, int>> sortedFrequencies(frequencyCount.begin(), frequencyCount.end());
    std::sort(sortedFrequencies.begin(), sortedFrequencies.end(), [](const auto& a, const auto& b) {
        return a.second > b.second;
    });
    return sortedFrequencies;
}

// funcao que calcula o deslocamento entre duas letras:
// EXEMPLO: se a letra mais frequente do grupo for 'S' e a letra mais frequente do alfabeto for 'A', o deslocamento eh 18
int calculateShift(char from, char to) {
    return (to - from + 26) % 26; 
}

// função que calcula o deslocamento entre as duas letras no alfabeto:
// EXEMPLO: se o deslocamento for 4, a letra 'A' deslocada 4 posições eh 'E'
char getLetterFromShift(int shift) {
    return 'A' + shift; 
}

void Kasiski(const std::string& cipher, size_t keywordLength, std::vector<int>& shifts) {
    std::vector<std::string> groups;
    std::vector<std::string> lettersByPosition(keywordLength); // posicao das letras no grupo
    // No caso 42994, o comprimento da chave eh 6, entao o texto cifrado eh dividido em grupos de 6 letras

    for (size_t i = 0; i < cipher.size(); i += keywordLength) {
        groups.push_back(cipher.substr(i, keywordLength));
    }

    for (const auto& group : groups) {
        auto frequencyCount = getFrequencies(group); // calcula a frequencia de cada letra no grupo

        for (size_t pos = 0; pos < group.length(); ++pos) { 
            if (pos < keywordLength) { // se a posicao for menor que o comprimento da chave
                lettersByPosition[pos] += group[pos]; // adiciona a letra na posicao 
            }
        }
    }

    // para cada posicao, calcula a frequencia de cada letra no grupo e ordena as frequencias
    for (size_t i = 0; i < lettersByPosition.size(); ++i) {
        auto frequencyCount = getFrequencies(lettersByPosition[i]);
        auto sortedFrequencies = sortFrequencies(frequencyCount); 

        // Essa parte do código é responsável por obter a chave, assumindo que a letra mais frequente
        // do idioma seja A, que é o caso do português. Caso deseja decifrar um texto em outro idioma,
        // é necessário alterar a letra mais frequente da tabela correspondente.
        
        if (!sortedFrequencies.empty()) { 
            char mostFrequentLetter = sortedFrequencies[0].first; // letra +frequente
            int shift = calculateShift('A', mostFrequentLetter); // deslocamento
            char letterFromShift = getLetterFromShift(shift); // letra do deslocamento
            shifts.push_back(shift); // adiciona ao vetor de deslocamentos
        }
    }
}

std::string decryptVigenere(const std::string& cipher, const std::string& key) {
    std::string decrypted;
    size_t keyIndex = 0;

    // para cada caractere do texto cifrado, se for uma letra, descriptografa
    for (char ch : cipher) { 
        if (isalpha(ch)) {
            // se a letra eh maiuscula, o offset eh 'A', se não for, eh 'a'
            char offset = isupper(ch) ? 'A' : 'a'; 
            char keyChar = key[keyIndex % key.length()]; // char da chave na posição keyIndex

            char decryptedChar = (ch - keyChar + 26) % 26 + offset;  // calcula o char descriptografado 
            decrypted += decryptedChar;

            keyIndex++;
        } else {
            decrypted += ch;
        }
    }

    return decrypted;
}

// lista os arquivos de texto no diretorio
void listCiphertexts(const std::string& directoryPath) {
    std::cout << "Available ciphertexts for decryption:\n";
    for (const auto& entry : fs::directory_iterator(directoryPath)) {
        if (entry.is_regular_file()) {
            std::cout << entry.path().filename().string() << '\n';
        }
    }
}

/*
 *
 * =D
 *
 */

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

    int patternLength = 4; // comprimento do padrao a ser buscado
    int keywordLength = divisorFrequencies(cipher, patternLength);  // comprimento da chave

    std::vector<int> shifts; // vetor de deslocamentos
    Kasiski(cipher, keywordLength, shifts);

    std::string key; 
    for (int shift : shifts) {
        key += getLetterFromShift(shift); // a chave eh formada pelas letras dos deslocamentos
    }

    std::string decryptedText = decryptVigenere(cipher, key);
    std::cout << "\nDecrypted Text: \n" << decryptedText << std::endl;

    std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
    std::cout << "\nKey: " << key << std::endl;
    std::cout << "Decryption took " << std::chrono::duration_cast<std::chrono::seconds>(end - begin).count() << " [s]" << std::endl;

    return 0;
}
