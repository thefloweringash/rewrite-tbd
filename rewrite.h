#ifndef REWRITE_TBD_REWRITE_H
#define REWRITE_TBD_REWRITE_H

#include <unordered_map>
#include <optional>
#include <vector>
#include <string>
#include <yaml.h>

struct rewrite_result;

struct rewrite_config {
    /**
     * Prefix matches
     */
    std::unordered_map<std::string, std::string> prefixes;

    /**
     * Absolute path rewrites, without any context.
     */
    std::unordered_map<std::string, std::string> constPaths;

    std::optional<std::string> requiredPrefix;

    bool checkExistence = true;

    friend std::ostream &operator<<(std::ostream &os, const rewrite_config &cfg);

    rewrite_result rewrite(yaml_document_t *document) const;
};

struct rewrite_result {
    std::vector<std::string> unmatchedPaths;

    std::vector<std::string> nonExistentPaths;

    std::vector<std::string> missingPrefixPaths;

    [[nodiscard]] bool failed() const {
        return !unmatchedPaths.empty() || !nonExistentPaths.empty();
    }
};

#endif //REWRITE_TBD_REWRITE_H
