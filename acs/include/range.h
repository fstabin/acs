#pragma once

#include <set>
#include <algorithm>

#include "attribute.h"
#include "limits.h"

namespace acs {
	namespace range {
		enum Condition :char{
			Less = 0,
			LessEqual = 1,
			Equal = 2,
			GraterEqual = 3,
			Grater = 4,
		};
		using T = size_t;
		class Range :public acs::StreamWritable {
			using condition_type = std::pair<T, Condition>;
			//condition_type��Condition��
			//Less, Equal, Grater�̂�
			//LessEqual��GraterEqual�͏�O�ŕ\������
			std::set<condition_type > mTree;
			bool mIsWhole = true;


		public:
			//�͈͂���ɂ���
			void setEmpty() {
				mIsWhole = false;
				mTree.clear();
			}

			//�͈͂�S�̂ɂ���
			void setWhole() {
				mIsWhole = true;
				mTree.clear();
			}

			//and/or coudition 
			//isMet
			//����O(logN)
 
			void andCondition(Condition sign, T val) {
				if (mIsWhole) {
					mIsWhole = false;
					switch (sign)
					{
					case acs::range::Less:
						mTree.insert({ val, Less });
						break;
					case acs::range::LessEqual:
						mTree.insert({ val, acs::range::Less });//less��ǉ�
						mTree.insert({ val, acs::range::Equal });//equal��ǉ�
						break;
					case acs::range::Grater:
						mTree.insert({ val, Grater });//grater��ǉ�
						break;
					case acs::range::GraterEqual:
						mTree.insert({ val, acs::range::Grater });//less��ǉ�
						mTree.insert({ val, acs::range::Equal });//equal��ǉ�]
						break;
					case acs::range::Equal:
						mTree.insert({ val, acs::range::Equal });//equal��ǉ�]
						break;
					default:
						throw(std::invalid_argument("Condition is invalid"));
					}
				}
				else {
					switch (sign)
					{
					case acs::range::Less:
						//�����P
						//�������������������łɂ���Ƃ��@�܂���
						//��O��grater�̂Ƃ� �܂��́@����less�̂Ƃ�
						//����less�̒ǉ��ɐ���
						//����ȊO�̂Ƃ��ǉ����Ȃ�
						//�����Q
						//���傫�����̂͂��ׂď���
					{
						auto result = mTree.insert({ val, sign });//less��ǉ�
						auto treeiter = result.first;
						bool wasSucceeded = false;
						auto beforePair = treeiter;
						auto nextPair = treeiter;
						++nextPair;
						bool hasBefore = (treeiter != mTree.begin());
						if (hasBefore)beforePair--;
						bool hasNext = (nextPair != mTree.end());

						if (result.second) {
							wasSucceeded = true;
						}
						if (hasBefore ? (beforePair->second == acs::range::Grater) : false
							|| hasNext ? (nextPair->second == acs::range::Less) : false) {
							wasSucceeded = true;
						}
						auto ietase = (wasSucceeded) ? nextPair : treeiter;//���s���͒ǉ��������̂��폜
						mTree.erase(ietase, mTree.end());//�s�v�ȏ������폜
					}
					break;
					case acs::range::LessEqual:
					{
						auto resultL = mTree.insert({ val, acs::range::Less });//less��ǉ�
						auto resultE = mTree.insert({ val, acs::range::Equal });//equal��ǉ�
						auto treeiterL = resultE.first;
						--treeiterL;
						auto treeiterE = resultE.first;
						auto beforePair = treeiterL;
						auto nextPair = treeiterE;
						++nextPair;
						bool hasBefore = (beforePair != mTree.begin());
						if (hasBefore)beforePair--;
						bool hasNext = (nextPair != mTree.end());

						//��v���铙�������łɂ���Ƃ�
						if (resultE.second == false) {
							//��v����s�������Ȃ��Ƃ�
							//�s�����폜
							if (resultL.second) {
								mTree.erase(treeiterL, treeiterE);
							}
							//�ȍ~�͈͍̔폜
							mTree.erase(nextPair, mTree.end());
						}

						//��v���铙�������łɂȂ��Ƃ�
						else {
							//��v����s�����݂̂��łɂ�������
							if (resultL.second == false) {
								//�����Ƃ���ȍ~�͈͍̔폜
								mTree.erase(treeiterE, mTree.end());
							}
							else if (hasBefore ? (beforePair->second == acs::range::Grater) : false
								|| hasNext ? (nextPair->second == acs::range::Less) : false) {
								//value���͈͓�
								//�ȍ~�͈͍̔폜
								mTree.erase(nextPair, mTree.end());
							}
							else {
								//value���͈͊O
								//�s�v���Ƃ���ȍ~�͈͍̔폜
								mTree.erase(treeiterL, mTree.end());
							}
						}
					}
					break;
					case acs::range::Grater:
						//�����P
						//�������������������łɂ���Ƃ��@�܂���
						//��O��grater�̂Ƃ� �܂��́@����less�̂Ƃ�
						//����grater�̒ǉ��ɐ���
						//����ȊO�̂Ƃ��ǉ����Ȃ�
						//�����Q
						//��菬�������̂͂��ׂď���
					{
						auto result = mTree.insert({ val, sign });//grater��ǉ�
						auto treeiter = result.first;
						bool wasSucceeded = false;
						auto beforePair = treeiter;
						auto nextPair = treeiter;
						++nextPair;
						bool hasBefore = (treeiter != mTree.begin());
						if (hasBefore)beforePair--;
						bool hasNext = (nextPair != mTree.end());

						if (result.second) {
							wasSucceeded = true;
						}
						if (hasBefore ? (beforePair->second == acs::range::Grater) : false
							|| hasNext ? (nextPair->second == acs::range::Less) : false) {
							wasSucceeded = true;
						}
						auto ietase = (wasSucceeded) ? treeiter : nextPair;//���s���͒ǉ��������̂��폜
						mTree.erase(mTree.begin(), ietase);//�s�v�ȏ������폜
					}
					break;
					case acs::range::GraterEqual:
					{
						auto resultG = mTree.insert({ val, acs::range::Grater });//less��ǉ�
						auto resultE = mTree.insert({ val, acs::range::Equal });//equal��ǉ�
						auto treeiterG = resultE.first;
						++treeiterG;
						auto treeiterE = resultE.first;
						auto beforePair = treeiterE;
						auto nextPair = treeiterG;
						++nextPair;
						bool hasBefore = (beforePair != mTree.begin());
						if (hasBefore)beforePair--;
						bool hasNext = (nextPair != mTree.end());

						//��v���铙�������łɂ���Ƃ�
						if (resultE.second == false) {
							//��v����s�������Ȃ��Ƃ�
							//�s�����폜
							if (resultG.second) {
								mTree.erase(treeiterG, nextPair);
							}
							//�ȑO�͈͍̔폜
							mTree.erase(mTree.begin(), treeiterE);
						}

						//��v���铙�������łɂȂ��Ƃ�
						else {
							//��v����s�����݂̂��łɂ�������
							if (resultG.second == false) {
								//�����Ƃ���ȑO�͈͍̔폜
								mTree.erase(mTree.begin(), treeiterG);
							}
							else if (hasBefore ? (beforePair->second == acs::range::Grater) : false
								|| hasNext ? (nextPair->second == acs::range::Less) : false) {
								//value���͈͓�
								//�ȑO�͈͍̔폜
								mTree.erase(mTree.begin(), treeiterE);
							}
							else {
								//value���͈͊O
								//�ǉ������ׂĂƂ���ȍ~�͈͍̔폜
								mTree.erase(mTree.begin(), nextPair);
							}
						}
					}
					break;
					case acs::range::Equal:
					{
						if (isMet(val)) {
							mTree.clear();
							mTree.insert({ val, acs::range::Equal });//equal��ǉ�]
						}
					}
						break;
					default:
						throw(std::invalid_argument("Condition is invalid"));
						break;
					}
				}
			}

			void orCondition(Condition sign, T val) {
				if (mIsWhole)return;
				switch (sign)
				{
				case acs::range::Less:
					//�����P
					//�������������������łɂ���Ƃ��@�܂���
					//��O��grater�̂Ƃ� �܂��́@����less�̂Ƃ�
					//����less�̒ǉ��ɐ���
					//����ȊO�̂Ƃ��ǉ����Ȃ�
					//�����Q
					//���傫�����̂͂��ׂď���
				{
					auto result = mTree.insert({ val, sign });//less��ǉ�
					auto treeiter = result.first;
					bool wasSucceeded = false;
					auto beforePair = treeiter;
					auto nextPair = treeiter;
					++nextPair;
					bool hasBefore = (treeiter != mTree.begin());
					if (hasBefore)beforePair--;
					bool hasNext = (nextPair != mTree.end());

					//�ǉ������L�������܂��
					if (hasBefore ? (beforePair->second == acs::range::Grater) : false
						|| hasNext ? (nextPair->second == acs::range::Less) : false) {
						mTree.erase(mTree.begin(), nextPair);
					}
					else {
						// �ǉ������L���̎���>=����������
						if ([&]()->bool {
							if (hasNext) {
								if (nextPair->second == acs::range::Equal && nextPair->first == val) {
									auto after2 = nextPair;
									++after2;
									if (after2 != mTree.end()) {
										if (nextPair->second == acs::range::Grater && nextPair->first == val) {
											mTree.erase(mTree.begin(), ++after2);
											return true;
										}
									}
								}
							}
							return false;
						}()) {
						}
						else {
							mTree.erase(mTree.begin(), treeiter);
						}
					}
				}
				break;
				case acs::range::LessEqual:
				{
					auto resultL = mTree.insert({ val, acs::range::Less });//less��ǉ�
					auto resultE = mTree.insert({ val, acs::range::Equal });//equal��ǉ�
					auto treeiterL = resultE.first;
					--treeiterL;
					auto treeiterE = resultE.first;
					auto beforePair = treeiterL;
					auto nextPair = treeiterE;
					++nextPair;
					bool hasBefore = (beforePair != mTree.begin());
					if (hasBefore)beforePair--;
					bool hasNext = (nextPair != mTree.end());

					//�ǉ������L�������܂��
					if (hasBefore ? (beforePair->second == acs::range::Grater) : false
						|| hasNext ? (nextPair->second == acs::range::Less) : false) {
						mTree.erase(mTree.begin(), nextPair);
					}
					else {
						// �ǉ������L���̎���>����������
						if ([&]()->bool {
							if (hasNext) {
								if (nextPair->second == acs::range::Grater && nextPair->first == val) {
									return true;
								}
							}
							return false;
						}()) {
							mTree.erase(mTree.begin(), ++nextPair);
						}
						else {
							mTree.erase(mTree.begin(), treeiterL);
						}
					}
				}
				break;
				case acs::range::Grater:
				{
					auto result = mTree.insert({ val, sign });//grater��ǉ�
					auto treeiter = result.first;
					bool wasSucceeded = false;
					auto beforePair = treeiter;
					auto nextPair = treeiter;
					++nextPair;
					bool hasBefore = (treeiter != mTree.begin());
					if (hasBefore)beforePair--;
					bool hasNext = (nextPair != mTree.end());

					//�ǉ������L�������܂��
					if (hasBefore ? (beforePair->second == acs::range::Grater) : false
						|| hasNext ? (nextPair->second == acs::range::Less) : false) {
						mTree.erase(treeiter, mTree.end());
					}
					else {
						// �ǉ������L���̑O��<=����������
						if ([&]()->bool {
							if (hasBefore) {
								if (beforePair->second == acs::range::Equal && beforePair->first == val) {
									auto before2 = beforePair;
									if (before2 != mTree.begin()) {
										--before2;
										if (nextPair->second == acs::range::Less && nextPair->first == val) {
											mTree.erase(before2, mTree.end());
											return true;
										}
									}
								}
							}
							return false;
						}()) {
						}
						else {
							mTree.erase(nextPair, mTree.end());
						}
					}
				}
				break;
				case acs::range::GraterEqual:
				{
					auto resultG = mTree.insert({ val, acs::range::Grater });//less��ǉ�
					auto resultE = mTree.insert({ val, acs::range::Equal });//equal��ǉ�
					auto treeiterG = resultE.first;
					++treeiterG;
					auto treeiterE = resultE.first;
					auto beforePair = treeiterE;
					auto nextPair = treeiterG;
					++nextPair;
					bool hasBefore = (beforePair != mTree.begin());
					if (hasBefore)beforePair--;
					bool hasNext = (nextPair != mTree.end());

					//�ǉ������L�������܂��
					if (hasBefore ? (beforePair->second == acs::range::Grater) : false
						|| hasNext ? (nextPair->second == acs::range::Less) : false) {
						mTree.erase(treeiterE, mTree.end());
					}
					else {
						// �ǉ������L���̎���<����������
						if ([&]()->bool {
							if (hasNext) {
								if (nextPair->second == acs::range::Less && nextPair->first == val) {
									return true;
								}
							}
							return false;
						}()) {
							mTree.erase(beforePair, mTree.end());
						}
						else {
							mTree.erase(nextPair, mTree.end());
						}
					}
				}
				break;
				case acs::range::Equal:
				{
					auto result = mTree.insert({ val, Equal });//grater��ǉ�
					auto treeiter = result.first;
					bool wasSucceeded = false;
					auto beforePair = treeiter;
					auto nextPair = treeiter;
					++nextPair;
					bool hasBefore = (treeiter != mTree.begin());
					if (hasBefore)beforePair--;
					bool hasNext = (nextPair != mTree.end());

					//�ǉ������L�������܂��
					if (hasBefore ? (beforePair->second == acs::range::Grater) : false
						|| hasNext ? (nextPair->second == acs::range::Less) : false) {
						mTree.erase(treeiter);
					}
					else {
						if(hasBefore ? (beforePair->second == acs::range::Less && beforePair->first == val) : false
							&& hasNext ? (nextPair->second == acs::range::Grater && nextPair->first == val) : false){
							mTree.erase(beforePair, ++nextPair);
						}
						else {
							mTree.erase(nextPair, mTree.end());
						}
					}
				}
				break;
				default:
					throw(std::invalid_argument("Condition is invalid"));
					break;
				}

				if (mTree.size() == 0)mIsWhole = true;
			}

			bool isMet(T val)const {
				if (mIsWhole)return true;
				auto iter = mTree.lower_bound({ val, Equal });
				//��v�����������
				if (iter != mTree.end())if (*iter == std::pair<T, Condition>(val, Equal))return true;
				//���̗v�f��less
				if (iter != mTree.end()) {
					if (iter->second == Less) return true;
				}
				//�O�̗v�f��grater
				if (iter != mTree.begin()) {
					--iter;
					if (iter->second == Grater)return true;
				}
				return false;
			}

			bool isWhole()const {
				return mIsWhole;
			}

			Range and (const Range& aOther) {
				if (this->mIsWhole && aOther.mIsWhole) {
					Range r;
					r.mIsWhole = true;
					return std::move(r);
				}
				else if (this->mIsWhole ^ aOther.mIsWhole) {
					Range r;
					r.mIsWhole = false;
					r.mTree = (mIsWhole) ? aOther.mTree : this->mTree;
					return std::move(r);
				}
				else {
					//�s�����ň͂܂ꂽ�����̐[�����ő�̂Ƃ��낪
					std::set<condition_type > conditions = mTree;
					conditions.insert(aOther.mTree.begin(), aOther.mTree.end());
					int dmin = 0;
					int d = 0;
					for (auto& c : conditions)
					{
						switch (c.second)
						{
						case acs::range::Less:
							dmin = std::min(dmin, --d);
							break;
						case acs::range::Grater:
							++d;
							break;
						case acs::range::Equal:
							break;
						default:
							break;
						}
					}
					d = -dmin;
					auto iter = conditions.begin();
					while (iter != conditions.end())
					{
						auto it = iter++;
						auto c = *it;
						int td;
						switch (c.second)
						{
						case acs::range::Less:
							td = d;
							--d;
							break;
						case acs::range::Grater:
							td = ++d;
							break;
						case acs::range::Equal:
							td = d + 1;
							break;
						default:
							td = -100;
						}
						if (td != 2) {
							conditions.erase(it);
						}
					}
					Range r;
					r.mIsWhole = false;
					r.mTree = std::move(conditions);
					return std::move(r);
				}
			}

			void streamWrite(std::ostream& os) {
				if (this->mIsWhole) {
					os << "whole";
				}
				else if (mTree.empty()) {
					os << "empty";
				}
				else {
					auto iter = mTree.begin();
					while (iter != mTree.end()) {
						auto c = *iter;
						++iter;
						switch (c.second)
						{
						case acs::range::Less:
							os << "<" << c.first;
							if (iter != mTree.end())os << ",";
							break;
						case acs::range::Grater:
							os << c.first << "<";
							break;
						case acs::range::Equal:
							os << c.first;
							if (iter != mTree.end())os << ",";
							break;
						default:
							break;
						}
					}
				}
			}

			void wstreamWrite(std::wostream& os) {
				if (this->mIsWhole) {
					os << "whole";
				}
				else if (mTree.empty()) {
					os << "empty";
				}
				else {
					auto iter = mTree.begin();
					while(iter != mTree.end()) {
						auto c = *iter;
						++iter;
						switch (c.second)
						{
						case acs::range::Less:
							os << "<" << c.first;
							if(iter != mTree.end())os << ",";
							break;
						case acs::range::Grater:
							os << c.first << "<";
							break;
						case acs::range::Equal:
							os << c.first;
							if (iter != mTree.end())os << ",";
							break;
						default:
							break;
						}
					}
				}
			}
		};
	}
}