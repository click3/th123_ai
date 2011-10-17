#include "stdafx.h"

namespace {

bool CharIDvalidate(CHAR_ID id) {
	if(::IsSWR()) {
		return SWR_CHAR_ID_MIN <= id && id <= SWR_CHAR_ID_MAX;
	} else {
		return SWRS_CHAR_ID_MIN <= id && id <= SWRS_CHAR_ID_MAX;
	}
}

bool is_swr = false;

} // anonymous

bool IsSWR(void) {
	return is_swr;
}

void AIModeSWR(void) {
	is_swr = true;
}

void AIModeSWRS(void) {
	is_swr = false;
}

const char *ID2Char(CHAR_ID id) {
	static const char * const db[] = {
		"—ì–²",	"–‚—¹",	"ç–é",	"ƒAƒŠƒX",
		"ƒpƒ`ƒ…ƒŠ[",	"—d–²",	"ƒŒƒ~ƒŠƒA",	"—HXŽq",
		"Ž‡",		"äÂ",	"—D“Ü‰Ø",	"•¶",
		"¬’¬",	"ˆß‹è",	"“VŽq",	"‘•c",
		"ƒ`ƒ‹ƒm",	"”ü—é",	"‹ó",		"z–KŽq"};
	if(!CharIDvalidate(id)) {
		printf("Error:id2char ”ÍˆÍŠO‚ÌID\n");
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
		printf("Error:id2char ”ÍˆÍŠO‚ÌID\n");
		BOOST_ASSERT(false);
		return db[0];
	}
	return db[id];
}


