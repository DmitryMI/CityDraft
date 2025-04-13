#pragma once

#include <GL/gl.h>

namespace CityDraft::UI::Rendering
{
	class OpenGlUtils
	{
	public:
		static constexpr std::string_view GlMessageSourceToString(GLenum source)
		{
			switch (source)
			{
			case GL_DEBUG_SOURCE_API:
				return "API";
				break;

			case GL_DEBUG_SOURCE_WINDOW_SYSTEM:
				return "WINDOW SYSTEM";
				break;

			case GL_DEBUG_SOURCE_SHADER_COMPILER:
				return "SHADER COMPILER";
				break;

			case GL_DEBUG_SOURCE_THIRD_PARTY:
				return "THIRD PARTY";
				break;

			case GL_DEBUG_SOURCE_APPLICATION:
				return "APPLICATION";
				break;

			case GL_DEBUG_SOURCE_OTHER:
				return "OTHER";
				break;
			}

			return "UNKNOWN";
		}

		static constexpr std::string_view GlMessageTypeToString(GLenum type)
		{
			switch (type)
			{
			case GL_DEBUG_TYPE_ERROR:
				return "ERROR";
				break;

			case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR:
				return "DEPRECATED BEHAVIOR";
				break;

			case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:
				return "UDEFINED BEHAVIOR";
				break;

			case GL_DEBUG_TYPE_PORTABILITY:
				return "PORTABILITY";
				break;

			case GL_DEBUG_TYPE_PERFORMANCE:
				return "PERFORMANCE";
				break;

			case GL_DEBUG_TYPE_OTHER:
				return "OTHER";
				break;

			case GL_DEBUG_TYPE_MARKER:
				return "MARKER";
				break;

			default:
				return "UNKNOWN";
				break;
			}

			return "UNKNOWN";
		}

		static constexpr spdlog::level::level_enum GlMessageSeverityToLogLevel(GLenum severity)
		{
			switch (severity)
			{
			case GL_DEBUG_SEVERITY_HIGH:
				return spdlog::level::err;
				break;

			case GL_DEBUG_SEVERITY_MEDIUM:
				return spdlog::level::warn;
				break;

			case GL_DEBUG_SEVERITY_LOW:
				return spdlog::level::info;
				break;

			case GL_DEBUG_SEVERITY_NOTIFICATION:
				return spdlog::level::debug;
				break;
			}

			return spdlog::level::debug;
		}
	};
}
