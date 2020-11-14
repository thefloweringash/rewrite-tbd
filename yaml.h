#ifndef REWRITE_TBD_YAML_H
#define REWRITE_TBD_YAML_H

#include <yaml.h>

void require_type(yaml_node_t *node, yaml_node_type_t type);

struct yaml_mapping_iterator
        : public std::iterator<std::forward_iterator_tag, std::pair<yaml_node_t *, yaml_node_t *>> {
    yaml_document_t *document;
    yaml_node_pair_t *pair;

    explicit yaml_mapping_iterator(yaml_document_t *document, yaml_node_pair_t *pair)
            : document{document}, pair{pair} {}

    yaml_mapping_iterator &operator++() {
        pair++;
        return *this;
    }

    bool operator!=(yaml_mapping_iterator &other) const {
        return other.pair != this->pair;
    }

    std::pair<yaml_node_t *, yaml_node_t *> operator*() const {
        return std::make_pair(
                yaml_document_get_node(document, pair->key),
                yaml_document_get_node(document, pair->value)
        );
    }
};

struct yaml_mapping_wrapper {
    yaml_document_t *document;
    yaml_node_t *mapping;

    yaml_mapping_wrapper(yaml_document_t *document, yaml_node_t *mapping)
            : document{document}, mapping{mapping} {
        require_type(mapping, YAML_MAPPING_NODE);
    }

    [[nodiscard]] yaml_mapping_iterator begin() const {
        return yaml_mapping_iterator{document, mapping->data.mapping.pairs.start};
    }

    [[nodiscard]] yaml_mapping_iterator end() const {
        return yaml_mapping_iterator{document, mapping->data.mapping.pairs.top};
    }
};

struct yaml_sequence_iterator
        : public std::iterator<std::forward_iterator_tag, yaml_node_t *> {
    yaml_document_t *document;
    yaml_node_item_t *item;

    explicit yaml_sequence_iterator(yaml_document_t *document, yaml_node_item_t *item)
            : document{document}, item{item} {}

    yaml_sequence_iterator &operator++() {
        item++;
        return *this;
    }

    bool operator!=(yaml_sequence_iterator &other) const {
        return other.item != this->item;
    }

    yaml_node_t *operator*() const {
        return yaml_document_get_node(document, *item);
    }
};

struct yaml_sequence_wrapper {
    yaml_document_t *document;
    yaml_node_t *sequence;

    yaml_sequence_wrapper(yaml_document_t *document, yaml_node_t *sequence)
            : document{document}, sequence{sequence} {
        require_type(sequence, YAML_SEQUENCE_NODE);
    }

    [[nodiscard]] yaml_sequence_iterator begin() const {
        return yaml_sequence_iterator{document, sequence->data.sequence.items.start};
    }

    [[nodiscard]] yaml_sequence_iterator end() const {
        return yaml_sequence_iterator{document, sequence->data.sequence.items.top};
    }
};

yaml_node_t *get_mapping_entry(
        yaml_document_t *document,
        yaml_node_t *mapping,
        const char *name
);


#endif //REWRITE_TBD_YAML_H
