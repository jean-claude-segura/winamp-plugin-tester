// winamp-plugin-tester.cpp : Définit le point d'entrée de l'application.
//

#include "framework.h"
#include "winamp-plugin-tester.h"
#include <memory>
#include <thread>
#include <chrono>
#include <random>
#include <atomic>

#define MAX_LOADSTRING 100

// Variables globales :
HINSTANCE hInst;                                // instance actuelle
WCHAR szTitle[MAX_LOADSTRING];                  // Texte de la barre de titre
WCHAR szWindowClass[MAX_LOADSTRING];            // nom de la classe de fenêtre principale

// Déclarations anticipées des fonctions incluses dans ce module de code :
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // TODO: Placez le code ici.

    // Initialise les chaînes globales
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_WINAMPPLUGINTESTER, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // Effectue l'initialisation de l'application :
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_WINAMPPLUGINTESTER));

    MSG msg;

    // Boucle de messages principale :
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    return (int) msg.wParam;
}

static std::unique_ptr<std::thread> renderingThread;
void renderingCalls();
static std::atomic<bool> stop = false;

//
//  FONCTION : MyRegisterClass()
//
//  OBJECTIF : Inscrit la classe de fenêtre.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style          = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc    = WndProc;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = hInstance;
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_WINAMPPLUGINTESTER));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_WINAMPPLUGINTESTER);
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}

//
//   FONCTION : InitInstance(HINSTANCE, int)
//
//   OBJECTIF : enregistre le handle d'instance et crée une fenêtre principale
//
//   COMMENTAIRES :
//
//        Dans cette fonction, nous enregistrons le handle de l'instance dans une variable globale, puis
//        nous créons et affichons la fenêtre principale du programme.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   hInst = hInstance; // Stocke le handle d'instance dans la variable globale

   /*
   HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, nullptr);
      */
   HWND hWnd = CreateWindow(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
       CW_USEDEFAULT, CW_USEDEFAULT, 400, 300, nullptr, nullptr, hInstance, nullptr);

   if (!hWnd)
   {
      return FALSE;
   }

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
}

//
//  FONCTION : WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  OBJECTIF : Traite les messages pour la fenêtre principale.
//
//  WM_COMMAND  - traite le menu de l'application
//  WM_PAINT    - Dessine la fenêtre principale
//  WM_DESTROY  - génère un message d'arrêt et retourne
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_COMMAND:
        {
            int wmId = LOWORD(wParam);
            // Analyse les sélections de menu :
            switch (wmId)
            {
            case IDM_ABOUT:
                DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
                break;
            case IDM_EXIT:
                DestroyWindow(hWnd);
                break;
            case IDM_PLUGIN_LOAD:
			    {
				    handleLib = LoadLibraryEx(L"C:\\Winamp\\Plugins\\monkey\\vis_monkey.dll", NULL, NULL);
                    //handleLib = LoadLibraryEx(L"C:\\PROGRA~2\\Winamp\\Plugins\\vis_monkey.dll", NULL, NULL);
                    // C:\Program Files (x86)\Winamp\Plugins
                    //handleLib = LoadLibraryEx(L"C:\\PROGRA~2\\Winamp\\Plugins\\vis_lhd.dll", NULL, NULL);
                    //handleLib = LoadLibraryEx(L"C:\\PROGRA~2\\Winamp\\Plugins\\vis_avs.dll", NULL, NULL);
                    //handleLib = LoadLibraryEx(L"C:\\PROGRA~2\\Winamp\\Plugins\\vis_milk2.dll", NULL, NULL);
                    //handleLib = LoadLibraryEx(L"C:\\Winamp\\Plugins\\vis_lhd.dll", NULL, NULL);
                    //handleLib = LoadLibraryEx(L"C:\\Winamp\\Plugins\\LHDance\\Plugins\\vis_lhd.dll", NULL, NULL);
				    //handleLib = LoadLibraryEx(L"C:\\Winamp\\Plugins\\others\\vis_milk2.dll", NULL, NULL);
				    //handleLib = LoadLibraryEx(L"C:\\Winamp\\Plugins\\vis_rave.dll", NULL, NULL);
				    if (handleLib == NULL)
				    {
					    auto dwError = GetLastError();

					    LPTSTR lpMsgBuf;

					    FormatMessage(
						    FORMAT_MESSAGE_ALLOCATE_BUFFER |
						    FORMAT_MESSAGE_FROM_SYSTEM |
						    FORMAT_MESSAGE_IGNORE_INSERTS,
						    NULL,
						    dwError,
						    MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
						    (LPTSTR)&lpMsgBuf,
						    0, NULL);

					    //std::wcout << (LPTSTR)lpMsgBuf << std::endl;
                        MessageBox(hWnd, lpMsgBuf, L"Erreur au chargement de la dll", MB_ICONERROR);
				    }
				    else
				    {
					    WinampVisGetHeader lpFunc = (WinampVisGetHeader)GetProcAddress(handleLib, "winampVisGetHeader");
					    auto lpModule = lpFunc();
					    lpWinampVisModule = lpModule->getModule(0);
                        lpWinampVisModule->hwndParent = hWnd;
                        lpWinampVisModule->hDllInstance = handleLib;

                        EnableMenuItem(GetMenu(hWnd), IDM_PLUGIN_LOAD, MF_DISABLED);
                        EnableMenuItem(GetMenu(hWnd), IDM_PLUGIN_CONFIG, MF_ENABLED);
                        EnableMenuItem(GetMenu(hWnd), IDM_PLUGIN_INIT, MF_ENABLED);
				    }
			    }
                break;
            case IDM_PLUGIN_CONFIG:
                {
                    auto resConfig = lpWinampVisModule->Config(lpWinampVisModule);
                }
                break;
            case IDM_PLUGIN_INIT:
                {
                    lpWinampVisModule->nCh = 2;
                    lpWinampVisModule->latencyMs = 1;
                    lpWinampVisModule->delayMs = 15;
                    lpWinampVisModule->sRate = 44100;
                    lpWinampVisModule->spectrumNCh = 2;
                    //lpWinampVisModule->spectrumData;
                    ZeroMemory(lpWinampVisModule->spectrumData, sizeof(unsigned char) * sizeof(lpWinampVisModule->spectrumData));
                    lpWinampVisModule->waveformNCh = 2;
                    //lpWinampVisModule->waveformData;
                    ZeroMemory(lpWinampVisModule->waveformData, sizeof(unsigned char) * sizeof(lpWinampVisModule->waveformData));

                    std::random_device rd; // To get a random seed.
                    std::mt19937 mt(rd()); // The actual randomizer.
                    std::uniform_int_distribution<int> dist(0, 255);

					for (int i = 0; i < 2; ++i)
					{
						for (int j = 0; j < 576; ++j)
						{
							lpWinampVisModule->spectrumData[i][j] = dist(mt);
							lpWinampVisModule->waveformData[i][j] = dist(mt);
						}
					}

                    auto resInit = lpWinampVisModule->Init(lpWinampVisModule);
            
                    EnableMenuItem(GetMenu(hWnd), IDM_PLUGIN_LOAD, MF_DISABLED);
                    EnableMenuItem(GetMenu(hWnd), IDM_PLUGIN_CONFIG, MF_DISABLED);
                    EnableMenuItem(GetMenu(hWnd), IDM_PLUGIN_INIT, MF_DISABLED);
                    EnableMenuItem(GetMenu(hWnd), IDM_PLUGIN_RENDER, MF_ENABLED);
                    EnableMenuItem(GetMenu(hWnd), IDM_PLUGIN_STOPRENDERING, MF_DISABLED);
                    EnableMenuItem(GetMenu(hWnd), IDM_PLUGIN_QUIT, MF_ENABLED);
                }
                break;
            case IDM_PLUGIN_RENDER:
				EnableMenuItem(GetMenu(hWnd), IDM_PLUGIN_RENDER, MF_DISABLED);
				EnableMenuItem(GetMenu(hWnd), IDM_PLUGIN_STOPRENDERING, MF_ENABLED);
                stop = false;
				renderingThread = std::make_unique<std::thread>(renderingCalls);
                break;
            case IDM_PLUGIN_STOPRENDERING:
                EnableMenuItem(GetMenu(hWnd), IDM_PLUGIN_RENDER, MF_ENABLED);
                EnableMenuItem(GetMenu(hWnd), IDM_PLUGIN_STOPRENDERING, MF_DISABLED);
                stop = true;
                renderingThread.get()->join();
                renderingThread.release();
                renderingThread = std::unique_ptr<std::thread>(nullptr);
                break;
            case IDM_PLUGIN_QUIT:
                EnableMenuItem(GetMenu(hWnd), IDM_PLUGIN_LOAD, MF_ENABLED);
                EnableMenuItem(GetMenu(hWnd), IDM_PLUGIN_CONFIG, MF_DISABLED);
                EnableMenuItem(GetMenu(hWnd), IDM_PLUGIN_INIT, MF_DISABLED);
                EnableMenuItem(GetMenu(hWnd), IDM_PLUGIN_RENDER, MF_DISABLED);
                EnableMenuItem(GetMenu(hWnd), IDM_PLUGIN_STOPRENDERING, MF_DISABLED);
                EnableMenuItem(GetMenu(hWnd), IDM_PLUGIN_QUIT, MF_DISABLED);
                lpWinampVisModule->Quit(lpWinampVisModule);
                if(handleLib != NULL) FreeLibrary(handleLib);
                break;
            default:
                return DefWindowProc(hWnd, message, wParam, lParam);
            }
        }
        break;
    case WM_PAINT:
        {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hWnd, &ps);
            // TODO: Ajoutez ici le code de dessin qui utilise hdc...
            EndPaint(hWnd, &ps);
        }
        break;
    case WM_DESTROY:

        stop = true;
        if (renderingThread != std::unique_ptr<std::thread>(nullptr))
        {
            if (renderingThread.get()->joinable())
                renderingThread.get()->join();
        }
        renderingThread.release();

        if (handleLib != NULL) FreeLibrary(handleLib);
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

// Gestionnaire de messages pour la boîte de dialogue À propos de.
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
    case WM_INITDIALOG:
        return (INT_PTR)TRUE;

    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
        {
            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
        }
        break;
    }
    return (INT_PTR)FALSE;
}

void renderingCalls()
{
    ZeroMemory(lpWinampVisModule->spectrumData, sizeof(unsigned char) * sizeof(lpWinampVisModule->spectrumData));
    ZeroMemory(lpWinampVisModule->waveformData, sizeof(unsigned char) * sizeof(lpWinampVisModule->waveformData));

    std::random_device rd; // To get a random seed.
    std::mt19937 mt(rd()); // The actual randomizer.
    std::uniform_int_distribution<int> dist(0, 255);
    
    do
    {
        for(int i = 0; i < 2; ++i)
        {
            for (int j = 0; j < 576; ++j)
            {
                lpWinampVisModule->spectrumData[i][j] = dist(mt);
                lpWinampVisModule->waveformData[i][j] = dist(mt);
            }
        }

        auto resRender = lpWinampVisModule->Render(lpWinampVisModule);
        std::this_thread::sleep_for(std::chrono::milliseconds(15));
    } while (!stop);
}
