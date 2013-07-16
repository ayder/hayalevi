/************************************************************************
  Copyright (c)  2008  Toros Gokkurt
  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program as gpl.licencse.txt.
  If not, see <http://www.gnu.org/licenses/>.

  Version: 2                                              
  Last Update: 28/04/2008                                 
*************************************************************************/

ANKA BBS DEVELOPERS' GUIDE

Contents
A. Download, Compile, Install and Run
B. Roles
C. Commands and Descriptions
D. Writing an Example Command


A. Download, Compile, Install and Run

How to download, compile, install and run Ankabbs on Windows:

This document is written for developers. All steps
from downloading to running will be introduced briefly. 
My development environment is as follows:

Hardware:
 AMD Athlon XP 1700+ 1.47 GHz,
 1 GB of ram
 Free Harddisk space > 10 Megabytes

Software:
 Windows XP with SP2
 TortoiseSVN (http://tortoisesvn.tigris.org/)
 Cygwin (http://www.cygwin.com/mirrors.html)
 Eclipse IDE for C/C++ Developers (http://www.eclipse.org/downloads/)


1. Download Source Code

Source codes are kept and served on a svn server. A subversion
client is needed to download them. Tortoisesvn is suggested, but
you can also use subclipse (http://subclipse.tigris.org/).

- Open windows explorer.
- Create an empty directory on your disk. (I assume D:\ankabbs)
- Right click in the directory and select "SVN Checkout"
- A window is opened, write "svn://ayder.org" in the box with title "URL of repository".
- In the "Checkout Directory" box, browse to the directory where you want sources to be downloaded (I assume D:\ankabbs)
- User and password will be asked, write your SVN username and password and ok. If you don't have a username and password for svn, then first contact an admin.
- Download starts automatically after authentication.
- After download finishes close the download window by pressing "ok".

2. Compile source code

- Install Cygwin. Source codes are written in GNU C libraries, so you need compiler/debugger and some libraries. I installed all cygwin tools and libraries, so it took some time and disk space (more than 3 GB). But you can choose only to install
"gnu compiler collection (C and C++), sqlite3".

- Install and run eclipse. 
- File -> New -> C Project
- Choose a name for project (I assume ankabbs)
- Uncheck "use default location" and browse to the source directory (D:\ankabbs\hayalevi\trunk\src)
- Choose "Makefile project" as Project type
- Choose "Cygwin GCC" as Toolchain and Finish.
- Press Control-b to build project. It is expected to build without errors.

3. Install

- Copy bbs.exe to lib directory (D:\ankabbs\hayalevi\trunk\lib)
- Edit user Baxter in users directory (D:\ankabbs\hayalevi\trunk\lib\user\Baxter). Don't forget to clear password.

4. Run

- Run cmd on windows (Start -> Run -> cmd)
- Change directory to D:\ankabbs\hayalevi\trunk\lib\
- Write "bbs.exe 3000" and press enter to run bbs on port 3000.
- Open a telnet client and "telnet localhost 3000"
- Log in with user Baxter.


B. Roles
- User
- Moderator
- Admin

C. Commands and their descriptions

1. User Commands :
 
alia : Error message "I'm sorry, alias must be entered in full."
alias : This command creates an alias for any command.
anonnote : This command is used to leave notes in forums anonymously.
bug : This command will let you warn the system about a bug you found.
cls : This command will clear the screen.
clip : This command copies the note to your clipboard in case you may want to take copies.
credits : This command runs command "help diku".
date : This command is an alias of command "time".
enemy : This command shows/adds/removes your enemies. 
finger : This command helps you to get some information about another user.
feeling : This command is used to send one of the feelings in feeling list to a user.
fl : This command shows you the list of available forums in BBS in alphabetical order.
fl_old : This command shows you the list of available forums in BBS in numerical order.
fly : This command is an alias to "jump".
fnotify : This command If you notify a forum using this command, when a note is left on that forum you get a message indicating that a note is left on the board.
forumlist : This command shows you the list of available forums in BBS in alphabetical order.
forumlist_old : This command shows you the list of available forums in BBS in numerical order. 
friend : This command shows/adds/removes your friends.
from : This command shows the headers of your new mail messages.
go : This command is an alias to "jump".
help : This command gives detailed information about various commands. 
hide : This command is used to hide your personal information from other users such as; real name, e-mail, alternative e-mail, icq number and homepage url.
info : This command will give you a brief information about the forum, which you are currently in, and the rules of forum.
ignore : This command shows/adds/removes your ignores.
idea : This command is used to suggest the system administrators new enhancements for the BBS.
jump : This command lets you go into a forum.
look : This command is used to see current users who are using the forum with you.
list : This command is used in forums in order to see a brief list of messages.
ln : This command ia an alias of "listnew".
listnew : This command is used in forums in order to see a brief list of messages. 
mail : This command is used to send/recieve/read mail in BBS. 
mx : This commands is used to look your message history that you had done before.
new : This commands shows you the new messages one by one to you when you enter a forum with unread messages.
newmsgs : This command shows you how many new messages there are in forums that you keep reading in alphabetical order.
newmsgs_old : This command shows you how many new messages there are in forums that you keep reading in numerical order.
next : After you read a note either with read or new command, executing this will display the next note in current forum.
nm : This command is an alias of "newmsgs".
nm_old : This command is an alias of "newmsgs_old".
note : This command is used to leave notes in forums.
notify : This command shows/adds/removes your notifies.
nx : This command is an alias of "next".
plan : This command is used to enter a message upto 5 lines to describe yourself.
previous : After you read a note either with read or new command, executing this will display the previous note in current forum.
profile : This command is an alias of "plan".
password : This command lets you change your password.
reply : This command will send an x message to whom he/she sent the last message to you. 
read : This command is used to read a specific number of note. 
readall : This command reads all the current new messages without showing you.
readlast : This command reads the last message in a forum. 
remove : This command removes the given number of note from your own moderated forum.
save : This command saves your user data.
set : This command is used to set your personal information such as; title, alternative e-mail, homepage url, icq number and idle message. 
search : This command searches the given string in all of the notes inside a forum and gives a list of notes including the search string.
sendclip : This command send your clipboard materials to your e-mail address.
showclip : This command will let you see the last note you copied to your clipboard.
skip : This command ... 
time : This command gives you the local date and time of BBS. 
title : This commands lets you change your title.
toggle : This command lets you change some options such as; express message, beep , ansi color, idle mode, notify channel, auto xers, and feeling.
q : This command quits your BBS login.
quit : This command quits your BBS login.
unalias : This command removes the alias you already defined.
unflash : This command ... 
unhide : This command is used to unhide your hidden personal information in order to show other users such as; real name, e-mail, alternative e-mail , icq number and homepage url.
uptime : This command shows the last rebooting time of BBS.
version : This command shows you the version of BBS.
who : This command shows you all users online with various items of information such as how long they are online, and their titles.
Who : This command is used to show the users' names and locations in aplhabetical order. 
Who_old : This command is used to show the users' names and locations in online time order. 
whoami : This command shows you, who you are.
x : This command lets you send an express message to another user upto 10 lines.
xers : This command shows you the names of users who are currently xing or replying you.
xm : This command is an alias of "xers".
xn : This command is an alias of "xers".
zap : This command makes the forums you don't read, invisible for you.
de2tr : This command translates the given German string to Turkish . 
tr2de : This command translates the given Turkish string to German.
en2en : This command translates the given English string to English.
en2tr : This command translates the given English string to Turkish .
tr2en : This command translates the given Turkish string to English .
fr2tr : This command translates the given French string to Turkish .
tr2fr : This command translates the given French string to Turkish . 
tr2tr : This command translates the given Turkish string to Turkish . 
dict : This command translates the given string to all available languages.
exchange : This command ... 
charset : This command shows your charset.

2. Moderator commands.
 
foruminfo : This command changes info of your own moderated forum.
kick : This command kicks the user from your own moderated forum.
stats : This command shows some statistics about your own moderated forum.


3. Admin commands.

ac : This command ... 
admin : This command ... 
banish : This command ... 
config : This command ... 
deluser : This command ... 
disconnect : This command ... 
echo : This command ... 
force : This command ... 
forumset : This command ... 
hostname : This command ... 
invis : This command ... 
memory : This command ... 
reboo : This command ... 
reboot : This command ... 
saveall : This command ... 
setuser : This command ... 
shutdow : This command ... 
shutdown : This command ... 
sockets : This command ... 
snoop : This command ... 
unbanish : This command ... 
userset : This command ... 
validate : This command ... 
unkick : This command ... 
addforum : This command ... 
delforum : This command ... 
setforum : This command ... 
showmods : This command ... 
statforum : This command ... 
banishes : This command ... 
loadfeeling : This command ... 
loadhelps : This command ... 



D. Writing an Example Command

Let's write an example command "example" which prints some text to user screen.

Open command.c and write the function:
void do_example(USER_DATA *usr, char *argument)
	char buf[INPUT];
	if (is_turkish(usr))
		sprintf(buf, "Bu ornek bir komuttur.\r\n");
	else
		sprintf(buf, "This is an example command.\r\n");
	send_to_user(buf, usr);
	return;
}

As you can see, the command's function name should be like do_functionname.

Then add your command to command table in command.c file:
Add to array const struct cmd_type cmd_table [] 
{ "example", do_example, 0, 1, 0 },

Then add your command function to bbs.h file:
CMD_DO_FUN( do_example );

That's all, now build your project again, run bbs, connect and try command "example":
[Lobby] example
This is an example command.
[Lobby] 