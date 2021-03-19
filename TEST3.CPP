#include <windows.h>
#include <windowsx.h>
#include <commdlg.h>
#include "test3.h"
#include "resource.h"
#include <string.h>
#include <stdio.h>
#include <winsock.h>
#include <commctrl.h>
#include <stdlib.h>


struct sockaddr_in toaddr;
HWND hWndMain; 
HWND hWndEditText;
HWND hWndEdit;
HWND hWndEditCurrent;
char editbuff[1024];
HINSTANCE g_hInst;             
WSADATA WDATA;  //on verra ca apres pour plutard! ;)
SOCKET sock;   //on verra ca apres



int APIENTRY WinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance,
    LPSTR lpCmdLine, int nCmdShow )
{

    

    WNDCLASS  wc;
	RECT CoorXY;
	MSG msg;                         
    
    wc.style = 0;                      
    wc.lpfnWndProc = (WNDPROC)MainWndProc; 
    wc.cbClsExtra = 0;                  
    wc.cbWndExtra = 0;                  
    wc.hInstance = hInstance;           
    wc.hIcon = NULL;
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = GetStockObject(WHITE_BRUSH); 
    wc.lpszMenuName = MAKEINTRESOURCE(IDR_MENU1);
	// wc.lpszMenuName =  "IDR_MAINMENU";  
    wc.lpszClassName = TEXT("MAINW");

    if(RegisterClass(&wc) == NULL)
		MessageBox(NULL,"Le RegisterClass de wc n'a pas fonctionné..","Error", MB_OK + MB_ICONSTOP);



    g_hInst = hInstance;


    hWndMain = CreateWindow(
        TEXT("MAINW"),
        "-=- PSCHAT vAlpha -=-",
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT,                  
        CW_USEDEFAULT,                  
        CW_USEDEFAULT,                  
        CW_USEDEFAULT,                  
        NULL,                           
        NULL,                           
        g_hInst,                        
        NULL                            
    );


	


  ShowWindow(hWndMain, nCmdShow);  
    UpdateWindow(hWndMain);          

	GetWindowRect(hWndMain, &CoorXY);

/*	
	hWndEdit = CreateWindow("EDIT",TEXT(""),
	ES_MULTILINE | ES_WANTRETURN | WS_VSCROLL | ES_READONLY| WS_CHILD | WS_VISIBLE | WS_DLGFRAME,
	0,0 ,	CoorXY.right - CoorXY.left -5,
	CoorXY.bottom - CoorXY.top - 120,
    hWndMain,                           
    NULL,                           
    g_hInst,                        
    NULL);
	
	
*/	
	
	hWndEditCurrent = CreateWindow("EDIT",TEXT(""),
	ES_READONLY| ES_LEFT | WS_CHILD | WS_VISIBLE | WS_DLGFRAME,
	0,CoorXY.bottom - CoorXY.top - 120,	CoorXY.right - CoorXY.left -5,
	50,
    hWndMain,                           
    NULL,                           
    g_hInst,                        
    NULL);

	
	hWndEditText = CreateWindow("EDIT",TEXT(""),
	ES_WANTRETURN | ES_LEFT | WS_CHILD | WS_VISIBLE | WS_DLGFRAME,
	0,CoorXY.bottom - CoorXY.top - 70,	CoorXY.right - CoorXY.left -5,
	25,
    hWndMain,                           
    NULL,                           
    g_hInst,                        
    NULL);


  while (GetMessage(&msg,(HWND) NULL,0,0))
        {
            TranslateMessage(&msg); 
            DispatchMessage(&msg);  
        }
	return 0;


	}



LRESULT CALLBACK MainWndProc(
 HWND  hwnd,	// handle of window	
 UINT  uMsg,	// message identifier	
 WPARAM  FirstParam,	// first message parameter	
 LPARAM  lParam 	// second message parameter	
  )

 {
RECT CoorXY;
	switch (uMsg) {

	case WM_DESTROY: 
		MessageBox(NULL,"Ceci est seulement une version Alpha!","PSCHAT : Message de départ",MB_OK);			
		PostQuitMessage(0);
		break;

	case WM_SIZE:
		GetWindowRect(hWndMain, &CoorXY);
//		MoveWindow(hWndEdit,0,0,CoorXY.right - CoorXY.left -5,CoorXY.bottom - CoorXY.top - 120,TRUE);
		MoveWindow(hWndEditText,0,CoorXY.bottom - CoorXY.top - 70,	CoorXY.right - CoorXY.left -5,25, TRUE);          
		MoveWindow(hWndEditCurrent,0,CoorXY.bottom - CoorXY.top - 120,	CoorXY.right - CoorXY.left -5,50, TRUE);          
	
	//	if(SetBkColor(GetDC(hWndEditText),RGB(100,200,50)) != CLR_INVALID)
	//		MessageBox(NULL,"ca a peut-etre marché!? ;)","PSCHAT",MB_OK);
		break;

	case WM_COMMAND:
		switch (LOWORD(FirstParam))
		{
		case ID_QUITTER:
			MessageBox(NULL,"Ceci est seulement une version Alpha!!","PSCHAT : Message de départ",MB_OK);			
			PostQuitMessage(0);
			break;
		case ID_ABOUT:
			DialogBox(g_hInst, MAKEINTRESOURCE(IDD_ABOUT), hwnd, (DLGPROC) AboutProc);
			break;
		/*case ID_AUTOANSWER:
			if(GetMenuItemInfo(IDR_MENU1,ID_AUTOANSWER,FALSE,MFS_CHECKED) == FALSE)
			{
				CheckMenuItem(
				IDR_MENU1,	
				ID_AUTOANSWER,
				MF_CHECKED
				);	
			}
			else 
				CheckMenuItem(
				IDR_MENU1,	
				ID_AUTOANSWER,
				MF_UNCHECKED
				);*/
			//case ID_MONIP:
		//	CheckYourIP();
		//	break;
		case ID_CONNECTION:
			DialogBox(g_hInst, MAKEINTRESOURCE(IDD_CONNECTION), hwnd, (DLGPROC) ConnectionProc);
			break;

		case ID_PLAYMEDIA:
			DialogBox(g_hInst, MAKEINTRESOURCE(IDD_MEDIA), hwnd, (DLGPROC) MediaProc);
			break;
		case ID_SETUP:
			DialogBox(g_hInst, MAKEINTRESOURCE(IDD_CONNECT_SETUP), hwnd, (DLGPROC) SetupProc);
			break;
		}

		//case 2:
		//	FindyourIP(YourIP);
		//	MessageBox(NULL,"Votre IP adresse est : %
	
	
}


 return(DefWindowProc(hwnd, uMsg, FirstParam, lParam));
 }


BOOL APIENTRY AboutProc( HWND hDlg, UINT message, UINT wParam, LONG lParam)
{
	
    if(message == WM_COMMAND)
    {
      if (LOWORD(wParam) == IDOK) 
            {        
                EndDialog(hDlg, TRUE);        
                return TRUE;
            }
            
    }
    return FALSE;                           

}
/*
void CheckYourIP()
{

	
	SOCKADDR_IN YourIP;
    char ipbuff[1024];
	getsockname(sock,YourIP,namelen);
	sprintf(ipbuff,"your sockname result is : %i",YourIP);
	MessageBox(NULL,ipbuff,"Getsockname",MB_OK);
}
*/




BOOL APIENTRY MediaProc( HWND hDlg, UINT message, UINT wParam, LONG lParam)
{
	 

    if(message == WM_COMMAND)
    {
      if (LOWORD(wParam) == IDOK) 
            {        
                EndDialog(hDlg, TRUE);        
                return TRUE;
            }
            
    }
    return FALSE;                           

}



BOOL APIENTRY SetupProc( HWND hDlg, UINT message, UINT wParam, LONG lParam)
{
int counter = 1, counter2;
char tempserv[150], tempserv2[150], tempserv3[150];
RECT CoorMain;
RECT CoorDlg;
	switch(message){
	case WM_INITDIALOG:
GetWindowRect(hWndMain, &CoorMain);
GetWindowRect(hDlg, &CoorDlg);
MoveWindow(hDlg,((CoorMain.right - CoorMain.left)-(CoorDlg.right - CoorDlg.left)) /2 + CoorMain.left,((CoorMain.bottom - CoorMain.top)-(CoorDlg.bottom - CoorDlg.top))/2+CoorMain.top,CoorDlg.right - CoorDlg.left,CoorDlg.bottom - CoorDlg.top,TRUE);          
GetPrivateProfileString("ConnectionSetup","Nickname","My-Nickname", tempserv,512,"anyad.ini");
SetDlgItemText(hDlg,IDC_NICKNAME1,tempserv);
GetPrivateProfileString("ConnectionSetup","Nickname2","Anyaduser", tempserv,512,"anyad.ini");
SetDlgItemText(hDlg,IDC_NICKNAME2,tempserv);
GetPrivateProfileString("ConnectionSetup","email","anyad@anyad.net", tempserv,512,"anyad.ini");
SetDlgItemText(hDlg,IDC_EMAIL,tempserv);
GetPrivateProfileString("ConnectionSetup","realname","Robin Lavallée", tempserv,512,"anyad.ini");
SetDlgItemText(hDlg,IDC_REALNAME,tempserv);
GetPrivateProfileString("ConnectionSetup","autoconnect","0", tempserv,2,"anyad.ini");
if (lstrcmp(tempserv, "0") == 0) CheckDlgButton(hDlg,IDC_ACONNECT,0);
else CheckDlgButton(hDlg,IDC_ACONNECT,1);
GetPrivateProfileString("ConnectionSetup","check_w","1", tempserv,2,"anyad.ini");
if (lstrcmp(tempserv, "0") == 0) CheckDlgButton(hDlg,IDC_WALLOPS,0);
else CheckDlgButton(hDlg,IDC_WALLOPS,1);
GetPrivateProfileString("ConnectionSetup","check_i","0", tempserv,2,"anyad.ini");
if (lstrcmp(tempserv, "0") == 0) CheckDlgButton(hDlg,IDC_INVISIBLE,0);
else CheckDlgButton(hDlg,IDC_INVISIBLE,1);
GetPrivateProfileString("ConnectionSetup","check_s","0", tempserv,2,"anyad.ini");
if (lstrcmp(tempserv, "0") == 0) CheckDlgButton(hDlg,IDC_SERVMSG,0);
else CheckDlgButton(hDlg,IDC_SERVMSG,1);
//check le nombre de Network dans la section [GENERAL] ..
GetPrivateProfileString("GENERAL","NumberOfNetworks","", tempserv,512,"server.ini");
counter2 = atoi(tempserv);
for (counter=0;counter!=counter2; counter++) // list les networks
{
	_itoa(counter+1,tempserv3,10); //convert int -> char
	GetPrivateProfileString("GENERAL",tempserv3,"",tempserv,512,"server.ini");
	ComboBox_AddString(GetDlgItem(hDlg,IDC_LISTNETWORK), tempserv);
}
GetPrivateProfileString("ConnectionSetup","NetworkByDefault","1", tempserv,512,"anyad.ini");//check le number du network by default
strcpy(tempserv2,tempserv);
GetPrivateProfileString("GENERAL",tempserv2,"",tempserv,512,"server.ini");//check le name du network by default
strcpy(tempserv3,tempserv);
GetPrivateProfileString(tempserv3,"NumOfServer","1", tempserv,512,"server.ini");//check le number de server dans le network
counter2 = atoi(tempserv);
sprintf(tempserv,"%i",counter2);
for(counter=0;counter!=counter2; counter++) //list les servers
{
	_itoa(counter+1,tempserv2,10); //convert int -> char
	GetPrivateProfileString(tempserv3,tempserv2,"", tempserv,512,"server.ini");
	ListBox_AddString(GetDlgItem(hDlg,IDC_LISTSERVER), tempserv);
	
}

break;
	case WM_COMMAND:
      switch (LOWORD(wParam)) {
	  case IDCONNECT: 
		  //write data into .ini
        if(IsDlgButtonChecked(hDlg,IDC_ACONNECT) == 0) WritePrivateProfileString("ConnectionSetup","autoconnect","0","anyad.ini");
		else WritePrivateProfileString("ConnectionSetup","autoconnect","1","anyad.ini");
		if(IsDlgButtonChecked(hDlg,IDC_INVISIBLE) == 0)WritePrivateProfileString("ConnectionSetup","check_i","0","anyad.ini");
		else WritePrivateProfileString("ConnectionSetup","check_i","1","anyad.ini");
		if(IsDlgButtonChecked(hDlg,IDC_SERVMSG) == 0)  WritePrivateProfileString("ConnectionSetup","check_s","0","anyad.ini");
		else WritePrivateProfileString("ConnectionSetup","check_s","1","anyad.ini");
		if(IsDlgButtonChecked(hDlg,IDC_WALLOPS) == 0)  WritePrivateProfileString("ConnectionSetup","check_w","0","anyad.ini");
		else WritePrivateProfileString("ConnectionSetup","check_w","1","anyad.ini");
		GetDlgItemText(hDlg, IDC_NICKNAME1, tempserv,50);
		WritePrivateProfileString("ConnectionSetup","Nickname",tempserv,"anyad.ini");
		GetDlgItemText(hDlg, IDC_NICKNAME2, tempserv,50);
		WritePrivateProfileString("ConnectionSetup","Nickname2",tempserv,"anyad.ini");
		GetDlgItemText(hDlg, IDC_EMAIL, tempserv,512);
		WritePrivateProfileString("ConnectionSetup","email",tempserv,"anyad.ini");
		GetDlgItemText(hDlg, IDC_REALNAME, tempserv,256);
		WritePrivateProfileString("ConnectionSetup","realname",tempserv,"anyad.ini");
		  EndDialog(hDlg, TRUE);        
                return TRUE;
				break;  
	  case IDC_EDIT_SERVLIST:
					DialogBox(g_hInst, MAKEINTRESOURCE(IDD_SERVEDIT), hDlg, (DLGPROC) ServEditProc);
					break;
	  }
		  
            
    
    }
	  return FALSE;                           

}










BOOL APIENTRY ServEditProc( HWND hDlg, UINT message, UINT wParam, LONG lParam)
{
int counter = 1, counter2;
char tempserv[150], tempserv2[150], tempserv3[150];
	switch(message){
	case WM_INITDIALOG:
GetPrivateProfileString("GENERAL","NumberOfNetworks","", tempserv,512,"server.ini");
counter2 = atoi(tempserv);
for (counter=0;counter!=counter2; counter++) // list les networks
{
	_itoa(counter+1,tempserv3,10); //convert int -> char
	GetPrivateProfileString("GENERAL",tempserv3,"",tempserv,512,"server.ini");
	ListBox_AddString(GetDlgItem(hDlg,IDC_LISTNETWORK), tempserv);
}
GetPrivateProfileString("ConnectionSetup","NetworkByDefault","1", tempserv,512,"anyad.ini");//check le number du network by default
strcpy(tempserv2,tempserv);
GetPrivateProfileString("GENERAL",tempserv2,"",tempserv,512,"server.ini");//check le name du network by default
strcpy(tempserv3,tempserv);
GetPrivateProfileString(tempserv3,"NumOfServer","1", tempserv,512,"server.ini");//check le number de server dans le network
counter2 = atoi(tempserv);
sprintf(tempserv,"%i",counter2);
for(counter=0;counter!=counter2; counter++) //list les servers
{
	_itoa(counter+1,tempserv2,10); //convert int -> char
	GetPrivateProfileString(tempserv3,tempserv2,"", tempserv,512,"server.ini");
	ListBox_AddString(GetDlgItem(hDlg,IDC_LISTSERVER), tempserv);
	
}

break;
	case WM_COMMAND:
      switch (LOWORD(wParam)) {
	  case IDOK:
		  break;
	  }
	}
return FALSE;
}








/*
BOOL OpenTheFile( HWND hWnd, HWND hWndEdit )
{
	HANDLE hFile;
	OPENFILENAME OpenFileName;
	TCHAR         szFile[MAX_PATH]      = "\0";
	char *lpBufPtr;

    strcpy( szFile, "");


	// Fill in the OPENFILENAME structure to support a template and hook.
	OpenFileName.lStructSize       = sizeof(OPENFILENAME);
    OpenFileName.hwndOwner         = hWnd; //hWndFrame ou MDIFRAME genre.. ou on peut le marquer...				
    OpenFileName.hInstance         = g_hInst;                      //.. quand on va caller la fonction OpenTheFile( HWND hWnd, HWND hWndEdit )
    OpenFileName.lpstrFilter       = "*.LOG";
    OpenFileName.lpstrCustomFilter = NULL;
    OpenFileName.nMaxCustFilter    = 0;
    OpenFileName.nFilterIndex      = 0;
    OpenFileName.lpstrFile         = szFile;
    OpenFileName.nMaxFile          = sizeof(szFile);
    OpenFileName.lpstrFileTitle    = NULL;
    OpenFileName.nMaxFileTitle     = 0;
    OpenFileName.lpstrInitialDir   = NULL;
    OpenFileName.lpstrTitle        = "Open a log File";
    OpenFileName.nFileOffset       = 0;
    OpenFileName.nFileExtension    = 0;
    OpenFileName.lpstrDefExt       = NULL;
    OpenFileName.lCustData         = (LPARAM)&sMyData;
	OpenFileName.lpfnHook 		   = ComDlg32DlgProc;
	OpenFileName.lpTemplateName    = MAKEINTRESOURCE(IDD_OPENLOG);
    OpenFileName.Flags             = OFN_SHOWHELP | OFN_EXPLORER | OFN_ENABLEHOOK | OFN_ENABLETEMPLATE;
	
	// Call the common dialog function.
    if (GetOpenFileName(&OpenFileName))
    
}
*/

BOOL APIENTRY ConnectionProc( HWND hDlg, UINT ConnectMSG, UINT wParam, LONG lParam)
{
    

	switch (ConnectMSG) {
		case WM_COMMAND:
			{
			
				switch(LOWORD(wParam)) {
				
			case IDCANCEL:
				{        
					EndDialog(hDlg, TRUE);        
					return TRUE;
				}
            case IDOK:
				{

					char IpToConnect[255];
					char Nickname[255];
					char RealName[255];
					char Instructions[255];
					strcpy(RealName,"");
					strcpy(Nickname,"");
					strcpy(IpToConnect,"");
					strcpy(Instructions, "");
					GetDlgItemText(hDlg, IDC_IPAD, IpToConnect,255);
					GetDlgItemText(hDlg, IDC_PSEUDONYME, Nickname,30);
					GetDlgItemText(hDlg, IDC_REALNAME, RealName,25);
										
					if(strcmp(IpToConnect, Instructions) == 0 || strcmp(Nickname, Instructions) == 0 || strcmp(RealName, Instructions) == 0 )
					{
						MessageBox(NULL, "Une donnée manque. Soit votre Pseudonyme, votre vrai nom ou l'ip ou se connecter","PSCHAT / erreur", MB_OK + MB_ICONSTOP);
						break;
					}
					strcpy(Instructions, "Connection..");
					//test
					//MessageBox(NULL, "avant le wsastartup","PSCHAT debuging!", MB_OK);
						


					return TRUE;
				}
		
			}

			}
}    
return FALSE;
}

//
//Pour pogner l'address :) ..
//
//
//

