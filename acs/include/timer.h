#pragma once
#include "stopwatch.h"

namespace acs {
	namespace timer {

		class 	[[deprecated("Use acs::clock::Stopwatch!")]] millisec_timer {
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

	namespace clock {
		class Timer :public StopwatchBase {
		protected:
			cDefaultClock::duration mRegisteredDur = cDefaultClock::duration();
		public:
			bool registerTime(cDefaultClock::duration dur) {
				if (isRunning())return false;
				mRegisteredDur = dur;
				return true;
			}

			auto getRemainingTime()noexcept {
				auto dur = mWholeDur;
				if (mIsRunning)dur += (cDefaultClock::now() - mTp);
				return mRegisteredDur - dur;
			}

			bool isTimeOver()noexcept {
				auto dur = mWholeDur;
				if (mIsRunning)dur += (cDefaultClock::now() - mTp);
				return mRegisteredDur <= dur;
			}
		};
	}
}