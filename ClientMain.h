#include <chrono>
#include <iostream>
#include <mutex>
#include <thread>
#include <vector>
#include <arpa/inet.h>
#include <poll.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <iomanip>

#define FOLLOWER 0
#define CANDIDATE 1
#define LEADER 2
