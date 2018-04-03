#pragma once

namespace acs {

	// �p�������N���X�̓R�s�[�ł��Ȃ�
	struct CopyDisable {
		CopyDisable() {};

		//�R�s�[�s��
		CopyDisable& operator=(CopyDisable&) = delete; 
		CopyDisable(CopyDisable&) = delete;

		//�ړ��\
		CopyDisable& operator=(CopyDisable&&) = default;
		CopyDisable(CopyDisable&&) = default;
	};

	// �p�������N���X��move�ł��Ȃ�
	struct MoveDisable {
		MoveDisable() {}

		//�ړ��s�\
		MoveDisable(MoveDisable&&) = delete;
		MoveDisable& operator=(MoveDisable&&) = delete;
	};

	//�X�g���[���ɏ������݊֐����������N���X��\��
	struct StreamWritable{
		// �ȉ��̊֐�������������
		// void streamWrite(std::ostream& os);
		// void wstreamWrite(std::wostream& os);
	};
}