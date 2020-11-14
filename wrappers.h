#ifndef REWRITE_TBD_WRAPPERS_H
#define REWRITE_TBD_WRAPPERS_H

#include <iostream>
#include <stdexcept>
#include <memory>
#include <yaml.h>
#include "yaml.h"

/**
 * These wrappers are intended to be the small things to let use write nicer C++
 * lets us use C++ resource management. This is far from a libyaml wrapper.
 */

struct yaml_parser_wrapper {
    yaml_parser_t yaml_parser{};

    yaml_parser_wrapper() {
        if (!yaml_parser_initialize(&yaml_parser)) {
            throw std::runtime_error{"Failed to initialize yaml parser"};
        }
    }

    ~yaml_parser_wrapper() {
        yaml_parser_delete(&yaml_parser);
    }
};

struct yaml_document_wrapper {
    yaml_document_t yaml_document{};

    explicit yaml_document_wrapper(yaml_parser_t *parser) {
        if (!yaml_parser_load(parser, &yaml_document)) {
            throw std::runtime_error{"Failed to parse yaml document"};
        }
    }

    ~yaml_document_wrapper() {
        yaml_document_delete(&yaml_document);
    }
};

struct yaml_emitter_wrapper {
    yaml_emitter_t yaml_emitter{};

    yaml_emitter_wrapper() {
        if (!yaml_emitter_initialize(&yaml_emitter)) {
            throw std::runtime_error{"Failed to initialize yaml emitter"};
        }
    }

    ~yaml_emitter_wrapper() {
        yaml_emitter_delete(&yaml_emitter);
    }
};


struct close_file {
    void operator()(FILE *f) {
        if (f) {
            fclose(f);
        }
    }
};

#endif //REWRITE_TBD_WRAPPERS_H
