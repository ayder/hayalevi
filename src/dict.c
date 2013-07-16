/************************************
 Written by ProgMan
 ************************************/

#include <sys/types.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <arpa/telnet.h>

#include "bbs.h"
#include <sqlite3.h>

static sqlite3 * db_de2tr= NULL;
static sqlite3 * db_tr2de= NULL;
static sqlite3 * db_en2en= NULL;
static sqlite3 * db_en2tr= NULL;
static sqlite3 * db_tr2en= NULL;
static sqlite3 * db_fr2tr= NULL;
static sqlite3 * db_tr2fr= NULL;
static sqlite3 * db_tr2tr= NULL;

struct callbackParams {
	USER_DATA* usr;
	int nRecs;
};

typedef struct callbackParams PARAMS;

void init_dict(void) {
	sqlite3_open(DBFILE_DE2TR, &db_de2tr);
	sqlite3_open(DBFILE_TR2DE, &db_tr2de);
	sqlite3_open(DBFILE_EN2EN, &db_en2en);
	sqlite3_open(DBFILE_EN2TR, &db_en2tr);
	sqlite3_open(DBFILE_TR2EN, &db_tr2en);
	sqlite3_open(DBFILE_FR2TR, &db_fr2tr);
	sqlite3_open(DBFILE_TR2FR, &db_tr2fr);
	sqlite3_open(DBFILE_TR2TR, &db_tr2tr);
}

void done_dict(void) {
	sqlite3_close(db_de2tr);
	sqlite3_close(db_tr2de);
	sqlite3_close(db_en2en);
	sqlite3_close(db_en2tr);
	sqlite3_close(db_tr2en);
	sqlite3_close(db_fr2tr);
	sqlite3_close(db_tr2fr);
	sqlite3_close(db_tr2tr);
}

int dictCallback(void *p, int nCols, char **values, char **headers) {
	char buf[STRING_LENGTH];
	int i = 0;
	PARAMS* params = (PARAMS*) p;
	params->nRecs++;
	for (i = 0; i < nCols; i++) {
		if (!(i % 2))
			sprintf(buf, "#G%s: ", values[i]);
		else
			sprintf(buf, "#W%s#x\r\n", values[i]);
		send_to_user(buf, params->usr);
	}
	return 0;
}

void queryDictionary(sqlite3 ** db, USER_DATA *usr, char * argument) {
	char arg[INPUT_LENGTH];
	char * sql;
	char * sqlErr= NULL;

	argument = one_argument(argument, arg);

	if (arg[0] == '\0')
		return;

	PARAMS p = { usr, 0 };
	sql = sqlite3_mprintf("select * from dict where word like '%q' limit 20;",
			arg);
	sqlite3_exec(*db, sql, dictCallback, &p, &sqlErr);
	sqlite3_free(sql);
	if (sqlErr) {
		sqlite3_free(sqlErr);
		send_to_user("Sorry, an error occurred while running your query.", usr);
		return;
	}
	if (p.nRecs == 0) {
		send_to_user("Sorry, don't know what that means.\r\n", usr);
	}
}

void do_de2tr(USER_DATA *usr, char *argument) {
	if (db_de2tr)
		queryDictionary(&db_de2tr, usr, argument);
}

void do_tr2de(USER_DATA *usr, char *argument) {
	if (db_tr2de)
		queryDictionary(&db_tr2de, usr, argument);
}

void do_en2en(USER_DATA *usr, char *argument) {
	if (db_en2en)
		queryDictionary(&db_en2en, usr, argument);
}

void do_en2tr(USER_DATA *usr, char *argument) {
	if (db_en2tr)
		queryDictionary(&db_en2tr, usr, argument);
}

void do_tr2en(USER_DATA *usr, char *argument) {
	if (db_tr2en)
		queryDictionary(&db_tr2en, usr, argument);
}

void do_fr2tr(USER_DATA *usr, char *argument) {
	if (db_fr2tr)
		queryDictionary(&db_fr2tr, usr, argument);
}

void do_tr2fr(USER_DATA *usr, char *argument) {
	if (db_tr2fr)
		queryDictionary(&db_tr2fr, usr, argument);
}

void do_tr2tr(USER_DATA *usr, char *argument) {
	if (db_tr2tr)
		queryDictionary(&db_tr2tr, usr, argument);
}

void do_dict(USER_DATA *usr, char *argument) {
    
    if ('\0' == argument[0]) {    // eger parametre yoksa
        do_help(usr, "dict");     // "dict" in helpini cagir
        return;
    }  
   
	send_to_user("#YUsing all dictionaries...#x\r\n", usr);

	if (db_de2tr) {	
        send_to_user("#Rde2tr (German to Turkish) :#x\r\n", usr);
		queryDictionary(&db_de2tr, usr, argument);
    }
	if (db_tr2de) {
        send_to_user("#Rtr2de (Turkish to German) :#x\r\n", usr);
		queryDictionary(&db_tr2de, usr, argument);
    }
	if (db_en2en) {
        send_to_user("#Ren2en (English to English):#x\r\n", usr);
		queryDictionary(&db_en2en, usr, argument);
    }
	if (db_en2tr) {
        send_to_user("#Ren2tr (English to Turkish):#x\r\n", usr);
		queryDictionary(&db_en2tr, usr, argument);
    }
	if (db_tr2en) {
        send_to_user("#Rtr2en (Turkish to English):#x\r\n", usr);
		queryDictionary(&db_tr2en, usr, argument);
    }
	if (db_fr2tr) {
        send_to_user("#Rfr2tr (French to Turkish) :#x\r\n", usr);
		queryDictionary(&db_fr2tr, usr, argument);
    }
	if (db_tr2fr) {
        send_to_user("#Rtr2fr (Turkish to French) :#x\r\n", usr);
		queryDictionary(&db_tr2fr, usr, argument);
    }
	if (db_tr2tr) {
        send_to_user("#Rtr2tr (Turkish to Turkish):#x\r\n", usr);
		queryDictionary(&db_tr2tr, usr, argument);
    }
}
