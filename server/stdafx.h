#define _USE_ASSERTS_

#include <boost/asio.hpp>
#include <boost/thread.hpp>
#include <boost/bind.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>

#include <iostream>
#include <fstream>
#include <string>
#include <deque>
#include <list>
#include <vector>

//#include "..\..\Ogre2\Ogre2\shared_enums.h"

#ifdef WIN32
#define _WIN32_WINNT 0x0501
#include <stdio.h>
#endif