#include "stdafx.h"

// �O���[�o���ϐ�
bool is_swr = false;

namespace {

bool CharIDvalidate(CHAR_ID id) {
	if(is_swr) {
		return SWR_CHAR_ID_MIN <= id && id <= SWR_CHAR_ID_MAX;
	} else {
		return SWRS_CHAR_ID_MIN <= id && id <= SWRS_CHAR_ID_MAX;
	}
}

} // anonymous

const char *ID2Char(CHAR_ID id) {
	static const char * const db[] = {
		"�얲",	"������",	"���",	"�A���X",
		"�p�`�����[",	"�d��",	"���~���A",	"�H�X�q",
		"��",		"��",	"�D�܉�",	"��",
		"����",	"�ߋ�",	"�V�q",	"���c",
		"�`���m",	"����",	"��",		"�z�K�q"};
	if(!CharIDvalidate(id)) {
		printf("Error:id2char �͈͊O��ID\n");
		BOOST_ASSERT(false);
		return db[0];
	}
	return db[id];
}
const char *ID2Char2(CHAR_ID id) {
	static const char * const db[] = {
		"reimu",	"marisa",	"sakuya",	"alice",
		"patchouli",	"youmu",	"remilia",	"yuyuko",
		"yukari",	"suika",	"udonge",	"aya",
		"komachi",	"iku",		"tenshi",	"sanae",
		"chirno",	"meirin",	"utsuho",	"suwako"};
	if(!CharIDvalidate(id)) {
		printf("Error:id2char �͈͊O��ID\n");
		BOOST_ASSERT(false);
		return db[0];
	}
	return db[id];
}
