#include "stdafx.h"

void ObjBase::SetProcessHandle(org::click3::Utility::SHARED_HANDLE ph) {
	this->ph = ph;
}
void ObjBase::Reload(AI_MODE mode) {
	ReloadBaseAddr(mode);
	if(base_addr == 0) {
		return;
	}
	//base+0x08	4	�摜�\���̓I�ȉ����ւ̃A�h���X�H
	ReadProcessMemory(ph, base_addr+ADDR_POINTXOFS,			x);
	ReadProcessMemory(ph, base_addr+ADDR_POINTYOFS,			y);
	ReadProcessMemory(ph, base_addr+ADDR_SPEEDXOFS,			speed.x);
	ReadProcessMemory(ph, base_addr+ADDR_SPEEDYOFS,			speed.y);
	ReadProcessMemory(ph, base_addr+ADDR_DIRECTIONOFS,		dir);//0x104�͉摜�̌����A0x1AE�͌���+����
	//base+0x11c	4	x�{��
	//base+0x120	4	y�{��
	ReadProcessMemory(ph, base_addr+ADDR_ACTIONIDOFS,		action);
	ReadProcessMemory(ph, base_addr+ADDR_ACTIONBLOCKIDOFS,		act_block);
	//base+0x140	2	���̃A�N�V�����̉��摜�ڂ�
	//base+0x142	2	���̉摜�̉��t���[���ڂ�
	ReadProcessMemory(ph, base_addr+ADDR_FRAMECOUNTOFS,		frame);
	//base+0x150	4	136byte�̃A�j���[�V����(?)�\���̂̃A�h���X(anim)
	//anim+0x0A	4	�摜ID
	unsigned int image_struct_addr;
	ReadProcessMemory(ph, base_addr+ADDR_IMAGESTRUCTOFS,		image_struct_addr);
	ReadProcessMemory(ph, image_struct_addr + ADDR_IMAGENUMBEROFS,	img_no);
	ReadProcessMemory(ph, image_struct_addr + ADDR_FRAMEFLAGSOFS,	fflags);
	ReadProcessMemory(ph, image_struct_addr + ADDR_ATTACKFLAGSOFS,	aflags);
	//base+0x158	4	158byte�̍\����(�ڍוs��)
	ReadProcessMemory(ph, base_addr+ADDR_HPOFS, hp);
	ReadProcessMemory(ph, base_addr+ADDR_HITSTOPOFS,			hit_stop);
	unsigned char hitarea_count;
	ReadProcessMemory(ph, base_addr+ADDR_HITAREACOUNTOFS,		hitarea_count);

	if(::IsSWR()) {//�H�炢����擾
		hitarea.clear();
		if(hitarea_count > 10) { // �������[�z�u��10���͂��蓾�Ȃ�
			return;
		}
		unsigned int motion_struct_addr;
		ReadProcessMemory(ph,  base_addr+ADDR_MOTIONSTRUCTOFS,	motion_struct_addr);
		unsigned int hitarea_base_addr;
		ReadProcessMemory(ph,  motion_struct_addr+ADDR_HITAREAOFS, hitarea_base_addr);
		for(unsigned int i = 0; i < hitarea_count; i++) {
			Box box;
			ReadProcessMemory(ph,  hitarea_base_addr + i*sizeof(box), &box, sizeof(box));
			box.left = static_cast<int>(x) + box.left;
			box.top = -static_cast<int>(y) + box.top;
			box.right = static_cast<int>(x) + box.right;
			box.bottom = -static_cast<int>(y)+ box.bottom;
			box.top *= -1;
			box.bottom *= -1;
			hitarea.push_back(box);
		}
	} else {
		hitarea.clear();
		if(hitarea_count > 16) { // �������[�z�u��16���͂��蓾�Ȃ�
			return;
		}
		for(unsigned int i = 0; i < hitarea_count; i++) {
			DWORD flag;
			ReadProcessMemory(ph,  base_addr + ADDR_HITAREAFLAGOFS + i * sizeof(flag), flag);
			Box box;
			ReadProcessMemory(ph,  base_addr + ADDR_HITAREA2OFS + i * sizeof(box), &box, sizeof(box));
			box.top *= -1;
			box.bottom *= -1;
			hitarea.push_back(box);
		}
	}
	unsigned char attackarea_count;
	ReadProcessMemory(ph, base_addr+ADDR_ATTACKAREACOUNTOFS,	attackarea_count);
	attackarea.clear();
	if(attackarea_count > 16) { // �������[�z�u��16���͂��蓾�Ȃ�
		return;
	}
	for(unsigned int i = 0; i < 16; i++) {
		unsigned int attackarea_addr;
		ReadProcessMemory(ph, base_addr+ADDR_ATTACKAREAOFS + i*4, attackarea_addr);
		Box box;
		if(attackarea_addr == 0) {
			//�����蔻���`���擾���ĕ\������(�n�ʍō����[����̑���)
			ReadProcessMemory(ph, base_addr+ADDR_ATTACKAREA2OFS + i * sizeof(box), &box, sizeof(box));
		} else {
			//�����蔻���`���擾���ĕ\������(�I�u�W�F�N�g�ʒu����̑���)
			ReadProcessMemory(ph, attackarea_addr, &box, sizeof(box));
			box.left = static_cast<int>(x) + box.left;
			box.top = -static_cast<int>(y) + box.top;
			box.right = static_cast<int>(x) + box.right;
			box.bottom = -static_cast<int>(y)+ box.bottom;
		}
		box.top *= -1;
		box.bottom *= -1;
		attackarea.push_back(box);
	}
	/*
	fflags =   0x003FFFFF;
	fflags &= ~0x0007E7C4;
	fflags |=  0x00000031;
	WriteProcessMemory(ph,(void *)(temp + ADDR_FRAMEFLAGSOFS),&fflags,4,NULL);*/
	ReloadVal(mode);
}
