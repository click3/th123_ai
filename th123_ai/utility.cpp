#include "stdafx.h"

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
		"ì²",	"¹",	"çé",	"AX",
		"p`[",	"d²",	"~A",	"HXq",
		"",		"äÂ",	"DÜØ",	"¶",
		"¬¬",	"ßè",	"Vq",	"c",
		"`m",	"üé",	"ó",		"zKq"};
	if(!CharIDvalidate(id)) {
		printf("Error:id2char ÍÍOÌID\n");
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
		printf("Error:id2char ÍÍOÌID\n");
		BOOST_ASSERT(false);
		return db[0];
	}
	return db[id];
}
