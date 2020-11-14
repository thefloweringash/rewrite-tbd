#include "rewrite.h"
#include "tbdv2.h"
#include "yaml.h"

namespace tbdv2 {
    void for_each_export(yaml_document_t *document,
                         std::function<void(yaml_node_t *)> &f) {
        yaml_node_t *root = yaml_document_get_root_node(document);
        yaml_node_t *exports = get_mapping_entry(document, root, "exports");

        if (!exports) {
            // Nothing to do
            return;
        }

        for (auto export_node : yaml_sequence_wrapper{document, exports}) {
            auto reexports = get_mapping_entry(document, export_node, "re-exports");
            if (!reexports) {
                continue;
            }

            require_type(reexports, YAML_SEQUENCE_NODE);

            for (auto reexport : yaml_sequence_wrapper{document, reexports}) {
                require_type(reexport, YAML_SCALAR_NODE);
                f(reexport);
            }
        }
    }
}