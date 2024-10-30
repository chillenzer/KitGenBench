#include <membenchmc/membenchmc.h>
#include <membenchmc/version.h>

#include <cstdlib>
#include <cxxopts.hpp>
#include <fstream>
#include <optional>

using nlohmann::json;

auto defineCmdlineInterface() {
  cxxopts::Options options("my_program", "A program to process a configuration file");
  // clang-format off
  options.add_options()
    ("c,config", "Path to the configuration file", cxxopts::value<std::string>())
    ("h,help", "Print help")
    ( "v,version", "Print the current version number");
  // clang-format on
  return options;
}

std::optional<std::string> extractConfigFilePath(int argc, char* argv[], auto& options) {
  auto result = options.parse(argc, argv);
  // Check if help was requested
  if (result.count("help")) {
    std::cout << options.help() << std::endl;
    exit(0);
  }
  if (result["version"].template as<bool>()) {
    std::cout << "MemBenchMC, version " << MEMBENCHMC_VERSION << std::endl;
    exit(0);
  }
  // Check if the config option was provided
  if (!result.count("config")) {
    std::cerr << "Warning: No configuration file specified. Use --config <path_to_config_file> to "
                 "do so. Using the default now."
              << std::endl;
  } else {
    // Get the path to the configuration file
    return result["config"].template as<std::string>();
  }
  return {};
}

json parseJsonFile(const std::string& filename) {
  // Open the file
  std::ifstream file(filename);
  if (!file.is_open()) {
    throw std::runtime_error("Could not open file: " + filename);
  }
  // Parse the JSON file into an json object
  json json;
  try {
    file >> json;
  } catch (const json::parse_error& e) {
    throw std::runtime_error("Error parsing JSON file: " + filename + " - " + e.what());
  }
  return json;
}

auto readConfigFilePath(int argc, char* argv[]) {
  auto options = defineCmdlineInterface();
  try {
    return extractConfigFilePath(argc, argv, options);
  } catch (const cxxopts::OptionException& e) {
    std::cerr << "Error parsing options: " << e.what() << std::endl;
    exit(1);
  }
}

json defaultConfig{};

json supplementWithDefaults(const json& providedConfig) {
  auto config = defaultConfig;
  config.merge_patch(providedConfig);
  return config;
}

json composeConfig(int argc, char* argv[]) {
  auto configFilePath = readConfigFilePath(argc, argv);
  if (configFilePath) {
    auto providedConfig = parseJsonFile(configFilePath.value());
    return supplementWithDefaults(providedConfig);
  }
  return defaultConfig;
}

struct IndividualReports {};

json composeReport(json const& metadata, json const& config,
                   [[maybe_unused]] IndividualReports const& individualReports) {
  json report{};
  report["metadata"] = metadata;
  report["config"] = config;
  return report;
}

void output(json const& report) { std::cout << report << std::endl; }

auto main(int argc, char* argv[]) -> int {
  auto metadata = membenchmc::gatherMetadata();

  std::cout << 7 << std::endl;
  auto config = composeConfig(argc, argv);
  std::cout << 8 << std::endl;
  auto reports = IndividualReports{};
  std::cout << 9 << std::endl;
  auto report = composeReport(metadata, config, reports);
  std::cout << 10 << std::endl;
  output(report);
  std::cout << 11 << std::endl;
  return EXIT_SUCCESS;
}
