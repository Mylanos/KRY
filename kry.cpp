#include <iostream>
#include <string>
#include <fstream>
#include "cxxopts.hpp"
#include <math.h>
#include <list>
#include <cctype>

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
    input = "";  // input content (string variable)
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
    std::cerr << "Error parsing fiels: No such file";
    exit(1);
  }
  else {
    file << "Learning File handling";
    file.close();
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
      ("input", "vstupni string", cxxopts::value<std::string>(spec.input))
      ;

    options.parse_positional({ "input" });

    auto result = options.parse(argc, argv);

    if (result.count("help"))
    {
      std::cout << options.help({ "", "Group" }) << std::endl;
      exit(0);
    }

    return spec;

  }
  catch (const cxxopts::OptionException& e)
  {
    std::cerr << "error parsing arguments: " << e.what() << std::endl;
    exit(1);
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
bool charSortBySec(const std::pair<char, int>& a, const std::pair<char, int>& b)
{
  return (a.second < b.second);
}

bool stringSortBySec(const std::pair<std::string, int>& a, const std::pair<std::string, int>& b)
{
  return (a.second < b.second);
}


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
      for (int j = 0; j < int(decrypted.size()) - 1; j++) {
        std::string bigram = decrypted.substr(j, 2);
        if (bigram_occurance.find(bigram) == bigram_occurance.end()) {
          bigram_occurance[bigram] = 0;
        }
        else {
          bigram_occurance[bigram]++;
        }
      }
    }
  }
}

std::pair<int, int> determine_keys(std::vector<std::pair<char, int>>& char_freq, std::vector<std::pair<std::string, int>>& bigram_freq) {
  std::vector<char> freq_cz_chars = { 'E', 'A', 'O', 'I' };

  // SINGLE CHAR KEYS ESTIMATION
  std::vector<char> most_freq_chars;
  for (int i = 1; i <= 4; i++) {
    most_freq_chars.push_back(char_freq.end()[-i].first);
  }
  std::vector<int> potentional_a_keys;
  std::vector<int> potentional_b_keys;

  reverse_calc(most_freq_chars[0], most_freq_chars[1], freq_cz_chars[0], freq_cz_chars[1], potentional_a_keys, potentional_b_keys);
  reverse_calc(most_freq_chars[0], most_freq_chars[2], freq_cz_chars[0], freq_cz_chars[2], potentional_a_keys, potentional_b_keys);
  reverse_calc(most_freq_chars[0], most_freq_chars[3], freq_cz_chars[0], freq_cz_chars[3], potentional_a_keys, potentional_b_keys);
  reverse_calc(most_freq_chars[1], most_freq_chars[3], freq_cz_chars[1], freq_cz_chars[3], potentional_a_keys, potentional_b_keys);
  reverse_calc(most_freq_chars[1], most_freq_chars[2], freq_cz_chars[1], freq_cz_chars[2], potentional_a_keys, potentional_b_keys);
  reverse_calc(most_freq_chars[2], most_freq_chars[3], freq_cz_chars[2], freq_cz_chars[3], potentional_a_keys, potentional_b_keys);
  sort(potentional_b_keys.begin(), potentional_b_keys.end());

  // BIGRAM KEYS ESTIMATION ACCORDING TO SINGLE CHAR ESTIMATION
  std::vector<std::string> bigrams_frequent = { "ST", "NI", "PO", "OV", "RO", "EN", "NA", "JE", "PR", "TE", "LE", "KO", "NE", "OD" };
  std::vector<std::string> most_freq_bigrams;
  for (int i = 1; i <= 8; i++) {
    most_freq_bigrams.push_back(bigram_freq.end()[-i].first);
  }

  int b_lower_bound = potentional_b_keys.begin()[0]; // first possible b key in determined range
  int b_upper_bound = potentional_b_keys.end()[-1]; // last possible b key in determined range
  int max_bigram_match_count = 0;
  std::pair<int, int> best_keys;
  for (auto& a_key : potentional_a_keys) {
    for (int b_key = b_lower_bound; b_key <= b_upper_bound; b_key++) {
      int bigram_match_count = 0;
      for (auto& freq_bigram : most_freq_bigrams) {
        std::string decrypted = decryption(freq_bigram, a_key, b_key);
        if (std::find(bigrams_frequent.begin(), bigrams_frequent.end(), decrypted) != bigrams_frequent.end()) {
          bigram_match_count++;
        }
      }
      if (bigram_match_count > max_bigram_match_count) {
        best_keys = { a_key, b_key };
        max_bigram_match_count = bigram_match_count;
      }
    }
  }
  return best_keys;
}

std::pair<int, int> frequence_analysis(std::string& content)
{
  int i = 0, j;
  // SINGLE CHAR FREQUENCY
  std::vector<int> char_frequency(26, 0);
  for (auto& c : content) {
    if (c >= 'A' && c <= 'Z') {
      j = c - 'A';
      ++char_frequency[j];
    }
    if (c >= 'a' && c <= 'z') {
      j = c - 'a';
      ++char_frequency[j];
    }
  }

  // SORTING SINGLE CHAR FREQUENCY
  std::vector<std::pair<char, int>> char_freq_sorted;
  //init vector of characters and their occurance
  for (i = 0; i < 26; i++)
    char_freq_sorted.push_back({ char(i + 'A'), char_frequency[i] });
  sort(char_freq_sorted.begin(), char_freq_sorted.end(), charSortBySec);


  // BIGRAM FREQUENCY
  std::vector<std::pair<std::string, int>>bigram_frequency;
  for (int j = 0; j < int(content.size()) - 2; j += 2) {
    std::string bigram = content.substr(j, 2);
    //skip last chars
    if (bigram[1] == ' ') {
      continue;
    }
    if (bigram[0] == ' ') {
      j--;
      continue;
    }
    auto it = std::find_if(bigram_frequency.begin(), bigram_frequency.end(), [&bigram](const std::pair<std::string, int>& p) { return p.first == bigram; });
    if (it == bigram_frequency.end()) {
      bigram_frequency.push_back({ bigram, 1 });
    }
    else {
      (*it).second++;
    }

  }

  // SORTING BIGRAM FREQUENCY
  sort(bigram_frequency.begin(), bigram_frequency.end(), stringSortBySec);


  return determine_keys(char_freq_sorted, bigram_frequency);
}

void print_result(std::string result, std::pair<int, int> determined_keys, Specification spec)
{
  if (!spec.output_file.empty()) {
    std::ofstream myfile(spec.output_file);
    if (myfile.is_open())
    {
      myfile << result;
      std::cout << "a=" << determined_keys.first << ",b=" << determined_keys.second << std::endl;
      myfile.close();
    }
    else {
      std::cerr << "Unable to open file";
      exit(1);
    }
  }
  else {
    std::cout << result;
  }
}

std::string read_file(std::string& file_path)
{
  std::fstream file;
  std::string content;
  file.open(file_path, std::ios::in);
  if (!file)
  {
    std::cerr << "Error parsing files: No such file";
    exit(1);
  }
  content.assign((std::istreambuf_iterator<char>(file)), (std::istreambuf_iterator<char>()));
  file.close();
  return content;
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
  else if (!spec.input.empty()) {
    return spec.input;
  }
  else {
    return read_stdin();
  }
}

int main(int argc, char** argv)
{
  auto spec = parse(argc, argv);

  std::string line = get_input(spec);
  std::string result;
  std::pair<int, int> determined_keys;
  if (spec.encryption) {
    result = encryption(line, spec);
  }
  if (spec.decryption) {
    result = decryption(line, spec.a, spec.b);
  }
  if (spec.decryption_no_key) {
    determined_keys = frequence_analysis(line);
    result = decryption(line, determined_keys.first, determined_keys.second);
  }

  print_result(result, determined_keys, spec);
  return 0;
}
