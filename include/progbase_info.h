/**	@file
	@brief library information
*/

#pragma once

#define PROGBASE_MAJOR_VERSION 0
#define PROGBASE_MINOR_VERSION 5
#define PROGBASE_PATCH_VERSION 0

#define STR_HELPER(x) #x
#define STR(x) STR_HELPER(x)
#define MAKE_VERSION(maj, min, patch) STR(maj) "." STR(min) "." STR(patch)
#define PROGBASE_VERSION \
    MAKE_VERSION(PROGBASE_MAJOR_VERSION, PROGBASE_MINOR_VERSION, PROGBASE_PATCH_VERSION)
#define PROGBASE_VERSION_CHECK(maj, min, patch) \
    ((maj == PROGBASE_MAJOR_VERSION) \
    && (min <= PROGBASE_MINOR_VERSION) \
    && ((min == PROGBASE_MINOR_VERSION) ? (patch <= PROGBASE_PATCH_VERSION) : (1)))

static const char * library_version __attribute__((unused)) = "Version: " PROGBASE_VERSION;  
