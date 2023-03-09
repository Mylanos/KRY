#include <iostream>
#include <string>
#include <fstream>
#include "cxxopts.hpp"
#include <math.h>
#include <list>

int LIMIT_ALPHABET = 26;

class Specification {         // The class
public:                     // Access specifier
  int a;                    // first key 'a' (int variable)
  int b;                    // second key 'b' (int variable)
  bool encryption;          // flag 
  bool decryption;          // flag
  bool decryption_no_key;   // flag
  std::string input_file;  // input file (string variable)
  std::string output_file;  // input file (string variable)
  std::string input;  // input content (string variable)
  std::string output;  // output content (string variable)

  // Default Constructor
  Specification()
  {
    a = 4;
    b = 7;
    encryption = false;
    decryption = false;
    decryption_no_key = false;
    input_file = "";  // input file (string variable)
    output_file = "";  // output file (string variable)
    input = "Toto je default retazec na sifrovanie a desifrovanie";  // input content (string variable)
    output = "";  // output content (string variable)
  }
};

void write_file(std::string& file_path)
{
  std::fstream file;
  std::string content;
  file.open(file_path, std::ios::out);
  if (!file)
  {
    std::cout << "Error parsing fiels: No such file";
    exit(1);
  }
  else {
    file << "Learning File handling";
    file.close();
  }
}

std::string read_file(std::string& file_path)
{
  std::fstream file;
  std::string content;
  file.open(file_path, std::ios::in);
  if (!file)
  {
    std::cout << "Error parsing fiels: No such file";
    exit(1);
  }
  else {
    content.assign((std::istreambuf_iterator<char>(file)),
      (std::istreambuf_iterator<char>()));
    file.close();
  }
  return content;
}
// source and credit to https://www.javatpoint.com/prime-number-program-in-cpp
void check_prime_number(int& number)
{
  if (number > LIMIT_ALPHABET) {
    std::cerr << "Error parsing arguments: given key was out of range for given abeceda" << std::endl;
    exit(1);
  }
  int i, m = 0;
  m = number / 2;

  for (i = 2; i <= m; i++)
  {
    if (number % i == 0)
    {
      std::cerr << "Error parsing arguments: given key was not a prime number" << std::endl;
      exit(1);
    }
  }
}
Specification parse(int argc, char* argv[])
{
  try
  {
    cxxopts::Options options(argv[0], " - example command line options");
    options
      .positional_help("[optional args]")
      .show_positional_help();

    Specification spec;

    options
      .add_options()
      ("e", "sifrovani", cxxopts::value<bool>(spec.encryption))
      ("d", "desifrovani", cxxopts::value<bool>(spec.decryption))
      ("c", "desifrovani bez znalosti klice", cxxopts::value<bool>(spec.decryption_no_key))
      ("a", "prvni klic", cxxopts::value<int>(spec.a), "a muze byt pouze prvocislo mensi nez delka abecedy 26.")
      ("b", "druhy klic", cxxopts::value<int>(spec.b), "b muze byt pouze prvocislo mensi nez delka abecedy 26.")
      ("f", "cesta k souboru", cxxopts::value<std::string>(spec.input_file), "IN_FILE")
      ("o", "vystupni soubor s otevrenym textem", cxxopts::value<std::string>(spec.output_file), "OUT_FILE")
      ("h,help", "Print help")
      ;

    options.parse_positional({ "f", "o", "b" });

    auto result = options.parse(argc, argv);

    if (result.count("help"))
    {
      std::cout << options.help({ "", "Group" }) << std::endl;
      exit(0);
    }

    if (spec.encryption)
    {
      std::cout << "Saw option ‘e’ " << result.count("e") << " times " << std::endl;
    }

    if (spec.decryption)
    {
      std::cout << "Saw option ‘d’ " << result.count("d") << " times " <<
        std::endl;
    }

    if (spec.decryption_no_key)
    {
      std::cout << "Saw option ‘c’ " << result.count("c") << " times " <<
        std::endl;
    }

    if (result.count("o"))
    {
      std::cout << "Output file = " << result["o"].as<std::string>()
        << std::endl;
    }
    if (result.count("a"))
    {
      check_prime_number(spec.a);
      std::cout << "a = " << spec.a << std::endl;
    }
    return spec;

  }
  catch (const cxxopts::OptionException& e)
  {
    std::cerr << "error parsing arguments: " << e.what() << std::endl;
    exit(1);
  }
}

std::string read_stdin()
{
  std::string line;

  getline(std::cin, line);

  return line;
}

std::string get_input(Specification& spec)
{
  std::string result;
  if (!spec.input_file.empty()) {
    return read_file(spec.input_file);
  }
  else {
    return read_stdin();
  }
}

char encrypt_character(char c, Specification spec)
{
  int a = spec.a;
  int b = spec.b;
  int x, result;
  if (isupper(c)) {
    x = int(c) - 'A';
    result = a * x;
    result += b;
    result %= LIMIT_ALPHABET;
    result += 'A';
  }
  else if (isspace(c)) {
    return ' ';
  }
  else {
    x = int(c) - 'a';
    result = a * x;
    result += b;
    result %= LIMIT_ALPHABET;
    result += 'a';
  }
  return char(result);
}

std::string encryption(std::string& content, Specification& spec)
{
  std::string result = "";
  for (char& c : content) {
    result.push_back(encrypt_character(c, spec));
  }
  return result;
}

// CREDIT TO https://www.geeksforgeeks.org/multiplicative-inverse-under-modulo-m/
int mod_inverse(int A, int M)
{
  int result;
  for (int X = 1; X < M; X++)
    if (((A % M) * (X % M)) % M == 1) {
      result = X;
      break;
    }
  return result;
}

char decrypt_character(char c, int spec_a, int spec_b)
{
  int a = mod_inverse(spec_a, LIMIT_ALPHABET);
  int b = spec_b;
  int result;
  if (isupper(c)) {
    result = a * ((c + 'A' - b)) % LIMIT_ALPHABET;
    result += 'A';
  }
  else if (isspace(c)) {
    return ' ';
  }
  else {
    result = a * ((int(c) + 'a' - b + 30)) % LIMIT_ALPHABET;
    result += 'a';
  }
  return char(result);
}

std::string decryption(std::string& content, int spec_a, int spec_b)
{
  std::string result = "";
  for (char& c : content) {
    result.push_back(decrypt_character(c, spec_a, spec_b));
  }
  return result;
}

// credit to https://www.geeksforgeeks.org/sort-vector-of-pairs-in-ascending-order-in-c/
bool sortbysec(const std::pair<char, int>& a, const std::pair<char, int>& b)
{
  return (a.second < b.second);
}

class DeterminedKeys {
public:                     // Access specifier
  int a;                    // first key 'a' (int variable)
  int b;                    // second key 'b' (int variable)
  int numberOfMistakesSingleChar;          // flag 
  int numberOfMistakesDoubleChar;          // flag 

  // Constructor
  DeterminedKeys(int first_key, int second_key)
  {
    numberOfMistakesDoubleChar = 0;
    numberOfMistakesSingleChar = 0;
    a = first_key;
    b = second_key;
  }
};

void reverse_calc(char cipher1, char cipher2, char plain1, char plain2, std::vector<int>& a_keys, std::vector<int>& b_keys) {
  std::pair<int, int> keys;
  std::list<int> possible_a_keys = { 1, 3, 5, 7, 9, 11, 15, 17, 19, 21, 23, 25 };

  int possible_a = (cipher1 - cipher2 + 'A') * mod_inverse(plain1 - plain2 + 'A', LIMIT_ALPHABET) % LIMIT_ALPHABET;
  for (auto& a_key : possible_a_keys) {
    if (a_key == possible_a) {
      keys.first = a_key;
    }
  }
  keys.second = ((cipher1 - keys.first + 'A') * plain1) % LIMIT_ALPHABET;

  if (!std::count(a_keys.begin(), a_keys.end(), keys.first)) {
    a_keys.push_back(keys.first);
  }
  if (!std::count(b_keys.begin(), b_keys.end(), keys.second)) {
    b_keys.push_back(keys.second);
  }

}

void bigram_analysis(std::string& content, std::vector<int>& potentional_a_keys, std::vector<int>& potentional_b_keys) {
  std::vector<std::string> bigrams_frequent = { "st", "ni", "po", "ov", "ro", "en", "na" };
  int b_lower_bound = potentional_b_keys.begin()[1]; // first possible b key in determined range
  int b_upper_bound = potentional_b_keys.end()[-1]; // last possible b key in determined range

  std::map<std::string, int> bigram_occurance;
  for (auto& a_key : potentional_a_keys) {
    for (int b_key = b_lower_bound; b_key <= b_upper_bound; b_key++) {
      std::string decrypted = decryption(content, a_key, b_key);
      for (int j = 0; j < decrypted.size() - 1; j++) {
        std::string bigram = decrypted.substr(j, 2);
        if (bigram_occurance.find(bigram) == bigram_occurance.end()) {
          bigram_occurance[bigram] = 0;
        }
        else {
          bigram_occurance[bigram]++;
        }
      }
      // ASI ROBIM BULLSHIT TOTO - POKUSAL SOM SA TO SPRAVIT TAK ZE PO DOSTANI POTENCIONALNYCH KLUCOV Z SINGLE CHAR ODVODENI KLUCOV BUDEM APLIKOVAT MOZNE KLUCE NA TEN TEXT 
      // A POTOM PO APLIKOVANI KLUCOV BY SOM ZNOVA SPOCITAL TU FREKVENCIU BIGRAMOV AZ POKYM BY SOM NEDOSTAL TAKE BIGRAMY KTORE SU NAJCASTEJSIE PRE CESTINU
      // PRITOM STACI SPOCITAT FREKVENCIU BIGRAMOV NA ZACIATKU A POTOM UZ LEN APLIKOVAT POTENCIONALNE KLUCE NA TIE NAJCASTEJSIE BIGRAMY, JE TO MNOHOM EFEKTIVNEJSIE DO IT A EZ CLAP

    }
  }
}

void determine_keys(std::vector<std::pair<char, int>>& occurance_sorted, std::string& content) {
  std::vector<char> frequent_czech_chars = { 'E', 'A', 'O', 'I' }; //, 'N', 'S', 'T', 'R', 'L', 'K', 'V', 'P', 'Y', 'M', 'U', 'D', 'J', 'H', 'C', 'Z', 'B', 'G', 'F', 'X', 'Q' };

  char most_frequent = occurance_sorted.end()[-1].first;
  char sec_most_frequent = occurance_sorted.end()[-2].first;
  char third_most_frequent = occurance_sorted.end()[-3].first;
  char fourth_most_frequent = occurance_sorted.end()[-4].first;
  char c;
  std::pair<int, int> keys;
  std::vector<int> potentional_a_keys;
  std::vector<int> potentional_b_keys;
  std::pair<int, int> keys2;
  std::pair<int, int> keys3;
  std::pair<int, int> keys4;


  std::vector<DeterminedKeys> analysis;
  reverse_calc(most_frequent, sec_most_frequent, 'E', 'A', potentional_a_keys, potentional_b_keys);
  reverse_calc(most_frequent, third_most_frequent, 'E', 'O', potentional_a_keys, potentional_b_keys);
  reverse_calc(most_frequent, fourth_most_frequent, 'E', 'I', potentional_a_keys, potentional_b_keys);
  reverse_calc(sec_most_frequent, third_most_frequent, 'A', 'O', potentional_a_keys, potentional_b_keys);
  sort(potentional_b_keys.begin(), potentional_b_keys.end());

  // std::cout << "Toto je kluc najdeny: " << a << " " << b << std::endl;
  //https://math.stackexchange.com/questions/375537/affine-encryption-and-frequency-analysis-need-help-seeing-where-im-going-wrong
}

void frequence_analysis(std::string& content)
{
  int i = 0, j;
  // SINGLE CHAR FREQUENCY
  std::vector<int> alphabet_frequency(26, 0);
  for (auto& c : content) {
    if (c >= 'A' && c <= 'Z') {
      j = c - 'A';
      ++alphabet_frequency[j];
    }
    if (c >= 'a' && c <= 'z') {
      j = c - 'a';
      ++alphabet_frequency[j];
    }
  }

  std::vector<std::pair<char, int>> occurance_sorted;

  //init vector of characters and their occurance
  for (i = 0; i < 26; i++)
    occurance_sorted.push_back({ char(i + 'A'), alphabet_frequency[i] });

  //sort it
  sort(occurance_sorted.begin(), occurance_sorted.end(), sortbysec);

  // BIGRAM FREQUENCY

  determine_keys(occurance_sorted, content);
  //for (i = 0; i < 26; i++)
  //  std::cout << occurance_sorted[i].first << " " << occurance_sorted[i].second << std::endl;


}

int main(int argc, char** argv)
{
  auto spec = parse(argc, argv);

  std::string line = get_input(spec);
  std::string result;
  if (spec.encryption) {
    result = encryption(line, spec);
  }
  if (spec.decryption) {
    result = decryption(line, spec.a, spec.b);
  }
  if (spec.decryption_no_key) {
    frequence_analysis(line);
  }

  std::cout << result << std::endl;
  std::cout << line << std::endl;
  return 0;
}
