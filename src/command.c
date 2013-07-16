/*
 * Husky BBS Server for Linux
 * Version 3.0 Copyright (C) 1997-1998-1999-2000-2001 by Hidayet Dogan
 *
 * E-mail: hdogan@bilcag.net or hidayet.dogan@pleksus.net.tr
 *
 * Refer to the file "LICENSE" included in this package for further
 * information and before using any of the following.
 *
 * + Original Diku Mud copyright (C) 1990, 1991 by Sebastian Hammer,
 *   Michael Seifert, Hans Henrik St{rfeldt, Tom Madsen, and Katja Nyboe.
 * + Merc Diku Mud improvments copyright (C) 1992, 1993 by Michael
 *   Chastain, Michael Quan, and Mitchell Tse.
 * + ROM 2.4 is copyright 1993-1998 Russ Taylor
 * + SMAUG 1.4 (C) 1994, 1995, 1996, 1998 by Derek Snider
 *
 * Original Diku license in the file 'doc/diku.license',
 * Merc license in the file 'doc/merc.license',
 * ROM license in the file 'doc/rom.license' and
 * Smaug license in the file 'doc/smaug.license'
 */

#include <sys/types.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <arpa/telnet.h>

#include "bbs.h"

const char echo_off [] = { IAC, WILL, TELOPT_ECHO, '\0' };
const char echo_on [] = { IAC, WONT, TELOPT_ECHO, '\0' };

/*
 * Command table. (name, function, level, show, valid)
 */
const struct cmd_type cmd_table [] = { // 
		{ "age", do_age, 0, 1, 1 }, //
				{ "alia", do_alia, 0, 0, 0 }, // 
				{ "alias", do_alias, 0, 1, 0 }, //
				{ "anonnote", do_anonnote, 0, 1, 0 }, //
				{ "bug", do_bug, 0, 1, 0 }, //
				{ "cls", do_cls, 0, 1, 1 }, //
				{ "clip", do_clip, 0, 1, 0 }, //
				{ "credits", do_credits, 0, 1, 0 }, //
				{ "date", do_time, 0, 1, 1 }, //
				{ "enemy", do_enemy, 0, 1, 0 }, //
				{ "finger", do_finger, 0, 1, 1 }, //
				{ "feeling", do_feeling, 0, 1, 0 }, //
				{ "fl", do_forumlist, 0, 0, 1 }, //
				{ "fl_old", do_forumlist_old, 0, 0, 1 }, //
				{ "fly", do_jump, 0, 0, 1 }, //
				{ "fnotify", do_fnotify, 0, 1, 0 }, //
				{ "forumlist", do_forumlist, 0, 1, 1 }, //
				{ "forumlist_old", do_forumlist_old, 0, 1, 1 }, //
				{ "friend", do_friend, 0, 1, 0 }, //
				{ "from", do_from, 0, 1, 0 }, //
				{ "go", do_jump, 0, 0, 1 }, //
				{ "help", do_help, 0, 1, 1 }, //
				{ "hide", do_hide, 0, 1, 0 }, //
				{ "info", do_info, 0, 1, 1 }, //
				{ "ignore", do_ignore, 0, 1, 0 }, //
				{ "idea", do_idea, 0, 1, 0 }, //
				{ "jump", do_jump, 0, 1, 1 }, //
				{ "look", do_look, 0, 1, 1 },//
				{ "list", do_list, 0, 1, 0 }, //
				{ "ln", do_listnew, 0, 1, 0 }, //
				{ "listnew", do_listnew, 0, 1, 0 }, //
				{ "mail", do_mail, 0, 1, 0 }, //
				{ "mx", do_mx, 0, 1, 0 }, //
				{ "new", do_new, 0, 1, 0 }, //
				{ "newmsgs", do_newmsgs, 0, 1, 1 }, //
				{ "newmsgs_old", do_newmsgs_old, 0, 1, 1 }, //
				{ "next", do_next, 0, 1, 0 }, //
				{ "nm", do_newmsgs, 0, 0, 1 }, //
				{ "nm_old", do_newmsgs_old, 0, 0, 1 }, //
				{ "note", do_note, 0, 1, 0 }, //
				{ "notify", do_notify, 0, 1, 0 }, //
				{ "nx", do_next, 0, 0, 0 }, //
				{ "plan", do_plan, 0, 1, 0 }, //
				{ "previous", do_previous, 0, 1, 0 }, // 
				{ "profile", do_plan, 0, 0, 0 }, //
				{ "password", do_passwd, 0, 1, 1 }, // 
				{ "reply", do_reply, 0, 1, 0 }, // 
				{ "read", do_read, 0, 1, 0 }, // do_readinterval
				{ "readinterval", do_readinterval, 0, 1, 0 }, // 
				{ "readall", do_readall, 0, 1, 0 }, //
				{ "readlast", do_readlast, 0, 1, 0 }, // 
				{ "remove", do_remove, 0, 1, 0 }, //
				{ "ri", do_readinterval, 0, 1, 0 }, // 
				{ "save", do_save, 0, 1, 1 }, // 
				{ "set", do_set, 0, 1, 0 }, // 
				{ "search", do_search, 0, 1, 0 }, // 
				{ "sendclip", do_sendclip, 0, 1, 0 }, // 
				{ "showclip", do_showclip, 0, 1, 0 }, //
				{ "skip", do_skip, 0, 1, 0 }, // 
				{ "time", do_time, 0, 1, 1 }, // 
				{ "title", do_title, 0, 1, 0 }, // 
				{ "toggle", do_toggle, 0, 1, 0 }, // 
				{ "q", do_qui, 0, 1, 1 }, // 
				{ "quit", do_quit, 0, 1, 1 }, // 
				{ "unalias", do_unalias, 0, 1, 0 }, // 
				{ "unflash", do_unflash, 0, 1, 1 }, // 
				{ "unhide", do_hide, 0, 1, 0 }, // 
				{ "uptime", do_uptime, 0, 1, 1 }, // 
				{ "version", do_version, 0, 1, 1 }, // 
				{ "who", do_who, 0, 1, 1 }, // 
				{ "Who", do_Who, 0, 1, 1 }, // 
				{ "Who_old", do_Who_old, 0, 1, 1 }, // 
				{ "whoami", do_whoami, 0, 1, 1 }, // 
				{ "x", do_x, 0, 1, 0 }, //
				{ "xers", do_xers, 0, 1, 1 }, // 
				{ "xm", do_xers, 0, 0, 1 }, // 
				{ "xn", do_xers, 0, 0, 1 }, // 
				{ "zap", do_zap, 0, 1, 0 }, //
				{ "de2tr", do_de2tr, 0, 1, 1 }, //
				{ "tr2de", do_tr2de, 0, 1, 1 }, //
				{ "en2en", do_en2en, 0, 1, 1 }, //
				{ "en2tr", do_en2tr, 0, 1, 1 }, //
				{ "tr2en", do_tr2en, 0, 1, 1 }, //
				{ "fr2tr", do_fr2tr, 0, 1, 1 }, //
				{ "tr2fr", do_tr2fr, 0, 1, 1 }, //
				{ "tr2tr", do_tr2tr, 0, 1, 1 }, //
				{ "dict", do_dict, 0, 1, 1 }, //
				{ "exchange", do_disp_exchange, 0, 1, 1 }, //
				{ "charset", do_charset, 0, 1, 1 }, //
				{ "fc", do_say, 0, 1, 1 }, //
				/*
				 * Moderator commands.
				 */
				{ "foruminfo", do_foruminfo, 1, 2, 0 }, // 
				{ "kick", do_kick, 1, 2, 0 }, //
				{ "stats", do_stats, 1, 2, 0 }, //

				/*
				 * Admin commands.
				 */
				{ "ac", do_admin, 3, 0, 0 }, // 
				{ "admin", do_admin, 3, 1, 0 }, // 
				{ "banish", do_banish, 3, 1, 0 }, //
				{ "config", do_config, 3, 1, 0 }, // 
				{ "deluser", do_deluser, 3, 1, 0 }, // 
				{ "disconnect", do_disconnect, 3, 1, 0 }, // 
				{ "echo", do_echo, 3, 1, 0 }, // 
				//{ "yalama", do_yalama, 3, 1, 0 }, // 
				{ "force", do_force, 3, 1, 0 }, // 
				{ "forumset", do_setforum, 3, 1, 0 }, // 
				{ "hostname", do_hostname, 3, 1, 0 }, // 
				{ "invis", do_invis, 3, 1, 0 }, // 
				{ "memory", do_memory, 3, 1, 0 }, // 
				{ "reboo", do_reboo, 3, 0, 0 }, //
				{ "reboot", do_reboot, 3, 1, 0 }, //
				{ "saveall", do_saveall, 3, 1, 0 }, // 
				{ "setuser", do_userset, 3, 1, 0 }, // 
				{ "shutdow", do_shutdow, 3, 0, 0 }, // 
				{ "shutdown", do_shutdown, 3, 1, 0 }, // 
				{ "sockets", do_sockets, 3, 1, 0 }, // 
				{ "snoop", do_snoop, 4, 1, 0 }, // 
				{ "unbanish", do_unbanish, 3, 1, 0 }, // 
				{ "userset", do_userset, 3, 1, 0 }, // 
				{ "validate", do_validate, 3, 1, 0 }, // 
				{ "unkick", do_unkick, 3, 1, 0 }, // 
				{ "addforum", do_addforum, 3, 1, 0 }, // 
				{ "delforum", do_delforum, 3, 1, 0 }, // 
				{ "setforum", do_setforum, 3, 1, 0 }, // 
				{ "showmods", do_showmods, 3, 1, 0 }, //
				{ "statforum", do_statforum, 3, 1, 0 }, //
				{ "banishes", do_banishes, 0, 1, 0 }, //
				{ "loadfeeling", do_loadfeeling, 3, 1, 0 }, //
				{ "loadhelps", do_loadhelp, 3, 1, 0 }, //
				{ "", 0, 0, 0, 1 } };

/*
 * The main entry point for executing commands.
 */
void process_command(USER_DATA *usr, char *argument) {
	char command[INPUT];
	int cmd;
	bool found;

	while (isspace(*argument))
		argument++;

	usr->timer = 0;
	EDIT_MODE(usr) = EDITOR_NONE;

	if (argument[0] == '\0')
		return;

	if (!isalpha(argument[0]) && !isdigit(argument[0])) {
		command[0] = argument[0];
		command[1] = '\0';
		argument++;
		while (isspace(*argument))
			argument++;
	} else {
		argument = one_argument_two(argument, command);
	}

	found = FALSE;
	for (cmd = 0; cmd_table[cmd].name[0] != '\0'; cmd++) {
		if (command[0] == cmd_table[cmd].name[0] && !str_prefix_two(command,
				cmd_table[cmd].name) && cmd_table[cmd].level <= usr->level) {
			found = TRUE;
			break;
		}
	}

	if (!found) {
		if (is_turkish(usr))
			send_to_user("Gecersiz komut.\r\n", usr);
		else
			send_to_user("Unknown command.\r\n", usr);
		return;
	}

	if (!is_moderator(usr) && cmd_table[cmd].show == 2) {
		if (is_turkish(usr))
			send_to_user("Gecersiz komut.\r\n", usr);
		else
			send_to_user("Unknown command.\r\n", usr);
		return;
	}

	if (!usr->Validated && cmd_table[cmd].valid == 0) {
		if (is_turkish(usr)) {
			send_to_user(
					"Validate edilmemis kullanicisiniz.  Bu komutu kullanamazsiniz.\r\n",
					usr);
		} else {
			send_to_user("You are not validated yet.  "
				"You cannot use that command.\r\n", usr);
		}
		return;
	}

	if (usr->lastCommand)
		free_string(usr->lastCommand);

	usr->lastCommand = str_dup(cmd_table[cmd].name);

	(*cmd_table[cmd].do_fun)(usr, argument);
	tail_chain();
	return;
}

void do_cls(USER_DATA *usr, char *argument) {
	if (IS_TOGGLE(usr, TOGGLE_ANSI)) {
		send_to_user("\033[2J", usr);
	} else {
		int i;
		for (i = 0; i < usr->desc->rows / 2; i++) {
			send_to_user("\r\n", usr);
		}
	}

	return;
}

void do_quit_org(USER_DATA *usr, char *argument, bool fXing) {
	char buf1[STRING_LENGTH];
	char buf2[STRING_LENGTH];
	USER_DATA *xing;
	int total_xers = 0;
	bool found= FALSE;
	DESC_DATA *d;

	buf1[0] = '\0';
	strcpy(buf2, "");

	for (xing = user_list; xing != NULL; xing = xing->next) {
		if (xing != NULL && !str_cmp(xing->xing_to, usr->name)) {
			found = TRUE;
			total_xers++;
			strcat(buf2, xing->name);
			strcat(buf2, " ");
		}
	}

	if (found && fXing) {
		if (is_turkish(usr))
			sprintf(buf1, "%s size x mesaji atiyor.\r\n", buf2);
		else
			sprintf(buf1, "%s%s x'ing you.\r\n", buf2, (total_xers > 1) ? "are"
					: "is");
		send_to_user(buf1, usr);
		EDIT_MODE(usr) = EDITOR_QUIT_ANSWER;
		return;
	}

	/*    if (usr->pNote)
	 free_note(usr->pNote); BAXTER */

	edit_note_free(usr);
	edit_mail_free(usr);

	if (usr->pBoard == board_lookup("chat", FALSE))
		cmd_chat_exit(usr);

	do_help(usr, "logout");
	print_to_user(usr, "\033]0;Goodbye from %s; come back soon!\007", config.bbs_name);
	sprintf(log_buf, "%s has left the bbs", usr->name);
	log_string(log_buf);
	syslog(log_buf, usr);
	info_notify(usr, FALSE);

	usr->last_logoff = current_time;
	save_user(usr);
	stop_snoop(usr);
	d = usr->desc;
	extract_user(usr);
	if (d != NULL)
		close_socket(d);
	return;
}

void do_qui(USER_DATA *usr, char *argument) {
	send_to_user("Please type #Yquit#x to exit.\r\n", usr);
}

void do_quit(USER_DATA *usr, char *argument) {
	do_quit_org(usr, argument, TRUE);
}

void do_time(USER_DATA *usr, char *argument) {
	char buf[INPUT];
	char *strtime;

	strtime = ctime(&current_time);
	strtime[strlen(strtime)-1] = '\0';

	if (is_turkish(usr))
		sprintf(buf, "Sistem tarihi (%s): %s\r\n", config.bbs_state, strtime);
	else
		sprintf(buf, "The system time is (%s): %s\r\n", config.bbs_state,
				strtime);
	send_to_user(buf, usr);
	return;
}

void do_help(USER_DATA *usr, char *argument) {
	HELP_DATA *pHelp;
	BUFFER *output;
	bool found= FALSE;

	output = new_buf();

	/*    strip_spaces(argument); OLMADI BAXTER */

	if (argument[0] == '\0')
		argument = "summary";

	for (pHelp = first_help; pHelp; pHelp = pHelp->next) {
		if (!str_cmp(argument, pHelp->keyword) && pHelp->level <= usr->level) {
			if (pHelp->text[0] == '.')
				add_buf(output, pHelp->text+1);
			else
				add_buf(output, pHelp->text);
			found = TRUE;
		}
	}

	if (!found) {
		if (is_turkish(usr))
			send_to_user("Bu kelime hakkinda bir yardim yok.\r\n", usr);
		else
			send_to_user("No help on that word.\r\n", usr);
		return;
	}

	page_to_user(buf_string(output), usr);
	free_buf(output);
	return;
}

void do_version(USER_DATA *usr, char *argument) {
	char buf[INPUT];

	sprintf(buf, "%s BBS version %s.\r\n", config.bbs_name, config.bbs_version);
	send_to_user(buf, usr);
	return;
}

void do_whoami(USER_DATA *usr, char *argument) {
	char buf[INPUT];

	if (is_turkish(usr))
		sprintf(buf, "%s.\r\n", usr->name);
	else
		sprintf(buf, "You are %s.\r\n", usr->name);
	send_to_user(buf, usr);
	return;
}

void do_save(USER_DATA *usr, char *argument) {
save_user(usr);
	if (is_turkish(usr))
		send_to_user("Kaydedildi.\r\n", usr);
	else
		send_to_user("Ok.\r\n", usr);
	return;
}

void do_charset(USER_DATA *usr, char *argument) {
  char buf[INPUT];     
	if (argument[0] == '\0') {
		if (is_turkish(usr))
			sprintf(buf, "Charset: %s#x\r\n", usr->clientCharset);
		else
			sprintf(buf, "#WYour charset is: #C%s#x\r\n", usr->clientCharset);
		send_to_user(buf, usr);
		return;
	}
	
	smash_tilde(argument);

	const char *cset = checkCharset(argument);

	if (cset)
	{
    free_string(usr->clientCharset);
  	usr->clientCharset = str_dup(cset);
  	if (is_turkish(usr))
  		sprintf(buf, "Yeni karakter setiniz: %s#x\r\n", usr->clientCharset);
  	else
  		sprintf(buf, "#WYour CharSet is now: #C%s#x\r\n", usr->clientCharset);
  }
  else
  {
    sprintf(buf,"#RThat's not a valid character set!#x\r\n");
  }
	send_to_user(buf, usr);
}

void do_set(USER_DATA *usr, char *argument) {
	char buf[STRING_LENGTH];
	char arg[INPUT_LENGTH];
	char *host;

	argument = one_argument(argument, arg);

	if (arg[0] == '\0') {
		if (is_turkish(usr))
			sprintf(buf, "#cGercek isim:       #C: #W%s\r\n"
				"#CTitle              #C: #W%s#x\r\n"
				"#cE-mail             #C: #W%s#x\r\n"
				"#CAlt#cernatif E-mail  #C: #W%s#x\r\n"
				"#cHomepage #CUrl       #C: #W%s#x\r\n"
				"#CICQ #cNumarasi       #C: #W%s#x\r\n"
				"#CIdle #cMesaji        #C: #W%s#x\r\n"
        "#CCharser            #C: #W%s#x\r\n", usr->real_name,
					usr->title, usr->email, usr->alt_email, usr->home_url,
					usr->icquin, usr->idlemsg, usr->clientCharset);
		else
			sprintf(buf, "#cReal name          #C: #W%s#x\r\n"
				"#CTitle              #C: #W%s#x\r\n"
				"#cE-mail             #C: #W%s#x\r\n"
				"#CAlt#cernative E-mail #C: #W%s#x\r\n"
				"#cHomepage #CUrl       #C: #W%s#x\r\n"
				"#CICQ #cNumber         #C: #W%s#x\r\n"
				"#CIdle #cMessage       #C: #W%s#x\r\n"
        "#CCharSet            #C: #W%s#x\r\n", usr->real_name,
					usr->title, usr->email, usr->alt_email, usr->home_url,
					usr->icquin, usr->idlemsg, usr->clientCharset);
		send_to_user(buf, usr);
		return;
	}

	if (!str_cmp(arg, "alt")) {
		if (argument[0] == '\0') {
			if (is_turkish(usr))
				send_to_user("Bilgi vermelisiniz.\r\n", usr);
			else
				send_to_user("You must give an argument.\r\n", usr);
			return;
		}

		if (!str_cmp(argument, "NONE")) {
			free_string(usr->alt_email);
			usr->alt_email = str_dup("NONE");
			if (is_turkish(usr))
				send_to_user("Tamam.\r\n", usr);
			else
				send_to_user("Ok.\r\n", usr);
			return;
		}

		if (!(host = strchr(argument, '@'))) {
			if (is_turkish(usr))
				send_to_user("Gecersiz e-mail adresi.\r\n", usr);
			else
				send_to_user("That's not a valid e-mail.\r\n", usr);
			return;
		}

		if (strlen(argument) > 50) {
			if (is_turkish(usr))
				send_to_user("Degisken cok uzun.\r\n", usr);
			else
				send_to_user("Argument is too long.\r\n", usr);
			return;
		}

		smash_tilde(argument);
		free_string(usr->alt_email);
		usr->alt_email = str_dup(argument);
		if (is_turkish(usr))
			send_to_user("Tamam.\r\n", usr);
		else
			send_to_user("Ok.\r\n", usr);
		return;
	} else if (!str_cmp(arg, "title")) {
		if (argument[0] == '\0') {
			if (is_turkish(usr))
				send_to_user("Bilgi vermelisiniz.\r\n", usr);
			else
				send_to_user("You must give an argument.\r\n", usr);
			return;
		}

		do_title(usr, argument);
		return;
	} else if (!str_cmp(arg, "charset")) {
		if (argument[0] == '\0') {
			if (is_turkish(usr))
				send_to_user("Bilgi vermelisiniz.\r\n", usr);
			else
				send_to_user("You must give an argument.\r\n", usr);
			return;
		}

		do_charset(usr, argument);
		return;
	} else if (!str_cmp(arg, "url")) {
		if (argument[0] == '\0') {
			if (is_turkish(usr))
				send_to_user("Bilgi vermelisiniz.\r\n", usr);
			else
				send_to_user("You must give an argument.\r\n", usr);
			return;
		}

		if (!str_cmp(argument, "NONE")) {
			free_string(usr->home_url);
			usr->home_url = str_dup("NONE");
			if (is_turkish(usr))
				send_to_user("Tamam.\r\n", usr);
			else
				send_to_user("Ok.\r\n", usr);
			return;
		}

		if (strlen(argument) > 100) {
			if (is_turkish(usr))
				send_to_user("Degisken cok uzun.\r\n", usr);
			else
				send_to_user("Argument is too long.\r\n", usr);
			return;
		}

		smash_tilde(argument);
		free_string(usr->home_url);
		usr->home_url = str_dup(argument);
		if (is_turkish(usr))
			send_to_user("Tamam.\r\n", usr);
		else
			send_to_user("Ok.\r\n", usr);
		return;
	} else if (!str_cmp(arg, "icq")) {
		if (argument[0] == '\0') {
			if (is_turkish(usr))
				send_to_user("Bilgi vermelisiniz.\r\n", usr);
			else
				send_to_user("You must give an argument.\r\n", usr);
			return;
		}

		if (!str_cmp(argument, "NONE")) {
			free_string(usr->icquin);
			usr->icquin = str_dup("NONE");
			if (is_turkish(usr))
				send_to_user("Tamam.\r\n", usr);
			else
				send_to_user("Ok.\r\n", usr);
			return;
		}

		if (!is_number(argument)) {
			if (is_turkish(usr))
				send_to_user("Gecersiz ICQ numarasi.\r\n", usr);
			else
				send_to_user("That's not a valid ICQ number.\r\n", usr);
			return;
		}

		if (atoi(argument) > 99999999 || atoi(argument) < 10000) {
			if (is_turkish(usr))
				send_to_user("Gecersiz ICQ numarasi.\r\n", usr);
			else
				send_to_user("That's not a valid ICQ number.\r\n", usr);
			return;
		}

		smash_tilde(argument);
		free_string(usr->icquin);
		usr->icquin = str_dup(argument);
		if (is_turkish(usr))
			send_to_user("Tamam.\r\n", usr);
		else
			send_to_user("Ok.\r\n", usr);
		return;
	} else if (!str_cmp(arg, "idle")) {
		if (argument[0] == '\0') {
			if (is_turkish(usr))
				send_to_user("Bilgi vermelisiniz.\r\n", usr);
			else
				send_to_user("You must give an argument.\r\n", usr);
			return;
		}

		if (!str_cmp(argument, "NONE")) {
			free_string(usr->idlemsg);
			usr->idlemsg = str_dup("NONE");
			if (is_turkish(usr))
				send_to_user("Tamam.\r\n", usr);
			else
				send_to_user("Ok.\r\n", usr);
			return;
		}

		if (strlen(argument) > 75) {
			if (is_turkish(usr))
				send_to_user("Degisken cok uzun.\r\n", usr);
			else
				send_to_user("Argument is too long.\r\n", usr);
			return;
		}

		smash_tilde(argument);
		free_string(usr->idlemsg);
		usr->idlemsg = str_dup(argument);
		if (is_turkish(usr))
			send_to_user("Tamam.\r\n", usr);
		else
			send_to_user("Ok.\r\n", usr);
		return;
	} else {
		if (is_turkish(usr)) {
			send_to_user("Gecersiz set degiskeni.\r\n", usr);
			send_to_user(
					"Gecerli degiskenler: title, alt, url, icq, idle.\r\n", usr);
		} else {
			send_to_user("That's not a valid set option.\r\n", usr);
			send_to_user("Valid options are: title, alt, url, icq, idle.\r\n",
					usr);
		}
		return;
	}
}

void do_title(USER_DATA *usr, char *argument) {
	char buf[INPUT];

	if (argument[0] == '\0') {
		if (is_turkish(usr))
			sprintf(buf, "Basliginiz: %s#x\r\n", usr->title);
		else
			sprintf(buf, "Your title is: %s#x\r\n", usr->title);
		send_to_user(buf, usr);
		return;
	}

	if (strlen_color(argument) > 52) {
		if (is_turkish(usr))
			send_to_user("Basliginiz cok uzun.\r\n", usr);
		else
			send_to_user("Title is too long.\r\n", usr);
		return;
	}

	smash_tilde(argument);
	free_string(usr->title);
	usr->title = str_dup(argument);
	if (is_turkish(usr))
		sprintf(buf, "Yeni basliginiz: %s#x\r\n", usr->title);
	else
		sprintf(buf, "Your title is now: %s#x\r\n", usr->title);
	send_to_user(buf, usr);
	return;
}
/*
extras.cpp içine taþýndý..
**************************
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
		sprintf(buf_time, "#WKullanici       Zaman  Baslik\r\n");
	else
		sprintf(buf_time, "#WNickname        Time   Title\r\n");
	add_buf(output, buf_time);
	add_buf(
			output,
			"#W=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=#x\r\n");

	for (d = desc_list; d != NULL; d = d->next) {
		if ((d->login != CON_LOGIN) || !USR(d) || (IS_TOGGLE(USR(d), TOGGLE_INVIS) && !IS_ADMIN(usr)) || (!USR(d)->Validated && !IS_ADMIN(usr)))
			continue;

		if ((bFriend && !is_friend(usr, USR(d)->name)) || (bEnemy && !is_enemy(usr, USR(d)->name)) || (bNotify && !is_notify(usr, USR(d)->name)))
			continue;

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
*/

void do_toggle(USER_DATA *usr, char *argument) {
	char buf[STRING];
	char arg[INPUT];

	one_argument(argument, arg);

	if (arg[0] == '\0') {
		if (is_turkish(usr))
			sprintf(buf, "#CX #cmesaji        #C: %s\r\n"
				"#CBeep            #C: %s\r\n"
				"#CRenk #cdestegi    #C: %s\r\n"
				"#CIdle #cmodu       #C: %s\r\n"
				"#CNotify          #C: %s\r\n"
				"#CAuto #cXers       #C: %s\r\n"
				"#CFeeling         #C: %s\r\n"
				"#CMore #cmesaji     #C: %s\r\n"
				"#cAuto#Cjump        #C: %s\r\n" 
				"#cSohbet        #C: %s\r\n" 
				"#cYardim        #C: %s\r\n", 
			IS_TOGGLE(usr, TOGGLE_XING) ? "#WAcik" : "#RKapali", 
			IS_TOGGLE(usr, TOGGLE_BEEP) ? "#WAcik" : "#RKapali", 
			IS_TOGGLE(usr, TOGGLE_ANSI) ? "#WAcik" : "#RKapali", 
			IS_TOGGLE(usr, TOGGLE_IDLE) ? "#WAcik" : "#RKapali", 
			IS_TOGGLE(usr, TOGGLE_INFO) ? "#WAcik" : "#RKapali", 
			IS_TOGGLE(usr, TOGGLE_WARN) ? "#WAcik" : "#RKapali", 
			IS_TOGGLE(usr, TOGGLE_FEEL) ? "#WAcik" : "#RKapali", 
			!IS_TOGGLE(usr, TOGGLE_MORE) ? "#WAcik" : "#RKapali", 
			IS_TOGGLE(usr, TOGGLE_AUTOJUMP) ? "#WAcik" : "#RKapali",
			IS_TOGGLE(usr, TOGGLE_SAY) ? "#WAcik" : "#RKapali",
			IS_TOGGLE(usr, TOGGLE_HELP) ? "#WAcik" : "#RKapali");
		else
			sprintf(buf, "#cMessage e#CX#cpress #C: %s\r\n"
				"#CBeep            #C: %s\r\n"
				"#CAnsi #ccolor      #C: %s\r\n"
				"#CIdle #cmode       #C: %s\r\n"
				"#CNotify          #C: %s\r\n"
				"#CAuto #cXers       #C: %s\r\n"
				"#CFeeling         #C: %s\r\n"
				"#CMore #cmessage    #C: %s\r\n"
				"#cAuto#Cjump        #C: %s\r\n" 
				"#cForum #CChat      #C: %s\r\n" 
				"#CHelp            #C: %s\r\n", 
			IS_TOGGLE(usr, TOGGLE_XING) ? "#WOn" : "#ROff", 
			IS_TOGGLE(usr, TOGGLE_BEEP) ? "#WOn" : "#ROff", 
			IS_TOGGLE(usr, TOGGLE_ANSI) ? "#WOn" : "#ROff", 
			IS_TOGGLE(usr, TOGGLE_IDLE) ? "#WOn" : "#ROff", 
			IS_TOGGLE(usr, TOGGLE_INFO) ? "#WOn" : "#ROff", 
			IS_TOGGLE(usr, TOGGLE_WARN) ? "#WOn" : "#ROff", 
			IS_TOGGLE(usr, TOGGLE_FEEL) ? "#WOn" : "#ROff", 
			!IS_TOGGLE(usr, TOGGLE_MORE) ? "#WOn" : "#ROff", 
			IS_TOGGLE(usr, TOGGLE_AUTOJUMP) ? "#WOn" : "#ROff",
			IS_TOGGLE(usr, TOGGLE_SAY) ? "#WOn" : "#ROff",
			IS_TOGGLE(usr, TOGGLE_HELP) ? "#WOn" : "#ROff");
		send_to_user(buf, usr);
		if (IS_ADMIN(usr)) {
			sprintf(buf, "#CAdm#cin channel   #C: %s#x\r\n", 
			IS_TOGGLE(usr, TOGGLE_ADM) ? "#WOn" : "#ROff");
			send_to_user(buf, usr);
		}
		return;
	} else if (!str_cmp(arg, "x")) {
		if (IS_TOGGLE(usr, TOGGLE_XING)) {
			if (is_turkish(usr))
				send_to_user("X mesaji degiskeni Kapatildi.\r\n", usr);
			else
				send_to_user("You toggle message eXpress Off.\r\n", usr);
			REM_TOGGLE(usr, TOGGLE_XING);
		} else {
			if (is_turkish(usr))
				send_to_user("X mesaji degiskeni Acildi.\r\n", usr);
			else
				send_to_user("You toggle message eXpress On.\r\n", usr);
			SET_TOGGLE(usr, TOGGLE_XING);
		}
		return;
	} else if (!str_cmp(arg, "beep")) {
		if (IS_TOGGLE(usr, TOGGLE_BEEP)) {
			if (is_turkish(usr))
				send_to_user("Beep degiskeni Kapatildi.\r\n", usr);
			else
				send_to_user("You toggle beep Off.\r\n", usr);
			REM_TOGGLE(usr, TOGGLE_BEEP);
		} else {
			if (is_turkish(usr))
				send_to_user("Beep degiskeni Acildi.\r\n", usr);
			else
				send_to_user("You toggle beep On.\r\n", usr);
			SET_TOGGLE(usr, TOGGLE_BEEP);
		}
		return;
	} else if (!str_cmp(arg, "more")) {
		if (!IS_TOGGLE(usr, TOGGLE_MORE)) {
			if (is_turkish(usr))
				send_to_user("More degiskeni Kapatildi.\r\n", usr);
			else
				send_to_user("You toggle \"More\" Off.\r\n", usr);
			SET_TOGGLE(usr, TOGGLE_MORE);
		} else {
			if (is_turkish(usr))
				send_to_user("More degiskeni Acildi.\r\n", usr);
			else
				send_to_user("You toggle \"More\" On.\r\n", usr);
			REM_TOGGLE(usr, TOGGLE_MORE);
		}
		return;
	} else if ((!str_cmp(arg, "renk") && is_turkish(usr)) || !str_cmp(arg,
			"ansi")) {
		if (IS_TOGGLE(usr, TOGGLE_ANSI)) {
			if (is_turkish(usr))
				send_to_user("Renk destegi degiskeni Kapatildi.\r\n", usr);
			else
				send_to_user("You toggle ansi color Off.\r\n", usr);
			REM_TOGGLE(usr, TOGGLE_ANSI);
		} else {
			if (is_turkish(usr))
				send_to_user("Renk destegi degiskeni Acildi.\r\n", usr);
			else
				send_to_user("You toggle ansi color On.\r\n", usr);
			SET_TOGGLE(usr, TOGGLE_ANSI);
		}
		return;
	} else if (!str_cmp(arg, "idle")) {
		if (IS_TOGGLE(usr, TOGGLE_IDLE)) {
			if (is_turkish(usr))
				send_to_user("Idle modu degiskeni Kapatildi.\r\n", usr);
			else
				send_to_user("You toggle idle mode Off.\r\n", usr);
			REM_TOGGLE(usr, TOGGLE_IDLE);
		} else {
			if (is_turkish(usr))
				send_to_user("Idle modu degiskeni Acildi.\r\n", usr);
			else
				send_to_user("You toggle idle mode On.\r\n", usr);
			SET_TOGGLE(usr, TOGGLE_IDLE);
		}
		return;
	} else if (!str_cmp(arg, "notify")) {
		if (IS_TOGGLE(usr, TOGGLE_INFO)) {
			if (is_turkish(usr))
				send_to_user("Notify degiskeni Kapatildi.\r\n", usr);
			else
				send_to_user("You toggle notify Off.\r\n", usr);
			REM_TOGGLE(usr, TOGGLE_INFO);
		} else {
			if (is_turkish(usr))
				send_to_user("Notify degiskeni Acildi.\r\n", usr);
			else
				send_to_user("You toggle notify On.\r\n", usr);
			SET_TOGGLE(usr, TOGGLE_INFO);
		}
		return;
	} else if (!str_cmp(arg, "auto")) {
		if (IS_TOGGLE(usr, TOGGLE_WARN)) {
			if (is_turkish(usr))
				send_to_user("Auto xers degiskeni Kapatildi.\r\n", usr);
			else
				send_to_user("You toggle auto xers Off.\r\n", usr);
			REM_TOGGLE(usr, TOGGLE_WARN);
		} else {
			if (is_turkish(usr))
				send_to_user("Auto xers degiskeni Acildi.\r\n", usr);
			else
				send_to_user("You toggle auto xers On.\r\n", usr);
			SET_TOGGLE(usr, TOGGLE_WARN);
		}
		return;
	} else if (!str_cmp(arg, "feeling")) {
		if (IS_TOGGLE(usr, TOGGLE_FEEL)) {
			if (is_turkish(usr))
				send_to_user("Feeling degiskeni Kapatildi.\r\n", usr);
			else
				send_to_user("You toggle feeling Off.\r\n", usr);
			REM_TOGGLE(usr, TOGGLE_FEEL);
		} else {
			if (is_turkish(usr))
				send_to_user("Feeling degiskeni Acildi.\r\n", usr);
			else
				send_to_user("You toggle feeling On.\r\n", usr);
			SET_TOGGLE(usr, TOGGLE_FEEL);
		}
		return;
	} else if (!str_cmp(arg, "jump")) {
		if (IS_TOGGLE(usr, TOGGLE_AUTOJUMP)) {
			if (is_turkish(usr))
				send_to_user("Autojump degiskeni Kapatildi.\r\n", usr);
			else
				send_to_user("You toggle autojump Off.\r\n", usr);
			REM_TOGGLE(usr, TOGGLE_AUTOJUMP);
		} else {
			if (is_turkish(usr))
				send_to_user("Autojump degiskeni Acildi.\r\n", usr);
			else
				send_to_user("You toggle autojump On.\r\n", usr);
			SET_TOGGLE(usr, TOGGLE_AUTOJUMP);
		}
		return;
	} else if (!str_cmp(arg, "help")) {
		if (IS_TOGGLE(usr, TOGGLE_HELP)) {
			if (is_turkish(usr))
				send_to_user("Yardim Kapatildi.\r\n", usr);
			else
				send_to_user("You toggle help Off.\r\n", usr);
			REM_TOGGLE(usr, TOGGLE_HELP);
		} else {
			if (is_turkish(usr))
				send_to_user("Yardim Acildi.\r\n", usr);
			else
				send_to_user("You toggle help On.\r\n", usr);
			SET_TOGGLE(usr, TOGGLE_HELP);
		}
		return;
	} else if (!str_cmp(arg, "adm") && IS_ADMIN(usr)) {
		if (IS_TOGGLE(usr, TOGGLE_ADM)) {
			send_to_user("You toggle admin channel Off.\r\n", usr);
			REM_TOGGLE(usr, TOGGLE_ADM);
		} else {
			send_to_user("You toggle admin channel On.\r\n", usr);
			SET_TOGGLE(usr, TOGGLE_ADM);
		}
		return;
	} else if (!str_cmp(arg, "chat") ) {
		if (IS_TOGGLE(usr, TOGGLE_SAY)) {
			send_to_user("You toggle forum chat Off.\r\n", usr);
			REM_TOGGLE(usr, TOGGLE_SAY);
		} else {
			send_to_user("You toggle forum chat On.\r\n", usr);
			SET_TOGGLE(usr, TOGGLE_SAY);
		}
		return;
	} else {
		if (is_turkish(usr)) {
			send_to_user("Gecersiz toggle degiskeni.\r\n", usr);
			send_to_user(
					"Gecerli degiskenler: x, beep, renk, idle, notify, auto, feeling, jump.\r\n",
					usr);
		} else {
			send_to_user("That's not a valid toggle option.\r\n", usr);
			send_to_user(
					"Valid options are: x, beep, ansi, idle, notify, auto, feeling, jump.\r\n",
					usr);
		}
		return;
	}
}

void do_hide(USER_DATA *usr, char *argument) {
	char arg[INPUT];

	one_argument(argument, arg);

	if (arg[0] == '\0') {
		if (is_turkish(usr))
			print_to_user(usr, "#cGercek #Cisim        #C: %s\r\n"
				"#CE-mail             #C: %s\r\n"
				"#CAlt#cernatif E-mail  #C: %s\r\n"
				"#CICQ #cNumarasi       #C: %s\r\n"
				"#cHomepage #CUrl       #C: %s#x\r\n", 
			IS_HIDE(usr, HIDE_REALNAME) ? "#WGizli" : "#RGizli Degil", 
			IS_HIDE(usr, HIDE_EMAIL) ? "#WGizli" : "#RGizli Degil", 
			IS_HIDE(usr, HIDE_ALTEMAIL) ? "#WGizli" : "#RGizli Degil", 
			IS_HIDE(usr, HIDE_ICQ) ? "#WGizli" : "#RGizli Degil", 
			IS_HIDE(usr, HIDE_HOMEPAGE) ? "#WGizli" : "#RGizli Degil");
		else
			print_to_user(usr, "#CReal #cname          #C: %s\r\n"
				"#CE-mail             #C: %s\r\n"
				"#CAlt#cernative E-mail #C: %s\r\n"
				"#CICQ #cNumber         #C: %s\r\n"
				"#cHomepage #CUrl       #C: %s#x\r\n", 
			IS_HIDE(usr, HIDE_REALNAME) ? "#WHidden" : "#RNot Hidden", 
			IS_HIDE(usr, HIDE_EMAIL) ? "#WHidden" : "#RNot Hidden", 
			IS_HIDE(usr, HIDE_ALTEMAIL) ? "#WHidden" : "#RNot Hidden", 
			IS_HIDE(usr, HIDE_ICQ) ? "#WHidden" : "#RNot Hidden", 
			IS_HIDE(usr, HIDE_HOMEPAGE) ? "#WHidden" : "#RNot Hidden");
		return;
	} else if ((!str_cmp(arg, "isim") && is_turkish(usr)) || !str_cmp(arg,
			"real")) {
		if (IS_HIDE(usr, HIDE_REALNAME)) {
			if (is_turkish(usr))
				send_to_user("Gercek isminizin gizliligi kaldirildi.\r\n", usr);
			else
				send_to_user("You unhide your real name.\r\n", usr);
			REM_HIDE(usr, HIDE_REALNAME);
		} else {
			if (is_turkish(usr))
				send_to_user("Gercek isminiz gizlendi.\r\n", usr);
			else
				send_to_user("You hide your real name.\r\n", usr);
			SET_HIDE(usr, HIDE_REALNAME);
		}
		return;
	} else if (!str_cmp(arg, "e-mail")) {
		if (IS_HIDE(usr, HIDE_EMAIL)) {
			if (is_turkish(usr))
				send_to_user("E-mailinizin gizliligi kaldirildi.\r\n", usr);
			else
				send_to_user("You unhide your e-mail.\r\n", usr);
			REM_HIDE(usr, HIDE_EMAIL);
		} else {
			if (is_turkish(usr))
				send_to_user("E-mailiniz gizlendi.\r\n", usr);
			else
				send_to_user("You hide your e-mail.\r\n", usr);
			SET_HIDE(usr, HIDE_EMAIL);
		}
		return;
	} else if (!str_cmp(arg, "alt")) {
		if (IS_HIDE(usr, HIDE_ALTEMAIL)) {
			if (is_turkish(usr))
				send_to_user(
						"Alternatif e-mailinizin gizliligi kaldirildi.\r\n",
						usr);
			else
				send_to_user("You unhide your alternative e-mail.\r\n", usr);
			REM_HIDE(usr, HIDE_ALTEMAIL);
		} else {
			if (is_turkish(usr))
				send_to_user("Alternatif e-mailiniz gizlendi.\r\n", usr);
			else
				send_to_user("You hide your alternative e-mail.\r\n", usr);
			SET_HIDE(usr, HIDE_ALTEMAIL);
		}
		return;
	} else if (!str_cmp(arg, "icq")) {
		if (IS_HIDE(usr, HIDE_ICQ)) {
			if (is_turkish(usr))
				send_to_user("ICQ numaranizin gizliligi kaldirildi.\r\n", usr);
			else
				send_to_user("You unhide your ICQ number.\r\n", usr);
			REM_HIDE(usr, HIDE_ICQ);
		} else {
			if (is_turkish(usr))
				send_to_user("ICQ numaraniz gizlendi.\r\n", usr);
			else
				send_to_user("You hide your ICQ number.\r\n", usr);
			SET_HIDE(usr, HIDE_ICQ);
		}
		return;
	} else if (!str_cmp(arg, "url")) {
		if (IS_HIDE(usr, HIDE_HOMEPAGE)) {
			if (is_turkish(usr))
				send_to_user("Homepage urlnizin gizliligi kaldirildi.\r\n", usr);
			else
				send_to_user("You unhide your homepage url.\r\n", usr);
			REM_HIDE(usr, HIDE_HOMEPAGE);
		} else {
			if (is_turkish(usr))
				send_to_user("Homepage urlniz gizlendi.\r\n", usr);
			else
				send_to_user("You hide your homepage url.\r\n", usr);
			SET_HIDE(usr, HIDE_HOMEPAGE);
		}
		return;
	} else {
		if (is_turkish(usr)) {
			print_to_user(usr, "Gecersiz %s degiskeni.\r\n", !str_cmp(
					usr->lastCommand, "hide") ? "hide" : "unhide");
			send_to_user(
					"Gecerli degiskenler: isim, e-mail, alt, icq, url.\r\n",
					usr);
		} else {
			print_to_user(usr, "That's not a valid %s option.\r\n", !str_cmp(
					usr->lastCommand, "hide") ? "hide" : "unhide");
			send_to_user("Valid options are: real, e-mail, alt, icq, url.\r\n",
					usr);
		}
		return;
	}
}

char * get_age(int second, bool fSec) {
	char buf[STRING];
	char mbuf[100], mbuf2[100], wbuf[100];
	char dbuf[100], hbuf[100];
	int month, week, day, hour, minute = 0;

	month = second / 2419200;
	second -= month * 2419200;
	week = second / 604800;
	second -= week * 604800;
	day = second / 86400;
	second -= day * 86400;
	hour = second / 3600;
	second -= hour * 3600;
	minute = second / 60;
	second -= minute * 60;

	buf[0] = '\0';

	sprintf(buf, "*error*");
	if (month > 0) {
		sprintf(mbuf, "%d month%s", month, month > 1 ? "s " : " ");
	}
	if (week > 0) {
		sprintf(wbuf, "%d week%s", week, week > 1 ? "s " : " ");
	}
	if (day > 0) {
		sprintf(dbuf, "%d day%s", day, day > 1 ? "s " : " ");
	}
	if (hour > 0) {
		sprintf(hbuf, "%d hour%s", hour, hour > 1 ? "s " : " ");
	}
	if (mbuf2 > 0) {
		sprintf(mbuf2, "%d minute%s", minute, minute > 1 ? "s " : " ");
	}

	if (fSec) {
		sprintf(buf, "%s%s%s%s%s, %d second%s", (month > 0) ? mbuf : "", (week
				> 0 ) ? wbuf : "", (day > 0 ) ? dbuf : "", (hour > 0 ) ? hbuf
				: "", (minute > 0) ? mbuf2 : "", second, second > 1 ? "s "
				: " ");
	} else {
		sprintf(buf, "%s%s%s%s%s", (month > 0) ? mbuf : "", (week > 0 ) ? wbuf
				: "", (day > 0 ) ? dbuf : "", (hour > 0 ) ? hbuf : "", (minute
				> 0) ? mbuf2 : "");
	}
	return str_dup(buf);
}

void do_age(USER_DATA *usr, char *argument) {
	char buf[STRING];

	int second = (usr->used + (int) (current_time - usr->age));

	if (is_turkish(usr))
		sprintf(buf, "Kullanim sureniz: %s\r\n", get_age(second, FALSE));
	else
		sprintf(buf, "Your age is: %s\r\n", get_age(second, FALSE));
	send_to_user(buf, usr);
	return;
}

void do_unflash(USER_DATA *usr, char *argument) {
	send_to_user("#x\033[0;0;37m\033(B\033[r\033[H\033[J", usr);
	do_cls(usr, "");
	return;
}

void do_plan(USER_DATA *usr, char *argument) {
	char buf[STRING];

	if (strlen(usr->plan) > 0) {
		if (is_turkish(usr))
			sprintf(buf, "Su anki planiniz:\r\n%s\r\n", usr->plan);
		else
			sprintf(buf, "Your currently profile is:\r\n%s\r\n", usr->plan);
		send_to_user(buf, usr);
		EDIT_MODE(usr) = EDITOR_PLAN_ANSWER;
		return;
	} else {
		if (is_turkish(usr))
			send_to_user(
					"Planiniz icin 10 satir ayrilmistir. Kaydetmek icin '**',\r\niptal etmek icin '~q' yaziniz.\r\n",
					usr);
		else
			send_to_user(
					"You have max 10 lines for your profile.  Press ** to stop,\r\npress '~q' to abort.\r\n",
					usr);
		EDIT_MODE(usr) = EDITOR_PLAN;
		string_edit(usr, &usr->plan);
		return;
	}
}

void edit_plan_answer(USER_DATA *usr, char *argument) {
	while (isspace(*argument))
		argument++;

	usr->timer = 0;

	switch (*argument) {
		case 'y':
		case 'Y':
			if (usr->old_plan)
				free_string(usr->old_plan);
			usr->old_plan = usr->plan ? str_dup(usr->plan) : str_dup("*ERROR*");
			if (is_turkish(usr))
				send_to_user(
						"Planiniz icin 10 satir ayrilmistir. Kaydetmek icin '**',\r\niptal etmek icin '~q' yaziniz.\r\n",
						usr);
			else
				send_to_user(
						"You have max 10 lines for your profile.  Press ** to stop,\r\npress '~q' to abort.\r\n",
						usr);
			EDIT_MODE(usr) = EDITOR_PLAN;
			string_edit(usr, &usr->plan);
			break;

		default:
			if (is_turkish(usr))
				send_to_user("Iptal edildi.\r\n", usr);
			else
				send_to_user("Aborted.\r\n", usr);
			EDIT_MODE(usr) = EDITOR_NONE;
			break;
	}
}

void edit_quit_answer(USER_DATA *usr, char *argument) {
	while (isspace(*argument))
		argument++;

	usr->timer = 0;

	switch (*argument) {
		case 'y':
		case 'Y':
			do_quit_org(usr, "", FALSE);
			break;

		default:
			EDIT_MODE(usr) = EDITOR_NONE;
			break;
	}
}

void do_passwd(USER_DATA *usr, char *argument) {
	write_to_buffer(usr->desc, echo_off, 0);
	EDIT_MODE(usr) = EDITOR_PWD_OLD;
	return;
}

void edit_pwd_old(USER_DATA *usr, char *argument) {
	while (isspace(*argument))
		argument++;

	usr->timer = 0;

	if (argument[0] == '\0') {
		if (usr->password[0] == '\0') {
			usr->password = str_dup("NONE");
			EDIT_MODE(usr) = EDITOR_PWD_NEW_ONE;
			return;
		}

		write_to_buffer(usr->desc, echo_on, 0);
		write_to_buffer(usr->desc, "\r\n", 0);
		EDIT_MODE(usr) = EDITOR_NONE;
		return;
	}

	if (strcmp(crypt(argument, usr->password), usr->password)) {
		write_to_buffer(usr->desc, echo_on, 0);
		if (is_turkish(usr))
			write_to_buffer(usr->desc, "\r\nYanlis sifre.\r\n", 0);
		else
			write_to_buffer(usr->desc, "\r\nWrong password.\r\n", 0);
		EDIT_MODE(usr) = EDITOR_NONE;
		return;
	}
	EDIT_MODE(usr) = EDITOR_PWD_NEW_ONE;
	return;
}

void edit_pwd_new_one(USER_DATA *usr, char *argument) {
	char *password_new;
	char *password;

	while (isspace(*argument))
		argument++;

	usr->timer = 0;

	if (strlen(argument) < 5) {
		if (is_turkish(usr))
			write_to_buffer(usr->desc,
					"\r\nSifre 5 karakterfen fazla olmali.\r\n", 0);
		else
			write_to_buffer(usr->desc,
					"\r\nPassword must be at least 5 characters long.\r\n", 0);
		return;
	} else if (strlen(argument) > 12) {
		if (is_turkish(usr))
			write_to_buffer(usr->desc,
					"\r\nSifre maksimum 12 karakter olmali.\r\n", 0);
		else
			write_to_buffer(usr->desc,
					"\r\nPassword must be maximum 12 characters.\r\n", 0);
		return;
	}

	password_new = crypt(argument, usr->name);
	for (password = password_new; *password != '\0'; password++) {
		if (*password == '~') {
			if (is_turkish(usr))
				write_to_buffer(usr->desc, "\r\nSifre gecersiz.\r\n", 0);
			else
				write_to_buffer(usr->desc, "\r\nPassword not acceptable.\r\n",
						0);
			return;
		}
	}

	free_string(usr->password);
	usr->password = str_dup(password_new);
	EDIT_MODE(usr) = EDITOR_PWD_NEW_TWO;
	return;
}

void edit_pwd_new_two(USER_DATA *usr, char *argument) {
	while (isspace(*argument))
		argument++;

	usr->timer = 0;

	if (strcmp(crypt(argument, usr->password), usr->password)) {
		if (is_turkish(usr))
			write_to_buffer(usr->desc, "\r\nSifreler ayni degil.\r\n", 0);
		else
			write_to_buffer(usr->desc, "\r\nPasswords don't match.\r\n", 0);
		EDIT_MODE(usr) = EDITOR_PWD_NEW_ONE;
		return;
	}

	write_to_buffer(usr->desc, echo_on, 0);
	if (is_turkish(usr))
		write_to_buffer(usr->desc, "\r\nTamam.\r\n", 0);
	else
		write_to_buffer(usr->desc, "\r\nOk.\r\n", 0);
	EDIT_MODE(usr) = EDITOR_NONE;
	return;
}

void do_uptime(USER_DATA *usr, char *argument) {
	extern char boot_time[STRING_LENGTH];
	extern time_t boot_time_t;
	char buf[INPUT];
	int uptime;

	if (is_turkish(usr))
		sprintf(buf, "%s BBS'in calismaya basladigi tarih: %s",
				config.bbs_name, boot_time);
	else
		sprintf(buf, "%s BBS started up at %s", config.bbs_name, boot_time);
	send_to_user(buf, usr);
	uptime = (int) current_time - boot_time_t;
	if (is_turkish(usr))
		sprintf(buf, "\r%s BBS'in calisma suresi: %s.\r\n", config.bbs_name,
				get_age(uptime, FALSE));
	else
		sprintf(buf, "\r%s BBS has passed %s online.\r\n", config.bbs_name,
				get_age(uptime, FALSE));
	send_to_user(buf, usr);
	return;
} 

void do_say(USER_DATA * usr, char *argument)
{
    char buf[STRING];
    DESC_DATA *d;

    if (!usr)
	return;

    if (!usr->pBoard)
	return;

    if (argument[0] == '\0') {
	syntax("say [message]", usr);
	return;
    }

    if (!IS_TOGGLE(usr, TOGGLE_SAY)) { 
	syntax ("You did not toggle chat yet",usr);
	return;
    }
    if (IS_TOGGLE(usr, TOGGLE_IDLE)) {
	syntax("You are idle. Idle people should not chat!", usr);
	return;
    }

    sprintf(buf, "[#Y%s#x][#C%s#x] %s#x\r\n", usr->pBoard->long_name, usr->name, argument);

    for (d = desc_list; d; d = d->next) {
	if (USR(d) && d->login == CON_LOGIN && USR(d)->Validated) {
	    if (USR(d)->pBoard != usr->pBoard) {
		continue;
	    }
            if (!IS_TOGGLE(USR(d), TOGGLE_SAY) || IS_TOGGLE(USR(d), TOGGLE_IDLE)  ) {
		continue;
	    }
	    if (is_ignore(USR(d), usr->name, TRUE)) {
		continue;
	    }
            if (is_ignore(usr, USR(d)->name, TRUE)) {
                continue;
            }

	    if (isBusySelf(USR(d)))
		add_buffer(USR(d), buf);
	    else
		send_to_user(buf, USR(d));
	}
    }

    return;
}
