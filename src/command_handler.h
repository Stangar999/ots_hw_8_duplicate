#pragma once

#include <iostream>
#include <sstream>
#include <vector>

#include "observer.h"

using namespace std::literals;

class CommandHandler : public ObserveredCmd {
  const int _block_size;
  int _current_n;
  std::vector<std::string> _commands;
  const std::string start_din;
  const std::string end_din;
  bool _din_blok_is_active;
  size_t _cout_inline_command;

  void EndDinBlock() {
    _din_blok_is_active = false;
    print_block();
  }

 public:
  CommandHandler(int block_size)
      : _block_size(block_size),
        _current_n(block_size),
        _din_blok_is_active(false),
        start_din("{"),
        end_din("}"),
        _cout_inline_command(0) {
  }

  bool add(std::string command, bool eof) {
    if (eof) {
      if (!_din_blok_is_active) {
        print_block();
      }
      return true;
    }
    if (command.empty()) {
      return false;
    }

    if (command == start_din) {
      _din_blok_is_active == true ? void() : print_block();
      _din_blok_is_active = true;
      ++_cout_inline_command;
    } else if (command == end_din) {
      if (_cout_inline_command != 0) {
        --_cout_inline_command;
        _cout_inline_command == 0 ? EndDinBlock() : void();
      }
    } else {
      _din_blok_is_active == true ? 0 : --_current_n;
      if (_commands.empty()) {
        NotifyBlockStart();
      }
      _commands.push_back(command);
    }

    bool end = _din_blok_is_active || _current_n > 0 ? false : true;

    if (end) {
      print_block();
    }
    return false;
  }

  void print_block() {
    _current_n = _block_size;
    if (_commands.empty()) {
      return;
    }
    std::string result = "bulk: "s;
    bool start = true;
    for (const auto& var : _commands) {
      if (!start) {
        result += ", ";
      }
      result += var;
      start = false;
    }
    _commands.clear();
    NotifyBlockEnd(result);
  }
};

static void ReadCommand(CommandHandler& ch, std::istream& _in) {
  for (bool end = false; end != true;) {
    std::string command;
    std::istream& in = std::getline(_in, command);
    end = ch.add(std::move(command), in.eof());
  }
}
