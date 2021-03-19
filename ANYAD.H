// Current implemented commands....
/**********************************
JOIN
PART
NICK
QUIT 
ME
ACTION
LIST
AWAY
QUERY
(auto PRIVMSG)
***********************************/

/* I need to redo the command parser... because.. something like /awayblabla
Will do a "AWAY :labla\n"
Only happen with commands that might contain either arguments or not.
So.. currently, nobody can create an alias which have command in them.. like /awaydd
or let's say.. /listchannels..
*/

#ifndef _MDITEST_H_
#define _MDITEST_H_

long APIENTRY MPFrameWndProc(HWND, UINT, WPARAM, LPARAM);
long APIENTRY MPMDIChildWndProc(HWND, UINT, WPARAM, LPARAM);
long APIENTRY EditWndProc(HWND, UINT, WPARAM, LPARAM);

long APIENTRY ConnectDialogProc(HWND, UINT, WPARAM, LPARAM);
long APIENTRY RawDialogProc(HWND, UINT, WPARAM, LPARAM);
long APIENTRY CreateWindowDialogProc(HWND, UINT, WPARAM, LPARAM);
long APIENTRY AddNickDialogProc(HWND, UINT, WPARAM, LPARAM);
long APIENTRY JoinDialogProc(HWND, UINT, WPARAM, LPARAM);
long APIENTRY TopicDialogProc(HWND, UINT, WPARAM, LPARAM);
long APIENTRY MediaDialogProc(HWND, UINT, WPARAM, LPARAM);
long APIENTRY AboutProc(HWND, UINT, WPARAM, LPARAM);

long APIENTRY PrefDialogProc1(HWND, UINT, WPARAM, LPARAM);
long APIENTRY PrefDialogProc2(HWND, UINT, WPARAM, LPARAM);

DWORD MediaPlayer(LPDWORD filename);
void DoPreferenceWindow(HWND hwnd);

BOOL CreateAWindow(char *windowname, int type);
void WriteText(char *text, int window);

int AnalyzeServerOutput(char *buff);
void CheckSocket(LPARAM lParam, WPARAM wParam);
void ReadDataTest();

u_long  GetAddr(LPSTR);

#define IDM_WINDOWCHILD 100

#define INCOMING_DATA WM_USER+100

#define NOTFOUND -1

void ReLoadFlag();

typedef struct INIFLAG { // main variables
	char curnick[50]; // current nick you're using... used everywhere :)
	char sounddir[256]; // can't be longer than 256 characters
	char logdir[256];
	char defquit[512]; // default quit message
} INIFLAG;

typedef struct PREFIX { // struct used for irc message...other people message
	char nick[50];
	char user[16];
	char host[100];
	int numparam;
	int sizeparam[256];
	char *ptrparam[256]; //$1, $2, $3...
	char *ptrfulparam[256]; //*1, *2, *3...
} PREFIX;

typedef struct SINFO { // struct used for 95% of server message, others are hard coded.
	char serv[100]; // server name
	int scode; // numerical code
	char snick[30]; // nick to who the message is for
	int numparam;
	int sizeparam[256];
	char *ptrparam[256];
	char *ptrfulparam[256];
} SINFO;

typedef struct NAME { // keep track of nick in the channel... it's a linked list too
	char nick[50];
	int op; // op flag. 0 = nonop, 1 = op 
	int voice; // voice flag. = nonvoice, 1 = voice
	struct NAME *next;
} NAME;


typedef struct CHANNEL { // struct used for each windows (channel or query)
	HWND hWndMDI; // Handle of the window (If this parameter = 0, then the window doesn't exist)
	HWND hWndLISTBOX; // handle of the list box
	HWND hWndEDITOR; // handle of the "new" edit control
	HWND hWndSCROLL; // handle of the vertical scroll bar
	int type; // type of window.. see above
	char title[256]; // name of the channel/window
	char topic[512]; // topic
	BOOL key;
	char keys[256];
	BOOL secret;
	BOOL topicflag;
	BOOL priv;
	BOOL moderated;
	BOOL limit;
	BOOL limitn;
	BOOL invite;
	unsigned int message;
	BOOL create; // If the flag is TRUE then the /names (353) goes into the listbox (entering the channel)
	int numuser; // number of user in the channel
	struct NAME *name; // linked list of name... if NULL then it's a query
	unsigned int linecount;
	char log[128]; // filename of the log file... Usually the same as title.
	HANDLE fhandle; // Handle of the opened log file...	
} CHANNEL;

void InitStuff(); // Initialisation du programme...

int SearchUnusedHandle();
int SearchString(char *buff); // Search a string and return a window handle

void GetNick(char *buff, PREFIX *info);
void GetUser(char *buff, PREFIX *info);
void GetHost(char *buff, PREFIX *info);
void GetServ(char *buff, SINFO *sinfo);
void GetNumCode(char *buff, SINFO *sinfo);
void GetSNick(char *buff, SINFO *sinfo);

int CheckMessageType(char *buff, PREFIX *info);

void InitParam(char *buff, PREFIX *info); // Fill the PREFIX with parameters stuff
void DeInitParam(PREFIX *info); // Unfill the PREFIX? Just deallocate memory...

void SInitParam(char *buff, SINFO *sinfo); // Same as above... for server message instead: SINFO
void SDeInitParam(SINFO *sinfo);

int GetNumParam(char *buff); // Get the number of parameter... (SPACE)
int GetPosParam(char *buff, int parm); // Get the position of the parameter
int GetSizeParam(char *buff, int parm); // Get the size 2 SPACES = 1 SPACE

void DoRecvPRIVMSG(PREFIX *info); // Function called when recieving a PRIVMSG
void DoRecvPART(PREFIX *info); // Function called when recieving a PART
void DoRecvJOIN(PREFIX *info); // guess...
void DoRecvNICK(PREFIX *info); 
void DoRecvQUIT(PREFIX *info); 
void DoRecvKICK(PREFIX *info); 
void DoRecvTOPIC(PREFIX *info);
void DoRecvMODE(PREFIX *info);

void DoServNAMREPLY(SINFO *sinfo);
void DoServENDOFNAMES(SINFO *sinfo);
void DoServTOPIC(SINFO *sinfo);
void DoServRPL_LISTSTART(SINFO *sinfo);
void DoServRPL_LIST(SINFO *sinfo);

void SendPRIVMSG(char *channel, char *text);
void SendJOIN(char *channel);
void SendPART(char *message);
void SendQUIT(char *quitmessage, int stringlen); // same as below
void SendME(char *channel, char *message);
void SendNICK(char *newnick);
void SendLIST(char *parms, int stringlen); // those might not contains any parms.. so the use of stringlen
void SendAWAY(char *parms, int stringlen);
void SendQUERY(char *parms, int stringlen);

void AddNick(char *nick, BOOL OpFlag, BOOL VoiceFlag, int window);
void DelNick(char *nick, int window);
void ReNameNick(char *nick, char *newnick, int window);
void Add(char *nick, BOOL OpFlag, BOOL VoiceFlag, int window);

BOOL IsVoice(char *nick, int window);
BOOL IsOp(char *nick, int window);
BOOL IsUser(char *nick, int window);
void ReFreshNameList(int window);

void SetChannelTitle(int window); 
void DeAllocateAndDestroyWindow(int window);


// TYPE
#define NONE 0
#define STATUS 1
#define CHAN 2
#define QUERY 3
#define CHANLIST 4


// identifiers for STATUS
#define STATUS_EDITOR 10

// identifiers for CHAN
#define CHANNEL_LISTBOX 20
#define CHANNEL_EDITOR 30
#define CHANNEL_SCROLLBAR 40

// identifiers for QUERY
#define QUERY_EDITOR 50

// identifier for CHANLIST
#define CHANLIST_LISTBOX 60

//*****************************************
// These defines used by CheckMessageType()
// They are the possible return values
//*****************************************

#define NOT_IMPLEMENTED 0
#define PRIVMSG 1
#define NOTICE 2
#define NICK 3
#define QUIT 4
#define JOIN 5
#define PART 6
#define MODE 7
#define TOPIC 8
#define KICK 9

#endif // _MDITEST_H_