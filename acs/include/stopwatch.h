#pragma once
#include <chrono>
#include <string>
#include <ctime>

namespace acs {
	namespace clock {
		class StopwatchBase {
		protected:
			using cDefaultClock = std::chrono::steady_clock;

			bool mIsRunning = false;
			cDefaultClock::time_point mTp;
			cDefaultClock::duration mWholeDur = cDefaultClock::duration(0);
		public:
			void start()noexcept {
				mIsRunning = true;
				mTp = cDefaultClock::now();
			}

			void stop()noexcept {
				if (mIsRunning) {
					cDefaultClock::time_point nowTp = cDefaultClock::now();
					mWholeDur += (nowTp - mTp);
					mTp = nowTp;
				}
				mIsRunning = false;
			}

			void reset()noexcept {
				mWholeDur = cDefaultClock::duration(0);
				mIsRunning = false;
			}

			bool isRunning()noexcept {
				return mIsRunning;
			}
		};

		class Stopwatch:public StopwatchBase {
		public:
			auto getTime()noexcept {
				auto dur = mWholeDur;
				if (mIsRunning)dur += (cDefaultClock::now() - mTp);
				return dur;
			}
		};
	}
}