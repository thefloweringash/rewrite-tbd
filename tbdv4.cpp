#include "tbdv4.h"
#include "yaml.h"

namespace tbdv4 {
    void for_each_export(yaml_document_t *document,
                         std::function<void(yaml_node_t *)> &f) {
        yaml_node_t *root = yaml_document_get_root_node(document);
        yaml_node_t *reexports = get_mapping_entry(document, root, "reexported-libraries");

        if (!reexports) {
            return;
        }

        require_type(reexports, YAML_SEQUENCE_NODE);

        for (auto entry_node : yaml_sequence_wrapper{document, reexports}) {
            yaml_node_t *libs = get_mapping_entry(document, entry_node, "libraries");

            if (!libs) {
                continue;
            }

            require_type(libs, YAML_SEQUENCE_NODE);

            for (auto library_name : yaml_sequence_wrapper{document, libs}) {
                require_type(library_name, YAML_SCALAR_NODE);
                f(library_name);
            }
        }
    }
}