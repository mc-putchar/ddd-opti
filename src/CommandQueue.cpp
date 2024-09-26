#include "CommandQueue.hpp"

void make_command(CommandQueue &que, int idx, e_cmd type,
                  std::string const &payload) {
  // TODO: validate command before pushing
  Command cmd(idx, type, payload);
  que.enque(cmd);
}

void process_command(CommandQueue &que) {
  Command next_cmd = que.deque();
  // TODO: process command
}

void CommandQueue::enque(Command const &cmd) {
  std::unique_lock<std::mutex> lock(this->_mutex);
  this->_que.push(cmd);
  lock.unlock();
  this->_conditional.notify_one();
}

Command CommandQueue::deque() {
  std::unique_lock<std::mutex> lock(this->_mutex);
  this->_conditional.wait(lock, [this] { return !this->_que.empty(); });
  Command cmd = this->_que.front();
  this->_que.pop();
  return (cmd);
}
