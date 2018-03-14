#pragma once
#include <chrono>

namespace acs {
	namespace timer {
		class millisec_timer {
			std::chrono::steady_clock::time_point tp;
		public:
			void start()noexcept {
				tp = std::chrono::steady_clock::now();
			}
			long long stop()noexcept {
				return std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - tp).count();
			}
		};
	}
}