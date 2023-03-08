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

std::string decryption(std::string& content, Specification& spec)
{
  std::string result = "";
  for (char& c : content) {
    result.push_back(decrypt_character(c, spec.a, spec.b));
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

void determine_keys(std::vector<std::pair<char, int>>& occurance_sorted) {
  std::list<int> possible_a_keys = { 1, 3, 5, 7, 9, 11, 15, 17, 19, 21, 23, 25 };
  std::vector<char> frequent_czech_chars = { 'E', 'A', 'O', 'I' }; //, 'N', 'S', 'T', 'R', 'L', 'K', 'V', 'P', 'Y', 'M', 'U', 'D', 'J', 'H', 'C', 'Z', 'B', 'G', 'F', 'X', 'Q' };

  char most_frequent = occurance_sorted.back().first;
  char c;
  int a, b;
  std::vector<DeterminedKeys> analysis;
  for (auto& a_key : possible_a_keys) {
    for (int b_key = 0; b_key < LIMIT_ALPHABET; b_key++) {
      c = decrypt_character(most_frequent, a_key, b_key);
      if (c == 'A') {
        a = a_key;
        b = b_key;
      }
    }
  }
  std::cout << "Toto je kluc najdeny: " << a << " " << b << std::endl;
  //https://math.stackexchange.com/questions/375537/affine-encryption-and-frequency-analysis-need-help-seeing-where-im-going-wrong
}

void frequence_analysis(std::string& content)
{
  int i = 0, j;
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

  determine_keys(occurance_sorted);
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
    result = decryption(line, spec);
  }
  if (spec.decryption_no_key) {
    frequence_analysis(line);
  }

  std::cout << result << std::endl;
  std::cout << line << std::endl;
  return 0;
}
