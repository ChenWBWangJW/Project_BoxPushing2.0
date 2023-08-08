#pragma once

#include <iostream>

#ifdef TEST
#define perrorText(msg) std::cout << msg << std::endl << flush;
#else
#define perrorText(msg)
#endif