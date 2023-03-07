#include<iostream>
#include <string>
#include "cxxopts.hpp"

class Specification {       // The class
  public:             // Access specifier
    int a;        // first key 'a' (int variable)
    int b;        // second key 'b' (int variable)
    std::string myString;  // Attribute (string variable)
};

cxxopts::ParseResult
parse(int argc, char* argv[])
{
  try
  {
    cxxopts::Options options(argv[0], " - example command line options");
    options
      .positional_help("[optional args]")
      .show_positional_help();

    bool encryption = false;
    bool decryption = false;
    bool decryption_no_key = false;

    options
      .allow_unrecognised_options()
      .add_options()
      ("e", "sifrovani", cxxopts::value<bool>(encryption))
      ("d", "desifrovani", cxxopts::value<bool>(decryption))
      ("c", "desifrovani bez znalosti klice", cxxopts::value<bool>(decryption_no_key))
      ("a", "prvni klic", cxxopts::value<int>(), "")
      ("b", "druhy klic", cxxopts::value<int>(), "N")
      ("f", "cesta k souboru", cxxopts::value<std::vector<std::string>>(), "FILE")
      ("o", "vystupni soubor s otevrenym textem", cxxopts::value<std::string>()
          ->default_value("a.out")->implicit_value("b.def"), "BIN")
      ("h,help", "Print help")
    ;

    options.parse_positional({"f", "o", "b"});

    auto result = options.parse(argc, argv);

    if (result.count("help"))
    {
      std::cout << options.help({"", "Group"}) << std::endl;
      exit(0);
    }

    if (encryption)
    {
      std::cout << "Saw option ‘e’ " << result.count("e") << " times " <<
        std::endl;
    }

    if (decryption)
    {
      std::cout << "Saw option ‘d’ " << result.count("d") << " times " <<
        std::endl;
    }

    if (result.count("f"))
    {
      auto& ff = result["f"].as<std::vector<std::string>>();
      std::cout << "Files" << std::endl;
      for (const auto& f : ff)
      {
        std::cout << f << std::endl;
      }
    }

    if (result.count("o"))
    {
      std::cout << "Output = " << result["output"].as<std::string>()
        << std::endl;
    }

    if (result.count("a"))
    {
      std::cout << "a = " << result["a"].as<int>() << std::endl;
    }

    return result;

  } catch (const cxxopts::OptionException& e)
  {
    std::cout << "error parsing options: " << e.what() << std::endl;
    exit(1);
  }
}

int main(int argc, char** argv)
{
    auto result = parse(argc, argv);
    auto arguments = result.arguments();
    std::cout << "Saw " << arguments.size() << " arguments" << std::endl;

    std::string line;
 
    getline(std::cin, line);
 
    std::cout << line << std::endl;
    return 0;
}