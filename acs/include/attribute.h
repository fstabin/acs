#pragma once

namespace acs {

	// 継承したクラスはコピーできない
	struct CopyDisable {
		CopyDisable() {};
		CopyDisable& operator=(CopyDisable&) = delete; 
		CopyDisable(CopyDisable&) = delete;
	};

	// 継承したクラスはmoveできない
	struct MoveDisable {
		MoveDisable() {}
		MoveDisable& operator=(MoveDisable&&) = delete;
	};

	//ストリームに書き込み関数を持ったクラスを表す
	struct StreamWritable{
		// 以下の関数を実装しいる
		// void streamWrite(std::ostream& os);
		// void wstreamWrite(std::wostream& os);
	};
}