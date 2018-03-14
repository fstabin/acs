#pragma once
#include <tuple>
#include <set>
namespace acs {
	namespace code {
		class CodeSet {
			std::set<std::pair<wchar_t, wchar_t> > set;
		public:
			CodeSet() {};
			
			void add(wchar_t begin, wchar_t end) {
				set.insert(std::make_pair(begin, end));
			}

			const std::set<std::pair<wchar_t, wchar_t> >& getSet() {
				return set;
			}
		};
	}
}