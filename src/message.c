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
#include <time.h>
#include <sys/time.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

#include "bbs.h"

void remove_message(USER_DATA *usr) {
	MESSAGE_DATA *pMessage;
	int count = 0;

	for (pMessage = usr->pMsgFirst; pMessage; pMessage = pMessage->next)
		count++;

	if (count > 20) {
		pMessage = usr->pMsgFirst;
		UNLINK(pMessage, usr->pMsgFirst, usr->pMsgLast);
		if (pMessage->message)
			free_string(pMessage->message);
		free_mem(pMessage, sizeof(MESSAGE_DATA));
	}
}

void set_xing_time(USER_DATA *usr) {
	char *strtime;

	usr->xing_time = str_dup("");
	strtime = ctime(&current_time);
	strtime[strlen(strtime)-9] = '\0';
	usr->xing_time = str_dup(strtime);
}

bool isBusy(USER_DATA *usr) {
	USER_DATA *xing;

	if ((xing = get_user(usr->xing_to)) == NULL) {
		bug("isBusy: Null usr->xing_to", 0);
		EDIT_MODE(usr) = EDITOR_NONE;
		return FALSE;
	}

	if (EDIT_MODE(xing) != EDITOR_NONE
	|| xing->desc->showstr_point> 0)
		return TRUE;

	return FALSE;
}

void do_xing(USER_DATA *usr, char *argument, bool fXing) {
	char buf[STRING_LENGTH];
	char arg[INPUT_LENGTH];
	USER_DATA *x_usr, *full;

	one_argument(argument, arg);

	set_xing_time(usr);

	if (IS_TOGGLE(usr, TOGGLE_IDLE)) {
		send_to_user("You are resting, you can't send x message.\n\r", usr);
		EDIT_MODE(usr) = EDITOR_NONE;
		return;
	}

	if (!IS_TOGGLE(usr, TOGGLE_XING) && fXing)
		send_to_user("Reception of X's disabled for you.  "
			"To enable receipt, type 'toggle x'.\n\r", usr);

	if (arg[0] == '\0') {
		EDIT_MODE(usr) = EDITOR_XING_RECEIPT;
		return;
	}

	if ((x_usr = get_user(arg)) == NULL) {
		if (!is_user(arg))
			send_to_user("No such user.\n\r", usr);
		else
			send_to_user("User is not online.\n\r", usr);
		usr->xing_to = "*NONE*";
		usr->last_xing = "*NONE*";
		usr->reply_to = "*NONE*";
		EDIT_MODE(usr) = EDITOR_NONE;
		return;
	}

	if (IS_TOGGLE(x_usr, TOGGLE_INVIS) && !IS_ADMIN(usr)) {
		send_to_user("User is not online.\n\r", usr);
		usr->xing_to = "*NONE*";
		usr->last_xing = "*NONE*";
		usr->reply_to = "*NONE*";
		EDIT_MODE(usr) = EDITOR_NONE;
		return;
	}

	if ((x_usr = get_user(arg)) != NULL) {
		if ((full = get_user_full(arg)) == NULL) {
			sprintf(buf, "Couldn't find %s.  X'ing %s.\n\r", capitalize(arg),
					colorize(usr, x_usr->name));
			send_to_user(buf, usr);
		}
	}

	if (!x_usr->Validated && !IS_ADMIN(usr)) {
		send_to_user("You can't send x message to a non-validated user.\n\r",
				usr);
		usr->xing_to = "*NONE*";
		usr->last_xing = "*NONE*";
		usr->reply_to = "*NONE*";
		EDIT_MODE(usr) = EDITOR_NONE;
		return;
	}

	if (is_enemy(x_usr, usr->name) && !IS_ADMIN(usr)) {
		sprintf(buf, "#R%s#w doesn't want your messages.\n\r", x_usr->name);
		send_to_user(buf, usr);
		usr->xing_to = "*NONE*";
		usr->last_xing = "*NONE*";
		usr->reply_to = "*NONE*";
		EDIT_MODE(usr) = EDITOR_NONE;
		return;
	}

	if (is_enemy(usr, x_usr->name) && !IS_ADMIN(usr)) {
		sprintf(buf, "#R%s#w is in your enemy list.\n\r", x_usr->name);
		send_to_user(buf, usr);
		usr->xing_to = "*NONE*";
		usr->last_xing = "*NONE*";
		usr->reply_to = "*NONE*";
		EDIT_MODE(usr) = EDITOR_NONE;
		return;
	}

	if (IS_TOGGLE(x_usr, TOGGLE_IDLE) && !IS_ADMIN(usr)) {
		sprintf(buf, "%s is resting now.  Your message don't be shown.\n\r",
				colorize(usr, x_usr->name));
		send_to_user(buf, usr);
		if (str_cmp(x_usr->idlemsg, "NONE")) {
			send_to_user_c(x_usr->idlemsg, usr, FALSE);
			send_to_user("\n\r", usr);
		}
		usr->xing_to = "*NONE*";
		usr->last_xing = "*NONE*";
		usr->reply_to = "*NONE*";
		EDIT_MODE(usr) = EDITOR_NONE;
		return;
	}

	if (!IS_TOGGLE(x_usr, TOGGLE_XING) && !is_friend(x_usr, usr->name) && !IS_ADMIN(usr)) {
		send_to_user("The user is X-disabled now.  Try again later.\n\r", usr);
		usr->last_xing = "*NONE*";
		usr->xing_to = "*NONE*";
		usr->reply_to = "*NONE*";
		EDIT_MODE(usr) = EDITOR_NONE;
		return;
	}

	if (!IS_TOGGLE(x_usr, TOGGLE_XING) && is_friend(x_usr, usr->name))
		send_to_user(
				"User has eXpress disabled.  You're in the user's friend list.\n\r",
				usr);

	sprintf(
			buf,
			"You have %d lines.  Please type <CR> twice to send, #W~q#w to quit.\n\r", 
			MAX_X_LINES);
	send_to_user(buf, usr);
	usr->xing_to = x_usr->name;
	usr->last_xing = x_usr->name;
	usr->msg = NULL;
	EDIT_LINE(usr) = 1;
	EDIT_MODE(usr) = EDITOR_XING;
	if (IS_TOGGLE(x_usr, TOGGLE_WARN)) {
		sprintf(buf, "%s is about to send a message eXpress to you.\n\r",
				colorize(x_usr, usr->name));
		if (isBusySelf(x_usr))
			add_buffer(x_usr, buf);
		else
			send_to_user(buf, x_usr);
	}
	return;
}

void do_x(USER_DATA *usr, char *argument) {
	do_xing(usr, argument, TRUE);
}

void edit_xing_receipt(USER_DATA *usr, char *argument) {
	while (isspace(*argument))
		argument++;

	usr->timer = 0;

	if (argument[0] == '\0') {
		if (get_user(usr->last_xing) == NULL) {
			EDIT_MODE(usr) = EDITOR_NONE;
			return;
		} else if (str_cmp(usr->last_xing, "*NONE*")) {
			do_xing(usr, usr->last_xing, FALSE);
			return;
		}
		EDIT_MODE(usr) = EDITOR_NONE;
		return;
	}

	do_xing(usr, argument, FALSE);
	return;
}

void edit_xing(USER_DATA *usr, char *argument) {
	char output[STRING_LENGTH];
	char buf[STRING_LENGTH];
	char msg[STRING_LENGTH];
	USER_DATA *xing;
	MESSAGE_DATA *pMessage;

	while (isspace(*argument))
		argument++;

	usr->timer = 0;

	strcpy(buf, argument);

	if (buf[0] == '\0') {
		if ((xing = get_user(usr->xing_to)) == NULL) {
			send_to_user(
					"Sorry, user has left before you could send your message.\n\r",
					usr);
			usr->xing_to = "*NONE*";
			usr->last_xing = "*NONE*";
			usr->reply_to = "*NONE*";
			EDIT_MODE(usr) = EDITOR_NONE;
			return;
		}

		if (!xing->Validated && !IS_ADMIN(usr)) {
			send_to_user("You can't send x message to not validated user.\n\r",
					usr);
			usr->xing_to = "*NONE*";
			usr->last_xing = "*NONE*";
			usr->reply_to = "*NONE*";
			EDIT_MODE(usr) = EDITOR_NONE;
			return;
		}

		if (is_enemy(xing, usr->name) && !IS_ADMIN(usr)) {
			sprintf(output, "#R%s#w doesn't want your messages.\n\r",
					usr->xing_to);
			send_to_user(output, usr);
			usr->xing_to = "*NONE*";
			usr->last_xing = "*NONE*";
			usr->reply_to = "*NONE*";
			EDIT_MODE(usr) = EDITOR_NONE;
			return;
		}

		if (is_enemy(usr, usr->xing_to) && !IS_ADMIN(usr)) {
			sprintf(output, "#R%s#w is in your enemy list.\n\r", usr->xing_to);
			send_to_user(output, usr);
			usr->xing_to = "*NONE*";
			usr->last_xing = "*NONE*";
			usr->reply_to = "*NONE*";
			EDIT_MODE(usr) = EDITOR_NONE;
			return;
		}

		if (IS_TOGGLE(xing, TOGGLE_IDLE) && !IS_ADMIN(usr)) {
			sprintf(output,
					"%s is resting now.  You message don't be shown.\n\r",
					colorize(xing, usr->xing_to));
			send_to_user(output, usr);
			if (str_cmp(xing->idlemsg, "NONE")) {
				send_to_user_c(xing->idlemsg, usr, FALSE);
				send_to_user("\n\r", usr);
			}
			usr->xing_to = "*NONE*";
			usr->last_xing = "*NONE*";
			usr->reply_to = "*NONE*";
			EDIT_MODE(usr) = EDITOR_NONE;
			return;
		}

		if (!IS_TOGGLE(xing, TOGGLE_XING) && !is_friend(xing, usr->name) && !IS_ADMIN(usr)) {
			send_to_user("The user X-disabled now.  Try again later.\n\r", usr);
			usr->xing_to = "*NONE*";
			usr->last_xing = "*NONE*";
			usr->reply_to = "*NONE*";
			EDIT_MODE(usr) = EDITOR_NONE;
			return;
		}

		if (EDIT_LINE(usr) == 1) {
			if (isBusy(usr))
				sprintf(
						output,
						"%s is %s now.\n\r",
						usr->xing_to,
						(EDIT_MODE(xing) == EDITOR_INFO
						|| EDIT_MODE(xing) == EDITOR_INFO_ANSWER) ? "writing a info"
								: (EDIT_MODE(xing) == EDITOR_XING
								|| EDIT_MODE(xing) == EDITOR_XING_RECEIPT
								|| EDIT_MODE(xing) == EDITOR_XING_REPLY) ? "x'ing"
										: (EDIT_MODE(xing) == EDITOR_FEELING
										|| EDIT_MODE(xing) == EDITOR_FEELING_RECEIPT) ? "feeling"
												: (EDIT_MODE(xing) == EDITOR_NOTE
												|| EDIT_MODE(xing) == EDITOR_NOTE_SUBJECT) ? "writing a note"
														: (EDIT_MODE(xing) == EDITOR_MAIL
														|| EDIT_MODE(xing) == EDITOR_MAIL_SUBJECT
														|| EDIT_MODE(xing) == EDITOR_MAIL_WRITE) ? "mailing"
																: (EDIT_MODE(xing) == EDITOR_PLAN
																|| EDIT_MODE(xing) == EDITOR_PLAN_ANSWER) ? "writing a profile"
																		: "busy");
			else
				sprintf(output, "%s is not busy.\n\r", colorize(usr,
						usr->xing_to));
			send_to_user(output, usr);
			EDIT_MODE(usr) = EDITOR_NONE;
			if (IS_TOGGLE(xing, TOGGLE_WARN)) {
				sprintf(output,
						"%s has cancelled sending message eXpress to you.\n\r",
						colorize(xing, usr->name));
				if (isBusySelf(xing))
					add_buffer(xing, output);
				else
					send_to_user(output, xing);
			}
			usr->xing_to = "*NONE*";
			return;
		} else {
			if (isBusy(usr)) {
				print_to_user(
						usr,
						"%s is %s now, will receive your message when done.\n\r",
						colorize(usr, xing->name),
						(EDIT_MODE(xing) == EDITOR_INFO_ANSWER
						|| EDIT_MODE(xing) == EDITOR_INFO) ? "writing a info"
								: (EDIT_MODE(xing) == EDITOR_XING
								|| EDIT_MODE(xing) == EDITOR_XING_RECEIPT
								|| EDIT_MODE(xing) == EDITOR_XING_REPLY) ? "x'ing"
										: (EDIT_MODE(xing) == EDITOR_FEELING
										|| EDIT_MODE(xing) == EDITOR_FEELING_RECEIPT) ? "feeling"
												: (EDIT_MODE(xing) == EDITOR_NOTE
												|| EDIT_MODE(xing) == EDITOR_NOTE_SUBJECT) ? "writing a note"
														: (EDIT_MODE(xing) == EDITOR_MAIL
														|| EDIT_MODE(xing) == EDITOR_MAIL_SUBJECT
														|| EDIT_MODE(xing) == EDITOR_MAIL_WRITE) ? "mailing"
																: (EDIT_MODE(xing) == EDITOR_PLAN
																|| EDIT_MODE(xing) == EDITOR_PLAN_ANSWER) ? "writing a profile"
																		: "busy");

				sprintf(output,
						"\n\rThese eXpress messages are held while you were busy."
							"\n\r\n\r%s#W**** #yMessage from: %s #yat #c%s#W ****#x"
							"\n\r%s#x\n\r", IS_TOGGLE(xing, TOGGLE_BEEP) ? "\007" : "", colorize(
								xing, usr->name), usr->xing_time, usr->msg);
				add_buffer(xing, output);

				sprintf(output, "**** Message FROM: %s at %s ****\n\r%s\n\r",
						usr->name, usr->xing_time, usr->msg);
				pMessage = alloc_mem(sizeof(MESSAGE_DATA));
				pMessage->message = str_dup(output);
				LINK(pMessage, xing->pMsgFirst, xing->pMsgLast);
				remove_message(xing);

				sprintf(output, "**** Message TO: %s at %s ****\n\r%s\n\r",
						xing->name, usr->xing_time, usr->msg);
				pMessage = alloc_mem(sizeof(MESSAGE_DATA));
				pMessage->message = str_dup(output);
				LINK(pMessage, usr->pMsgFirst, usr->pMsgLast);
				remove_message(usr);

				xing->reply_to = usr->name;
				usr->xing_to = "*NONE*";
				usr->msg = NULL;
				EDIT_MODE(usr) = EDITOR_NONE;
				return;
			}

			print_to_user(usr, "%s has received your message.\n\r", colorize(usr, xing->name));

			print_to_user(xing,
					"\n\r%s#W**** #yMessage from: #Y%s #yat #c%s#W ****#x\n\r"
						"%s#x\n\r", IS_TOGGLE(xing, TOGGLE_BEEP) ? "\007" : "", colorize(xing,
							usr->name), usr->xing_time, usr->msg);

			sprintf(output, "**** Message FROM: %s at %s ****\n\r%s\n\r",
					usr->name, usr->xing_time, usr->msg);
			pMessage = alloc_mem(sizeof(MESSAGE_DATA));
			pMessage->message = str_dup(output);
			LINK(pMessage, xing->pMsgFirst, xing->pMsgLast);
			remove_message(xing);

			sprintf(output, "**** Message TO: %s at %s ****\n\r%s\n\r",
					xing->name, usr->xing_time, usr->msg);
			pMessage = alloc_mem(sizeof(MESSAGE_DATA));
			pMessage->message = str_dup(output);
			LINK(pMessage, usr->pMsgFirst, usr->pMsgLast);
			remove_message(usr);

			xing->reply_to = usr->name;
			usr->xing_to = "*NONE*";
			usr->msg = NULL;
			EDIT_MODE(usr) = EDITOR_NONE;
			return;
		}
	}

	if (!str_cmp(buf, "~q")) {
		send_to_user("Aborted.\n\r", usr);
		xing = get_user(usr->xing_to);
		usr->msg = NULL;
		EDIT_MODE(usr) = EDITOR_NONE;
		if (xing && IS_TOGGLE(xing, TOGGLE_WARN)) {
			sprintf(output,
					"%s has cancelled sending message eXpress to you.\n\r",
					colorize(xing, usr->name));
			if (isBusySelf(xing))
				add_buffer(xing, output);
			else
				send_to_user(output, xing);
		}
		usr->xing_to = "*NONE*";
		return;
	}

	if (usr->msg) {
		strcpy(msg, usr->msg);
		usr->msg = NULL;
	} else
		strcpy(msg, "");

	strcat(msg, "] ");
	strcat(msg, buf);
	strcat(msg, "#x\n\r");
	free_string(usr->msg);
	usr->msg = str_dup(msg);
	EDIT_LINE(usr)++;

	if (strlen(buf) > 78) {
		EDIT_LINE(usr)++;
		/*	send_to_user("Line too long.\n\r", usr);
		 return; REMBAX */
	}

	if (EDIT_LINE(usr)> MAX_X_LINES) {
		if ((xing = get_user(usr->xing_to)) == NULL) {
			send_to_user(
					"Sorry, user has left before you could send your message.\n\r",
					usr);
			usr->xing_to = "*NONE*";
			usr->last_xing = "*NONE*";
			usr->reply_to = "*NONE*";
			EDIT_MODE(usr) = EDITOR_NONE;
			return;
		}

		if (!xing->Validated && !IS_ADMIN(usr)) {
			send_to_user("You can't send x message to not validated user.\n\r",
					usr);
			usr->xing_to = "*NONE*";
			usr->last_xing = "*NONE*";
			usr->reply_to = "*NONE*";
			EDIT_MODE(usr) = EDITOR_NONE;
			return;
		}

		if (is_enemy(xing, usr->name) && !IS_ADMIN(usr)) {
			sprintf(output, "%s don't wants your messages.\n\r", usr->xing_to);
			send_to_user(output, usr);
			usr->xing_to = "*NONE*";
			usr->last_xing = "*NONE*";
			usr->reply_to = "*NONE*";
			EDIT_MODE(usr) = EDITOR_NONE;
			return;
		}

		if (is_enemy(usr, usr->xing_to) && !IS_ADMIN(usr)) {
			sprintf(output, "%s is in your enemy list.\n\r", usr->xing_to);
			send_to_user(output, usr);
			usr->xing_to = "*NONE*";
			usr->last_xing = "*NONE*";
			usr->reply_to = "*NONE*";
			EDIT_MODE(usr) = EDITOR_NONE;
			return;
		}

		if (IS_TOGGLE(xing, TOGGLE_IDLE) && !IS_ADMIN(usr)) {
			sprintf(output,
					"%s is resting now.  You message don't be shown.\n\r",
					xing->name);
			send_to_user(output, usr);
			if (str_cmp(xing->idlemsg, "NONE")) {
				send_to_user_c(xing->idlemsg, usr, FALSE);
				send_to_user("\n\r", usr);
			}
			usr->xing_to = "*NONE*";
			usr->last_xing = "*NONE*";
			usr->reply_to = "*NONE*";
			EDIT_MODE(usr) = EDITOR_NONE;
			return;
		}

		if (!IS_TOGGLE(xing, TOGGLE_XING) && !is_friend(xing, usr->name) && !IS_ADMIN(usr)) {
			send_to_user("The user X-disabled now.  Try again later.\n\r", usr);
			usr->xing_to = "*NONE*";
			usr->last_xing = "*NONE*";
			usr->reply_to = "*NONE*";
			EDIT_MODE(usr) = EDITOR_NONE;
			return;
		}

		if (isBusy(usr)) {
			print_to_user(
					usr,
					"%s is %s now, will receive your message when done.\n\r",
					xing->name,
					(EDIT_MODE(xing) == EDITOR_INFO_ANSWER
					|| EDIT_MODE(xing) == EDITOR_INFO) ? "writing a info"
							: (EDIT_MODE(xing) == EDITOR_XING
							|| EDIT_MODE(xing) == EDITOR_XING_RECEIPT
							|| EDIT_MODE(xing) == EDITOR_XING_REPLY) ? "x'ing"
									: (EDIT_MODE(xing) == EDITOR_FEELING
									|| EDIT_MODE(xing) == EDITOR_FEELING_RECEIPT) ? "feeling"
											: (EDIT_MODE(xing) == EDITOR_NOTE
											|| EDIT_MODE(xing) == EDITOR_NOTE_SUBJECT) ? "writing a note"
													: (EDIT_MODE(xing) == EDITOR_MAIL
													|| EDIT_MODE(xing) == EDITOR_MAIL_SUBJECT
													|| EDIT_MODE(xing) == EDITOR_MAIL_WRITE) ? "mailing"
															: (EDIT_MODE(xing) == EDITOR_PLAN
															|| EDIT_MODE(xing) == EDITOR_PLAN_ANSWER) ? "writing a profile"
																	: "busy");

			sprintf(output,
					"\n\rThese eXpress messages are held while you were busy."
						"\n\r\n\r%s#W**** #yMessage from: #Y%s #yat #c%s#W ****#x"
						"\n\r%s#x\n\r", IS_TOGGLE(xing, TOGGLE_BEEP) ? "\007" : "", colorize(xing,
							usr->name), usr->xing_time, usr->msg);
			add_buffer(xing, output);

			sprintf(output, "**** Message FROM: %s at %s ****\n\r%s\n\r",
					usr->name, usr->xing_time, usr->msg);
			pMessage = alloc_mem(sizeof(MESSAGE_DATA));
			pMessage->message = str_dup(output);
			LINK(pMessage, xing->pMsgFirst, xing->pMsgLast);
			remove_message(xing);

			sprintf(output, "**** Message TO: %s at %s ****\n\r%s\n\r",
					xing->name, usr->xing_time, usr->msg);
			pMessage = alloc_mem(sizeof(MESSAGE_DATA));
			pMessage->message = str_dup(output);
			LINK(pMessage, usr->pMsgFirst, usr->pMsgLast);
			remove_message(usr);

			xing->reply_to = usr->name;
			usr->xing_to = "*NONE*";
			usr->msg = NULL;
			EDIT_MODE(usr) = EDITOR_NONE;
			return;
		} else {
			print_to_user(usr, "%s has received your message.\n\r", colorize(usr, xing->name));

			print_to_user(xing,
					"\n\r%s#W**** #yMessage from: #Y%s #yat #c%s#W ****#x"
						"\n\r%s#x\n\r", IS_TOGGLE(xing, TOGGLE_BEEP) ? "\007" : "", colorize(xing,
							usr->name), usr->xing_time, usr->msg);

			sprintf(output, "**** Message FROM: %s at %s ****\n\r%s\n\r",
					usr->name, usr->xing_time, usr->msg);
			pMessage = alloc_mem(sizeof(MESSAGE_DATA));
			pMessage->message = str_dup(output);
			LINK(pMessage, xing->pMsgFirst, xing->pMsgLast);
			remove_message(xing);

			sprintf(output, "**** Message TO: %s at %s ****\n\r%s\n\r",
					xing->name, usr->xing_time, usr->msg);
			pMessage = alloc_mem(sizeof(MESSAGE_DATA));
			pMessage->message = str_dup(output);
			LINK(pMessage, usr->pMsgFirst, usr->pMsgLast);
			remove_message(usr);

			xing->reply_to = usr->name;
			usr->xing_to = "*NONE*";
			usr->msg = NULL;
			EDIT_MODE(usr) = EDITOR_NONE;
			return;
		}
	}
}

void do_xers(USER_DATA *usr, char *argument) {
	char buf[STRING_LENGTH];
	USER_DATA *xusr;
	bool xFound= FALSE;

	strcpy(buf, "You are currently being x'ed by:");
	for (xusr = user_list; xusr != NULL; xusr = xusr->next) {
		if (!str_cmp(xusr->xing_to, usr->name)) {
			xFound = TRUE;
			strcat(buf, " ");
			strcat(buf, colorize(usr, xusr->name));
		}
	}

	if (!xFound)
		send_to_user("Nobody is x'ing you.\n\r", usr);
	else {
		send_to_user(buf, usr);
		send_to_user("\n\r", usr);
	}
	return;
}

void do_reply(USER_DATA *usr, char *argument) {
	char buf[STRING_LENGTH];
	USER_DATA *reply;

	if (!(reply = get_user(usr->reply_to)))
		usr->reply_to = "*NONE*";

	set_xing_time(usr);

	if (IS_TOGGLE(usr, TOGGLE_IDLE)) {
		send_to_user("You are resting, you can't send x message.\n\r", usr);
		EDIT_MODE(usr) = EDITOR_NONE;
		return;
	}

	if (!IS_TOGGLE(usr, TOGGLE_XING))
		send_to_user(
				"Reception of X's disabled for you.  To enable receipt, type 'toggle x'.\n\r",
				usr);

	if (get_user(usr->reply_to) == NULL) {
		EDIT_MODE(usr) = EDITOR_XING_REPLY;
		return;
	} else if (str_cmp(usr->reply_to, "*NONE*")) {
		sprintf(buf, "Replying to %s.\n\r", colorize(usr, usr->reply_to));
		send_to_user(buf, usr);
		do_xing(usr, usr->reply_to, FALSE);
		return;
	}

	EDIT_MODE(usr) = EDITOR_XING_REPLY;
	return;
}

void edit_xing_reply(USER_DATA *usr, char *argument) {
	while (isspace(*argument))
		argument++;

	usr->timer = 0;

	if (argument[0] == '\0') {
		EDIT_MODE(usr) = EDITOR_NONE;
		return;
	}

	do_xing(usr, argument, FALSE);
	return;
}

void do_mx(USER_DATA *usr, char *argument) {
	MESSAGE_DATA *pMessage;
	char buf[10 * STRING];
	bool found= FALSE;

	strcpy(buf, "");

	for (pMessage = usr->pMsgLast; pMessage; pMessage = pMessage->prev) {
		found = TRUE;

		if (strlen(buf) > (10 * STRING) -1)
			continue;

		strncat(buf, pMessage->message, strlen(pMessage->message));
	}

	if (!found) {
		send_to_user("You have no message eXpress buffer.\n\r", usr);
		return;
	}

	page_to_user_c(buf, usr, FALSE);
	return;
}

void do_bug(USER_DATA *usr, char *argument) {
	FILE *fpBug;

	if (argument[0] == '\0') {
		syntax("bug <message>", usr);
		return;
	}

	if ((fpBug = fopen(BUG_FILE, "a")) == NULL) {
		bug("Do_bug: cannot open bug file.", 0);
		send_to_user("ERROR: Cannot open bug file.\n\r", usr);
		return;
	} else {
		set_xing_time(usr);
		fprintf(fpBug, "[%-12s] [%s]: %s\n", usr->name, usr->xing_time,
				argument);
		fclose(fpBug);
		send_to_user("Darn!.. Really?.. Sorry about that.. But, we'll fix that in a jiffy. :)\n\r", usr);
		return;
	}
}

void do_idea(USER_DATA *usr, char *argument) {
	FILE *fpIdea;

	if (argument[0] == '\0') {
		syntax("idea <message>", usr);
		return;
	}

	if ((fpIdea = fopen(IDEA_FILE, "a")) == NULL) {
		bug("Do_idea: cannot open idea file.", 0);
		send_to_user("ERROR: Cannot open idea file.\n\r", usr);
		return;
	} else {
		set_xing_time(usr);
		fprintf(fpIdea, "[%-12s] [%s]: %s\n", usr->name, usr->xing_time,
				argument);
		fclose(fpIdea);
		send_to_user("Wow! What a great idea! Thanks!!\n\r", usr);
		return;
	}
}

void do_admin(USER_DATA *usr, char *argument) {
	char buf[STRING_LENGTH];
	USER_DATA *musr;

	if (argument[0] == '\0') {
		syntax("admin <message>", usr);
		return;
	}

	for (musr = user_list; musr != NULL; musr = musr->next) {
		if (IS_ADMIN(musr) && IS_TOGGLE(musr, TOGGLE_ADM)) {
			sprintf(buf, "#x[#RAdmin#x][#G%s#x] %s#x\n\r", usr->name,
					argument);
			if (isBusySelf(musr))
				add_buffer(musr, buf);
			else
				send_to_user(buf, musr);
		}
	}

	return;
}

void do_feeling_org(USER_DATA *usr, char *argument, bool fXing) {
	char buf[STRING_LENGTH];
	char arg[INPUT_LENGTH];
	USER_DATA *x_usr, *full;

	one_argument(argument, arg);

	set_xing_time(usr);

	if (IS_TOGGLE(usr, TOGGLE_IDLE)) {
		send_to_user("You are resting, you can't send feeling.\n\r", usr);
		EDIT_MODE(usr) = EDITOR_NONE;
		return;
	}

	if (!IS_TOGGLE(usr, TOGGLE_FEEL) && fXing)
		send_to_user(
				"Reception of feeling's disabled for you.\n\rTo enable receipt, type 'toggle feeling'.\n\r",
				usr);

	if (arg[0] == '\0') {
		EDIT_MODE(usr) = EDITOR_FEELING_RECEIPT;
		return;
	}

	if ((x_usr = get_user(arg)) == NULL) {
		if (!is_user(arg))
			send_to_user("No such user.\n\r", usr);
		else
			send_to_user("User is not online.\n\r", usr);
		usr->xing_to = "*NONE*";
		usr->last_xing = "*NONE*";
		usr->reply_to = "*NONE*";
		EDIT_MODE(usr) = EDITOR_NONE;
		return;
	}

	if (IS_TOGGLE(x_usr, TOGGLE_INVIS) && !IS_ADMIN(usr)) {
		send_to_user("User is not online.\n\r", usr);
		usr->xing_to = "*NONE*";
		usr->last_xing = "*NONE*";
		usr->reply_to = "*NONE*";
		EDIT_MODE(usr) = EDITOR_NONE;
		return;
	}

	if ((x_usr = get_user(arg)) != NULL) {
		if ((full = get_user_full(arg)) == NULL) {
			sprintf(buf, "Couldn't find %s.  Feeling %s.\n\r", capitalize(arg),
					x_usr->name);
			send_to_user(buf, usr);
		}
	}

	if (!x_usr->Validated && !IS_ADMIN(usr)) {
		send_to_user("You can't send feeling to not validated user.\n\r", usr);
		usr->xing_to = "*NONE*";
		usr->last_xing = "*NONE*";
		usr->reply_to = "*NONE*";
		EDIT_MODE(usr) = EDITOR_NONE;
		return;
	}

	if (is_enemy(x_usr, usr->name) && !IS_ADMIN(usr)) {
		sprintf(buf, "%s don't wants your messages.\n\r", x_usr->name);
		send_to_user(buf, usr);
		usr->xing_to = "*NONE*";
		usr->last_xing = "*NONE*";
		usr->reply_to = "*NONE*";
		EDIT_MODE(usr) = EDITOR_NONE;
		return;
	}

	if (is_enemy(usr, x_usr->name) && !IS_ADMIN(usr)) {
		sprintf(buf, "%s is in your enemy list.\n\r", x_usr->name);
		send_to_user(buf, usr);
		usr->xing_to = "*NONE*";
		usr->last_xing = "*NONE*";
		usr->reply_to = "*NONE*";
		EDIT_MODE(usr) = EDITOR_NONE;
		return;
	}

	if (IS_TOGGLE(x_usr, TOGGLE_IDLE) && !IS_ADMIN(usr)) {
		sprintf(buf, "%s is resting now.  Your message don't be shown.\n\r",
				x_usr->name);
		send_to_user(buf, usr);
		if (str_cmp(x_usr->idlemsg, "NONE")) {
			send_to_user_c(x_usr->idlemsg, usr, FALSE);
			send_to_user("\n\r", usr);
		}
		usr->xing_to = "*NONE*";
		usr->last_xing = "*NONE*";
		usr->reply_to = "*NONE*";
		EDIT_MODE(usr) = EDITOR_NONE;
		return;
	}

	if (!IS_TOGGLE(x_usr, TOGGLE_FEEL) && !is_friend(x_usr, usr->name) && !IS_ADMIN(usr)) {
		send_to_user("The user is Feeling-disabled now.  Try again later.\n\r",
				usr);
		usr->last_xing = "*NONE*";
		usr->xing_to = "*NONE*";
		usr->reply_to = "*NONE*";
		EDIT_MODE(usr) = EDITOR_NONE;
		return;
	}

	if (!IS_TOGGLE(x_usr, TOGGLE_FEEL) && is_friend(x_usr, usr->name)) {
		send_to_user(
				"User has feeling disabled.  You're in the user's friend list.\n\r",
				usr);
	}

	FEELING_DATA* pFeel = first_feeling;
	BUFFER* output = new_buf();
	sprintf(buf, "\n\r#WAvailable feelings are:#x\n\r\n\r");
	add_buf(output, buf);
	int i = 1;
	for (; pFeel; pFeel = pFeel->next, i++) {
		sprintf(buf, "#C%7s#W - #M%-10s#x", pFeel->keyword, pFeel->name);
		if (i % 3 == 0) {
			strcat(buf, "\n\r");
		}
		add_buf(output, buf);
	}
	sprintf(buf, "\n\r      #Cq#W - #RExit menu#x\n\r\n\r");
	add_buf(output, buf);
	send_to_user(buf_string(output), usr);
	free_buf(output);

	usr->xing_to = x_usr->name;
	usr->last_xing = x_usr->name;
	EDIT_MODE(usr) = EDITOR_FEELING;
	if (IS_TOGGLE(x_usr, TOGGLE_WARN) && fXing) {
		sprintf(buf, "%s is about to send you something nice.\n\r", colorize(
				x_usr, usr->name));
		if (isBusySelf(x_usr)) {
			add_buffer(x_usr, buf);
		} else {
			send_to_user(buf, x_usr);
		}
	}
	return;
}

void edit_feeling_receipt(USER_DATA *usr, char *argument) {
	while (isspace(*argument))
		argument++;

	usr->timer = 0;

	if (argument[0] == '\0') {
		if (get_user(usr->last_xing) == NULL) {
			EDIT_MODE(usr) = EDITOR_NONE;
			return;
		} else if (str_cmp(usr->last_xing, "*NONE*")) {
			do_feeling_org(usr, usr->last_xing, TRUE);
			return;
		}
		EDIT_MODE(usr) = EDITOR_NONE;
		return;
	}

	do_feeling_org(usr, argument, TRUE);
	return;
}

void edit_feeling(USER_DATA *usr, char *argument) {
	char buf[STRING_LENGTH];
	USER_DATA *xing;
	FEELING_DATA* pFeel = first_feeling;

	while (isspace(*argument)) {
		argument++;
	}

	usr->timer = 0;

	strcpy(buf, argument);

	if (UPPER(buf[0]) == 'Q' || buf[0] == '\0') {
		send_to_user("Aborted.\n\r", usr);
		xing = get_user(usr->xing_to);
		usr->xing_to = "*NONE*";
		EDIT_MODE(usr) = EDITOR_NONE;
		if (xing && IS_TOGGLE(xing, TOGGLE_WARN)) {
			sprintf(buf, "%s has cancelled sending something nice to you.\n\r", colorize(xing, usr->name));
			if (isBusySelf(xing)) {
				add_buffer(xing, buf);
			} else {
				send_to_user(buf, xing);
			}
		}
		return;
	}

	for (; pFeel; pFeel = pFeel->next) {
		if (!str_prefix(buf, pFeel->keyword)) {
			send_feeling(usr, pFeel->text);
			return;
		}
	}

	send_to_user("We don't have a feeling for that. Sorry...\n\r\n\r", usr);
	EDIT_MODE(usr) = EDITOR_NONE;
	do_feeling_org(usr, usr->last_xing, FALSE);
	return;
}

void send_feeling(USER_DATA *usr, const char* feeling) {
	char output[STRING_LENGTH];
	USER_DATA *xing;

	if ((xing = get_user(usr->xing_to)) == NULL) {
		send_to_user(
				"Sorry, user has left before you could send your feeling.\n\r",
				usr);
		usr->xing_to = "*NONE*";
		usr->last_xing = "*NONE*";
		usr->reply_to = "*NONE*";
		EDIT_MODE(usr) = EDITOR_NONE;
		return;
	}

	if (!xing->Validated && !IS_ADMIN(usr)) {
		send_to_user("You can't send feeling to not validated user.\n\r", usr);
		usr->xing_to = "*NONE*";
		usr->last_xing = "*NONE*";
		usr->reply_to = "*NONE*";
		EDIT_MODE(usr) = EDITOR_NONE;
		return;
	}

	if (is_enemy(xing, usr->name) && !IS_ADMIN(usr)) {
		sprintf(output, "%s don't wants your messages.\n\r", usr->xing_to);
		send_to_user(output, usr);
		usr->xing_to = "*NONE*";
		usr->last_xing = "*NONE*";
		usr->reply_to = "*NONE*";
		EDIT_MODE(usr) = EDITOR_NONE;
		return;
	}

	if (is_enemy(usr, usr->xing_to) && !IS_ADMIN(usr)) {
		sprintf(output, "%s is in your enemy list.\n\r", usr->xing_to);
		send_to_user(output, usr);
		usr->xing_to = "*NONE*";
		usr->last_xing = "*NONE*";
		usr->reply_to = "*NONE*";
		EDIT_MODE(usr) = EDITOR_NONE;
		return;
	}

	if (IS_TOGGLE(xing, TOGGLE_IDLE) && !IS_ADMIN(usr)) {
		sprintf(output, "%s is resting now.  You message don't be shown.\n\r",
				usr->xing_to);
		send_to_user(output, usr);
		if (str_cmp(xing->idlemsg, "NONE")) {
			send_to_user_c(xing->idlemsg, usr, FALSE);
			send_to_user("\n\r", usr);
		}
		usr->xing_to = "*NONE*";
		usr->last_xing = "*NONE*";
		usr->reply_to = "*NONE*";
		EDIT_MODE(usr) = EDITOR_NONE;
		return;
	}

	if (!IS_TOGGLE(xing, TOGGLE_FEEL) && !is_friend(xing, usr->name) && !IS_ADMIN(usr)) {
		send_to_user("The user Feeling-disabled now.  Try again later.\n\r",
				usr);
		usr->xing_to = "*NONE*";
		usr->last_xing = "*NONE*";
		usr->reply_to = "*NONE*";
		EDIT_MODE(usr) = EDITOR_NONE;
		return;
	}

	if (isBusy(usr)) {
		sprintf(
				output,
				"%s is %s now, will receive your feeling when done.\n\r",
				colorize(usr, usr->xing_to),
				(EDIT_MODE(xing) == EDITOR_INFO_ANSWER
				|| EDIT_MODE(xing) == EDITOR_INFO) ? "writing a info"
						: (EDIT_MODE(xing) == EDITOR_XING
						|| EDIT_MODE(xing) == EDITOR_XING_RECEIPT
						|| EDIT_MODE(xing) == EDITOR_XING_REPLY) ? "x'ing"
								: (EDIT_MODE(xing) == EDITOR_FEELING
								|| EDIT_MODE(xing) == EDITOR_FEELING_RECEIPT) ? "feeling"
										: (EDIT_MODE(xing) == EDITOR_NOTE
										|| EDIT_MODE(xing) == EDITOR_NOTE_SUBJECT) ? "writing a note"
												: (EDIT_MODE(xing) == EDITOR_MAIL
												|| EDIT_MODE(xing) == EDITOR_MAIL_SUBJECT
												|| EDIT_MODE(xing) == EDITOR_MAIL_WRITE) ? "mailing"
														: (EDIT_MODE(xing) == EDITOR_PLAN
														|| EDIT_MODE(xing) == EDITOR_PLAN_ANSWER) ? "writing a profile"
																: "busy");
		send_to_user(output, usr);
		sprintf(
				output,
				"\n\rThese feelings are held while you were busy.\n\r\n\r"
					"%s#W**** #ySomething nice has been received from: %s #yat #c%s#W ****#x\n\r%s#x\n\r\n\r", 
				IS_TOGGLE(xing, TOGGLE_BEEP) ? "\007" : "", colorize(
						get_user(usr->xing_to), usr->name), usr->xing_time,
				feeling);
		add_buffer(get_user(usr->xing_to), output);
		get_user(usr->xing_to)->reply_to = usr->name;
		usr->xing_to = "*NONE*";
		EDIT_MODE(usr) = EDITOR_NONE;
		return;
	} else {
		sprintf(output, "%s has received your feeling.\n\r", colorize(usr,
				usr->xing_to));
		send_to_user(output, usr);
		sprintf(
				output,
				"\n\r%s#W**** #ySomething nice has been received from: %s #yat #c%s#W ****#x\n\r%s#x\n\r\n\r", 
				IS_TOGGLE(get_user(usr->xing_to), TOGGLE_BEEP) ? "\007" : "",
				colorize(get_user(usr->xing_to), usr->name), usr->xing_time,
				feeling);
		send_to_user(output, get_user(usr->xing_to));
		get_user(usr->xing_to)->reply_to = usr->name;
		usr->xing_to = "*NONE*";
		EDIT_MODE(usr) = EDITOR_NONE;
		return;
	}
}

void do_feeling(USER_DATA *usr, char *argument) {
	if (!first_feeling) {
		bbs_bug("do_feeling: NULL first_feeling!");
		send_to_user("\n\r#RUh oh, seems like feelings were not loaded yet.#x\n\r\n\r", usr);
		return;
	}
	do_feeling_org(usr, argument, TRUE);
}


