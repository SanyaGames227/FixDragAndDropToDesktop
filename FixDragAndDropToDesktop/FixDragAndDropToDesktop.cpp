#include <windows.h>
#include <shlobj.h>
#include <iostream>
#include <string>
#include <locale>

// ---------- utils ----------

void RefreshDesktopFast()
{
    HWND hProgman = FindWindow(L"Progman", NULL);
    HWND hShellViewWin = NULL;

    // ищем SHELLDLL_DefView
    HWND hWorkerW = FindWindowEx(NULL, NULL, L"WorkerW", NULL);
    while (hWorkerW)
    {
        hShellViewWin = FindWindowEx(hWorkerW, NULL, L"SHELLDLL_DefView", NULL);
        if (hShellViewWin)
            break;
        hWorkerW = FindWindowEx(NULL, hWorkerW, L"WorkerW", NULL);
    }
    if (!hShellViewWin)
        hShellViewWin = FindWindowEx(hProgman, NULL, L"SHELLDLL_DefView", NULL);

    if (hShellViewWin)
    {
        // это то самое, что explorer делает на F5
        PostMessage(hShellViewWin, WM_COMMAND, 28931, 0); // 28931 = refresh
        std::wcout << L"рабочий стол обновлён мгновенно (WM_COMMAND Refresh)" << std::endl;
    }
}

// ---------- watcher ----------

void WatchDesktop() {
    wchar_t path[MAX_PATH];
    if (SHGetFolderPathW(NULL, CSIDL_DESKTOP, NULL, 0, path) != S_OK) {
        std::wcerr << L"Не удалось получить путь к рабочему столу" << std::endl;
        return;
    }

    HANDLE hDir = CreateFileW(
        path,
        FILE_LIST_DIRECTORY,
        FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
        NULL,
        OPEN_EXISTING,
        FILE_FLAG_BACKUP_SEMANTICS,
        NULL
    );

    if (hDir == INVALID_HANDLE_VALUE) {
        std::wcerr << L"Не удалось открыть папку рабочего стола" << std::endl;
        return;
    }

    char buffer[4096];
    DWORD bytesReturned;

    while (true) {
        if (ReadDirectoryChangesW(
            hDir,
            buffer,
            sizeof(buffer),
            FALSE,
            FILE_NOTIFY_CHANGE_FILE_NAME | FILE_NOTIFY_CHANGE_DIR_NAME,
            &bytesReturned,
            NULL,
            NULL
        )) {
            FILE_NOTIFY_INFORMATION* fni = (FILE_NOTIFY_INFORMATION*)buffer;

            do {
                std::wstring fileName(fni->FileName, fni->FileNameLength / sizeof(WCHAR));

                if (fni->Action == FILE_ACTION_ADDED) {
                    std::wcout << L"Создано: " << fileName << std::endl;
                    RefreshDesktopFast();
                }

                if (fni->NextEntryOffset == 0)
                    break;
                fni = (FILE_NOTIFY_INFORMATION*)((LPBYTE)fni + fni->NextEntryOffset);

            } while (true);
        }
    }

    CloseHandle(hDir);
}

// ---------- main ----------

int WINAPI wWinMain(HINSTANCE, HINSTANCE, PWSTR cmdLine, int) {
    bool enableConsole = false;

    // простой парс аргументов
    std::wstring args = cmdLine;
    if (args.find(L"-enable_console") != std::wstring::npos) {
        enableConsole = true;
    }

    if (enableConsole) {
        AllocConsole();

        // привязываем stdout/stderr к консоли
        FILE* fp;
        freopen_s(&fp, "CONOUT$", "w", stdout);
        freopen_s(&fp, "CONOUT$", "w", stderr);

        // ставим кодировку UTF-8
        SetConsoleOutputCP(CP_UTF8);
        SetConsoleCP(CP_UTF8);

        // локаль для wcout
        std::locale utf8_locale(".UTF8"); // на новых виндах работает
        std::wcout.imbue(utf8_locale);
        std::wcin.imbue(utf8_locale);

        std::wcout << L"Наблюдение за рабочим столом... (Ctrl+C для выхода)" << std::endl;
    }

    WatchDesktop();
    return 0;
}
