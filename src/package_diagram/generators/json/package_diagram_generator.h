/**
 * @file src/package_diagram/generators/json/package_diagram_generator.h
 *
 * Copyright (c) 2021-2024 Bartek Kryza <bkryza@gmail.com>
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#pragma once

#include "common/generators/json/generator.h"
#include "common/generators/nested_element_stack.h"
#include "common/model/package.h"
#include "common/model/relationship.h"
#include "config/config.h"
#include "package_diagram/model/diagram.h"
#include "package_diagram/visitor/translation_unit_visitor.h"
#include "util/util.h"

#include <filesystem>
#include <fstream>
#include <iostream>
#include <sstream>

namespace clanguml::package_diagram::generators::json {

using diagram_config = clanguml::config::package_diagram;
using diagram_model = clanguml::package_diagram::model::diagram;

template <typename C, typename D>
using common_generator = clanguml::common::generators::json::generator<C, D>;

using clanguml::common::model::access_t;
using clanguml::common::model::package;
using clanguml::common::model::relationship_t;
using namespace clanguml::util;

/**
 * @brief Package diagram JSON generator
 */
class generator : public common_generator<diagram_config, diagram_model> {
public:
    generator(diagram_config &config, diagram_model &model);

    using common_generator<diagram_config, diagram_model>::generate;

    /**
     * @brief Main generator method.
     *
     * This method is called first and coordinates the entire diagram
     * generation.
     *
     * @param ostr Output stream.
     */
    void generate_diagram(nlohmann::json &parent) const override;

    /**
     * @brief Generate relationships originating from package `p`
     *
     * @param p Diagram element
     * @param parent JSON node
     */
    void generate_relationships(const package &p, nlohmann::json &parent) const;

    /**
     * @brief Generate diagram package
     *
     * @param p Diagram package element
     * @param parent Parent JSON node
     */
    void generate(const package &p, nlohmann::json &parent) const;
};

} // namespace clanguml::package_diagram::generators::json
