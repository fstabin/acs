#pragma once

namespace acs {

	// �p�������N���X�̓R�s�[�ł��Ȃ�
	struct CopyDisable {
		CopyDisable() {};
		CopyDisable& operator=(CopyDisable&) = delete; 
		CopyDisable(CopyDisable&) = delete;
	};

	// �p�������N���X��move�ł��Ȃ�
	struct MoveDisable {
		MoveDisable();
		MoveDisable& operator=(MoveDisable&&) = delete;
	};

	//�X�g���[���ɏ������݊֐����������N���X��\��
	struct StreamWritable{
		// �ȉ��̊֐�������������
		// void streamWrite(std::ostream& os);
		// void wstreamWrite(std::wostream& os);
	};
}