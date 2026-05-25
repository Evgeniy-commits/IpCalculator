#define _CRT_SECURE_NO_WARNINGS

#include<Windows.h>
#include<CommCtrl.h>
#include<iostream>
#include"resource.h"

BOOL CALLBACK DlgProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
VOID PrintInfo(HWND hwnd);
CHAR* FormatAddress(CHAR szBuffer[], DWORD dwIPaddress);

INT WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInst, LPSTR lpCmdLine, INT nCmdShow)
{
	DialogBoxParam(hInstance, MAKEINTRESOURCE(IDD_DIALOG_MAIN), NULL, (DLGPROC)DlgProc, NULL);
	return 0;
}

BOOL CALLBACK DlgProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	HWND hIPaddress = GetDlgItem(hwnd, IDC_IP_ADDRESS);
	HWND hIPmask = GetDlgItem(hwnd, IDC_IP_MASK);
	HWND hIPPrefix = GetDlgItem(hwnd, IDC_EDIT_PREFIX);
	DWORD dwIPaddress = 0;
	DWORD dwIPmask = 0;
	DWORD dwIPPrefix = 0;
	CHAR szIPPrefix[3] = {};

	switch (uMsg)
	{
	case WM_INITDIALOG:
	{
		//InitCommonControls();
		AllocConsole();
		freopen("CONOUT$", "w", stdout);
		std::cout << "Init" << std::endl;
		SetFocus(GetDlgItem(hwnd, IDC_IP_ADDRESS));
		SendMessage(GetDlgItem(hwnd, IDC_SPIN_PREFIX), UDM_SETRANGE, 0, MAKEWORD(30, 0));
	}
	break;

	case WM_NOTIFY:
	{
		NMHDR* pNMHDR = (NMHDR*)lParam;
		if (wParam == IDC_IP_MASK || wParam == IDC_IP_ADDRESS)
		{
			SendMessage(hIPmask, IPM_GETADDRESS, 0, (LPARAM)&dwIPmask);
			for (dwIPPrefix = 0; dwIPmask; dwIPPrefix++) dwIPmask <<= 1;
			CHAR szIPPrefix[3] = {};
			sprintf(szIPPrefix, "%i", dwIPPrefix);
			std::cout << szIPPrefix << std::endl;
			SendMessage(hIPPrefix, WM_SETTEXT, 0, (LPARAM)szIPPrefix);
		}
	}
	break;
	/*case WM_NOTIFY:
	{
		NMHDR* pNMHDR = (NMHDR*)lParam;
		if (pNMHDR->idFrom == IDC_SPIN_PREFIX && pNMHDR->code == UDN_DELTAPOS)
		{
			LPNMUPDOWN pUD = (LPNMUPDOWN)lParam;
			GetDlgItemText(hwnd, IDC_EDIT_PREFIX, szIPPrefix, 3);
			dwIPPrefix = atoi(szIPPrefix);
			dwIPPrefix += pUD->iDelta;

			if (dwIPPrefix < 0) dwIPPrefix = 0;
			else if (dwIPPrefix > 30) dwIPPrefix = 30;
			if (dwIPPrefix == 0) dwIPmask = 0;

			dwIPmask = (0xFFFFFFFF << (32 - dwIPPrefix)) & 0xFFFFFFFF;

			std::cout << "Prefix: " << dwIPPrefix << std::endl;
			std::cout << "Mask: " << dwIPmask << std::endl;
			SendMessage(hIPmask, IPM_SETADDRESS, 0, dwIPmask);

			sprintf(szIPPrefix, "%i", dwIPPrefix);
			std::cout << "New Prefix:" << szIPPrefix << std::endl;
			SendMessage(hIPPrefix, WM_SETTEXT, 0, (LPARAM)szIPPrefix);

			return TRUE;
		}

	}
	  break;*/

	case WM_COMMAND:
	{
		switch (LOWORD(wParam))
		{
		case IDC_IP_ADDRESS:
		{
			if (HIWORD(wParam) == EN_CHANGE)
			{
				SendMessage(hIPaddress, IPM_GETADDRESS, 0, (LPARAM)&dwIPaddress);
				if (FIRST_IPADDRESS(dwIPaddress) < 128) dwIPmask = 0xFF000000, dwIPPrefix = 8;		// dwIPPrefix = 8;
				else if (FIRST_IPADDRESS(dwIPaddress) < 192) dwIPmask = 0xFFFF0000, dwIPPrefix = 16; // dwIPPrefix = 16;
				else if (FIRST_IPADDRESS(dwIPaddress) < 224) dwIPmask = 0xFFFFFF00, dwIPPrefix = 24; // dwIPPrefix = 24;
				SendMessage(hIPmask, IPM_SETADDRESS, 0, dwIPmask);
				sprintf(szIPPrefix, "%i", dwIPPrefix);
				SendMessage(hIPPrefix, WM_SETTEXT, 0, (LPARAM)szIPPrefix);
			}
		}
		//break;

		case IDC_IP_MASK:
		{
			//if (HIWORD(wParam) == EN_CHANGE)
			//{
			//	SendMessage(hIPmask, IPM_GETADDRESS, 0, (LPARAM)&dwIPmask);
			//	dwIPmask &= 0xFFFFFFFC;
			//	for (dwIPPrefix = 0; dwIPmask; dwIPPrefix++) dwIPmask <<= 1;
			//	CHAR szIPPrefix[3] = {};
			//	sprintf(szIPPrefix, "%i", dwIPPrefix);
			//	std::cout << szIPPrefix << std::endl;
			//	SendMessage(hIPPrefix, WM_SETTEXT, 0, (LPARAM)szIPPrefix);
			////if (HIWORD(wParam) == EN_KILLFOCUS) SendMessage(hIPmask, IPM_SETADDRESS, 0, dwIPaddress);
			//}
		}
		break;

		case IDC_EDIT_PREFIX:
		{
			if (HIWORD(wParam) == EN_CHANGE)
			{
				CHAR szIPPrefix[4] = {};
				SendMessage(hIPPrefix, WM_GETTEXT, 4, (LPARAM)szIPPrefix);
				DWORD dwIPPrefix = std::atoi(szIPPrefix);
				dwIPmask = UINT_MAX;
				for (int i = 0; i < 32 - dwIPPrefix; i++) dwIPmask <<= 1;
				SendMessage(hIPmask, IPM_SETADDRESS, 0, dwIPmask);
			}
		}
		break;

		case IDC_BUTTON_RESET:
			SendMessage(hIPaddress, IPM_CLEARADDRESS, 0, 0);
			SendMessage(hIPmask, IPM_CLEARADDRESS, 0, 0);
			SendMessage(hIPPrefix, WM_SETTEXT, 0, (LPARAM)"");
			break;

		case IDOK:
			PrintInfo(hwnd);
			break;

		case IDCANCEL: EndDialog(hwnd, 0);

		case WM_CLOSE: EndDialog(hwnd, 0);
			break;
		}

	}
	}
	return FALSE;
}

CHAR* FormatAddress(CHAR szBuffer[], DWORD dwIPaddress)
{
	sprintf
	(
		szBuffer,
		"%i.%i.%i.%i",
		FIRST_IPADDRESS(dwIPaddress),
		SECOND_IPADDRESS(dwIPaddress),
		THIRD_IPADDRESS(dwIPaddress),
		FOURTH_IPADDRESS(dwIPaddress)
	);
	return szBuffer;
}

VOID PrintInfo(HWND hwnd)
{
	HWND hIPaddress = GetDlgItem(hwnd, IDC_IP_ADDRESS);
	HWND hIPmask = GetDlgItem(hwnd, IDC_IP_MASK);
	HWND hStaticInfo = GetDlgItem(hwnd, IDC_STATIC_INFO);
	DWORD dwIPaddress = 0;
	DWORD dwIPmask = 0;
	SendMessage(hIPaddress, IPM_GETADDRESS, 0, (LPARAM)&dwIPaddress);
	SendMessage(hIPmask, IPM_GETADDRESS, 0, (LPARAM)&dwIPmask);
	CHAR szInfo[1024] = {};
	CHAR szBuffer[1024] = {};

	sprintf
	(
		szInfo,
		"Адрес сети: %s;\nШироковещательный адрес: %s;\nКоличество IP-адресов: %s;\nКоличество узлов: %s;\n",
		FormatAddress(szBuffer, dwIPaddress & dwIPmask),
		FormatAddress(szBuffer, dwIPaddress | ~dwIPmask),
		"",
		""
	);
	SendMessage(hStaticInfo, WM_SETTEXT, 0, (LPARAM)szInfo);
}