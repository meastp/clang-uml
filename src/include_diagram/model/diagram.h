/**
 * src/include_diagram/model/diagram.h
 *
 * Copyright (c) 2021-2022 Bartek Kryza <bkryza@gmail.com>
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

#include "common/model/diagram.h"
#include "common/model/package.h"
#include "source_file.h"

#include <type_safe/optional_ref.hpp>

#include <string>
#include <vector>

namespace clanguml::include_diagram::model {

class diagram : public clanguml::common::model::diagram,
                public clanguml::common::model::nested_trait<source_file,
                    filesystem_path> {
public:
    diagram() = default;

    diagram(const diagram &) = delete;
    diagram(diagram &&) = default;
    diagram &operator=(const diagram &) = delete;
    diagram &operator=(diagram &&) = default;

    common::model::diagram_t type() const override;

    type_safe::optional_ref<const common::model::diagram_element> get(
        const std::string &full_name) const;

    void add_file(std::unique_ptr<include_diagram::model::source_file> &&f);

    type_safe::optional_ref<const include_diagram::model::source_file> get_file(
        const std::string &name) const;

    std::string to_alias(const std::string &full_name) const;

private:
    std::vector<
        type_safe::object_ref<const include_diagram::model::source_file, false>>
        files_;
};
}