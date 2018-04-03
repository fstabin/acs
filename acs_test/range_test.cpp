#pragma once
#include <iostream>
#include "acs\include\range.h"

#include "test.h"
using namespace std;
using namespace acs::range;
int range_test() {
	cout << "�e�X�g1�@�J�n" << endl;
	{
		Range<int> range;
		range.setWhole();
		range.andCondition(Condition::LessEqual, 2);
		range.andCondition(Condition::Grater, -100);
		range.andCondition(Condition::GraterEqual, -80);
		range.andCondition(Condition::Grater, -50);
		range.andCondition(Condition::LessEqual, 255);
		range.orCondition(Condition::Equal, -1000);
		range.streamWrite(cout);
	}
	cout << endl;
	cout << "�e�X�g1�@�I��" << endl;

	cout << "�e�X�g�Q�@�J�n" << endl;
	{
		Range<float> range;
		range.setEmpty();
		range.orCondition(Condition::LessEqual, 100.23456);
		range.andCondition(Condition::Grater, 50.55);
		range.orCondition(Condition::Equal, -1.35);
		range.streamWrite(std::cout);
	}
	cout << endl << "�e�X�g�Q�@�I��" << endl;

	return 0;
}