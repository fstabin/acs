#pragma once

namespace acs {

	// 継承したクラスはコピーできない
	struct CopyDisable {
		CopyDisable() {};

		//コピー不可
		CopyDisable& operator=(CopyDisable&) = delete; 
		CopyDisable(CopyDisable&) = delete;

		//移動可能
		CopyDisable& operator=(CopyDisable&&) = default;
		CopyDisable(CopyDisable&&) = default;
	};

	// 継承したクラスはmoveできない
	struct MoveDisable {
		MoveDisable() {}

		//移動不可能
		MoveDisable(MoveDisable&&) = delete;
		MoveDisable& operator=(MoveDisable&&) = delete;
	};

	//ストリームに書き込み関数を持ったクラスを表す
	struct StreamWritable{
		// 以下の関数を実装しいる
		// void streamWrite(std::ostream& os);
		// void wstreamWrite(std::wostream& os);
	};
}