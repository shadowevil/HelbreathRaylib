#pragma once
// Include Windows API wrapper (only on Windows)
#include "windows_wrapper.h"

// Include raylib with guards to prevent multiple includes
#ifndef _RAYLIB_CPP_INCLUDE_
#define _RAYLIB_CPP_INCLUDE_
	// Include math.h first in global namespace before any raylib headers
	#include <math.h>

	namespace raylib {
		extern "C" {
		#ifdef __EMSCRIPTEN__
			#include "web_include/raylib.h"
			#include "web_include/rlgl.h"
		#else
			#include "raylib.h"
			#include "rlgl.h"
		#endif
		}
		// Include raymath inside namespace to match raylib types
		#ifdef __EMSCRIPTEN__
			#include "web_include/raymath.h"
		#else
			#include "raymath.h"
		#endif
	}
#endif

// Include abstracted utility headers
// All implementation logic has been moved to separate, focused header files
#include "path_utils.h"      // Path manipulation (get_executable_path, path_combine, etc.)
#include "color_types.h"      // RGB/RGBA color structures
#include "geometry_types.h"   // Rectangle, Box, Padding, Margin structures
#include "file_utils.h"       // File and Directory namespace utilities
#include "render_utils.h"     // Upscaling, cursor lock, mouse position utilities
#include "string_utils.h"     // String manipulation utilities (to_lower, etc.)