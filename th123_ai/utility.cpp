#include "stdafx.h"

// グローバル変数
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
		"霊夢",	"魔理沙",	"咲夜",	"アリス",
		"パチュリー",	"妖夢",	"レミリア",	"幽々子",
		"紫",		"萃香",	"優曇華",	"文",
		"小町",	"衣玖",	"天子",	"早苗",
		"チルノ",	"美鈴",	"空",		"諏訪子"};
	if(!CharIDvalidate(id)) {
		printf("Error:id2char 範囲外のID\n");
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
		printf("Error:id2char 範囲外のID\n");
		BOOST_ASSERT(false);
		return db[0];
	}
	return db[id];
}
