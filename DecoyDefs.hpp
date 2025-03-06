#pragma once

#define TITLE "Decoy Compiler"

#define VERSION "1.0"
#define BRANCH "stable"

#define COMPILE_TAG VERSION "-" BRANCH
#define COMPILE_TAG_LEN (sizeof(COMPILE_TAG) - 1)

#define CURRENT_YEAR (__DATE__ + 7)