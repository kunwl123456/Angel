#include "stdafx.h"
#include "base_lib/vm_space.h"
#include "base_lib/hash.h"
#include "base_lib/rb_tree.h"

#ifdef __linux__
#include <link.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/shm.h>
#endif

