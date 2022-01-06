/*
 * Copyright (c) Lucian Radu Teodorescu
 *
 * Licensed under the Apache License Version 2.0 with LLVM Exceptions
 * (the "License"); you may not use this file except in compliance with
 * the License. You may obtain a copy of the License at
 *
 *   https://llvm.org/LICENSE.txt
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <catch2/catch.hpp>
#include <execution.hpp>
#include <test_common/schedulers.hpp>
#include <test_common/receivers.hpp>
#include <test_common/type_helpers.hpp>

namespace ex = std::execution;

TEST_CASE("done_as_error returns a sender", "[adaptors][done_as_error]") {
  auto snd = ex::done_as_error(ex::just(11), -1);
  static_assert(ex::sender<decltype(snd)>);
  (void)snd;
}
TEST_CASE("done_as_error returns a typed_sender", "[adaptors][done_as_error]") {
  auto snd = ex::done_as_error(ex::just(11), -1);
  static_assert(ex::typed_sender<decltype(snd), empty_env>);
  (void)snd;
}
TEST_CASE("done_as_error simple example", "[adaptors][done_as_error]") {
  done_scheduler sched;
  auto snd = ex::done_as_error(ex::transfer_just(sched, 11), -1);
  auto op = ex::connect(std::move(snd), expect_error_receiver{});
  ex::start(op);
}

TEST_CASE("done_as_error can we piped", "[adaptors][done_as_error]") {
  inline_scheduler sched;
  ex::sender auto snd = ex::transfer_just(sched, 11) | ex::done_as_error(std::exception_ptr{});
  auto op = ex::connect(std::move(snd), expect_value_receiver{11});
  ex::start(op);
}

TEST_CASE("TODO: done_as_error can work with `just`", "[adaptors][done_as_error]") {
  // TODO: fix this
  // ex::sender auto snd = ex::just(11) | ex::done_as_error(-1);
  // auto op = ex::connect(std::move(snd), expect_value_receiver{11});
  // ex::start(op);
}

TEST_CASE("TODO: done_as_error can we waited on", "[adaptors][done_as_error]") {
  inline_scheduler sched;
  ex::sender auto snd = ex::transfer_just(sched, 11) | ex::done_as_error(std::exception_ptr{});
  // TODO: fix this
  // wait_for_value(std::move(snd), 11);
  (void)snd;
}

TEST_CASE("TODO: done_as_error using int error type", "[adaptors][done_as_error]") {
  inline_scheduler sched;
  ex::sender auto snd = ex::transfer_just(sched, 11) | ex::done_as_error(-1);
  // TODO: fix this
  // wait_for_value(std::move(snd), 11);
  (void)snd;
}

// TODO: add more tests; at this point, it's hard to work with `done_as_error`

TEST_CASE("done_as_error keeps values_type from input sender", "[adaptors][done_as_error]") {
  inline_scheduler sched;
  check_val_types<type_array<type_array<int>>>(
      ex::transfer_just(sched, 23) | ex::done_as_error(-1));
  check_val_types<type_array<type_array<double>>>(
      ex::transfer_just(sched, 3.1415) | ex::done_as_error(-1));
}
TEST_CASE("TODO: done_as_error keeps error_types from input sender", "[adaptors][done_as_error]") {
  inline_scheduler sched1{};
  error_scheduler sched2{};
  error_scheduler<int> sched3{-1};

  check_err_types<type_array<std::exception_ptr>>( //
      ex::transfer_just(sched1, 11) | ex::done_as_error(std::exception_ptr{}));
  check_err_types<type_array<std::exception_ptr>>( //
      ex::transfer_just(sched2, 13) | ex::done_as_error(std::exception_ptr{}));

  // TODO: error types should be forwarded (transfer_just bug)
  // check_err_types<type_array<int, std::exception_ptr>>( //
  //     ex::transfer_just(sched3, 13) | ex::done_as_error(std::exception_ptr{}));
  // Invalid check:
  check_err_types<type_array<std::exception_ptr>>( //
      ex::transfer_just(sched3, 13) | ex::done_as_error(std::exception_ptr{}));
}

TEST_CASE("TODO: done_as_error can add more types to error_types", "[adaptors][done_as_error]") {
  inline_scheduler sched1{};
  error_scheduler sched2{};
  error_scheduler<int> sched3{-1};

  check_err_types<type_array<std::exception_ptr, int>>( //
      ex::transfer_just(sched1, 11) | ex::done_as_error(-1));
  check_err_types<type_array<std::exception_ptr, int>>( //
      ex::transfer_just(sched2, 13) | ex::done_as_error(-1));

  // TODO: error types should be forwarded (transfer_just bug)
  // check_err_types<type_array<int, std::exception_ptr>>( //
  //     ex::transfer_just(sched3, 13) | ex::done_as_error(-1));
  // Invalid check:
  check_err_types<type_array<std::exception_ptr, int>>( //
      ex::transfer_just(sched3, 13) | ex::done_as_error(-1));

  check_err_types<type_array<std::exception_ptr, int, std::string>>( //
      ex::transfer_just(sched1, 11)                                  //
      | ex::done_as_error(-1)                                        //
      | ex::done_as_error(std::string{"err"}));
}

TEST_CASE("done_as_error overrides send_done to false", "[adaptors][done_as_error]") {
  inline_scheduler sched1{};
  error_scheduler sched2{};
  done_scheduler sched3{};

  check_sends_done<false>( //
      ex::transfer_just(sched1, 1) | ex::done_as_error(-1));
  check_sends_done<false>( //
      ex::transfer_just(sched2, 2) | ex::done_as_error(-1));
  check_sends_done<false>( //
      ex::transfer_just(sched3, 3) | ex::done_as_error(-1));
}
