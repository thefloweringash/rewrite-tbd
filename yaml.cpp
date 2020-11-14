#include <stdexcept>
#include <yaml.h>
#include "wrappers.h"

yaml_node_t *get_mapping_entry(
        yaml_document_t *document,
        yaml_node_t *mapping,
        const char *name
) {
    if (!mapping) {
        return nullptr;
    }

    for (const auto &pair : yaml_mapping_wrapper{document, mapping}) {
        const auto &key = pair.first;
        if (!key || key->type != YAML_SCALAR_NODE) {
            return nullptr;
        }
        if (strncmp((const char *) key->data.scalar.value, name, key->data.scalar.length) != 0) {
            continue;
        }
        return pair.second;
    }

    return nullptr;
}

void require_type(yaml_node_t *node, yaml_node_type_t type) {
    if (node->type != type) {
        throw std::runtime_error{"Unexpected node type"};
    }
}

