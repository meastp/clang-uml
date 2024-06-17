/**
 * tests/t20050/test_case.h
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

TEST_CASE("t20050")
{
    using namespace clanguml::test;

    auto [config, db, diagram, model] =
        CHECK_SEQUENCE_MODEL("t20050", "t20050_sequence");

    CHECK_SEQUENCE_DIAGRAM(*config, diagram, *model, [](const auto &src) {
        REQUIRE(IsFileParticipant(src, "t20050.cu"));

        REQUIRE(MessageOrder(src,
            {
                //
                {Entrypoint{}, "t20050.cu", "tmain()"}, //
                {"t20050.cu", "t20050.cu",
                    "vector_square_add(float *,float *,float *,int)",
                    CUDAKernel{}},                                         //
                {"t20050.cu", "t20050.cu", "square(float)", CUDADevice{}}, //
                {"t20050.cu", "t20050.cu", "square(float)", CUDADevice{}}, //
                {"t20050.cu", "t20050.cu", "add<float>(float,float)",
                    CUDADevice{}}, //
            }));
    });
}