#include <stdio.h>
#include <windows.h>
#include <stdlib.h>
#include <time.h>

//#Defines for menu events
#define FILEMENU_NEW 1
#define FILEMENU_OPEN 2
#define FILEMENU_SAVE 3
#define exit_window 4
#define change_title 5
#define SAUCE 6
LRESULT CALLBACK WindowProcedure(HWND,UINT,WPARAM,LPARAM);

void AddMenus(HWND);
void AddControls(HWND);
void registerDialogClass(HINSTANCE);
void displayDialog(HWND);
HMENU hMenu;
HWND hPic,hMainWindow,hEdit;
HCURSOR hCursor;
HFONT textfont;

int WINAPI WinMain(HINSTANCE hInst, HINSTANCE hPrevInst, LPSTR args, int nCmdShow) //Initializes the main window
{
    WNDCLASSW wc = {0};
    wc.hbrBackground=(HBRUSH)COLOR_WINDOW;
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hInstance= hInst;
    wc.lpszClassName = L"windowclass";
    wc.lpfnWndProc = WindowProcedure;
    //Checks if the main window was initialized correctly
    if(!RegisterClassW(&wc)) {
        return -1;
    }
    //Registers the dialog window
    registerDialogClass(hInst);

    hMainWindow = CreateWindowW(L"windowclass",L"TextEditing",WS_OVERLAPPEDWINDOW | WS_VISIBLE,-10,0,1940,1100,NULL,NULL,NULL,NULL);

    MSG msg = {0};

    while(GetMessage(&msg,NULL,0,0))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    return 0;
}

void display_file(char* path) //Function for getting a files contents and putting it into the text editor window
{
    FILE *file;

    file= fopen(path,"rb");
    fseek(file,0,SEEK_END);

    int _size = ftell(file);
    rewind(file);
    //Allocates memory for the files contents
    char *data = malloc(_size+1);
    fread(data,_size,1,file);
    data[_size] = '\0';

    SetWindowText(hEdit,data);
    fclose(file);
    free(data);
}

void open_file_window(HWND hWnd) //Function that sets up file menu and stores the information for the file to be opened
{
    OPENFILENAME ofn;

    char file_name[100];

    ZeroMemory(&ofn,sizeof(OPENFILENAME));

    ofn.lStructSize = sizeof(OPENFILENAME);
    ofn.hwndOwner = hWnd;
    ofn.lpstrFile =  file_name;
    ofn.lpstrFile[0] = '\0';
    ofn.nMaxFile = 100;
    ofn.lpstrFilter = "Text Files\0*.txt\0All Files\0*.*\0";
    ofn.nFilterIndex = 1;

    if(GetOpenFileName(&ofn)==0) {  //Checks if they pressed cancel and exits the function so the empty memory isn't accessed
        return;
    }
    //else if(SaveFileName(&ofn))
    else {
        display_file(ofn.lpstrFile);
        return;
    }
}

void write_file(char* path)
{
    FILE *file;

    file= fopen(path,"wb");
    int _size = GetWindowTextLength(hEdit) + 1;
    char *data = malloc(_size);
    data[_size] = '\0';
    GetWindowText(hEdit, data, _size);

    fwrite(data, _size, 1, file);
    fclose(file);
}

void save_file(HWND hWnd) //Function that sets up file menu and stores the information for the file to be opened
{
    OPENFILENAME ofn;

    char file_name[100];

    ZeroMemory(&ofn,sizeof(OPENFILENAME));

    ofn.lStructSize = sizeof(OPENFILENAME);
    ofn.hwndOwner = hWnd;
    ofn.lpstrFile =  file_name;
    ofn.lpstrFile[0] = '\0';
    ofn.nMaxFile = 100;
    ofn.lpstrFilter = "Text Files\0*.txt\0All Files\0*.*\0";
    ofn.nFilterIndex = 1;

    GetSaveFileName(&ofn);

    write_file(ofn.lpstrFile);
}
LRESULT CALLBACK WindowProcedure(HWND hWnd,UINT msg,WPARAM wp,LPARAM lp)
{
    int value;

    switch(msg)
    {
    case WM_COMMAND:  //Cases for menu events in the main window
        switch (wp)
        {
        case FILEMENU_NEW:  //Event for new button being pressed
            displayDialog(hWnd);
            break;
        case FILEMENU_OPEN:  //Events for open button being pressed
            open_file_window(hWnd);
            break;
        case FILEMENU_SAVE:
            save_file(hWnd);
            break;
        case exit_window: //Event for exit button being pressed
            value = MessageBoxW(NULL,L"Unfortunately while processing your request, time stopped",L"A Fatal Error Occurred",MB_YESNO | MB_ICONERROR);
            if(value==IDYES)
            {
                DestroyWindow(hWnd);
            }
            else {
                MessageBoxW(NULL,L"Okay then",L"?",MB_DEFBUTTON1);
            }
            break;
        case SAUCE:  //Event for sauce button being pressed
            {
            srand(time(0));
            ( rand());  //Might sound crazy but generating here with rand() helps make sauce generator more uniform for some reason,
            //Without this it just goes up by about 100 every time you push it, (Depends on how much time has passed from when you last pressed it)
            int masons_numbers = rand() / (RAND_MAX + 1.0) * (300000 - 100000) + 100000;
            char numbers[7];
            sprintf(numbers,"%d", masons_numbers);
            MessageBox(NULL,numbers,"Your Sauce",MB_DEFBUTTON1);
            break;
            }
        }
        break;

    case WM_CREATE: //Case for when window is created
        AddMenus(hWnd); // all of these functions are needed to
        AddControls(hWnd); // create everything on startup
        break;
    case WM_DESTROY: //Case for when window is closed
        DeleteObject(textfont);
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProcW(hWnd,msg,wp,lp);
    }
    return 1;
}
void AddMenus(HWND hWnd)  //Created the menus at the top of the window
{
    hMenu = CreateMenu();
    HMENU hFileMenu = CreateMenu();
    HMENU hSauceMenu = CreateMenu();
    AppendMenu(hMenu,MF_POPUP,(UINT_PTR)hFileMenu,"File");
    //All in the submenu of the file menu
    AppendMenu(hFileMenu,MF_STRING,FILEMENU_NEW,"New");
    AppendMenu(hFileMenu,MF_STRING,FILEMENU_OPEN,"Open");
    AppendMenu(hFileMenu,MF_STRING,FILEMENU_SAVE,"Save");
    AppendMenu(hFileMenu,MF_SEPARATOR,0,0);
    AppendMenu(hFileMenu,MF_STRING,exit_window,"Exit");

    //Sauce menu
    AppendMenu(hMenu,MF_POPUP,(UINT_PTR)hSauceMenu,"Free Sauce");
    AppendMenu(hSauceMenu,MF_STRING,SAUCE,"Did you really think you'd be that lucky?");
    SetMenu(hWnd,hMenu);
}
void AddControls(HWND hWnd)//Adds all the windows that are not the main one
{
    //Creates top banner window
    CreateWindowW(L"Static",L"Type away!",WS_VISIBLE|WS_CHILD|WS_BORDER|SS_CENTER,0,0,1920,20,hWnd,NULL,NULL,NULL);
    //Creates the text editing window
    hEdit = CreateWindowW(L"Edit",L"",WS_VISIBLE | WS_CHILD | WS_BORDER | ES_MULTILINE | ES_WANTRETURN | WS_VSCROLL,0,20,1920,1010,hWnd,NULL,NULL,NULL);
    textfont = CreateFont(18,7,0,0,700,0,0,0,ANSI_CHARSET,OUT_DEFAULT_PRECIS,CLIP_DEFAULT_PRECIS,ANTIALIASED_QUALITY,0,"Visually_Basic_Font");
    SendMessageW(hEdit, WM_SETFONT, (WPARAM)textfont, TRUE);
}

LRESULT CALLBACK DialogProcedure(HWND hWnd, UINT msg, WPARAM wp,LPARAM lp)
{
    switch(msg)
    {
    case WM_CLOSE: //Events for dialog window closing
        EnableWindow(hMainWindow,1);
        DestroyWindow(hWnd);
        break;
    case WM_COMMAND: //Events for button being pressed in the dialog window
        switch(wp)
        {
        case 1:  //Events for the window closing
            EnableWindow(hMainWindow,1);
            DestroyWindow(hWnd);
            break;
        }
    default:
        return DefWindowProcW(hWnd,msg,wp,lp);
    }
    return 1;
}

void registerDialogClass(HINSTANCE hInst)  //Registers the dialog window
{
    WNDCLASSW dialog = {0};

    dialog.hbrBackground = (HBRUSH)COLOR_WINDOW;
    dialog.hCursor = LoadCursor(NULL,IDC_ARROW);
    dialog.hInstance = hInst;
    dialog.lpszClassName = L"myDialogClass";
    dialog.lpfnWndProc = DialogProcedure;

    RegisterClassW(&dialog);
}

void displayDialog(HWND hWnd)  //Displays the dialog window
{
    HWND hDlg = CreateWindowW(L"myDialogClass",L"Dialog",WS_OVERLAPPEDWINDOW|WS_VISIBLE,400,400,
                  500,400, hWnd,NULL,NULL,NULL);
    CreateWindowW(L"Button",L"Close Window",WS_VISIBLE|WS_CHILD,20,20,100,40,hDlg,(HMENU)1,NULL,NULL); //Close button in dialog window

    EnableWindow(hWnd,0); //Disables all but the dialog window when the dialog window is open
}
