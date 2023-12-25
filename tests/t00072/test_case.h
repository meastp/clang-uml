/**
 * tests/t00072/test_case.h
 *
 * Copyright (c) 2021-2023 Bartek Kryza <bkryza@gmail.com>
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

TEST_CASE("t00072", "[test-case][class]")
{
    auto [config, db] = load_config("t00072");

    auto diagram = config.diagrams["t00072_class"];

    REQUIRE(diagram->name == "t00072_class");

    auto model = generate_class_diagram(*db, diagram);

    REQUIRE(model->name() == "t00072_class");

    {
        auto src = generate_class_puml(diagram, *model);
        AliasMatcher _A(src);

        REQUIRE_THAT(src, StartsWith("@startuml"));
        REQUIRE_THAT(src, EndsWith("@enduml\n"));

        // Check if all classes exist
        REQUIRE_THAT(src, IsPackage("app"));
        REQUIRE_THAT(src, IsPackage(":lib1"));
        REQUIRE_THAT(src, IsPackage(":lib2"));
        REQUIRE_THAT(src, IsPackage("mod1"));
        REQUIRE_THAT(src, IsPackage("mod2"));

        REQUIRE_THAT(src, IsClass(_A("A")));
        REQUIRE_THAT(src, IsClass(_A("C")));
        REQUIRE_THAT(src, IsClassTemplate("CC", "T"));
        REQUIRE_THAT(src, IsEnum(_A("detail::CCC")));

        REQUIRE_THAT(src, IsClass(_A("B")));
        REQUIRE_THAT(src, IsClassTemplate("BB", "T"));
        REQUIRE_THAT(src, IsEnum(_A("detail::BBB")));

        REQUIRE_THAT(src, IsClass(_A("D")));
        REQUIRE_THAT(src, IsClass(_A("E")));

        save_puml(config.output_directory(), diagram->name + ".puml", src);
    }

    {
        auto j = generate_class_json(diagram, *model);

        using namespace json;

        REQUIRE(IsClass(j, "clanguml::t00072::A"));
        REQUIRE(IsClass(j, "clanguml::t00072::B"));
        REQUIRE(IsClass(j, "clanguml::t00072::C"));
        REQUIRE(IsClass(j, "clanguml::t00072::D"));
        REQUIRE(IsEnum(j, "clanguml::t00072::detail::CCC"));
        REQUIRE(IsEnum(j, "clanguml::t00072::detail::BBB"));

        REQUIRE(IsPackage(j, "app", "module"));
        REQUIRE(IsPackage(j, "app:lib1", "module"));
        REQUIRE(IsPackage(j, "app:lib2", "module"));
        REQUIRE(IsPackage(j, "app:lib1.mod1", "module"));
        REQUIRE(IsPackage(j, "app:lib1.mod2", "module"));

        save_json(config.output_directory(), diagram->name + ".json", j);
    }

    {
        auto src = generate_class_mermaid(diagram, *model);

        mermaid::AliasMatcher _A(src);
        using mermaid::IsClass;
        using mermaid::IsEnum;

        REQUIRE_THAT(src, IsClass(_A("A")));
        REQUIRE_THAT(src, IsClass(_A("C")));
        REQUIRE_THAT(src, IsClass(_A("CC<T>")));
        REQUIRE_THAT(src, IsEnum(_A("detail::CCC")));

        REQUIRE_THAT(src, IsClass(_A("B")));
        REQUIRE_THAT(src, IsClass(_A("BB<T>")));
        REQUIRE_THAT(src, IsEnum(_A("detail::BBB")));

        REQUIRE_THAT(src, IsClass(_A("D")));
        REQUIRE_THAT(src, IsClass(_A("E")));

        save_mermaid(config.output_directory(), diagram->name + ".mmd", src);
    }
}