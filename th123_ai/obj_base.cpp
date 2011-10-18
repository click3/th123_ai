#include "stdafx.h"

void obj_base::SetProcessHandle(org::click3::Utility::SHARED_HANDLE ph) {
	this->ph = ph;
}
void obj_base::Reload(int param) {
	ReloadBaseAddr(param);
	if(base_addr == 0) {
		return;
	}
	int temp;
	//base+0x08	4	�摜�\���̓I�ȉ����ւ̃A�h���X�H
	ReadProcessMemory(ph, base_addr+ADDR_POINTXOFS, x);
	ReadProcessMemory(ph, base_addr+ADDR_POINTYOFS, y);
	ReadProcessMemory(ph, base_addr+ADDR_SPEEDXOFS, speed.x);
	ReadProcessMemory(ph, base_addr+ADDR_SPEEDYOFS, speed.y);
	ReadProcessMemory(ph, base_addr+ADDR_DIRECTIONOFS, dir);//0x104�͉摜�̌����A0x1AE�͌���+����
	//base+0x11c	4	x�{��
	//base+0x120	4	y�{��
	ReadProcessMemory(ph, base_addr+ADDR_ACTIONIDOFS, action);
	ReadProcessMemory(ph, base_addr+ADDR_ACTIONBLOCKIDOFS, act_block);
	//base+0x140	2	���̃A�N�V�����̉��摜�ڂ�
	//base+0x142	2	���̉摜�̉��t���[���ڂ�
	ReadProcessMemory(ph, base_addr+ADDR_FRAMECOUNTOFS, frame);
	//base+0x150	4	136byte�̃A�j���[�V����(?)�\���̂̃A�h���X(anim)
	//anim+0x0A	4	�摜ID
	ReadProcessMemory(ph, base_addr+ADDR_IMAGESTRUCTOFS, temp);
	ReadProcessMemory(ph, temp + ADDR_IMAGENUMBEROFS, img_no);
	ReadProcessMemory(ph, temp + ADDR_FRAMEFLAGSOFS, fflags);
	ReadProcessMemory(ph, temp + ADDR_ATTACKFLAGSOFS, aflags);
	//base+0x158	4	158byte�̍\����(�ڍוs��)
	ReadProcessMemory(ph, base_addr+ADDR_HPOFS, hp);
	ReadProcessMemory(ph, base_addr+ADDR_HITSTOPOFS, hit_stop);
	ReadProcessMemory(ph, base_addr+ADDR_ATTACKAREACOUNTOFS, attackarea_n);
	ReadProcessMemory(ph, base_addr+ADDR_HITAREACOUNTOFS, hitarea_n);

	if(::IsSWR()) {//�H�炢����擾
		int p;
		ReadProcessMemory(ph,  base_addr+ADDR_MOTIONSTRUCTOFS, p);
		ReadProcessMemory(ph,  p+ADDR_HITAREAOFS, p);
		int i = 0;
		Box *a = hitarea;
		while(i < 10) {
			if(i < hitarea_n) {
				ReadProcessMemory(ph,  p + i*16, a, sizeof(*a), NULL);
				a->left = (int)x+a->left;
				a->top = -(int)y+a->top;
				a->right = (int)x+a->right;
				a->bottom = -(int)y+a->bottom;
				a->top *= -1;
				a->bottom *= -1;
			} else {
				memset(a, 0, sizeof(*a));
			}
			a++;
			i++;
		}
	} else {
		int i = 0;
		Box *a = hitarea;
		while(i < 16) {
			if(i < hitarea_n) {
				DWORD flag;
				ReadProcessMemory(ph,  base_addr + ADDR_HITAREAFLAGOFS + i * sizeof(flag), flag);
				ReadProcessMemory(ph,  base_addr + ADDR_HITAREA2OFS + i * sizeof(*a), a, sizeof(*a));
				if(flag!=0) {//���΍��W
					a->left = (int)x+a->left;
					a->top = -(int)y+a->top;
					a->right = (int)x+a->right;
					a->bottom = -(int)y+a->bottom;
				}
				a->top *= -1;
				a->bottom *= -1;
			} else {
				memset(a, 0, sizeof(*a));
			}
			a++;
			i++;
		}
	}
	{	//�U������擾
		int p;
		int i = 0;
		Box *a = attackarea;
		while(i < 16) {
			if(i < attackarea_n) {
				ReadProcessMemory(ph, base_addr+ADDR_ATTACKAREAOFS + i*4, p);
				if(p == 0) {
					//�����蔻���`���擾���ĕ\������(�n�ʍō����[����̑���)
					ReadProcessMemory(ph, base_addr+ADDR_ATTACKAREA2OFS + i * sizeof(*a), a, sizeof(*a));
				} else {
					//�����蔻���`���擾���ĕ\������(�I�u�W�F�N�g�ʒu����̑���)
					ReadProcessMemory(ph, p, a, sizeof(*a), NULL);
					a->left = (int)x+a->left;
					a->top = -(int)y+a->top;
					a->right = (int)x+a->right;
					a->bottom = -(int)y+a->bottom;
				}
				a->top *= -1;
				a->bottom *= -1;
			} else {
				memset(a,0,sizeof(*a));
			}
			a++;
			i++;
		}
	}/*
	fflags =   0x003FFFFF;
	fflags &= ~0x0007E7C4;
	fflags |=  0x00000031;
	WriteProcessMemory(ph,(void *)(temp + ADDR_FRAMEFLAGSOFS),&fflags,4,NULL);*/
	ReloadVal(param);
}
