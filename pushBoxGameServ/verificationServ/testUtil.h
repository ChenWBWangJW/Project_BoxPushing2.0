#pragma once

#include <iostream>

#ifdef TEST
#define perrorText(msg) std::cout << msg << strerror(errno) << std::endl;
#else
#define perrorText(msg)
#endif