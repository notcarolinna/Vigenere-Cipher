# Vigenère Cipher Decryption   

## Introdução  
Este projeto implementa um algoritmo para decifrar mensagens codificadas usando o **Cifra de Vigenère**, um método clássico de criptografia que utiliza uma chave para encriptar e decriptar textos. O sistema emprega o método **Kasiski** para determinar o comprimento da chave, bem como análises de frequência de caracteres para identificar deslocamentos no alfabeto e reconstruir a chave.

## Metodologia  

### 1. **Análise de Frequências de Divisores**  
O algoritmo identifica padrões repetidos no texto cifrado para determinar possíveis comprimentos da chave.  
- **Passos principais**:
  - Localizar padrões repetidos e calcular as distâncias entre suas ocorrências.  
  - Determinar os divisores das distâncias e registrar suas frequências.  
  - Selecionar o divisor mais frequente (ou próximo) como o comprimento da chave.  

### 2. **Cálculo dos Deslocamentos (Kasiski)**  
Com o comprimento da chave determinado, o texto é dividido em grupos de caracteres correspondentes a cada posição na chave. Para cada grupo:  
- Calcula-se a frequência de cada letra.  
- Identifica-se a letra mais frequente (presumindo que ela representa 'A' no idioma alvo).  
- Determina-se o deslocamento necessário para cada posição da chave.  

### 3. **Descriptografia do Texto**  
Com a chave reconstruída:  
- Cada caractere cifrado é deslocado de acordo com o valor correspondente na chave.  
- Caracteres não alfabéticos são preservados.  

### 4. **Listagem de Arquivos Cifrados**  
O programa exibe os arquivos disponíveis no diretório especificado para escolha do texto a ser decifrado.

## Execução  
Este projeto requer um ambiente Linux devido ao uso de funcionalidades específicas do sistema.  
Para compilar e executar, utilize o comando `makerun`. Após a execução, será solicitado que você selecione o arquivo a ser decifrado.
