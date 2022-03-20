/**
 * src/class_diagram/model/class_element.cc
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

#include "class_element.h"

namespace clanguml::class_diagram::model {

class_element::class_element(common::model::scope_t scope,
    const std::string &name, const std::string &type)
    : scope_{scope}
    , name_{name}
    , type_{type}
{
}

common::model::scope_t class_element::scope() const { return scope_; }

std::string class_element::name() const { return name_; }

std::string class_element::type() const { return type_; }

inja::json class_element::context() const
{
    inja::json ctx;
    ctx["name"] = name();
    ctx["type"] = type();
    ctx["scope"] = to_string(scope());
    return ctx;
}
}
