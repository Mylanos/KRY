#include<iostream>
#include <string>
#include "cxxopts.hpp"

int LIMIT_ABECEDA = 26;

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
        input_file = "input.txt";  // input file (string variable)
        output_file = "output.txt";  // output file (string variable)
        input = "Toto je retazec na sifrovanie a desifrovanie";  // input content (string variable)
        output = "";  // output content (string variable)
    }
};

//void read_file(std::string file_path)
//{
// std::ifstream myfile;
//  myfile.open ("example.txt");
// myfile << "Writing this to a file.\n";
//  myfile.close();
//}
//
// source and credit to https://www.javatpoint.com/prime-number-program-in-cpp
void check_prime_number(int number)
{
  if(number > LIMIT_ABECEDA){
    std::cerr << "Error parsing options: given key was out of range for given abeceda" << std::endl;
    exit(1);
  }
  int n, i, m=0;  
  m=number/2;  

  for(i = 2; i <= m; i++)  
  {  
      if(n % i == 0)  
      {   
          std::cerr << "Error parsing options: given key was not a prime number" << std::endl;
          exit(1);
          std::cout << "test";
      }  
       std::cout << "test " << n;
  }   
}
cxxopts::ParseResult
parse(int argc, char* argv[])
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

    options.parse_positional({"f", "o", "b"});

    auto result = options.parse(argc, argv);

    if (result.count("help"))
    {
      std::cout << options.help({"", "Group"}) << std::endl;
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

    if (result.count("f"))
    {
      std::cout << "Input file = " << result["f"].as<std::string>()
        << std::endl;
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

     if (result.count("b"))
    {
      check_prime_number(spec.b);
      std::cout << "b = " << spec.b << std::endl;
    }


    return result;

  } catch (const cxxopts::OptionException& e)
  {
    std::cerr << "error parsing options: " << e.what() << std::endl;
    exit(1);
  }
}

std::string read_stdin()
{
    std::string line;
 
    getline(std::cin, line);
    
    return line;
}

int main(int argc, char** argv)
{
    auto result = parse(argc, argv);
    auto arguments = result.arguments();
    std::cout << "Saw " << arguments.size() << " arguments" << std::endl;

    std::string line = read_stdin();
    
    std::cout << line << std::endl;
    return 0;
}
