#ifndef COMMANDQUEUE_HPP
#define COMMANDQUEUE_HPP

#include <condition_variable>
#include <mutex>
#include <queue>
#include <string>

enum e_cmd { HELP, ARM, DISARM, SETPOINT, SETPOS, TRIM };

struct Command {
  int drone_index;
  e_cmd type;
  std::string payload;
  Command(int i, e_cmd t, std::string const &p)
      : drone_index(i), type(t), payload(p) {}
};

class CommandQueue {
public:
  void enque(Command const &cmd);
  Command deque();

private:
  std::queue<Command> _que;
  std::mutex _mutex;
  std::condition_variable _conditional;
};

void make_command(CommandQueue &que, int idx, e_cmd type,
                  std::string const &payload);
void process_command(CommandQueue &que);

#endif // COMMANDQUEUE_HPP
