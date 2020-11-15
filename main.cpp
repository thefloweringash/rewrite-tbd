#include <stdexcept>
#include <cstdio>
#include <cerrno>
#include <yaml.h>
#include <memory>
#include <unordered_map>
#include <getopt.h>

#include "wrappers.h"
#include "rewrite.h"

struct rewrite_errors : public std::runtime_error {
    explicit rewrite_errors(const std::string &error) : std::runtime_error{error} {}
};

static std::pair<std::string, std::string> parse_mapping(const std::string &input) {
    auto pos = input.find(':');
    if (pos == std::string::npos) {
        throw std::runtime_error{
                std::string{"invalid mapping (no colon delimiter): "} + input
        };
    }

    std::string from = input.substr(0, pos);
    std::string to = input.substr(pos + 1);

    return std::make_pair(from, to);
}

static void emit_yaml(yaml_document_t *document, FILE *to) {
    yaml_emitter_wrapper emitter;

    yaml_emitter_set_output_file(&emitter.yaml_emitter, to);

    if (!yaml_emitter_dump(&emitter.yaml_emitter, document)) {
        throw std::runtime_error{"Failed to  dump yaml"};
    }
}

static void display_errors(const rewrite_config &cfg, const std::string &path, const rewrite_result &result) {
    std::cerr << "Errors occurred during rewrite of " << path << std::endl;

    std::cerr << "Rewrite config:" << std::endl
              << cfg << std::endl;

    if (!result.unmatchedPaths.empty()) {
        std::cerr << "The following paths did not match any rewrite rule" << std::endl;
        for (const auto &unmatchedPath : result.unmatchedPaths) {
            std::cerr << "  " << unmatchedPath << std::endl;
        }
    }

    if (!result.nonExistentPaths.empty()) {
        std::cerr << "The following rewritten paths did not exist" << std::endl;
        for (const auto &nonExistentPath : result.nonExistentPaths) {
            std::cerr << "  " << nonExistentPath << std::endl;
        }
    }

    if (!result.missingPrefixPaths.empty()) {
        std::cerr << "The following rewritten paths did not have the required prefix" << std::endl;
        for (const auto &missingPrefixPath : result.missingPrefixPaths) {
            std::cerr << "  " << missingPrefixPath << std::endl;
        }
    }
}

static void rewrite_file(const rewrite_config &cfg, const std::string &path, bool dryRun) {
    auto inputFile = std::unique_ptr<FILE, close_file>{fopen(path.c_str(), "r")};
    auto temporaryName = path + ".rewrite.tmp";

    if (!inputFile) {
        throw std::runtime_error{std::string{"opening input file: "} + strerror(errno)};
    }

    FILE *outputFile;
    if (!dryRun) {
        outputFile = fopen(temporaryName.c_str(), "w");
        if (!outputFile) {
            throw std::runtime_error{std::string{"opening output file"} + strerror(errno)};
        }
    }

    yaml_parser_wrapper parser;
    yaml_parser_set_input_file(&parser.yaml_parser, inputFile.get());

    while (!feof(inputFile.get())) {
        yaml_document_wrapper document{&parser.yaml_parser};

        auto result = cfg.rewrite(&document.yaml_document);

        if (result.failed()) {
            display_errors(cfg, path, result);
            throw rewrite_errors{"Rewriting failed"};
        }

        emit_yaml(&document.yaml_document, dryRun ? stdout : outputFile);
    }

    if (dryRun) {
        return;
    }

    if (fclose(outputFile) != 0) {
        throw std::runtime_error{std::string{"closing output file"} + strerror(errno)};
    }

    if (rename(temporaryName.c_str(), path.c_str()) != 0) {
        throw std::runtime_error{std::string{"replacing file"} + strerror(errno)};
    }
}

static void usage() {
    std::cerr << "TODO" << std::endl;
}

int main(int argc, char **argv) {
    rewrite_config cfg;
    bool dryRun = false;

    int option;
    while ((option = getopt(argc, argv, "p:c:r:n")) != -1) {
        switch (option) {
            case 'p':
                cfg.prefixes.insert(parse_mapping(optarg));
                break;
            case 'c':
                cfg.constPaths.insert(parse_mapping(optarg));
                break;
            case 'r':
                cfg.requiredPrefix = optarg;
                break;
            case 'n':
                dryRun = true;
                break;
            case '?':
                usage();
                return 2;
            default:
                throw std::runtime_error{"unexpected option from getopt"};
        }
    }

    for (int i = optind; i < argc; i++) {
        rewrite_file(cfg, argv[i], dryRun);
    }

    return 0;
}
