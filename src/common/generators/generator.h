/**
 * @file src/common/generators/generator.h
 *
 * Copyright (c) 2021-2025 Bartek Kryza <bkryza@gmail.com>
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

#include "common/model/diagram_element.h"
#include "common/model/jinja_context.h"
#include "common/model/source_location.h"
#include "display_adapters.h"
#include "util/error.h"
#include "util/util.h"

#include <inja/inja.hpp>

#include <optional>
#include <ostream>
#include <regex>
#include <string>

namespace clanguml::common::generators {

void make_context_source_relative(
    inja::json &context, const std::string &prefix);

/**
 * @brief Common diagram generator interface
 *
 * This class defines common interface for all diagram generators.
 */
template <typename ConfigType, typename DiagramType> class generator {
public:
    /**
     * @brief Constructor
     *
     * @param config Reference to instance of @link clanguml::config::diagram
     * @param model Reference to instance of @link clanguml::model::diagram
     */
    generator(ConfigType &config, DiagramType &model)
        : config_{config}
        , model_{model}
    {
        init_context();
        init_env();
    }

    virtual ~generator() = default;

    /**
     * @brief Generate diagram
     *
     * This is the main diagram generation entrypoint. It is responsible for
     * calling other methods in appropriate order to generate the diagram into
     * the output stream. It generates diagram elements, that are common
     * to all types of diagrams in a given generator.
     *
     * @param ostr Output stream
     */
    virtual void generate(std::ostream &ostr) const = 0;

    /**
     * @brief Get reference to diagram config
     *
     * @return Diagram config
     */
    const ConfigType &config() const { return config_; }

    /**
     * @brief Get reference to diagram model
     *
     * @return Diagram model
     */
    const DiagramType &model() const { return model_; }

    std::optional<std::pair<std::string, std::string>> get_link_pattern(
        const common::model::source_location &sl) const;

    std::optional<std::pair<std::string, std::string>> get_tooltip_pattern(
        const common::model::source_location &sl) const;

    std::optional<std::string> render_link(
        const common::model::diagram_element &e) const;

    std::optional<std::string> render_link(
        const common::model::relationship &e) const;

    std::optional<std::string> render_tooltip(
        const common::model::diagram_element &e) const;

    std::optional<std::string> render_tooltip(
        const common::model::relationship &e) const;

    /**
     * @brief Initialize diagram Jinja context
     */
    void init_context();

    /**
     * @brief Update diagram Jinja context
     *
     * This method updates the diagram context with models properties
     * which can be used to render Jinja templates in the diagram (e.g.
     * in notes or links)
     */
    void update_context() const;

    void init_env();

    const inja::json &context() const;

    inja::Environment &env() const;

protected:
    mutable inja::json m_context;
    mutable inja::Environment m_env;

private:
    ConfigType &config_;
    DiagramType &model_;
};

template <typename C, typename D> void generator<C, D>::init_context()
{
    const auto &config = generators::generator<C, D>::config();

    if (config.git) {
        m_context["git"]["branch"] = config.git().branch;
        m_context["git"]["revision"] = config.git().revision;
        m_context["git"]["commit"] = config.git().commit;
        m_context["git"]["toplevel"] = config.git().toplevel;
    }

    if (config.user_data) {
        m_context["user_data"] = config.user_data();
    }
}

template <typename C, typename D>
const inja::json &generator<C, D>::context() const
{
    return m_context;
}

template <typename C, typename D>
inja::Environment &generator<C, D>::env() const
{
    return m_env;
}

template <typename C, typename D> void generator<C, D>::init_env()
{
    const auto &model = generators::generator<C, D>::model();
    const auto &config = generators::generator<C, D>::config();

    //
    // Add basic string functions to inja environment
    //

    // Check if string is empty
    m_env.add_callback("empty", 1, [](inja::Arguments &args) {
        return args.at(0)->get<std::string>().empty();
    });

    // Remove spaces from the left of a string
    m_env.add_callback("ltrim", 1, [](inja::Arguments &args) {
        return util::ltrim(args.at(0)->get<std::string>());
    });

    // Remove trailing spaces from a string
    m_env.add_callback("rtrim", 1, [](inja::Arguments &args) {
        return util::rtrim(args.at(0)->get<std::string>());
    });

    // Remove spaces before and after a string
    m_env.add_callback("trim", 1, [](inja::Arguments &args) {
        return util::trim(args.at(0)->get<std::string>());
    });

    // Make a string shorted with a limit to
    m_env.add_callback("abbrv", 2, [](inja::Arguments &args) {
        return util::abbreviate(
            args.at(0)->get<std::string>(), args.at(1)->get<unsigned>());
    });

    m_env.add_callback("replace", 3, [](inja::Arguments &args) {
        std::string result = args[0]->get<std::string>();
        std::regex pattern(args[1]->get<std::string>());
        return std::regex_replace(result, pattern, args[2]->get<std::string>());
    });

    m_env.add_callback("split", 2, [](inja::Arguments &args) {
        return util::split(
            args[0]->get<std::string>(), args[1]->get<std::string>());
    });

    //
    // Add PlantUML specific functions
    //

    // Return the entire element JSON context based on element name
    // e.g.:
    //   {{ element("clanguml::t00050::A").comment }}
    //
    m_env.add_callback("element", 1, [&model, &config](inja::Arguments &args) {
        inja::json res{};
        auto element_opt = model.get_with_namespace(
            args[0]->get<std::string>(), config.using_namespace());

        if (element_opt.has_value()) {
            res = common::jinja::diagram_context<model::element>{
                dynamic_cast<const model::element &>(element_opt.value())};
        }

        return res;
    });

    // Convert C++ entity to PlantUML alias, e.g.
    //   "note left of {{ alias("A") }}: This is a note"
    // Shortcut to:
    //   {{ element("A").alias }}
    //
    m_env.add_callback("alias", 1, [&model, &config](inja::Arguments &args) {
        const auto &element_name = args[0]->get<std::string>();
        auto element_opt =
            model.get_with_namespace(element_name, config.using_namespace());

        if (!element_opt.has_value())
            throw clanguml::error::uml_alias_missing(
                args[0]->get<std::string>());

        return element_opt.value().alias();
    });

    // Get elements' comment:
    //   "note left of {{ alias("A") }}: {{ comment("A") }}"
    // Shortcut to:
    //   {{ element("A").comment }}
    //
    m_env.add_callback("comment", 1, [&model, &config](inja::Arguments &args) {
        inja::json res{};
        auto element_opt = model.get_with_namespace(
            args[0]->get<std::string>(), config.using_namespace());

        if (!element_opt.has_value())
            throw clanguml::error::uml_alias_missing(
                args[0]->get<std::string>());

        auto comment = element_opt.value().comment();

        if (comment.has_value()) {
            assert(comment.value().is_object());
            res = comment.value();
        }

        return res;
    });
}

template <typename C, typename D>
std::optional<std::pair<std::string, std::string>>
generator<C, D>::get_link_pattern(
    const common::model::source_location &sl) const
{
    if (sl.file_relative().empty()) {
        return config().generate_links().get_link_pattern(sl.file());
    }

    return config().generate_links().get_link_pattern(sl.file_relative());
}

template <typename C, typename D>
std::optional<std::pair<std::string, std::string>>
generator<C, D>::get_tooltip_pattern(
    const common::model::source_location &sl) const
{
    if (sl.file_relative().empty()) {
        return config().generate_links().get_tooltip_pattern(sl.file());
    }

    return config().generate_links().get_tooltip_pattern(sl.file_relative());
}

template <typename C, typename D>
std::optional<std::string> generator<C, D>::render_link(
    const common::model::diagram_element &e) const
{
    using common::generators::make_context_source_relative;
    using common::jinja::render_template;

    if (e.file().empty() && e.file_relative().empty())
        return {};

    auto maybe_link_pattern = generators::generator<C, D>::get_link_pattern(e);

    if (!maybe_link_pattern)
        return {};

    const auto &[link_prefix, link_pattern] = *maybe_link_pattern;

    inja::json ec = jinja::element_context<common::model::diagram_element>(
        e, generators::generator<C, D>::context());

    make_context_source_relative(ec, link_prefix);

    return render_template(
        generators::generator<C, D>::env(), ec, link_pattern);
}

template <typename C, typename D>
std::optional<std::string> generator<C, D>::render_link(
    const common::model::relationship &e) const
{
    using common::generators::make_context_source_relative;
    using common::jinja::render_template;

    if (e.file().empty() && e.file_relative().empty())
        return {};

    auto maybe_link_pattern = generators::generator<C, D>::get_link_pattern(e);

    if (!maybe_link_pattern)
        return {};

    const auto &[link_prefix, link_pattern] = *maybe_link_pattern;

    inja::json ec = jinja::element_context<common::model::relationship>(
        e, generators::generator<C, D>::context());

    make_context_source_relative(ec, link_prefix);

    return render_template(
        generators::generator<C, D>::env(), ec, link_pattern);
}

template <typename C, typename D>
std::optional<std::string> generator<C, D>::render_tooltip(
    const common::model::diagram_element &e) const
{
    using common::generators::make_context_source_relative;
    using common::jinja::render_template;

    if (e.file().empty() && e.file_relative().empty())
        return {};

    auto maybe_tooltip_pattern =
        generators::generator<C, D>::get_tooltip_pattern(e);

    if (!maybe_tooltip_pattern)
        return {};

    const auto &[tooltip_prefix, tooltip_pattern] = *maybe_tooltip_pattern;

    inja::json ec = jinja::element_context<common::model::diagram_element>(
        e, generators::generator<C, D>::context());

    make_context_source_relative(ec, tooltip_prefix);

    return render_template(
        generators::generator<C, D>::env(), ec, tooltip_pattern);
}

template <typename C, typename D>
std::optional<std::string> generator<C, D>::render_tooltip(
    const common::model::relationship &e) const
{
    using common::generators::make_context_source_relative;
    using common::jinja::render_template;

    if (e.file().empty() && e.file_relative().empty())
        return {};

    auto maybe_tooltip_pattern =
        generators::generator<C, D>::get_tooltip_pattern(e);

    if (!maybe_tooltip_pattern)
        return {};

    const auto &[tooltip_prefix, tooltip_pattern] = *maybe_tooltip_pattern;

    inja::json ec = jinja::element_context<common::model::relationship>(
        e, generators::generator<C, D>::context());

    make_context_source_relative(ec, tooltip_prefix);

    return render_template(
        generators::generator<C, D>::env(), ec, tooltip_pattern);
}
} // namespace clanguml::common::generators