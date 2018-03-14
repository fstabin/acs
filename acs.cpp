// acs.cpp : コンソール アプリケーションのエントリ ポイントを定義します。
//

#include "stdafx.h"
#include <iostream>
#include <map>
#include <list>
#include <memory>
#include<unordered_map>

#include "acs_geometory.h"
#include "acs_range.h"

using namespace std;
size_t fooCnt = 0;



int main()
{
	acs::range::Range range;
	range.andCondition(acs::range::Grater, 10);
	range.andCondition(acs::range::LessEqual, 20);
	range.orCondition(acs::range::Grater, 80);
	range.andCondition(acs::range::LessEqual, 99);

	acs::range::Range range2;
	range2.andCondition(acs::range::Grater, 5);
	range2.andCondition(acs::range::LessEqual, 15);
	range2.orCondition(acs::range::GraterEqual, 82);
	range2.andCondition(acs::range::Less, 90);

	auto true_range = range. and (range2);
	true_range.streamWrite(cout);
	cout << endl;
	for (size_t i = 0; i < 1000; i++)
	{
		if (true_range.isMet(i)) {
			cout << i << endl;
		}
	}

	char k;
	cin >> k;
    return 0;
}

