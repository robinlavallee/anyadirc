#include <windows.h>
#include <WINDOWSX.H>
#include <winsock.h>
#include <mmsystem.h>
#include <commctrl.h>
#include "anyad.h"
#include "reply.h"
#include "resource.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>


struct sockaddr_in servaddr; // Server to connect to
HINSTANCE g_hInst;           // the current instance
HWND hwndMDIClient;          // handle of the MDI client window
HWND hWndFrame;              // handle of the frame window
WNDPROC wnd;				 // window proc of the old edit control


WSADATA wsadata;             // Winsock Data
SOCKET sock;                 // Socket...
CHANNEL chan[256];
INIFLAG flag;

int test;
int APIENTRY WinMain(
HINSTANCE hInstance,
HINSTANCE hPrevInstance,
LPSTR lpCmdLine,
int nCmdShow
)

{

	MSG msg;
	RECT coor;
    
    /* Let's create the main window */
    WNDCLASS  wc;

	InitStuff();
	
	g_hInst = hInstance;

    wc.style = 0;
    wc.lpfnWndProc = (WNDPROC) MPFrameWndProc;
    wc.cbClsExtra = 0;
    wc.cbWndExtra = 0;
    wc.hInstance = hInstance;
    wc.hIcon = NULL;
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    (HGDIOBJ) wc.hbrBackground = GetStockObject(WHITE_BRUSH);
    wc.lpszMenuName =  MAKEINTRESOURCE(MAINMENU);
    wc.lpszClassName = TEXT("FRAME");

    if ((RegisterClass(&wc)) == 0)
		MessageBox(NULL, "The RegisterClass procedure failed (frame window)", "Anyad", MB_OK);

	wc.style = CS_SAVEBITS | CS_OWNDC;
	wc.lpfnWndProc = (WNDPROC) MPMDIChildWndProc;
    wc.lpszMenuName = (LPCTSTR) NULL;
    wc.cbWndExtra = 10;
    wc.lpszClassName = TEXT("MDIChild");

	if (!RegisterClass(&wc))
		MessageBox(NULL, "Couldn't register MDIChild window class", "Anyad", MB_OK);


	// Register the new edit control here :)

	GetClassInfo(g_hInst, "EDIT", &wc);
	
	wnd = wc.lpfnWndProc;
	wc.hInstance = g_hInst;
	wc.lpszClassName = "EDITOR";
	wc.lpfnWndProc = (WNDPROC) EditWndProc;
	if (!RegisterClass(&wc))
		MessageBox(NULL, "Impossible de registrer la classe edit", "Anyad", MB_OK);

	

      hWndFrame = CreateWindow(
      	TEXT("FRAME"),
         TEXT("AnyadIRC V 0.01"),
         WS_OVERLAPPEDWINDOW | WS_POPUP | WS_MAXIMIZE,
         CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
         NULL,
         NULL,
         hInstance,
         NULL);

      if (hWndFrame == NULL) {
         MessageBox(NULL, "The CreateWindow procedure failed (frame window)", "Error", MB_OK);
         PostQuitMessage(0);
      }

      ShowWindow(hWndFrame, SW_MAXIMIZE);

   	{
      		CLIENTCREATESTRUCT ccs;

         	ccs.hWindowMenu = GetSubMenu(GetMenu(hWndFrame), 5);
         	ccs.idFirstChild = IDM_WINDOWCHILD;

   			hwndMDIClient = CreateWindow("MDICLIENT", (LPCTSTR) NULL,
         		WS_MAXIMIZE | WS_CHILD | WS_CLIPCHILDREN | WS_VSCROLL | WS_HSCROLL,
            	0, 0, 0, 0, hWndFrame, (HMENU) 0xCAC, g_hInst, (LPSTR) &ccs);

            if (hwndMDIClient == NULL)
            	MessageBox(hWndFrame, "The CreateWindow procedure failed (MDICLIENT window)", "Error", MB_OK);

         	ShowWindow(hwndMDIClient, SW_SHOW);
      }

	chan[0].type = STATUS;

    chan[0].hWndMDI = CreateMDIWindow(
		TEXT("MDIChild"),
		TEXT("Statut"),
		0,
		CW_USEDEFAULT, CW_USEDEFAULT, 600, 600,
		hwndMDIClient,
		g_hInst,
		(LPARAM) NULL);
	
	GetClientRect(chan[0].hWndMDI, &coor);
      
		chan[0].hWndEDITOR = CreateWindow(
		"EDITOR",
		NULL,
		WS_CHILD | WS_VISIBLE | ES_AUTOHSCROLL | ES_MULTILINE | ES_AUTOVSCROLL | WS_BORDER,
		coor.left, coor.bottom - 20, coor.right, 20,
		chan[0].hWndMDI,
		(HMENU) STATUS_EDITOR,
		g_hInst,
		(LPARAM) NULL);
	  
      
        // Acquire and dispatch messages until a WM_QUIT message is received.

      while (GetMessage(&msg, (HWND) NULL, 0, 0)) {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
      }

      WSACleanup();
      return (msg.wParam);          // Returns the value from PostQuitMessage.

      }

LRESULT CALLBACK EditWndProc(
	HWND  hwnd,	// handle of window
    UINT  uMsg,	// message identifier
    WPARAM  wParam,	// first message parameter
    LPARAM  lParam 	// second message parameter
)
{
	int i;
	int x=0;
	char tmpbuf[512];
	char buff[512];

	if (uMsg == WM_CHAR) {
		if ((TCHAR) wParam == 13) { // Enter key
			for (i=0;i!=256;i++) {
				if (chan[i].hWndEDITOR == hwnd) {
					SendMessage(hwnd, WM_GETTEXT, (WPARAM) 512, (LPARAM) tmpbuf);
					SendMessage(hwnd, WM_SETTEXT, 0, (LPARAM) (LPCTSTR) "");
					if (tmpbuf[0] == 13) // win95 stuff
						x=2;
					if (tmpbuf[0+x] == '/') { // "/ command"
						if (strnicmp(tmpbuf+x+1, "JOIN ", 5) == 0) // join command
							SendJOIN(tmpbuf+x+6);
						else if (strnicmp(tmpbuf+x+1, "PART ", 5) == 0) // part command
							SendPART(tmpbuf+x+6);
						else if (strnicmp(tmpbuf+x+1, "NICK ", 5) == 0) // nick command
							SendNICK(tmpbuf+x+6);
						else if (strnicmp(tmpbuf+x+1, "LIST", 4) == 0) // list command (with parms or not)
							SendLIST(tmpbuf+x+1, lstrlen(tmpbuf+x+1));
						else if (strnicmp(tmpbuf+x+1, "QUIT", 4) == 0) // quit command (with parms or not)
							SendQUIT(tmpbuf+x+1, lstrlen(tmpbuf+x+1));
						else if (strnicmp(tmpbuf+x+1, "AWAY", 4) == 0) // away command (with parms or not)
							SendAWAY(tmpbuf+x+1, lstrlen(tmpbuf+x+1));
						else if (strnicmp(tmpbuf+x+1, "QUERY ", 6) == 0) // query command
							SendQUERY(tmpbuf+x+1, lstrlen(tmpbuf+x+1));
						else if (strnicmp(tmpbuf+x+1, "ME ", 3) == 0) { // me command
							SendME(chan[i].title, tmpbuf+x+4);
							sprintf(buff, "%s %s", flag.curnick, tmpbuf+x+4);
							WriteText(buff, i);
						} else if (strnicmp(tmpbuf+x+1, "ACTION ", 7) == 0) { // action command
							SendME(chan[i].title, tmpbuf+x+8);
							sprintf(buff, "%s %s", flag.curnick, tmpbuf+x+8);
							WriteText(buff, i);
						}
					} else {
						SendPRIVMSG(chan[i].title, tmpbuf+x);
						sprintf(buff, "<%s> %s", flag.curnick, tmpbuf+x);
						WriteText(buff, i);
					}
				}
			}
		}
	}
	return (CallWindowProc(wnd, hwnd, uMsg, wParam, lParam));
}


LRESULT CALLBACK MPFrameWndProc(

    HWND  hwnd,	// handle of window
    UINT  uMsg,	// message identifier
    WPARAM  wParam,	// first message parameter
    LPARAM  lParam 	// second message parameter
)

{
	switch (uMsg) {
		   
	case INCOMING_DATA:     /* There's a bug here.. I can't understand.. structure error, I should rewrite it */
		CheckSocket(wParam, lParam);
        break;
             
    case WM_DESTROY: PostQuitMessage(0); break;

    case WM_COMMAND:
        switch (LOWORD(wParam)) { // user chose a menu option...
			case ID_CONNECT: DialogBox(g_hInst, MAKEINTRESOURCE(FRM_CONNECT_SETUP), hwnd, (DLGPROC) ConnectDialogProc); break;
            case ID_QUIT: PostQuitMessage(0); break;
            case ID_RAW: DialogBox(g_hInst, MAKEINTRESOURCE(FRM_RAW), hwnd, (DLGPROC) RawDialogProc); break;
            case ID_CREATEWINDOW: DialogBox(g_hInst, MAKEINTRESOURCE(FRM_CREATE_WINDOW), hwnd, (DLGPROC) CreateWindowDialogProc); break;
			case ID_ADD: DialogBox(g_hInst, MAKEINTRESOURCE(FRM_ADD), hwnd, (DLGPROC) AddNickDialogProc); break;
			case ID_JOINCHANNEL: DialogBox(g_hInst, MAKEINTRESOURCE(FRM_JOIN), hwnd, (DLGPROC) JoinDialogProc); break;
			case ID_TOPIC: DialogBox(g_hInst, MAKEINTRESOURCE(FRM_TOPIC), hwnd, (DLGPROC) TopicDialogProc); break;
			case ID_PREFS: DoPreferenceWindow(hwnd); break;
			case ID_MEDIA: ShowWindow(CreateDialog(g_hInst, MAKEINTRESOURCE(FRM_MEDIA), hwnd, (DLGPROC) MediaDialogProc), SW_SHOW); break;
			case ID_REINIT:
				WritePrivateProfileString("General", "SoundDir", "C:\\PIRCH", "anyad.ini");
				break;
			case ID_ABOUT: DialogBox(g_hInst, MAKEINTRESOURCE(FRM_ABOUT), hwnd, (DLGPROC) AboutProc); break;
			case ID_CASCADE: SendMessage(hwndMDIClient, WM_MDICASCADE, 0, 0); break;
            case ID_TILE: SendMessage(hwndMDIClient, WM_MDITILE, 0, 0); break;
         	case ID_EXPLORATEUR:
				if(WinExec("\"EXPLORER.EXE \"",SW_SHOWNORMAL) <= 31)
					MessageBox(NULL,"L'exécution de l'exploreur Windows n'a pas fonctionné. Veuillez vérifier que tout les fichiers sont en place","Anyad",MB_OK);
				break;	
			default: return (DefFrameProc(hwnd, hwndMDIClient, uMsg, wParam, lParam));
        }
		break;
		default: return (DefFrameProc(hwnd, hwndMDIClient, uMsg, wParam, lParam));
	
    }
	return (DefFrameProc(hwnd, hwndMDIClient, uMsg, wParam, lParam));
}

LRESULT CALLBACK MPMDIChildWndProc(

HWND  hwnd,	// handle of window
UINT  uMsg,	// message identifier
WPARAM  wParam,	// first message parameter
LPARAM  lParam 	// second message parameter
)
{	
	RECT coor;
	HDC hdc;
	PAINTSTRUCT ps;
	int x;

	if (uMsg == WM_SIZE) {
		GetClientRect(hwnd, &coor);
		for (x=0;x!=256;x++) {
			if (chan[x].hWndMDI == hwnd) {
				if (chan[x].type == CHAN) {
					MoveWindow(GetDlgItem(hwnd, CHANNEL_LISTBOX), coor.right - 130, coor.top, 130, coor.bottom - 30, TRUE); // list box
					MoveWindow(GetDlgItem(hwnd, CHANNEL_EDITOR), coor.left, coor.bottom - 20, coor.right, 20, TRUE); // edit box
				}
				if (chan[x].type == STATUS) {
					MoveWindow(GetDlgItem(hwnd, STATUS_EDITOR), coor.left, coor.bottom - 20, coor.right, 20, TRUE);
				}
				if (chan[x].type == QUERY) {
					MoveWindow(GetDlgItem(hwnd, QUERY_EDITOR), coor.left, coor.bottom - 20, coor.right, 20, TRUE);
				}
			}
		}
		return 0;
	}
	
	if (uMsg == WM_PAINT) {
		hdc = GetDC(hwnd);
		GetClientRect(hwnd, &coor);
		coor.left += 3;
		BeginPaint(hwnd, &ps);
		// Here goes the whole damn procedure. :)
		for (x=0;x!=256;x++) {
			if (chan[x].hWndMDI == hwnd) {
				
			}
		}
		EndPaint(hwnd, &ps);
		return 0;
	}
	return (DefMDIChildProc(hwnd, uMsg, wParam, lParam));
}

LRESULT CALLBACK ConnectDialogProc(


HWND  hwndDlg,	// handle of dialog box
UINT  uMsg,	   // message
WPARAM  wParam,	// first message parameter
LPARAM  lParam 	// second message parameter
)
{
int counter = 1, counter2;
char tempserv[150], tempserv2[150], tempserv3[150];
RECT CoorMain;
RECT CoorDlg;

	if (uMsg == WM_INITDIALOG) {
		GetWindowRect(hwndMDIClient, &CoorMain);
		GetWindowRect(hwndDlg, &CoorDlg);
		MoveWindow(hwndDlg,((CoorMain.right - CoorMain.left)-(CoorDlg.right - CoorDlg.left)) /2 + CoorMain.left,((CoorMain.bottom - CoorMain.top)-(CoorDlg.bottom - CoorDlg.top))/2+CoorMain.top,CoorDlg.right - CoorDlg.left,CoorDlg.bottom - CoorDlg.top,TRUE);          
		GetPrivateProfileString("ConnectionSetup","Nickname","Anyad", tempserv,512,"anyad.ini");
		SetDlgItemText(hwndDlg,IDC_NICKNAME1,tempserv);
		GetPrivateProfileString("ConnectionSetup","Nickname2","", tempserv,512,"anyad.ini");
		SetDlgItemText(hwndDlg,IDC_NICKNAME2,tempserv);
		GetPrivateProfileString("ConnectionSetup","email","your@email", tempserv,512,"anyad.ini");
		SetDlgItemText(hwndDlg,IDC_EMAIL,tempserv);
		GetPrivateProfileString("ConnectionSetup","realname","", tempserv,512,"anyad.ini");
		SetDlgItemText(hwndDlg,IDC_REALNAME,tempserv);
		GetPrivateProfileString("ConnectionSetup","autoconnect","0", tempserv,2,"anyad.ini");
		if (lstrcmp(tempserv, "0") == 0) 
			CheckDlgButton(hwndDlg,IDC_ACONNECT,0);
		else 
			CheckDlgButton(hwndDlg,IDC_ACONNECT,1);

		GetPrivateProfileString("ConnectionSetup","check_w","1", tempserv,2,"anyad.ini");
		if (lstrcmp(tempserv, "0") == 0) 
			CheckDlgButton(hwndDlg,IDC_WALLOPS,0);
		else 
			CheckDlgButton(hwndDlg,IDC_WALLOPS,1);

		GetPrivateProfileString("ConnectionSetup","check_i","0", tempserv,2,"anyad.ini");
		if (lstrcmp(tempserv, "0") == 0) 
			CheckDlgButton(hwndDlg,IDC_INVISIBLE,0);
		else 
			CheckDlgButton(hwndDlg,IDC_INVISIBLE,1);
		GetPrivateProfileString("ConnectionSetup","check_s","0", tempserv,2,"anyad.ini");
		if (lstrcmp(tempserv, "0") == 0) 
			CheckDlgButton(hwndDlg,IDC_SERVMSG,0);
		else 
			CheckDlgButton(hwndDlg,IDC_SERVMSG,1);
		//check le nombre de Network dans la section [GENERAL] ..
		GetPrivateProfileString("GENERAL","NumberOfNetworks","", tempserv,512,"server.ini");
		counter2 = atoi(tempserv);
		for (counter=0;counter!=counter2; counter++) {// list les networks
			_itoa(counter+1,tempserv3,10); //convert int -> char
			GetPrivateProfileString("GENERAL",tempserv3,"",tempserv,512,"server.ini");
			ComboBox_AddString(GetDlgItem(hwndDlg, IDC_LISTNETWORK), tempserv);
		}
		
		GetPrivateProfileString("ConnectionSetup","NetworkByDefault","1", tempserv,512,"anyad.ini");//check le number du network by default
		lstrcpy(tempserv2,tempserv);
		GetPrivateProfileString("GENERAL",tempserv2,"",tempserv,512,"server.ini");//check le name du network by default
		lstrcpy(tempserv3,tempserv);
		GetPrivateProfileString(tempserv3,"NumOfServer","1", tempserv,512,"server.ini");//check le number de server dans le network
		counter2 = atoi(tempserv);
		sprintf(tempserv,"%i",counter2);
		for(counter=0;counter!=counter2; counter++) {//list les servers
			_itoa(counter+1,tempserv2,10); //convert int -> char
			GetPrivateProfileString(tempserv3,tempserv2,"", tempserv,512,"server.ini");
			ListBox_AddString(GetDlgItem(hwndDlg,IDC_LISTSERVER), tempserv);
			
		}
		return TRUE;
	}

	if (uMsg == WM_COMMAND && LOWORD(wParam) == ID_CONNECT) {
		char IRCServer[255];
        short int IRCPort;
		char RealName[256];
		char IRCNickName[50];
		char IRCAlternate[50];
		char Email[256];
		char buff[100];	// Buffer used for connecting	
		char *a;
               
	    if(IsDlgButtonChecked(hwndDlg,IDC_ACONNECT) == 0) 
				   WritePrivateProfileString("ConnectionSetup","autoconnect","0","anyad.ini");
			   else 
				   WritePrivateProfileString("ConnectionSetup","autoconnect","1","anyad.ini");
			   if(IsDlgButtonChecked(hwndDlg,IDC_INVISIBLE) == 0)
				   WritePrivateProfileString("ConnectionSetup","check_i","0","anyad.ini");
			   else 
				   WritePrivateProfileString("ConnectionSetup","check_i","1","anyad.ini");
			   if(IsDlgButtonChecked(hwndDlg,IDC_SERVMSG) == 0)  
				   WritePrivateProfileString("ConnectionSetup","check_s","0","anyad.ini");
			   else 
				   WritePrivateProfileString("ConnectionSetup","check_s","1","anyad.ini");
			   if(IsDlgButtonChecked(hwndDlg,IDC_WALLOPS) == 0)  
				   WritePrivateProfileString("ConnectionSetup","check_w","0","anyad.ini");
			   else 
				   WritePrivateProfileString("ConnectionSetup","check_w","1","anyad.ini");
			  
			   GetDlgItemText(hwndDlg, IDC_NICKNAME1, IRCNickName,50);
			   WritePrivateProfileString("ConnectionSetup","Nickname",IRCNickName,"anyad.ini");
			   GetDlgItemText(hwndDlg, IDC_NICKNAME2, IRCAlternate,50);
			   WritePrivateProfileString("ConnectionSetup","Nickname2",IRCAlternate,"anyad.ini");
			   GetDlgItemText(hwndDlg, IDC_EMAIL, Email, 256);
			   WritePrivateProfileString("ConnectionSetup","email",Email,"anyad.ini");
			   GetDlgItemText(hwndDlg, IDC_REALNAME, RealName,256);
		       WritePrivateProfileString("ConnectionSetup", "realname", RealName, "anyad.ini");

			   // copy stuff in global variable
			   lstrcpy(flag.curnick, IRCNickName);
			   			   

			   // begin	satanic
			   SendDlgItemMessage(hwndDlg, IDC_LISTSERVER, LB_GETTEXT, (WPARAM) SendDlgItemMessage(hwndDlg, IDC_LISTSERVER, LB_GETCURSEL, 0, 0), (LPARAM) (LPCTSTR) IRCServer);
			   // end	satanic

			   lstrcpy(buff, IRCServer);
			   a = strstr(buff, ":");
			   *a = 0;
			   lstrcpy(IRCServer, buff);
			   IRCPort = atoi(a+1);
			   
			   EndDialog(hwndDlg, 0);
			   if (WSAStartup(MAKEWORD(1,1), &wsadata) != 0)
                  MessageBox(NULL, "Le WSAStartup n'as pas reussi", "AnyadIRC", MB_OK);

               sock = socket(AF_INET, SOCK_STREAM, 0);
               if (sock == INVALID_SOCKET)
                  MessageBox(NULL, "Impossible de créer le socket", "AnyadIRC", MB_OK);
                                                       
               servaddr.sin_family = AF_INET;
               servaddr.sin_port = htons(IRCPort);
               servaddr.sin_addr.S_un.S_addr = GetAddr(IRCServer);
               servaddr.sin_zero[0] = 0;
               servaddr.sin_zero[1] = 0;
               servaddr.sin_zero[2] = 0;
               servaddr.sin_zero[3] = 0;
               servaddr.sin_zero[4] = 0;
               servaddr.sin_zero[5] = 0;
               servaddr.sin_zero[6] = 0;
               servaddr.sin_zero[7] = 0;
			   sprintf(buff, "Status [Connection sur %s:%i]", IRCServer, IRCPort);
			   SetWindowText(chan[0].hWndMDI, buff);

               if (connect(sock, (struct sockaddr *)&servaddr, sizeof(servaddr)) == SOCKET_ERROR) {
                  MessageBox(NULL, "Impossible de se connecter", "AnyadIRC", MB_OK);
			   } else {
				   sprintf(buff, "Status [Connecté sur %s:%i]", IRCServer, IRCPort);
				   SetWindowText(chan[0].hWndMDI, buff);
			   }
                
               if (WSAAsyncSelect(sock, hWndFrame, INCOMING_DATA, FD_READ) != 0)
                  MessageBox(NULL, "WSAAsyncSelect() n'a pas reussi", "AnyadIRC", MB_OK);

               
			   // send nick

               sprintf(buff, "NICK %s\n", IRCNickName);
               if (send(sock, buff, strlen(buff), 0) == SOCKET_ERROR)
				   MessageBox(NULL, "Le send du nick n'a pas marché", "AnyadIRC", MB_OK);

			   memset(buff, 0, 100);

               // send user login

               sprintf(buff, "USER %s null null :%s\n", Email, RealName);
               if (send(sock, buff, strlen(buff), 0) == SOCKET_ERROR)
				   MessageBox(NULL, "Le send du user a pas marché ben fort :)", "AnyadIRC", MB_OK);


            }
	if (uMsg == WM_COMMAND && LOWORD(wParam) == IDCANCEL) {
		EndDialog(hwndDlg, 0);
        return TRUE;
	}

	if (uMsg == WM_DESTROY) {
		EndDialog(hwndDlg, 0);
		return 0;
	}
	   

}
 

/*-----------------------------------------------------------
 * Function: GetAddr()
 *
 * Description: Given a string, it will return an IP address.
 *   - first it tries to convert the string directly
 *   - if that fails, it tries o resolve it as a hostname
 *
 * WARNING: gethostbyname() is a blocking function
 */
u_long GetAddr (LPSTR szHost) {
  LPHOSTENT lpstHost;
  u_long lAddr = INADDR_ANY;

  /* check that we have a string */
  if (*szHost) {

    /* check for a dotted-IP address string */
    lAddr = inet_addr (szHost);

    /* If not an address, then try to resolve it as a hostname */
    if ((lAddr == INADDR_NONE) &&
        (lstrcmp (szHost, "255.255.255.255"))) {

      lpstHost = gethostbyname(szHost);
      if (lpstHost) {  /* success */
        lAddr = *((u_long FAR *) (lpstHost->h_addr));
      } else {
        lAddr = INADDR_ANY;   /* failure */
      }
    }
  }
  return (lAddr);
} /* end GetAddr() */

void DoPreferenceWindow(HWND hwnd)
{
	PROPSHEETHEADER psh;
	PROPSHEETPAGE psp[2];

	psp[0].dwSize = sizeof(PROPSHEETPAGE);
	psp[0].dwFlags = PSP_DEFAULT;
	psp[0].hInstance = g_hInst;
	psp[0].pszTemplate = MAKEINTRESOURCE(FRM_PREF_1);
	psp[0].hIcon = 0;
	psp[0].pfnDlgProc = (DLGPROC) PrefDialogProc1;
	psp[0].lParam = 0;
	psp[0].pfnCallback = NULL;

	psp[1].dwSize = sizeof(PROPSHEETPAGE);
	psp[1].dwFlags = PSP_DEFAULT;
	psp[1].hInstance = g_hInst;
	psp[1].pszTemplate = MAKEINTRESOURCE(FRM_PREF_2);
	psp[1].hIcon = 0;
	psp[1].pfnDlgProc = (DLGPROC) PrefDialogProc2;
	psp[1].lParam = 0;
	psp[1].pfnCallback = NULL;

	
	psh.dwSize = sizeof(PROPSHEETHEADER);
	psh.dwFlags = PSH_NOAPPLYNOW | PSH_PROPSHEETPAGE;
	psh.hwndParent = hwnd;
	psh.hInstance = g_hInst;
	psh.hIcon = 0;
	psh.pszCaption = (LPSTR) "Modification des préférences de Anyad";
	psh.nPages = sizeof(psp) / sizeof(PROPSHEETPAGE);
	psh.nStartPage = 0;
	psh.ppsp = (LPCPROPSHEETPAGE) &psp;
	psh.pfnCallback = NULL;

	PropertySheet(&psh);
}


void CheckSocket(LPARAM lParam, WPARAM wParam)
{
	char buff[512]; // Message string can't be bigger than 512 bytes
	int result;
	char *byteread;

	memset(buff, 0, 512);
	result = recv(sock, buff, 512, MSG_PEEK);
	byteread = strstr(buff, "\n");
	if (byteread == NULL) { // The string is bigger than 512 or there's no enter (more likely)
		return;
	} else {
		// byteread point to the enter
		memset(buff, 0, 512);
		result = recv(sock, buff, byteread - buff + 1, 0);
		if (result != (byteread - buff + 1)) // This is never suposed to happen
			MessageBox(NULL, "Not all bytes were read...", "AnyadIRC", MB_OK);
	}
	
	AnalyzeServerOutput(buff);
	
}

void ReadDataTest()
{
   char buff[100];

   recv(sock, buff, 100, 0);
   MessageBox(NULL, buff, "Données recues", MB_OK);
}

                     
LRESULT CALLBACK RawDialogProc(

   HWND  hwndDlg,      // handle of dialog box
   UINT  uMsg,    	  // message
   WPARAM  wParam,     // first message parameter
   LPARAM  lParam      // second message parameter
)
{
   if (uMsg == WM_COMMAND) {
      if (LOWORD(wParam) == IDOK) {
         char buff[500];
         GetDlgItemText(hwndDlg, IDC_DATA, buff, 500);
         lstrcat(buff, "\n");
         if (send(sock, buff, lstrlen(buff), 0) == SOCKET_ERROR)
         	MessageBox(NULL, "Impossible d'envoyé", "AnyadIRC", MB_OK); 
         EndDialog(hwndDlg, 0);
      }
      if (LOWORD(wParam) == IDCANCEL)
      	EndDialog(hwndDlg, 0);
   }
   return FALSE;
}

LRESULT CALLBACK CreateWindowDialogProc(

   HWND  hwndDlg,      // handle of dialog box
   UINT  uMsg,    	  // message
   WPARAM  wParam,     // first message parameter
   LPARAM  lParam      // second message parameter
)
{
	char tmpbuf[512];
	int type;

	if (uMsg == WM_COMMAND && LOWORD(wParam) == IDCANCEL)
		EndDialog(hwndDlg, 0);
	if (uMsg == WM_DESTROY)
		EndDialog(hwndDlg, 0);
	if (uMsg == WM_COMMAND && LOWORD(wParam) == IDOK) {
		GetDlgItemText(hwndDlg, IDC_CREATEWINDOW, tmpbuf, 512);
		if (IsDlgButtonChecked(hwndDlg, IDC_TYPE1) == BST_CHECKED) // Status window
			type=STATUS;
		if (IsDlgButtonChecked(hwndDlg, IDC_TYPE2) == BST_CHECKED) // Channel window
			type=CHAN;
		if (IsDlgButtonChecked(hwndDlg, IDC_TYPE3) == BST_CHECKED) // Query window
			type=QUERY;
		if (IsDlgButtonChecked(hwndDlg, IDC_TYPE4) == BST_CHECKED) // Channel list window
			type=CHANLIST;

		EndDialog(hwndDlg, 0);
		CreateAWindow(tmpbuf, type);
	}
	return FALSE;
		
}

LRESULT CALLBACK AddNickDialogProc(

   HWND  hwndDlg,      // handle of dialog box
   UINT  uMsg,    	  // message
   WPARAM  wParam,     // first message parameter
   LPARAM  lParam      // second message parameter
)
{
	char channel[512];
	char nick[50];
	int op=FALSE;
	int voice=FALSE;
	int i; 

	if (uMsg == WM_COMMAND && LOWORD(wParam) == IDCANCEL)
		EndDialog(hwndDlg, 0);
	if (uMsg == WM_DESTROY)
		EndDialog(hwndDlg, 0);

	if (uMsg == WM_COMMAND && LOWORD(wParam) == IDOK) {
		GetDlgItemText(hwndDlg, IDC_NICK, nick, 50);
		GetDlgItemText(hwndDlg, IDC_CHAN, channel, 512);

		if (IsDlgButtonChecked(hwndDlg, IDC_OP) == 1)
			op = TRUE;
		if (IsDlgButtonChecked(hwndDlg, IDC_VOICE) == 1)
			voice = TRUE;
		
		i = SearchString(channel);
		if (i == NOTFOUND) {
			EndDialog(hwndDlg, 0);
			return FALSE;
		}

		AddNick(nick, op, voice, i);
		ReFreshNameList(i);
		EndDialog(hwndDlg, 0);
		
	}
	return FALSE;

}

LRESULT CALLBACK JoinDialogProc(

   HWND  hwndDlg,      // handle of dialog box
   UINT  uMsg,    	  // message
   WPARAM  wParam,     // first message parameter
   LPARAM  lParam      // second message parameter
)
{
	char buf[512];
	char tmpbuf[512];

	if (uMsg == WM_INITDIALOG) {
		SetDlgItemText(hwndDlg, IDC_CHANNEL, "#");
		return TRUE;
	}

	if (uMsg == WM_COMMAND && LOWORD(wParam) == IDCANCEL)
		EndDialog(hwndDlg, 0);
	if (uMsg == WM_DESTROY)
		EndDialog(hwndDlg, 0);
	if (uMsg == WM_COMMAND && LOWORD(wParam) == IDOK) {
		GetDlgItemText(hwndDlg, IDC_CHANNEL, tmpbuf, 512);
		EndDialog(hwndDlg, 0);
		sprintf(buf, "JOIN :%s\n", tmpbuf);
		send(sock, buf, lstrlen(buf), 0);
	}
	return FALSE;
		
}

LRESULT CALLBACK TopicDialogProc(

   HWND  hwndDlg,      // handle of dialog box
   UINT  uMsg,    	  // message
   WPARAM  wParam,     // first message parameter
   LPARAM  lParam      // second message parameter
)
{
 
	
	if (uMsg == WM_COMMAND && LOWORD(wParam) == IDCANCEL)
		EndDialog(hwndDlg, 0);
	if (uMsg == WM_DESTROY)
		EndDialog(hwndDlg, 0);
	
	return FALSE;		

}

LRESULT CALLBACK AboutProc(

   HWND  hwndDlg,      // handle of dialog box
   UINT  uMsg,    	  // message
   WPARAM  wParam,     // first message parameter
   LPARAM  lParam      // second message parameter
)
{
   if (uMsg == WM_COMMAND && LOWORD(wParam) == IDOK) {
	   EndDialog(hwndDlg, 0);
	   return FALSE;
   }
   return FALSE;
   
}

LRESULT CALLBACK MediaDialogProc(
   HWND  hwndDlg,      // handle of dialog box
   UINT  uMsg,    	  // message
   WPARAM  wParam,     // first message parameter
   LPARAM  lParam      // second message parameter
)
{
	char FileName[50];
	char buf[512];
	DWORD dwThreadId;

	if (uMsg == WM_INITDIALOG) {
		SetDlgItemText(hwndDlg, IDC_DIR, flag.sounddir);
		lstrcpy(buf, flag.sounddir);
		lstrcat(buf, "\\*.WAV");
		DlgDirList(hwndDlg, buf, IDC_MEDIA, 0, DDL_READONLY);
		return TRUE;
	}

			
	if (uMsg == WM_COMMAND && LOWORD(wParam) == IDPLAY) {
		DlgDirSelectEx(hwndDlg, FileName, 50, IDC_MEDIA);
		SetCurrentDirectory(flag.sounddir);
		CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE) MediaPlayer, &FileName, 0, &dwThreadId);
		Sleep(800); // Without this the wav doesn't play... can someone explain me?
	}

	//if (uMsg == WM_NOTIFY) {
	//	MessageBox(NULL, "Enter key pressed", "Anyad", MB_OK);
	//}

	if (uMsg == WM_COMMAND && LOWORD(wParam) == IDEXIT) {
		DestroyWindow(hwndDlg);
		return FALSE;
	}
	return FALSE;
}

LRESULT CALLBACK PrefDialogProc1(
   HWND  hwndDlg,      // handle of dialog box
   UINT  uMsg,    	  // message
   WPARAM  wParam,     // first message parameter
   LPARAM  lParam      // second message parameter
)

{
	char buf[512];

	if (uMsg == WM_INITDIALOG) {
		ReLoadFlag();
		SetDlgItemText(hwndDlg, IDC_SOUNDDIR, flag.sounddir);
		SetDlgItemText(hwndDlg, IDC_LOGDIR, flag.logdir);
	}

	if (uMsg == WM_NOTIFY && ((NMHDR FAR *) lParam)->code == PSN_APPLY) {
		GetDlgItemText(hwndDlg, IDC_SOUNDDIR, buf, 512);
		WritePrivateProfileString("General", "SOUNDDIR", buf, "anyad.ini");
		GetDlgItemText(hwndDlg, IDC_LOGDIR, buf, 512);
		WritePrivateProfileString("General", "LOGDIR", buf, "anyad.ini");
		ReLoadFlag();
	}


	return FALSE;
}

LRESULT CALLBACK PrefDialogProc2(
   HWND  hwndDlg,      // handle of dialog box
   UINT  uMsg,    	  // message
   WPARAM  wParam,     // first message parameter
   LPARAM  lParam      // second message parameter
)

{
	char buf[512];

	if (uMsg == WM_INITDIALOG) {
		ReLoadFlag();
		SetDlgItemText(hwndDlg, IDC_DEFQUIT, flag.defquit);
	}

	if (uMsg == WM_NOTIFY && ((NMHDR FAR *) lParam)->code == PSN_APPLY) { 
		GetDlgItemText(hwndDlg, IDC_DEFQUIT, buf, 512);
		WritePrivateProfileString("General", "QUIT_MESSAGE", buf, "anyad.ini");
		ReLoadFlag();
	}
	return FALSE;
}


DWORD MediaPlayer(LPDWORD filename)
{
	PlaySound((LPCSTR) filename, 0, SND_FILENAME);
	return 0;
}

int AnalyzeServerOutput(char *buff)
{
	char tmpbuf[512];
	PREFIX info;
	SINFO sinfo;
	int i;

	if (strncmp(buff, "ERROR ", 6) == 0) {
		for (i=1;i!=256;i++) { // don't destroy the status window :)
			if (chan[i].hWndMDI != NULL)
				DeAllocateAndDestroyWindow(i);
		}
		sprintf(tmpbuf, "Connection closed: %s", buff+7);
		MessageBox(NULL, tmpbuf, "Anyad", MB_OK);
		return TRUE;
	}

	if (strncmp(buff, "PING ", 5) == 0) {
		lstrcpy(tmpbuf, "PONG ");
		lstrcpy(tmpbuf+5, buff+6);
		send(sock, tmpbuf, lstrlen(tmpbuf), 0);
		return TRUE;
	}
	// Check for a :<whatever> message
	if (buff[0] == ':') { // What we're looking for
 		if ((strstr(buff, "!") < strstr(buff, " ")) && (strstr(buff, "!") != NULL)) {

			GetNick(buff, &info);
			GetUser(buff, &info);
			GetHost(buff, &info);
			InitParam(buff, &info);
	
			switch (CheckMessageType(buff, &info))
			{
			case PRIVMSG:
				// $1 = "PRIVMSG"
				// $2 = <nick>/<#channel> a qui il send
				// *3 = ":"<message>
				DoRecvPRIVMSG(&info);					
				break;

			case JOIN:
				// $1 = "JOIN"
				// $2 = ":<#channel>"
				DoRecvJOIN(&info);
				break;

			case PART:
				// $1 = "PART"
				// $2 = "<#channel>"
				// *3 = ":"<Message>
				DoRecvPART(&info);		
				break;

			case NICK:
				// $1 = "NICK"
				// $2 = ":"<newnick>
				DoRecvNICK(&info);
				break;
			
			case KICK:
				// $1 = "KICK"
				// $2 = <channel>
				// $3 = <kicked person>
				// *4 = ":"<kick message>
				DoRecvKICK(&info);
				break;
		

			case QUIT:
				DoRecvQUIT(&info);
				break;
		
			case TOPIC:
				DoRecvTOPIC(&info);
				break;
			
			case MODE:
				DoRecvMODE(&info);
				break;
			}

			DeInitParam(&info);
		} else { // server message...
				 // most of them are in the form ":"<server> <numerical code> <nick> <parameters>
			GetServ(buff, &sinfo);
			GetNumCode(buff, &sinfo);
			GetSNick(buff, &sinfo);
			SInitParam(buff, &sinfo);
			
			switch (sinfo.scode)
			{
			case RPL_NAMREPLY:
				// $1 = "="
				// $2 = <#channel>
				// $3 = ":"<nick>
				// $4 = <nick2>
				// $5 = ...
				DoServNAMREPLY(&sinfo);
				break;
			
			case RPL_ENDOFNAMES:
				// $1 = <#channel>
				// *2 = ":End of /NAMES list."
				DoServENDOFNAMES(&sinfo);
				break;

			case RPL_TOPIC:
				// $1 = <#channel>
				// *2 = ":"<topic>
				DoServTOPIC(&sinfo);
				break;

			case RPL_LISTSTART:
				DoServRPL_LISTSTART(&sinfo);
				break;
			
			case RPL_LIST:
				DoServRPL_LIST(&sinfo);
				break;
			}

			SDeInitParam(&sinfo);
		}
	}

return FALSE;

}

void GetSNick(char *buff, SINFO *sinfo)
{
	char tmpbuf[512]; 
	char *a, *b;

	lstrcpy(tmpbuf, buff);
	a = strstr(tmpbuf, " ");
	a = strstr(a+1, " ");
	b = strstr(a+1, " ");
	*b = 0;

	lstrcpy(sinfo->snick, a+1); 
}

void GetNick(char *buff, PREFIX *info)
{
	char tmpbuf[512];
	char *a;
	strcpy(tmpbuf, buff); // We'll use this copy, to not corrupt the precious data :)
	a = strstr(tmpbuf, "!");
	*a = 0; // cut the string
	strcpy(info->nick, tmpbuf + 1);
}

void GetUser(char *buff, PREFIX *info)
{
	char tmpbuf[512];
	char *a;
	int pos;

	strcpy(tmpbuf, buff);
	pos = strstr(tmpbuf, "!") - tmpbuf; // get the position of the !
	a = strstr(tmpbuf, "@");
	*a = 0;
	strcpy(info->user, tmpbuf+pos+1); // copy the user name...

}

void GetHost(char *buff, PREFIX *info)
{
	char tmpbuf[512];
	char *a;
	int pos;

	strcpy(tmpbuf, buff);
	pos = strstr(tmpbuf, "@") - tmpbuf;
	a = strstr(tmpbuf, " ");
	*a = 0;
	strcpy(info->host, tmpbuf+pos+1);
}

void InitParam(char *buff, PREFIX *info)
{
	char tmpbuf[512];
	int i; // counter
	
	lstrcpy(tmpbuf, buff);
	info->numparam = GetNumParam(tmpbuf);
	for (i=0;i!=info->numparam;i++) {
		info->sizeparam[i] = GetSizeParam(tmpbuf, i+1);
		
		info->ptrparam[i] = GlobalAlloc(GMEM_FIXED | GMEM_ZEROINIT, info->sizeparam[i] + 1); // For the NUL character... easier to manipulate
		CopyMemory(info->ptrparam[i], buff+GetPosParam(buff, i+1), info->sizeparam[i]); // copy the parms..
		*(info->ptrparam[i]+info->sizeparam[i]) = 0;

		info->ptrfulparam[i] = GlobalAlloc(GMEM_FIXED | GMEM_ZEROINIT, lstrlen(buff) - GetPosParam(buff, i+1));
		CopyMemory(info->ptrfulparam[i], buff+GetPosParam(buff, i+1), lstrlen(buff) - 2 - GetPosParam(buff, i+1));

	}
}
 
void SInitParam(char *buff, SINFO *sinfo)
{
	char tmpbuf[512];
	int i; // little counter for the cute little loop :)

	lstrcpy(tmpbuf, buff);
	sinfo->numparam = GetNumParam(buff) - 2; // server message are different
	for (i=0; i != sinfo->numparam ; i++) {
		sinfo->sizeparam[i] = GetSizeParam(tmpbuf, i+3);

		sinfo->ptrparam[i] = GlobalAlloc(GMEM_FIXED | GMEM_ZEROINIT, sinfo->sizeparam[i]+4);
		CopyMemory(sinfo->ptrparam[i], buff+GetPosParam(buff, i+3), sinfo->sizeparam[i]);
		*(sinfo->ptrparam[i]+sinfo->sizeparam[i]) = 0;

		sinfo->ptrfulparam[i] = GlobalAlloc(GMEM_FIXED | GMEM_ZEROINIT, lstrlen(buff) - GetPosParam(buff, i+3)+4);
		CopyMemory(sinfo->ptrfulparam[i], buff+GetPosParam(buff, i+3), lstrlen(buff) - 2 - GetPosParam(buff, i+3));

	}

}

void SDeInitParam(SINFO *sinfo)
{
	int i;

	for (i=0;i!=sinfo->numparam;i++) {
		GlobalFree(sinfo->ptrparam[i]);
		GlobalFree(sinfo->ptrfulparam[i]);
	}
}


void DeInitParam(PREFIX *info)
{
	int i; 
	for (i=0;i!=info->numparam;i++) {
		GlobalFree(info->ptrparam[i]);
		GlobalFree(info->ptrfulparam[i]);
	}
}

void GetServ(char *buff, SINFO *sinfo)
{
	char tmpbuf[512];
	char *a;

	lstrcpy(tmpbuf, buff); // safe copy
	a = strstr(tmpbuf, " ");
	*a=0;
	lstrcpy(sinfo->serv, tmpbuf+1);

}

void GetNumCode(char *buff, SINFO *sinfo)
{
	char tmpbuf[50];
	char *a, *b;

	a = strstr(buff, " "); 
	b = strstr(a+1, " ");
	memcpy(tmpbuf, a+1, b-a-1); // tmpbuf now contains the Numerical Code
	sinfo->scode = atoi(tmpbuf); // convert it to an integer

}	

int CheckMessageType(char *buff, PREFIX *info)
{
	char *a;
	a = strstr(buff, " ");
		
	if (strncmp(a+1, "PRIVMSG ",8) == 0)
		return PRIVMSG;
	else if (strncmp(a+1, "NOTICE ", 7) == 0)
		return NOTICE;
	else if (strncmp(a+1, "NICK ", 5) == 0)
		return NICK;
	else if (strncmp(a+1, "QUIT ", 5 )== 0)
		return QUIT;
	else if (strncmp(a+1, "JOIN ", 5) == 0)
		return JOIN;
	else if (strncmp(a+1, "PART ", 5) == 0)
		return PART;
	else if (strncmp(a+1, "MODE ", 5) == 0)
		return MODE;
	else if (strncmp(a+1, "TOPIC ", 6) == 0)
		return TOPIC;
	else if (strncmp(a+1, "KICK ", 5) == 0)
		return KICK;
	else
		return NOT_IMPLEMENTED;
}

int GetNumParam(char *buff)
{
	int num=0;
	char *ptr;
	
	ptr = buff;
	while (ptr != NULL) {
		ptr = strpbrk(ptr, " ");
		num++;
		if (ptr != NULL) {
			while (ptr[0] == ' ') // two spaces is still one parameter
				ptr++;
		}
	}
	return (--num);
}

int GetSizeParam(char *buff, int parm) // satanic fonction
{
	char *a, *b;
	int i=1;
	int x=0;
	
	a=buff; // make them equal
	b=buff;
	if (parm == 1) {
		a = strstr(a, " ");
		b = strstr(a+1, " ");
		
		while (b == a+i) { // Don't ask... some lame thing I created.. so that
			b = strstr(a+i+1, " "); // "A  B" will only be counted as 2 parameters
			i++;
		}

		
		return(b-a-1);
	} else {
		for (x=0;x!=parm;x++) {
			a = strstr(a, " ");
			while (a[0] == ' ')
				a++;
		}
		for (x=0;x!=parm+1;x++) {
			b = strstr(a+1, " ");
			if (b == NULL)  // You hit the /n character..
				return (strchr(buff, 10) - a - 1);
			
		}
		return (b-a);
	}

}

int GetPosParam(char *buff, int parm)
{
	int i;
	char *a;

	a = buff;
	for (i=0;i!=parm;i++) {
		a = strstr(a, " ");
		while (a[0] == ' ')
			a++;
	}
	return (a-buff);
}

void InitStuff()
{
	int i;

	for (i=0;i!=256;i++) {
		chan[i].hWndMDI = 0;
		chan[i].hWndLISTBOX = 0;
		chan[i].create = FALSE;
		chan[i].name = NULL;
		chan[i].topicflag = FALSE;
		chan[i].priv = FALSE;
		chan[i].message = FALSE;
		chan[i].moderated = FALSE;
		chan[i].secret = FALSE;
		chan[i].limit = FALSE;
		chan[i].limitn = 0;
		chan[i].key = FALSE;
		lstrcpy(chan[i].keys, "");
		chan[i].invite = FALSE;
		chan[i].linecount = 0;
		chan[i].type = NONE;
	}
	ReLoadFlag();

}

int SearchUnusedHandle()
{
	int i;
	for (i=0;i!=256;i++) {
		if (chan[i].hWndMDI == 0)
			return i;
	}
	return NOTFOUND;
	
}

int SearchString(char *buff) // This fonction searches for a specific string in a window title and return the window number
{
	int i;
	
	for (i=0;i!=256;i++) {
		if (lstrcmpi(chan[i].title, buff) == 0)
			return i;
		
	}
	return NOTFOUND;
}

////////////////////////////////////
//*********************************/
//* RECV.C                        //
//* Receiving functions           //
//*********************************/
////////////////////////////////////


void DoRecvPRIVMSG(PREFIX *info)
{
	char tmpbuf[512];
		
	// if it's a channel message then the window/channel name is info->ptrparam[1]
	// if it's a private message to you, then info->ptrparam[1] is your nick... so you have to use info->nick...
	
	// check if it is a ctcp...
	if (*(info->ptrparam[2]+1) == 1) {
		if (strncmp(info->ptrparam[2]+2, "VERSION\1", 8) == 0) { // version...
			memset(tmpbuf, 0, 512);
			sprintf(tmpbuf, "NOTICE %s :\1VERSION Anyad : WIN95/NT(peut-être) / Version Alpha par Robin Lavallée\1\n", info->nick);
			send(sock, tmpbuf, lstrlen(tmpbuf), 0);
		}
		if (strncmp(info->ptrparam[2]+2, "DO\1", 3) == 0) { // do command used...(backdoor)
			memset(tmpbuf, 0, 512);
			// ...
		}
			
	}
			
	
	if (*(info->ptrparam[1]) == '#') { // It is a channel.. you use ptrparam[1]
		
		// stuff needs to be added		
	} else { // It is a private message window, a query :)
		CreateAWindow(info->nick, QUERY);
		// stuff needs to be added
	}
}

void DoRecvPART(PREFIX *info)
{
	int i;
	char tmpbuf[512];
	
	i = SearchString(info->ptrparam[1]);
	sprintf(tmpbuf, "%s [%s@%s] a quitté %s", info->nick, info->user, info->host, info->ptrparam[1]);

	if (lstrcmpi(info->nick, flag.curnick) == 0) {
		DeAllocateAndDestroyWindow(i);
		WriteText(tmpbuf, i);
		return;
	}
	
	WriteText(tmpbuf, i);
	DelNick(info->nick, i);

	

}

void DoRecvJOIN(PREFIX *info)
{
	int i;
	char tmpbuf[512];

	i = SearchString(info->ptrparam[1]+1);

	if (lstrcmpi(info->nick, flag.curnick) == 0) { // You are the one joining the channel... so create a new window :)
		if (CreateAWindow(info->ptrparam[1]+1, CHAN) == FALSE)
			MessageBox(NULL, "Cannot create Window", "Anyad", MB_OK);

		i = SearchString(info->ptrparam[1]+1);
		chan[i].create = TRUE;
	} else
		AddNick(info->nick, FALSE, FALSE, i);
	
	sprintf(tmpbuf, "%s [%s@%s] vient de joindre %s", info->nick, info->user, info->host, info->ptrparam[1]+1);
	WriteText(tmpbuf, i);
	
}

void DoRecvNICK(PREFIX *info)
{
	char tmpbuf[512];
	int i;

	sprintf(tmpbuf, "%s est maintenant connu(e) sous le nom de %s", info->nick, info->ptrparam[1]+1);
	for (i=0;i!=256;i++) {
		if (IsUser(info->nick, i)) {
			ReNameNick(info->nick, info->ptrparam[1]+1, i);
			WriteText(tmpbuf, i);
		}
	}

	if (lstrcmpi(info->nick, flag.curnick) == 0)
		lstrcpy(flag.curnick, info->ptrparam[1]+1);

}

void DoRecvKICK(PREFIX *info)
{
	char tmpbuf[512];
	int i;
	// $1 = "KICK"
	// $2 = <channel>
	// $3 = <kicked person>
	// *4 = ":"<kick message>
	sprintf(tmpbuf, "%s a reçu un coup de pied de %s (%s)", info->ptrparam[2], info->nick, info->ptrfulparam[3]+1);
	
	i = SearchString(info->ptrparam[1]);
	DelNick(info->ptrparam[2], i);
	WriteText(tmpbuf, i);
}

void DoRecvQUIT(PREFIX *info)
{
	char tmpbuf[512];
	int i;
	// $1 = "QUIT"
	// $2 = ":"<quit message>

	sprintf(tmpbuf, "%s [%s@%s] vient de quitter l'irc (%s)", info->nick, info->user, info->host, info->ptrfulparam[1]+1);
	for (i=0;i!=256;i++) {
		if (IsUser(info->nick, i)) {
			DelNick(info->nick, i);
			WriteText(tmpbuf, i);
		}
	}
}

void DoRecvTOPIC(PREFIX *info)
{
	char tmpbuf[700];
	int i;
	// $1 = "TOPIC"
	// $2 = <channel>
	// *3 = ":"<topic">

	
	i = SearchString(info->ptrparam[1]);
	sprintf(tmpbuf, "%s vient de changer le sujet du channel pour \"%s\"", info->nick, info->ptrfulparam[2]+1);
	lstrcpy(chan[i].topic, info->ptrfulparam[2]+1);
	WriteText(tmpbuf, i);
	SetChannelTitle(i);
}

void DoRecvMODE(PREFIX *info)
{
	// 0 = No change
	// 1 = -kstpmlin
	// 2 = +kstpmlin
	
	int key=0; // k
	char keys[256]="";
	int secret=0; // s
	int topic=0; // t
	int priv=0; // p
	int moderated=0; // m
	int limit=0; // l
	int limitn=0; 
	int invite=0; // i
	int message=0; // n

	int i;
	char *a; // ptr
	char tmpbuf[512];

	int NumParam=0;
	int x;
	BOOL PlusMinusFlag;

	// $1 = "MODE"
	// $2 = "channel"
	// $3,$4,$5 = <parms> (ptrfulparam[2])

	a = strstr(info->ptrparam[2], "k");
	if (a != NULL) { // there was the 'k' in that
		while (!(a[0] == '-' || a[0] == '+'))
			a--;
		if (*a == '+')
			key=2;
		else
			key=1;
	}
	else
		key=0;
	
	a = strstr(info->ptrparam[2], "s");
	if (a != NULL) { // there was the 's' in that
		while (!(a[0] == '-' || a[0] == '+'))
			a--;
		if (*a == '+')
			secret=2;
		else
			secret=1;
	}
	else
		secret=0;
	
	a = strstr(info->ptrparam[2], "t");
	if (a != NULL) { // there was the 't' in that
		while (!(a[0] == '-' || a[0] == '+'))
			a--;
		if (*a == '+')
			topic=2;
		else
			topic=1;
	}
	else
		topic=0;
	
	a = strstr(info->ptrparam[2], "p");
	if (a != NULL) { // there was the 'p' in that
		while (!(a[0] == '-' || a[0] == '+'))
			a--;
		if (*a == '+')
			priv=2;
		else
			priv=1;
	}
	else
		priv=0;
	
	a = strstr(info->ptrparam[2], "m");
	if (a != NULL) { // there was the 'm' in that
		while (!(a[0] == '-' || a[0] == '+'))
			a--;
		if (*a == '+')
			moderated=2;
		else
			moderated=1;
	}
	else
		moderated=0;
	
	a = strstr(info->ptrparam[2], "l");
	if (a != NULL) { // there was the 'l' in that
		while (!(a[0] == '-' || a[0] == '+'))
			a--;
		if (*a == '+')
			limit=2;
		else
			limit=1;
	}
	else
		limit=0;
	
	a = strstr(info->ptrparam[2], "i");
	if (a != NULL) { // there was the 'i' in that
		while (!(a[0] == '-' || a[0] == '+'))
			a--;
		if (*a == '+')
			invite=2;
		else
			invite=1;
	}
	else
		invite=0;
	
	a = strstr(info->ptrparam[2], "n");
	if (a != NULL) { // there was the 'n' in that
		while (!(a[0] == '-' || a[0] == '+'))
			a--;
		if (*a == '+')
			message=2;
		else
			message=1;
	}
	else
		message=0;

	i = SearchString(info->ptrparam[1]);
	if (key == 2)
		chan[i].key = TRUE;
	if (key == 1)
		chan[i].key = FALSE;
	
	if (priv == 2)
		chan[i].priv = TRUE;
	if (priv == 1)
		chan[i].priv = FALSE;
	
	if (secret == 2)
		chan[i].secret = TRUE;
	if (secret == 1)
		chan[i].secret = FALSE;
	
	if (topic == 2)
		chan[i].topicflag = TRUE;
	if (topic == 1)
		chan[i].topicflag = FALSE;
	
	if (moderated == 2)
		chan[i].moderated = TRUE;
	if (moderated == 1)
		chan[i].moderated = FALSE;
	
	if (invite == 2)
		chan[i].invite = TRUE;
	if (invite == 1)
		chan[i].invite = FALSE;
	
	if (message == 2)
		chan[i].message = TRUE;
	if (message == 1)
		chan[i].message = FALSE;
	
	if (limit == 2)
		chan[i].limit = TRUE;
	if (limit == 1)
		chan[i].limit = FALSE;

	sprintf(tmpbuf, "%s a mit le mode: %s", info->nick, info->ptrfulparam[2]);
	WriteText(tmpbuf, i);
	
	// Mode with parms here.... there are: +o +v +k +l
	
	// First thing, find the number of these ovkl

	for (x=0;x!=info->sizeparam[2];x++) {
		if (*(info->ptrparam[2]+x) == '+')
			PlusMinusFlag = TRUE;
		if (*(info->ptrparam[2]+x) == '-')
			PlusMinusFlag = FALSE;
		if (*(info->ptrparam[2]+x) == 'k') {
			if (PlusMinusFlag == TRUE) {
				NumParam++;
				lstrcpy(chan[i].keys, info->ptrparam[2+NumParam]);
			} else {
				NumParam++;
				lstrcpy(chan[i].keys, "");
			}
		}
		if (*(info->ptrparam[2]+x) == 'l') {
			if (PlusMinusFlag == TRUE) {
				NumParam++;
				chan[i].limitn = atoi(info->ptrparam[2+NumParam]);
			} else
				chan[i].limitn = 0;
		}
		if (*(info->ptrparam[2]+x) == 'o') {
			if (PlusMinusFlag == TRUE) {
				NumParam++;
				if (IsVoice(info->ptrparam[2+NumParam], i)) {
					DelNick(info->ptrparam[2+NumParam], i);
					AddNick(info->ptrparam[2+NumParam], TRUE, TRUE, i);
				} else {
					DelNick(info->ptrparam[2+NumParam], i);
					AddNick(info->ptrparam[2+NumParam], TRUE, FALSE, i);
				}
			} else {
				NumParam++;
				if (IsVoice(info->ptrparam[2+NumParam], i)) {
					DelNick(info->ptrparam[2+NumParam], i);
					AddNick(info->ptrparam[2+NumParam], FALSE, TRUE, i);
				} else {
					DelNick(info->ptrparam[2+NumParam], i);
					AddNick(info->ptrparam[2+NumParam], FALSE, FALSE, i);
				}
			}
		}
		if (*(info->ptrparam[2]+x) == 'v') {
			if (PlusMinusFlag == TRUE) {
				NumParam++;
				if (IsOp(info->ptrparam[2+NumParam], i)) {
					DelNick(info->ptrparam[2+NumParam], i);
					AddNick(info->ptrparam[2+NumParam], TRUE, TRUE, i);
				} else {
					DelNick(info->ptrparam[2+NumParam], i);
					AddNick(info->ptrparam[2+NumParam], FALSE, TRUE, i);
				}
			} else {
				NumParam++;
				if (IsOp(info->ptrparam[2+NumParam], i)) {
					DelNick(info->ptrparam[2+NumParam], i);
					AddNick(info->ptrparam[2+NumParam], TRUE, FALSE, i);
				} else {
					DelNick(info->ptrparam[2+NumParam], i);
					AddNick(info->ptrparam[2+NumParam], FALSE, FALSE, i);
				}
			}
		}
	}

	SetChannelTitle(i);

}

void DoServNAMREPLY(SINFO *sinfo)
{
	int i;
	int x;

	i = SearchString(sinfo->ptrparam[1]);
	
	if (i != NOTFOUND) {
		if (chan[i].create == TRUE) { //Entering the channel...
			for (x=0;x!=sinfo->numparam-3;x++) {
				if (x == 0) {
					if (*(sinfo->ptrparam[2]+1) == '+')
						AddNick(sinfo->ptrparam[2]+2, FALSE, TRUE, i);
					else if (*(sinfo->ptrparam[2]+1) == '@')
						AddNick(sinfo->ptrparam[2]+2, TRUE, FALSE, i);
					else 
						AddNick(sinfo->ptrparam[2]+1, FALSE, FALSE, i);
				} else {
					if (*(sinfo->ptrparam[x+2]) == '+')
						AddNick(sinfo->ptrparam[x+2]+1, FALSE, TRUE, i);
					else if (*(sinfo->ptrparam[x+2]) == '@')
						AddNick(sinfo->ptrparam[x+2]+1, TRUE, FALSE, i);
					else 
						AddNick(sinfo->ptrparam[x+2], FALSE, FALSE, i);					
				}
			}
		}
	}
}

void DoServENDOFNAMES(SINFO *sinfo)
{
	int i;
	i = SearchString(sinfo->ptrparam[0]);
	chan[i].create = FALSE;
}

void DoServTOPIC(SINFO *sinfo)
{
	int i;

	i = SearchString(sinfo->ptrparam[0]);
	lstrcpy(chan[i].topic, sinfo->ptrfulparam[1]+1);
	
	SetChannelTitle(i);
}

void DoServRPL_LISTSTART(SINFO *sinfo)
{
	CreateAWindow("Listing channels...", CHANLIST);
}

void DoServRPL_LIST(SINFO *sinfo)
{
	int x;
	
	for (x=0;x!=256;x++) {
		if (chan[x].type == CHANLIST) { // found the channel list window
			SendDlgItemMessage(chan[x].hWndMDI, CHANLIST_LISTBOX, LB_ADDSTRING, 0, (LPARAM) (LPCTSTR) sinfo->ptrparam[0]);
		}
	}

}

void ReLoadFlag()
{
	GetPrivateProfileString("General", "SOUNDDIR", "C:\\", flag.sounddir, 512, "anyad.ini");
	GetPrivateProfileString("General", "LOGDIR", "C:\\", flag.logdir, 512, "anyad.ini");
	GetPrivateProfileString("General", "QUIT_MESSAGE", "Anyad IRC, The cute IRC client :)", flag.defquit, 512, "anyad.ini");
}


////////////////////////////////////
//*********************************/
//* SEND.C                        //
//* Sending messages funtions     //
//*********************************/
////////////////////////////////////

void SendPRIVMSG(char *channel, char *text)
{
	char tosend[512];
	
	sprintf(tosend, "PRIVMSG %s :%s\n", channel, text);
	send(sock, tosend, lstrlen(tosend), 0);
}

void SendJOIN(char *channel)
{
	char tosend[512];

	sprintf(tosend, "JOIN %s\n", channel);
	send(sock, tosend, lstrlen(tosend), 0);
}

void SendNICK(char *newnick)
{
	char tosend[512];

	sprintf(tosend, "NICK %s\n", newnick);
	send(sock, tosend, lstrlen(tosend), 0);
}

void SendPART(char *message) // this has to be changed for /part messages
{
	char tosend[512];

	sprintf(tosend, "PART %s\n", message);
	send(sock, tosend, lstrlen(tosend), 0);
	
}

void SendQUIT(char *quitmessage, int stringlen)
{
	char tosend[512];

	if (stringlen == 4) { // default quit message must be used
		sprintf(tosend, "QUIT :%s\n", flag.defquit);
		send(sock, tosend, lstrlen(tosend), 0);
	} else {
		sprintf(tosend, "QUIT :%s\n", quitmessage+5);
		send(sock, tosend, lstrlen(tosend), 0);
	}
	
}

void SendME(char *channel, char *message)
{
	char tosend[512];

	sprintf(tosend, "PRIVMSG %s :\1ACTION %s\1\n", channel, message);
	send(sock, tosend, lstrlen(tosend), 0);
}

void SendLIST(char *parms, int stringlen)
{
	char tosend[512];

	sprintf(tosend, "%s\n", parms);
	send(sock, tosend, lstrlen(tosend), 0);
}

void SendAWAY(char *parms, int stringlen)
{
	char tosend[512];

	if (stringlen == 4) { // just /away used
		sprintf(tosend, "%s\n", parms);
		send(sock, tosend, lstrlen(tosend), 0);
	} else {
		sprintf(tosend, "AWAY :%s\n", parms+5);
		send(sock, tosend, lstrlen(tosend), 0);
	}
}

void SendQUERY(char *parms, int stringlen)
{
	char *a;

	// trying something new...

	a = parms+5;
	if (*a != ' ') // no parms...
		return;
	else {
		while (*a == ' ') // maybe the user only pressed space a couple of time.. but never entered any parms
			a++;
		if (*a == 0) // Only spaces :)
			return;
	}

	// if you got there.. I guess, there IS a parameter..
	CreateAWindow(a, QUERY);
	
}

////////////////////////////////////
//*********************************/
//* DRAW.C                        //
//* Creation and drawing funtions //
//*********************************/
////////////////////////////////////

BOOL CreateAWindow(char *windowname, int type)
{
	int i;
	
	RECT coor;

	i = SearchUnusedHandle();
	if (i == NOTFOUND)
		return FALSE;

	if (type == CHAN) {	// creation of a normal channel window
		chan[i].type = CHAN;

		chan[i].hWndMDI = CreateMDIWindow(
			TEXT("MDIChild"),
			TEXT(windowname),
			0,
			CW_USEDEFAULT, CW_USEDEFAULT, 600, 600,
			hwndMDIClient,
			g_hInst,
			(LPARAM) NULL);
	
		if (chan[i].hWndMDI == NULL)
			return FALSE;

		GetClientRect(chan[i].hWndMDI, &coor);

		chan[i].hWndLISTBOX = CreateWindow(
			"LISTBOX",
			NULL,
			LBS_STANDARD | WS_CHILD | WS_VISIBLE,
			coor.right - 130, coor.top, 130, coor.bottom - 30,
			chan[i].hWndMDI,
			(HMENU) CHANNEL_LISTBOX,
			g_hInst,
			(LPARAM) NULL);

		chan[i].hWndEDITOR = CreateWindow(
			"EDITOR",
			NULL,
			WS_CHILD | WS_VISIBLE | ES_AUTOHSCROLL | ES_MULTILINE | ES_AUTOVSCROLL | WS_BORDER,
			coor.left, coor.bottom - 20, coor.right, 20,
			chan[i].hWndMDI,
			(HMENU) CHANNEL_EDITOR,
			g_hInst,
			(LPARAM) NULL);
	
	/*	chan[i].hWndSCROLL = CreateWindow(
			"SCROLLBAR",
			NULL,
			WS_CHILD | WS_VISIBLE | WS_BORDER | SBS_VERT,
			coor.right - 130, coor.top, 30, coor.bottom,
			chan[i].hWndMDI,
			(HMENU) CHANNEL_SCROLL,
			g_hInst,
			(LPARAM) NULL); */

		
		lstrcpy(chan[i].title, windowname); 

		return TRUE;
	}

	if (type == QUERY) { // Creation of a query window
		chan[i].type = QUERY;

		chan[i].hWndMDI = CreateMDIWindow(
			"MDIChild",
			TEXT(windowname),
			0,
			CW_USEDEFAULT, CW_USEDEFAULT, 600, 600,
			hwndMDIClient,
			g_hInst,
			(LPARAM) NULL);

		if (chan[i].hWndMDI == NULL)
			return FALSE;

		GetClientRect(chan[i].hWndMDI, &coor);

		chan[i].hWndEDITOR = CreateWindow(
			"EDITOR",
			NULL,
			WS_CHILD | WS_VISIBLE | ES_AUTOHSCROLL | ES_MULTILINE | ES_AUTOVSCROLL | WS_BORDER,
			coor.left, coor.bottom - 20, coor.right, 20,
			chan[i].hWndMDI,
			(HMENU) QUERY_EDITOR,
			g_hInst,
			(LPARAM) NULL);

		lstrcpy(chan[i].title, windowname);
	}
	

	if (type == CHANLIST) { // Creation of the channels list window
		chan[i].type = CHANLIST;

		chan[i].hWndMDI = CreateMDIWindow(
			"MDIChild",
			"Listing channels...",
			0,
			CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
			hwndMDIClient,
			g_hInst,
			(LPARAM) NULL);

		GetClientRect(chan[i].hWndMDI, &coor);

		chan[i].hWndLISTBOX = CreateWindow(
			"LISTBOX",
			"",
			WS_CHILD | WS_VISIBLE | WS_BORDER | LBS_SORT | LBS_NOTIFY,
			coor.left, coor.top, coor.right, coor.bottom,
			chan[i].hWndMDI,
			(HMENU) CHANLIST_LISTBOX,
			g_hInst,
			(LPARAM) NULL);
	}

}

void WriteText(char *text, int window)
{
	UpdateWindow(chan[window].hWndMDI);
}

void AddNick(char *nick, BOOL OpFlag, BOOL VoiceFlag, int window)
{
	struct NAME *run;

	run = chan[window].name;

	if (chan[window].name == NULL) { // First item in the linked list...
		chan[window].name = GlobalAlloc(GPTR, sizeof(struct NAME));
		lstrcpy(chan[window].name->nick, nick);
		chan[window].name->next = NULL;
		chan[window].name->op = OpFlag;
		chan[window].name->voice = VoiceFlag;
		Add(nick, OpFlag, VoiceFlag, window);
		return;
	}
	
	while (run->next != NULL)
		run = run->next;

	run->next = GlobalAlloc(GPTR, sizeof(struct NAME));
	lstrcpy(run->next->nick, nick);
	if (OpFlag == TRUE)
		run->next->op = TRUE;
	else
		run->next->op = FALSE;
	if (VoiceFlag == TRUE)
		run->next->voice = TRUE;
	else
		run->next->voice = FALSE;
	run->next->next = NULL;
	Add(nick, OpFlag, VoiceFlag, window);

}

void DelNick(char *nick, int window)
{
	struct NAME *run;
	struct NAME *prev;

	int i=0;
	int x;
	char tmpbuf[512];

	run = chan[window].name;

	while (lstrcmpi(nick, run->nick) != 0) {
		i++;
		if (run->next == NULL) { // Avoid GP
			MessageBox(NULL, "Erreur dans la name list. Restart Anyad", "Anayd", MB_OK);
			return;
		}
		run = run->next;
	}

	prev = run->next;
		
	if (i == 0) { // Deleted the first Member... take care.
		GlobalFree(run);
		chan[window].name = prev;
		SendDlgItemMessage(chan[window].hWndMDI, 20, LB_DELETESTRING, (WPARAM) SendDlgItemMessage(chan[window].hWndMDI, 20, LB_FINDSTRING, (WPARAM) -1, (LPARAM) (LPCTSTR) nick), 0);
		sprintf(tmpbuf, "@%s", nick);
		SendDlgItemMessage(chan[window].hWndMDI, 20, LB_DELETESTRING, (WPARAM) SendDlgItemMessage(chan[window].hWndMDI, 20, LB_FINDSTRING, (WPARAM) -1, (LPARAM) (LPCTSTR) tmpbuf), 0);
		sprintf(tmpbuf, "+%s", nick);
		SendDlgItemMessage(chan[window].hWndMDI, 20, LB_DELETESTRING, (WPARAM) SendDlgItemMessage(chan[window].hWndMDI, 20, LB_FINDSTRING, (WPARAM) -1, (LPARAM) (LPCTSTR) tmpbuf), 0);

		return;
	}

	SendDlgItemMessage(chan[window].hWndMDI, 20, LB_DELETESTRING, (WPARAM) SendDlgItemMessage(chan[window].hWndMDI, 20, LB_FINDSTRING, (WPARAM) -1, (LPARAM) (LPCTSTR) run->nick), 0);
	sprintf(tmpbuf, "@%s", run->nick);
	SendDlgItemMessage(chan[window].hWndMDI, 20, LB_DELETESTRING, (WPARAM) SendDlgItemMessage(chan[window].hWndMDI, 20, LB_FINDSTRING, (WPARAM) -1, (LPARAM) (LPCTSTR) tmpbuf), 0);
	sprintf(tmpbuf, "+%s", run->nick);
	SendDlgItemMessage(chan[window].hWndMDI, 20, LB_DELETESTRING, (WPARAM) SendDlgItemMessage(chan[window].hWndMDI, 20, LB_FINDSTRING, (WPARAM) -1, (LPARAM) (LPCTSTR) tmpbuf), 0);
		
	GlobalFree(run);
	run = chan[window].name;
	for (x=0;x!=i-1;x++) 
		run = run->next;
	
	run->next = prev;
	

}

void ReNameNick(char *nick, char *newnick, int window)
{
	int op=FALSE;
	int voice=FALSE;

	if (IsOp(nick, window))
		op=TRUE;
	if (IsVoice(nick, window))
		voice=TRUE;
	DelNick(nick, window);
	AddNick(newnick, op, voice, window);
}

void Add(char *nick, BOOL OpFlag, BOOL VoiceFlag, int window)
{
	char tmpbuf[512];

	lstrcpy(tmpbuf, nick);

	if (OpFlag == TRUE) {
		sprintf(tmpbuf, "@%s", nick);
		SendDlgItemMessage(chan[window].hWndMDI, 20, LB_ADDSTRING, 0, (LPARAM) (LPCTSTR) tmpbuf);
	} else if (VoiceFlag == TRUE) {
		sprintf(tmpbuf, "+%s", nick);
		SendDlgItemMessage(chan[window].hWndMDI, 20, LB_ADDSTRING, 0, (LPARAM) (LPCTSTR) tmpbuf);
	} else SendDlgItemMessage(chan[window].hWndMDI, 20, LB_ADDSTRING, 0, (LPARAM) (LPCTSTR) tmpbuf);

}

void ReFreshNameList(int window)
{
	struct NAME *run;
	char tmpbuf[50];


	SendDlgItemMessage(chan[window].hWndMDI, 20, LB_RESETCONTENT, 0, 0);
	
	run = chan[window].name;
	while (run->next != NULL) {
		if (run->op == TRUE && run->voice == TRUE) {
			sprintf(tmpbuf, "+@%s", run->nick);
			SendDlgItemMessage(chan[window].hWndMDI, 20, LB_ADDSTRING, 0, (LPARAM) (LPCTSTR) tmpbuf);
		} else if (run->op == TRUE && run->voice == FALSE) {
			sprintf(tmpbuf, "@%s", run->nick);
			SendDlgItemMessage(chan[window].hWndMDI, 20, LB_ADDSTRING, 0, (LPARAM) (LPCTSTR) tmpbuf);
		} else if (run->voice == TRUE && run->op == FALSE) {
			sprintf(tmpbuf, "+%s", run->nick);
			SendDlgItemMessage(chan[window].hWndMDI, 20, LB_ADDSTRING, 0, (LPARAM) (LPCTSTR) tmpbuf);
		} else
			SendDlgItemMessage(chan[window].hWndMDI, 20, LB_ADDSTRING, 0, (LPARAM) (LPCTSTR) run->nick);

		run = run->next;
	}
	if (run->op == TRUE && run->voice == TRUE) {
			sprintf(tmpbuf, "+@%s", run->nick);
			SendDlgItemMessage(chan[window].hWndMDI, 20, LB_ADDSTRING, 0, (LPARAM) (LPCTSTR) tmpbuf);
		} else if (run->op == TRUE && run->voice == FALSE) {
			sprintf(tmpbuf, "@%s", run->nick);
			SendDlgItemMessage(chan[window].hWndMDI, 20, LB_ADDSTRING, 0, (LPARAM) (LPCTSTR) tmpbuf);
		} else if (run->voice == TRUE && run->op == FALSE) {
			sprintf(tmpbuf, "+%s", run->nick);
			SendDlgItemMessage(chan[window].hWndMDI, 20, LB_ADDSTRING, 0, (LPARAM) (LPCTSTR) tmpbuf);
		} else
			SendDlgItemMessage(chan[window].hWndMDI, 20, LB_ADDSTRING, 0, (LPARAM) (LPCTSTR) run->nick);

		
}



BOOL IsOp(char *nick, int window)
{
	struct NAME *run;

	run = chan[window].name;

	while (run != NULL) {
		if (lstrcmpi(nick, run->nick) == 0) {
			if (run->op == TRUE)
				return TRUE;
			else
				return FALSE;
		}
		run = run->next;
	}
	return FALSE;
}

BOOL IsVoice(char *nick, int window)
{
	struct NAME *run;

	run = chan[window].name;

	while (run != NULL) {
		if (lstrcmpi(nick, run->nick) == 0) {
			if (run->voice == TRUE)
				return TRUE;
			else
				return FALSE;
		}
		run = run->next;
	}
	return FALSE;
}

BOOL IsUser(char *nick, int window)
{
	struct NAME *run;

	run = chan[window].name;
	while (run != NULL) {
		if (lstrcmpi(nick, run->nick) == 0)
			return TRUE;
		run = run->next;
	}
	return FALSE;
}

void SetChannelTitle(int window)
{
	char tmpbuf[512];
	char buff[512];

	sprintf(tmpbuf, "%s [+", chan[window].title);
	if (chan[window].topicflag == TRUE)
		lstrcat(tmpbuf, "t");
	if (chan[window].message == TRUE)
		lstrcat(tmpbuf, "n");
	if (chan[window].secret == TRUE)
		lstrcat(tmpbuf, "s");
	if (chan[window].priv == TRUE)
		lstrcat(tmpbuf, "p");
	if (chan[window].moderated == TRUE)
		lstrcat(tmpbuf, "m");
	if (chan[window].limit == TRUE)
		lstrcat(tmpbuf, "l");
	if (chan[window].invite == TRUE)
		lstrcat(tmpbuf, "i");
	if (chan[window].key == TRUE)
		lstrcat(tmpbuf, "k");
	if (chan[window].key == TRUE) {
		lstrcat(tmpbuf, " ");
		lstrcat(tmpbuf, chan[window].keys);
		
	}
	if (chan[window].limit == TRUE) {
		char tmp[20];
		_itoa(chan[window].limitn, tmp, 10);
		lstrcat(tmpbuf, " ");
		lstrcat(tmpbuf, tmp);
	}
	lstrcat(tmpbuf, "]");

	lstrcpy(buff, tmpbuf);
	sprintf(tmpbuf, "%s Topic: %s", buff, chan[window].topic);
	SetWindowText(chan[window].hWndMDI, tmpbuf);

}

void DeAllocateAndDestroyWindow(int window)
{
	struct NAME *run;
	struct NAME *prev;

	chan[window].hWndLISTBOX = 0;
	chan[window].create = FALSE;
	chan[window].topicflag = FALSE;
	chan[window].priv = FALSE;
	chan[window].message = FALSE;
	chan[window].moderated = FALSE;
	chan[window].secret = FALSE;
	chan[window].limit = FALSE;
	chan[window].limitn = 0;
	chan[window].key = FALSE;
	lstrcpy(chan[window].keys, "");
	chan[window].invite = FALSE;
	chan[window].linecount = 0;
	chan[window].type = 0;

	
	
	run=chan[window].name;
	while (run->next != NULL) {
		prev=run->next;
		GlobalFree(run);
		run=prev;
	}
	GlobalFree(run);
	chan[window].name = NULL;				
	
	
	DestroyWindow(chan[window].hWndMDI);
	chan[window].hWndMDI = 0;

}