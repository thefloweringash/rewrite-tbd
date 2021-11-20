#ifndef REWRITE_TBD_TBDV2_H
#define REWRITE_TBD_TBDV2_H

#include <functional>

#include <yaml.h>
#include "rewrite.h"

namespace tbdv2 {
    void for_each_export(yaml_document_t *document,
                         std::function<void(yaml_node_t *)> &f);
}

#endif //REWRITE_TBD_TBDV2_H