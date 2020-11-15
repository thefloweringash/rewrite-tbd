#include <iostream>
#include <optional>
#include <yaml.h>
#include <sys/stat.h>

#include "rewrite.h"
#include "tbdv2.h"
#include "yaml.h"
#include "tbdv4.h"

static bool starts_with(const std::string &prefix, const std::string &str) {
    if (str.length() < prefix.length()) {
        return false;
    }
    return str.compare(0, prefix.length(), prefix) == 0;
}

static bool ends_with(const std::string &suffix, const std::string &str) {
    if (str.length() < suffix.length()) {
        return false;
    }
    return str.compare(str.length() - suffix.length(), suffix.length(), suffix) == 0;
}

static std::optional<std::string> map_library_path(const rewrite_config &cfg, const std::string &entry) {
    auto constReplacement = cfg.constPaths.find(entry);
    if (constReplacement != cfg.constPaths.end()) {
        return constReplacement->second;
    }

    for (const auto &prefix : cfg.prefixes) {
        if (starts_with(prefix.first, entry)) {
            return prefix.second + entry.substr(prefix.first.length());
        }
    }

    return {};
}

static bool target_exists(const std::string &replacement) {
    struct stat st{};

    // The literal file is present, accept immediately.
    if (stat(replacement.c_str(), &st) == 0) {
        return true;
    }

    // Framework style
    // tbd file refers to /S/L/F/Foo.framework/Foo
    // resolved file is at /S/L/F/Foo.framework/Foo.tbd
    if (!ends_with(".tbd", replacement)) {
        if (stat((replacement + ".tbd").c_str(), &st) == 0) {
            return true;
        }
    }

    // dylib style
    // tbd file refers to /usr/lib/libfoo.dylib
    // resolved file is at /usr/lib/libfoo.tbd
    std::string suffix = ".dylib";
    if (ends_with(suffix, replacement)) {
        std::string dylibTbd = replacement.substr(0, replacement.length() - suffix.length()) + ".tbd";
        if (stat(dylibTbd.c_str(), &st) == 0) {
            return true;
        }
    }

    return false;
}

static void rewrite_library_path(rewrite_result *result, const rewrite_config &cfg, yaml_node_t *lib_node) {
    require_type(lib_node, YAML_SCALAR_NODE);

    auto lib_name = std::string{
            reinterpret_cast<char *>(lib_node->data.scalar.value),
            lib_node->data.scalar.length};

    auto replacement = map_library_path(cfg, lib_name);

    if (!replacement) {
        result->unmatchedPaths.push_back(lib_name);
        return;
    }

    // TODO: is this the correct way to replace a value? should I free() the old one?
    // TODO: is the length correct?
    lib_node->data.scalar.value = reinterpret_cast<yaml_char_t *>(strdup(replacement->c_str()));
    lib_node->data.scalar.length = replacement->length();

    if (cfg.checkExistence) {
        if (!target_exists(*replacement)) {
            result->nonExistentPaths.push_back(*replacement);
        }
    }

    if (cfg.requiredPrefix) {
        if (!starts_with(*cfg.requiredPrefix, *replacement)) {
            result->missingPrefixPaths.push_back(*replacement);
        }
    }
}

std::ostream &operator<<(std::ostream &os, const rewrite_config &cfg) {
    os << "Exact mappings:" << std::endl;
    for (const auto &prefix : cfg.constPaths) {
        os << "  - " << prefix.first << " -> " << prefix.second << std::endl;
    }

    os << "Prefix mappings:" << std::endl;
    for (const auto &prefix : cfg.prefixes) {
        os << "  - " << prefix.first << " -> " << prefix.second << std::endl;
    }

    os << "Required prefix: " << (cfg.requiredPrefix ? *cfg.requiredPrefix : "none") << std::endl;

    os << "Check existence of resulting paths: " << (cfg.checkExistence ? "yes" : "no") << std::endl;

    return os;
}

rewrite_result rewrite_config::rewrite(yaml_document_t *document) const {
    rewrite_result r;

    std::function<void(yaml_node_t *)> rewrite = [&](yaml_node_t *library_path) {
        rewrite_library_path(&r, *this, library_path);
    };

    tbdv2::for_each_export(document, rewrite);
    tbdv4::for_each_export(document, rewrite);

    return r;
}