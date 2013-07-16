/************************************
 Written by ProgMan
 not: 
 www.altinkaynak.com un IP adresi statik olarak gomuldu,
 adres degisirse update edilecek..      
 ************************************/
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <sys/resource.h>
#include <sys/wait.h>
#include <pthread.h>
#include <string>
#include <vector>
#include <algorithm>
#include <locale>

#include <iconv.h>

#include "bbs.h"

using namespace std;
//-----------------------
void initCpp(void) {
/***************************
  bu kismi eklemiþtim ama tekrar çýkardým
  turkish sort olayý için yazýlmýþtý
	try
	{
		locale::global(locale("tr_TR.utf8"));
	}
	catch (...)
	{
	}
***************************/
}
//-----------------------
bool sortCriteriaForBoard(const BOARD_DATA *bd1, const BOARD_DATA * bd2) {
	string s1(bd1->short_name);
	string s2(bd2->short_name);
	return (s1 < s2);
}

bool sortCriteriaForDescData(const DESC_DATA *dd1, const DESC_DATA * dd2) {

	string s1(USR(dd1)->name);
	string s2(USR(dd2)->name);
	return (s1 < s2);
}

void do_who(USER_DATA *usr, char *argument) {
	char buf[STRING];
	BUFFER *output;
	char buf_idle_hr[INPUT];
	char buf_idle_mn[INPUT];
	char buf_time[INPUT];
	char *strtime;
	DESC_DATA *d;
	int nMatch = 0;
	int idle, idle_hr, idle_mn;
	bool bFriend= FALSE;
	bool bEnemy= FALSE;
	bool bNotify= FALSE;
	bool bSortedByName= FALSE;

	for (;;) {
		char arg[INPUT];

		one_argument(argument, arg);

		if (arg[0] == '\0')
			break;

		if (!str_cmp(arg, "-f")) {
			bFriend = TRUE;
			break;
		} else if (!str_cmp(arg, "-e")) {
			bEnemy = TRUE;
			break;
		} else if (!str_cmp(arg, "-n")) {
			bNotify = TRUE;
			break;
		} else if (!str_cmp(arg, "-a")) {
			bSortedByName = TRUE;
			break;
		} else if (!str_cmp(arg, "-h")) {
			do_help(usr, "who");
			return;
		} else {
			if (is_turkish(usr))
				sprintf(buf, "Gecersiz opsiyon %s.\r\n", arg);
			else
				sprintf(buf, "Unknown option %s.\r\n", arg);
			send_to_user(buf, usr);
			return;
		}
	}

	buf[0] = '\0';
	buf_idle_hr[0] = '\0';
	buf_idle_mn[0] = '\0';
	output = new_buf();
	strtime = ctime(&current_time);
	strtime[strlen(strtime)-1] = '\0';
	if (is_turkish(usr))
		sprintf(buf_time, "#WKullanici        Zaman  Baslik\r\n");
	else
		sprintf(buf_time, "#WNickname         Time   Title\r\n");
	add_buf(output, buf_time);
	add_buf(
			output,
			"#W=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=#x\r\n");

	std::vector<DESC_DATA *> dv;

	for (d = desc_list; d != NULL; d = d->next) {
		if ((d->login != CON_LOGIN) || !USR(d) || (IS_TOGGLE(USR(d), TOGGLE_INVIS) && !IS_ADMIN(usr)) || (!USR(d)->Validated && !IS_ADMIN(usr)))
			continue;

		if ((bFriend && !is_friend(usr, USR(d)->name)) || (bEnemy && !is_enemy(usr, USR(d)->name)) || (bNotify && !is_notify(usr, USR(d)->name)))
			continue;

		dv.push_back(d);
	}

	if (bSortedByName)
		std::sort(dv.begin(), dv.end(), sortCriteriaForDescData);

	for (unsigned di = 0; di < dv.size(); di++) {

		d = dv[di];

		nMatch++;

		idle = (int) (current_time - USR(d)->logon) / 60;
		idle_hr = idle / 60;
		idle_mn = idle % 60;

		if (idle_hr > 9)
			sprintf(buf_idle_hr, "%d", idle_hr);
		else
			sprintf(buf_idle_hr, "0%d", idle_hr);

		if (idle_mn > 9)
			sprintf(buf_idle_mn, "%d", idle_mn);
		else
			sprintf(buf_idle_mn, "0%d", idle_mn);

		sprintf(buf, "%s%-12s #R%s%s%s #y%s:%s  #x%-35s#x\r\n", is_friend(usr, 
		USR(d)->name) ? "#C" : is_notify(usr, USR(d)->name) ? "#G" : is_enemy(usr, USR(d)->name) ? "#R" : !d->user->Validated ? "#D" : "#Y", 
		USR(d)->name, 
		IS_TOGGLE(USR(d), TOGGLE_IDLE) ? "%" : " ", 
		IS_TOGGLE(USR(d), TOGGLE_XING) ? " " : "*", is_ignore(usr, USR(d)->name, TRUE) ? "@" : " ", buf_idle_hr, buf_idle_mn, 
		USR(d)->title);
		add_buf(output, buf);
	}

	if (nMatch < 1) {
		if (bFriend) {
			if (is_turkish(usr))
				send_to_user(
						"Friend listenizde kayitli, aktif kullanici bulunamadi.\r\n",
						usr);
			else
				send_to_user(
						"None of the users in your friend list is online.\r\n",
						usr);
		} else if (bNotify) {
			if (is_turkish(usr))
				send_to_user(
						"Notify listenizde kayitli, aktif kullanici bulunamadi.\r\n",
						usr);
			else
				send_to_user(
						"None of the users in your notify list is online.\r\n",
						usr);
		} else if (bEnemy) {
			if (is_turkish(usr))
				send_to_user(
						"Enemy listenizde kayitli, aktif kullanici bulunamadi.\r\n",
						usr);
			else
				send_to_user(
						"None of the users in your enemy list is online.\r\n",
						usr);
		} else {
			if (is_turkish(usr))
				send_to_user("Aktif kullanici yok.\r\n", usr);
			else
				send_to_user("None.\r\n", usr);
		}

		free_buf(output);
		return;
	}

	add_buf(
			output,
			"#W=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=#x\r\n");
	if (is_turkish(usr))
		sprintf(buf, "#GToplam #Y%d #Gkullanici#x\r\n", nMatch);
	else
		sprintf(buf, "#GThere %s #Y%d #Guser%s#x\r\n", (nMatch > 1) ? "are"
				: "is", nMatch, (nMatch > 1) ? "s" : "");
	add_buf(output, buf);
	page_to_user(buf_string(output), usr);
	free_buf(output);
	return;
}

void do_newmsgs(USER_DATA * usr, char *argument) {
	char buf[STRING];
	char nw_buf[STRING];
	char no_buf[STRING];
	char fr_buf[INPUT];
	char zap[INPUT];
	BUFFER *output;
	BOARD_DATA *pBoard;
	bool foundNew= FALSE;
	bool foundNo= FALSE;
	int col_new = 0;
	int col_no = 0;
	unsigned int i;

	output = new_buf();
	strcpy(nw_buf, "");
	strcpy(no_buf, "");

	std::vector<BOARD_DATA *> bv;

	for (pBoard = first_board; pBoard; pBoard = pBoard->next)
		bv.push_back(pBoard);

	std::sort(bv.begin(), bv.end(), sortCriteriaForBoard);
	for (unsigned ib = 0; ib < bv.size(); ib++) {
		pBoard = bv[ib];
		sprintf(zap, "%s ", pBoard->short_name);

		if (pBoard->type != BOARD_GAME && pBoard->type != BOARD_SECRET
				&& !is_name_full(zap, usr->zap) && board_number(pBoard) < 100
				&& unread_notes(usr, pBoard) > 0) {
			foundNew = TRUE;
			strcpy(fr_buf, "");
			for (i = 0; i < (13 - strlen(pBoard->long_name)); i++)
				strcat(fr_buf, ".");

			sprintf(buf, "%s%s%s%2d. %s#W%s#c%-3d  ", is_name_full(
					pBoard->short_name, usr->fnotify) ? "#W" : "#Y", can_write(
					usr, pBoard) ? " " : "*", is_kicked(usr, pBoard, 
			FALSE) ? "!" : " ", pBoard->vnum, pBoard->long_name, fr_buf,
					unread_notes(usr, pBoard));
			strcat(nw_buf, buf);
			if (++col_new % 3 == 0)
				strcat(nw_buf, "\r\n");
		} else if (pBoard->type != BOARD_GAME && pBoard->type != BOARD_SECRET
				&& !is_name_full(zap, usr->zap)) {
			foundNo = TRUE;
			sprintf(buf, "%s%s%s%2d. %-12s  ", is_name_full(pBoard->short_name,
					usr->fnotify) ? "#W" : "#R", can_write(usr, pBoard) ? " "
					: "*", is_kicked(usr, pBoard, 
			FALSE) ? "!" : " ", pBoard->vnum, pBoard->long_name);
			strcat(no_buf, buf);
			if (++col_no % 4 == 0)
				strcat(no_buf, "\r\n");
		}
	}

	if (col_new % 3 != 0)
		strcat(nw_buf, "\r\n");
	if (col_no % 4 != 0)
		strcat(no_buf, "\r\n");

	if (foundNew) {
		add_buf(output, "#GNEW MESSAGES in:#x\r\n");
		add_buf(output, nw_buf);
	}

	if (!foundNo) {
		page_to_user(buf_string(output), usr);
		free_buf(output);
		return;
	}

	if (foundNew)
		add_buf(output, "\r\n#GNO NEW MESSAGES in:#x\r\n");
	else
		add_buf(output, "#GNO NEW MESSAGES in:#x\r\n");

	add_buf(output, no_buf);
	add_buf(output, "#x");
	page_to_user(buf_string(output), usr);
	free_buf(output);
	return;
}

void do_forumlist(USER_DATA * usr, char *argument) {
	char buf[STRING];
	char forum_list[STRING];
	BUFFER *output;
	BOARD_DATA *pBoard;
	int col = 0;

	forum_list[0] = '\0';
	output = new_buf();

	add_buf(output, "#GKnown forums:#x\r\n");
	std::vector<BOARD_DATA *> bv;

	for (pBoard = first_board; pBoard; pBoard = pBoard->next)
		if (pBoard->type != BOARD_SECRET)
			bv.push_back(pBoard);

	std::sort(bv.begin(), bv.end(), sortCriteriaForBoard);

	for (unsigned ib = 0; ib < bv.size(); ib++) {
		pBoard = bv[ib];
		sprintf(buf, "%s%s%s%2d. %-12s ", is_name_full(pBoard->short_name,
				usr->zap) ? "#R" : is_name_full(pBoard->short_name,
				usr->fnotify) ? "#W" : "#G",
				can_write(usr, pBoard) ? " " : "*", is_kicked(usr, pBoard, 
				FALSE) ? "!" : " ", pBoard->vnum, pBoard->long_name);
		strcat(forum_list, buf);
		if (++col % 4 == 0)
			strcat(forum_list, "\r\n");
	}

	if (col % 4 != 0)
		strcat(forum_list, "\r\n");

	add_buf(output, forum_list);
	add_buf(output, "#x");
	page_to_user(buf_string(output), usr);
	free_buf(output);
	return;
}

void do_Who(USER_DATA * usr, char *argument) {
	char buf[STRING];
	char cBuf[STRING];
	char for_buf[INPUT];
	char arg[INPUT];
	BOARD_DATA *pBoard;
	BUFFER *output;
	DESC_DATA *d;
	int col = 0;
	bool noArg= FALSE;
	unsigned int i;

	one_argument(argument, arg);

	pBoard = board_lookup(arg, FALSE);

	if (arg[0] == '\0')
		noArg = TRUE;
	else if (!pBoard) {
		send_to_user("No such forum.\r\n", usr);
		return;
	}

	output = new_buf();
	strcpy(cBuf, "");

	std::vector<DESC_DATA *> dv;

	for (d = desc_list; d; d = d->next) {
		if ((d->login != CON_LOGIN) || !USR(d) || (IS_TOGGLE(USR(d), TOGGLE_INVIS) && !IS_ADMIN(usr)) || (!USR(d)->Validated && !IS_ADMIN(usr)))
			continue;

		if (!noArg && USR(d)->pBoard != pBoard)
			continue;

		dv.push_back(d);
	}

	std::sort(dv.begin(), dv.end(), sortCriteriaForDescData);

	for (unsigned di = 0; di < dv.size(); di++) {
		d = dv[di];

		strcpy(for_buf, "");
		for (i = 1; i < (13 - strlen(USR(d)->name)); i++)
			strcat(for_buf, "_");

		sprintf(buf, "#c%s%s%s%s%s#W%s#c%-12s ", 
		IS_TOGGLE(USR(d), TOGGLE_IDLE) ? "%" : " ", 
		IS_TOGGLE(USR(d), TOGGLE_XING) ? " " : "*", is_ignore(usr, USR(d)->name, TRUE) ? "@" : " ", !USR(d)->Validated ? "#D" : is_friend(usr, USR(d)->name) ? "#C" : is_notify(usr, USR(d)->name) ? "#G" : is_enemy(usr, USR(d)->name) ? "#R" : "#Y", 
		USR(d)->name, for_buf, (EDIT_MODE(USR(d)) == EDITOR_MAIL_SUBJECT
		|| EDIT_MODE(USR(d)) == EDITOR_MAIL_WRITE
		|| EDIT_MODE(USR(d)) == EDITOR_MAIL) ? "<Mail>" : 
		USR(d)->pBoard->type == BOARD_SECRET ? "<Secret>" :
		USR(d)->pBoard->long_name);
		strcat(cBuf, buf);
		if (++col % 3 == 0)
			strcat(cBuf, "\r\n");
	}

	if (col % 3 != 0)
		strcat(cBuf, "\r\n");

	if (dv.size() == 0 && !noArg) {
		print_to_user(usr, "There is no one in %s forum.\r\n",
				pBoard->long_name);
		return;
	}

	sprintf(buf, "#Y%s BBS Who List (#GTotal #Y%d #Guser%s#Y):\r\n"
		"#W=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=\r\n", config.bbs_name,
			dv.size(), dv.size() > 1 ? "s" : "");
	add_buf(output, buf);
	add_buf(output, cBuf);
	add_buf(output, "#x");
	page_to_user(buf_string(output), usr);
	free_buf(output);
	return;
}

//----------- buradan sonrasi i18n ile ilgili ------------
static const char *cp_alias[] = { "CP437", "IBM437", "437", "CSPC8CODEPAGE437",
		"CP737", "CP775", "IBM775", "CSPC775BALTIC", "CP852", "IBM852", "852",
		"CSPCP852", "CP853", "CP855", "IBM855", "855", "CSIBM855", "CP857",
		"IBM857", "857", "CSIBM857", "CP858", "CP860", "IBM860", "860",
		"CSIBM860", "CP861", "IBM861", "861", "CP-IS", "CSIBM861", "CP863",
		"IBM863", "863", "CSIBM863", "CP864", "IBM864", "CSIBM864", "CP865",
		"IBM865", "865", "CSIBM865", "CP869", "IBM869", "869", "CP-GR",
		"CSIBM869", "CP1125", "857", "CP857", "CP1254", "CN", "R8", "866",
		"862", "CP1361", "CP866", "CP1251", "CP1256", "CP1255", "CP1133",
		"ASCII", "CP862", "CP1253", "CHAR", "CP1252", "CP936", "CP1258", "C99",
		"CP932", "ISO-IR-6", "CP819", "ISO-IR-166", "ISO-IR-165", "ISO-IR-126",
		"ISO-IR-58", "ISO-IR-226", "ISO8859-1", "ISO8859-6", "ISO-IR-138",
		"ISO8859-5", "ISO8859-16", "ISO8859-15", "ISO8859-3", "ISO8859-2",
		"ISO8859-13", "ISO8859-8", "ISO-8859-1", "GB2312", "ISO-8859-6",
		"EUCCN", "ISO-8859-5", "ISO-8859-16", "ISO-IR-159", "ISO-8859-15",
		"ISO-8859-3", "ISO-8859-2", "ISO-8859-13", "ISO-8859-8", "ISO-IR-101",
		"850", "EUC-CN", "ISO8859-9", "ISO-IR-199", "CSASCII", "ISO646-CN",
		"CP850", "ISO-IR-203", "CP1250", "HZ", "ISO-8859-9", "CP950",
		"ISO-2022-CN", "ISO_8859-1", "CP949", "ISO_8859-6", "ISO-IR-148",
		"ISO_8859-5", "ISO_8859-16", "ISO-IR-109", "ISO_8859-15", "ISO_8859-3",
		"ISO_8859-16:2001", "ISO_8859-2", "ISO_8859-13", "ISO_8859-8",
		"ISO-IR-110", "ISO_8859-15:1998", "ISO-2022-CN-EXT", "CP1254",
		"ISO-IR-149", "L1", "L6", "L5", "L3", "L2", "L8", "ISO_8859-9",
		"ISO8859-10", "CSISO2022CN", "ISO-IR-179", "UHC", "ISO-IR-14",
		"ISO-8859-10", "CP367", "ISO_8859-10:1992", "ISO-IR-100", "LATIN1",
		"CP1257", "LATIN6", "ISO8859-4", "LATIN5", "TIS620", "ISO8859-14",
		"ELOT_928", "LATIN3", "SJIS", "LATIN2", "LATIN8", "ISO_8859-14:1998",
		"GEORGIAN-ACADEMY", "MAC", "ISO-8859-4", "TIS-620", "ISO-8859-14",
		"GB18030", "X0212", "L4", "ISO-IR-57", "ISO_8859-10", "IBM866",
		"ISO-IR-157", "ISO-IR-87", "ISO-IR-127", "US", "CP874", "IBM862",
		"MS936", "ISO8859-7", "L7", "LATIN-9", "ISO-IR-144", "L10", "X0201",
		"ROMAN8", "ISO-8859-7", "ISO_8859-4", "IBM819", "ARABIC",
		"ISO_8859-14", "GB_2312-80", "BIG5", "TIS620-0", "UCS-2", "X0208",
				"CSBIG5", "CSKOI8R", "GB_1988-80", "BIG-5", "KOI8-R",
				"IBM-CP1133", "JP", "US-ASCII", "CN-BIG5", "LATIN10",
		"CHINESE", "CSUNICODE11", "ISO-CELTIC", "CSGB2312", "ISO_8859-7",
				"CSISOLATIN1", "CSISOLATIN6", "CSISOLATIN5", "TIS620.2533-1",
				"MACCROATIAN", "CSISOLATIN3", "UNICODE-1-1", "CSISOLATIN2",
				"KOI8-T", "CSISOLATINCYRILLIC", "IBM850", "MS-ANSI",
				"TIS620.2529-1", "LATIN4", "GEORGIAN-PS", "EUCKR",
				"CSISOLATINARABIC", "ECMA-118", "UTF-16", "ARMSCII-8",
		"EUC-KR", "ISO-10646-UCS-2", "UTF-8", "KOREAN", "CYRILLIC", "UTF-32",
				"TIS620.2533-0", "CSUNICODE", "ISO_8859-5:1988",
				"ISO_8859-3:1988", "ISO_8859-8:1988", "LATIN7", "ISO-2022-KR",
				"KSC_5601", "MACTHAI", "CSUCS4", "UCS-4", "CSUNICODE11UTF7",
				"ISO_8859-9:1989", "CN-GB-ISOIR165", "EUCJP", "IBM367",
				"HP-ROMAN8", "ASMO-708", "ISO646-US", "ISO-10646-UCS-4",
				"UNICODE-1-1-UTF-7", "EUC-JP", "WCHAR_T", "EUCTW",
				"ISO-2022-JP-1", "CSHPROMAN8", "ISO646-JP", "CSISO2022KR",
				"TCVN", "ISO-2022-JP-2", "ISO_8859-4:1988", "EUC-TW",
				"CSISO58GB231280", "MS-EE", "ISO-2022-JP", "CSISOLATIN4",
				"CSPC862LATINHEBREW", "NEXTSTEP", "ISO_8859-1:1987",
				"ISO_8859-6:1987", "CSIBM866", "ISO_8859-2:1987", "HZ-GB-2312",
				"WINDOWS-1251", "WINDOWS-1256", "WINDOWS-1255", "ECMA-114",
				"WINDOWS-1253", "WINDOWS-1252", "WINDOWS-1258", "GREEK8",
				"MACROMAN", "JIS_C6226-1983", "CSISO2022JP2", "WINDOWS-936",
				"JIS0208", "VISCII", "CSISO57GB1988", "KS_C_5601-1989",
				"CSISO2022JP", "CSVISCII", "CN-GB", "MACARABIC",
		"WINDOWS-1250", "MACROMANIA", "CSKSC56011987", "JIS_C6220-1969-RO",
				"UTF-7", "CSEUCKR", "CSISO14JISC6220RO", "WINDOWS-1254",
				"CSISO159JISX02121990", "ISO_8859-7:1987", "MACICELAND",
				"WINDOWS-1257", "GBK", "KS_C_5601-1987", "TCVN5712-1",
				"TCVN-5712", "UCS-2-INTERNAL", "MACINTOSH", "UNICODELITTLE",
				"UCS-2LE", "ANSI_X3.4-1986", "MS-CYRL", "ANSI_X3.4-1968",
				"CSISOLATINHEBREW", "MACCYRILLIC", "CSMACINTOSH", "CSEUCTW",
				"UNICODEBIG", "UCS-2-SWAPPED", "CSISOLATINGREEK", "UCS-2BE",
				"KOI8-U", "UCS-4-INTERNAL", "VISCII1.1-1", "KOI8-RU",
		"UCS-4LE", "MS-HEBR", "EXTENDED_UNIX_CODE_PACKED_FORMAT_FOR_JAPANESE",
				"UTF-16LE", "MULELAO-1", "UTF-32LE", "MACCENTRALEUROPE",
				"UCS-4-SWAPPED", "WINDOWS-874", "ISO_646.IRV:1991", "UCS-4BE",
				"SHIFT-JIS", "JIS_X0212", "MS-ARAB", "GREEK", "UTF-16BE",
				"JISX0201-1976", "UTF-32BE", "JAVA", "JIS_X0201", "HEBREW",
				"SHIFT_JIS", "JIS_X0208", "CSISO87JISX0208", "JIS_X0212-1990",
				"JIS_X0208-1983", "TCVN5712-1:1993", "CSSHIFTJIS",
				"JIS_X0208-1990", "MACUKRAINE", "MS_KANJI",
				"CSHALFWIDTHKATAKANA", "JOHAB", "CSPC850MULTILINGUAL",
				"JIS_X0212.1990-0", "BIG5HKSCS", "BIG5-HKSCS", "MACGREEK",
				"MS-TURK", "MS-GREEK", "BIGFIVE", "BIG-FIVE", "MACTURKISH",
				"WINBALTRIM", "MACHEBREW", "CSEUCPKDFMTJAPANESE", NULL };

const char * checkCharset(const char * name) {
	for (unsigned i = 0; cp_alias[i] != NULL; ++i)
		if (str_cmp(name, cp_alias[i]) == 0)
			return cp_alias[i];
	return NULL;
}

/*
 bu method kullanildiktan sonra ayrilan memory nin free
 edilmesi cagiran tarafin sorumlulugundadir..
 */
char * iconv_strndup(const char * str, size_t n, const char * toCharset,
		const char * fromCharset, int *outLen) {
	if (n < 1)
		return NULL;
	std::string sBuf(str, n);
	iconv_t cd;
	cd = iconv_open(toCharset, fromCharset);

	if (cd == (iconv_t)-1) {
		*outLen = n;
		return strndup(str, n);
	}

	char *outbuf, *defbuf;
	size_t inbufbytes, outbufbytes;

	inbufbytes = n;
	outbufbytes = n * 3 + 4;
	outbuf = (char*)malloc(outbufbytes + 1);
	memset(outbuf, '\0', outbufbytes + 1);
	defbuf = outbuf;
	size_t s;
	while (inbufbytes) {
#ifdef __CYGWIN__
		s = iconv(cd, &str, &inbufbytes, &outbuf, &outbufbytes);
#else
		s = iconv(cd, const_cast<char**>(&str), &inbufbytes, &outbuf, &outbufbytes);
#endif
		if (s == (size_t)(-1)) {
			free(defbuf);
			//fprintf(stderr, "Error in iconv conversion! str: %s\n", str );
			std::string s;
			unsigned i = 0;
			while (i < sBuf.length()) {
				unsigned char ch = sBuf[i];
				if (ch == 0xC3) {
					i++;
					ch = sBuf[i];
					if (ch == 0xBC) {
						s += 'u';
						i++;
					} else if (ch == 0xB6) {
						s += 'o';
						i++;
					} else if (ch == 0xA7) {
						s += 'c';
						i++;
					} else if (ch == 0x9C) {
						s += 'U';
						i++;
					} else if (ch == 0x96) {
						s += 'O';
						i++;
					} else if (ch == 0x87) {
						s += 'C';
						i++;
					}
					continue;
				} else if (ch == 0xC4) {
					i++;
					ch = sBuf[i];
					if (ch == 0x9F) {
						s += 'g';
						i++;
					} else if (ch == 0xB1) {
						s += 'I';
						i++;
					} else if (ch == 0x9E) {
						s += 'G';
						i++;
					} else if (ch == 0xB0) {
						s += 'i';
						i++;
					}
					continue;
				} else if (ch == 0xC5) {
					i++;
					ch = sBuf[i];
					if (ch == 0x9E || ch == 0x9F) {
						s += '$';
						i++;
					}
					continue;
				} else
					s += ch;
				i++;
			}
			*outLen = s.length();
			return strndup(s.data(), s.length());
		}
	}
	iconv_close( cd );
	*outLen = outbuf - defbuf + 1;
	return defbuf;
}

//----------- buradan sonrasi doviz kurlari ile ilgili ------------

// bu bizim HTML GET request imiz
const char urlGET[] = "GET /currency.aspx HTTP/1.0\r\n"
	"User-Agent: CERN-LineMode/2.15 libwww/2.17b3\r\n"
	"Accept: */*\r\n"
	"Host: www.altinkaynak.com\r\n\r\n";

static string exUpdateTime = "Thread not started or connection error!";

struct doviz_t {
	const char *kisaltma;
	const char *aciklama;
	float al;
	float sat;
} dovizler[] = { 
  /*  -------- BU KISIMDAKI BOSLUKLARA ELLESMEYIN --------- */
  { "USD", "Amerikan Dolar\u0131       ", 0.0, 0.0 }, 
  { "EUR", "Avrupa Para Birimi  ", 0.0, 0.0 },
	{ "CHF", "\u0130svicre Frang\u0131        ", 0.0, 0.0 }, 
  { "GBP", "\u0130ngiliz Sterlini      ", 0.0, 0.0 }, 
  { "DKK", "Danimarka Kronu  ", 0.0, 0.0 }, 
  { "SEK", "\u0130sve\u00E7 Kronu           ", 0.0, 0.0 }, 
  { "NOK", "Norve\u00E7 Kronu          ", 0.0, 0.0 },
	{ "JPY", "Japon Yeni  ", 0.0, 0.0 }, 
  { "SAR", "S.Arabistan Riyali  ", 0.0, 0.0 }, 
  { "AUD", "Avustralya Dolar\u0131     ", 0.0, 0.0 }, 
  { "CAD", "Kanada Dolar\u0131         ", 0.0, 0.0 }, 
  { NULL, NULL, 0.0, 0.0 } 
};

/*
 "str" icinde "pos" dan itibaren "findStr" arar, index ini doner
 */
size_t findInString(const std::string & str, size_t pos, const char * findStr) {
	std::string::size_type findPos = str.find(findStr, pos);
	return findPos;
}

/*
 stringBetween: str icinden tag1 ve tag2 arasini dondurur
 */
std::string stringBetween(const std::string & str, const char * tag1,
		const char * tag2) {
	string ret;
	string::size_type start1 = findInString(str, 0, tag1);
	if (string::npos != start1) {
		start1 += strlen(tag1);
		string::size_type start2 = findInString(str, start1, tag2);
		ret = str.substr(start1, start2 - start1);
	}
	return ret;
}

void * exchangeEntryPoint(void *parameters);

static pthread_t exThread = 0;
static pthread_mutex_t exMutex= PTHREAD_MUTEX_INITIALIZER;
static bool exThreadFinished = false;

void * exchangeEntryPoint(void *parameters) {
	while (!exThreadFinished) {
		int s = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
		if (s >= 0) {
			struct sockaddr_in addr;
			addr.sin_family = AF_INET;
			addr.sin_addr.s_addr = inet_addr("212.50.55.71"); // www.altinkaynak.com.tr
			addr.sin_port = htons(80); // HTTP port
			int iResult = connect(s, (struct sockaddr *)&addr, sizeof(addr));
			if (iResult >= 0) {
				send(s, urlGET, strlen(urlGET), 0);
				char buf[4096];
				string str; // HTML sayfayi str icine alacagiz
				int okunan = 0;
				while ((iResult = recv(s, buf, sizeof(buf), 0)) > 0) {
					okunan += iResult;
					//fprintf(stderr,"\r%u bytes read..", okunan);
					str.append(buf, iResult);
				}
				close(s);
				pthread_mutex_lock(&exMutex);
				exUpdateTime = stringBetween(str,
						"<span id=\"ctl00_ContentPlaceHolder1_lb_time\">",
						"</span>");
				for (struct doviz_t * dt = dovizler; dt->kisaltma; ++dt) {
					string s1("<span id=\"ctl00_ContentPlaceHolder1_lb_");
					s1 += dt->kisaltma;
					string s2 = s1;
					s1 += "_al\">";
					s2 += "_sat\">";
					dt->al = atof(stringBetween(str, s1.c_str(),"</span>").c_str());
					dt->sat = atof(stringBetween(str, s2.c_str(),"</span>").c_str());
				}
				pthread_mutex_unlock(&exMutex);
			}
		}
		usleep(1000000*60*5); // 5 dk. bekle
	}
	return 0;
}

void createExchangeThread(void) {
	pthread_mutex_init(&exMutex, NULL);
	pthread_create(&exThread, NULL, &exchangeEntryPoint, NULL);
}

void finishExchangeThread(void) {
	exThreadFinished = true;
	pthread_cancel(exThread);
	pthread_mutex_destroy(&exMutex);
	pthread_join(exThread, NULL); // thread in bitisini bekle
}

void do_disp_exchange(USER_DATA *usr, char *argument) {
	char buf[STRING_LENGTH];
	pthread_mutex_lock(&exMutex);
	sprintf(buf, "#W  Kur g\u00FCncelleme zaman\u0131: #R%s#x\r\n", exUpdateTime.c_str());
	send_to_user(buf, usr);
	send_to_user("#y ================================ Al\u0131\u015F === Sat\u0131\u015F ==\r\n",
			usr);
	for (struct doviz_t * dt = dovizler; dt->kisaltma; ++dt) {
		sprintf(buf, "#C %4s - %-22s   %2.4f   %2.4f#x\r\n", dt->kisaltma,
				dt->aciklama, dt->al, dt->sat);
		send_to_user(buf, usr);
	}
	sprintf(buf, "#W  EUR / USD Parite: %2.4f#x\r\n", (float)dovizler[1].sat
			/ dovizler[0].sat);

	send_to_user(buf, usr);
	pthread_mutex_unlock(&exMutex);
}
//----------- end of file -----------------
