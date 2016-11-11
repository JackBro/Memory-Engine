#include "qtpro.h"
#include "ui_qtpro.h"
#include "ui_about.h"
#include "ui_aboutce.h"
#include "qt_windows.h"
#include "ui_openProcess.h"
#include "DebuggedProcess.h"
#include "MemRead.h"
#include <windows.h>
#include <tlhelp32.h>
#include <tchar.h>
#include <string>
#include <memory>
#include <Psapi.h>
#include "ui_ShowDLLs.h"
// ConsoleApplication10.cpp : Defines the entry point for the console application.
//
#define BEA_ENGINE_STATIC  /* specify the usage of a static version of BeaEngine */
#define BEA_USE_STDCALL    /* specify the usage of a stdcall version of BeaEngine */
#include "BeaEngine/BeaEngine.h"
#include <fstream>
#include <stdio.h>
#include <stdlib.h> 
#include <iostream>
#include <iomanip>
#include <sstream> 
#include <cstdio>
using namespace std;
/*	int n;
_asm
{
push eax
call get_eip
jmp out1
get_eip: mov eax, [esp]
ret
out1:
mov n, eax
pop eax
}*/
int insertDisas(MemoryViewer * aDialog)
{
	fout << "test1" << endl;
	if (WAIT_TIMEOUT == WaitForSingleObject(DebuggedProc.hwnd, 0))
		fout << "Handle of process is still active" << endl;
	else
		fout << "Handle is dead." << endl;
	if (!DebuggedProc.addressOfInterest)
		fout << "error, no EIP used" << endl;
	else
	DebuggedProc.mb = QueryMemoryAddrress(DebuggedProc.addressOfInterest);//create_scan(DebuggedProc.hwnd, 1);
	if (DebuggedProc.mb)
	{
		//DebuggedProc.mb = create_scan(DebuggedProc.hwnd, 1);
		ostringstream stre;
		string str;
		QTreeWidgetItem* itm;
		/* ============================= Init datas */
		DISASM MyDisasm;
		int nfalse = 0, ntrue = 1;
		int len, i = 0;
		bool Error = nfalse;

		/* ============================= Init the Disasm structure (important !)*/
		(void)memset(&MyDisasm, 0, sizeof(DISASM));

		/* ============================= Init EIP */
		int64_t n;
		n = reinterpret_cast<int64_t>(DebuggedProc.mb->buffer);
		fout << "Address of Interest : " << hex << DebuggedProc.addressOfInterest << endl;
		fout << "Start of memory page : " << hex << DebuggedProc.basePageAddress << endl;
		fout << "size : " << hex << DebuggedProc.mb->size << endl;
		n = n + DebuggedProc.addressOfInterest - DebuggedProc.basePageAddress;
		int64_t nTargetedProcessAddress = DebuggedProc.addressOfInterest;
		//fout << "address : " << hex << n << " and : "<< hex << nTargetedProcessAddress << std::endl;
		//n = reinterpret_cast<int64_t>(hwnd);
		cout << "n : " << hex << n << endl;
		MyDisasm.Archi = DebuggedProc.architecture;
		MyDisasm.EIP = n;// 0x401000;
		MyDisasm.VirtualAddr = DebuggedProc.addressOfInterest;
		cout << "sizeof " << sizeof(MyDisasm.EIP) << endl;
		cout << "start address : " << hex << MyDisasm.EIP << endl;
		/* ============================= Loop for Disasm */
		while ((i < 400)) {
			itm = new QTreeWidgetItem(aDialog->ui.treeWidget);
			len = Disasm(&MyDisasm);
			string str1;
			int len2;//because len = -1 for unknown opcodes
			if (len > 0)
				len2 = len;
			else
				len2 = 1;
			str1[len2 * 2] = 0;//assigning null terminator to string
			for (int u = 0; u < len2; ++u)
			{
				if (((((*(unsigned char*)(MyDisasm.EIP + u)) >> 4) & 0xF) < 0xA) && ((((*(unsigned char*)(MyDisasm.EIP + u)) >> 4) & 0xF) >= 0))
					str1[u * 2] = (((*(unsigned char*)(MyDisasm.EIP  + u)) >> 4) & 0xF) + '30';
				else
					str1[u * 2] = (((*(unsigned char*)(MyDisasm.EIP  + u)) >> 4) & 0xF) + '37';
				if ((((*(unsigned char*)(MyDisasm.EIP + u)) & 0xF) < 0xA) && (((*(unsigned char*)(MyDisasm.EIP + u)) & 0xF) >= 0))
					str1[u * 2 + 1] = ((*(unsigned char*)(MyDisasm.EIP  + u)) & 0xF) + '30';
				else
					str1[u * 2 + 1] = ((*(unsigned char*)(MyDisasm.EIP  + u)) & 0xF) + '37';
			}
			itm->setText(1, str1.c_str());
			if (len != UNKNOWN_OPCODE) {
				str.clear();
				stre.str("");
				MyDisasm.EIP = MyDisasm.EIP + len;
				MyDisasm.VirtualAddr = MyDisasm.VirtualAddr + len;
				i++;
				stre << std::hex << nTargetedProcessAddress;
				str = stre.str();
				itm->setText(0, str.c_str());
				itm->setText(2, MyDisasm.CompleteInstr);
				nTargetedProcessAddress += len;
			}
			else { //error
				i++;
				Error = false;
				++MyDisasm.EIP;
				++MyDisasm.VirtualAddr;
				str.clear();
				stre.str("");
				stre << std::hex << nTargetedProcessAddress;
				str = stre.str();
				itm->setText(0, str.c_str());
				itm->setText(2, "???");
				++nTargetedProcessAddress;
			}
		};
		fout << "finished disasembling." << endl;
	}
	else
		fout << "Disassembling didn't take place because there is no memory block." << endl;
	return 0;
}


void printError(TCHAR* msg);
void QtPro::ShowAboutDialog()
{
	this->w = new About(this);
	w->setModal(true);
	w->setAttribute(Qt::WA_DeleteOnClose);
	w->show();
	
}
void QtPro::UpdateProcessName()
{

}
void MemoryViewer::showDlls()
{
	ShowDll * showDll = new ShowDll(this);
}
void MemoryViewer::ShowGotoDialogBox()
{
	InputGotoBox * inputgotobox = new InputGotoBox(this, this);
	inputgotobox->setWindowFlags(inputgotobox->windowFlags() & ~Qt::WindowContextHelpButtonHint);
	inputgotobox->show();
	inputgotobox->ui.textEdit->setFocus();
}
void QtPro::ShowPickProcess()
{
	OpenProcessClass* w = new OpenProcessClass(this, this);
	w->show();
	w->setAttribute(Qt::WA_DeleteOnClose);//takes care of releasing memory when the object is closed
	GetProcessList(w->ui.listView);
}
void QtPro::OpenHandle()
{

}
void foo(MemoryViewer * aDialog)
{
	QTreeWidgetItem* itm = new QTreeWidgetItem(aDialog->ui.treeWidget);
	itm->setText(0, "premier text");
	itm->setText(1, "second text");
	itm->setText(2, "troisieme text");
	//ui.treeWidget->addScrollBarWidget(ui.treeWidget, Qt::AlignmentFlag::AlignAbsolute);
}
MemoryViewer::MemoryViewer(QWidget *parent) : QMainWindow(parent)
{
	ui.setupUi(this);

	ui.treeWidget->setHeaderLabels(QStringList() << "Address" << "Bytes" << "Opcode" << "Comment");
	ui.treeWidget->setColumnWidth(0, 110);
	ui.treeWidget->setColumnWidth(1, 150);
	ui.treeWidget->setColumnWidth(2, 250);
	if (DebuggedProc.hwnd)
	insertDisas(this);
	QObject::connect(ui.viewDlls, &QAction::triggered, this, &MemoryViewer::showDlls);
	QObject::connect(ui.shortcut, &QShortcut::activated, this, &MemoryViewer::ShowGotoDialogBox);
}


QtPro::QtPro(QWidget *parent)
	: QMainWindow(parent)
{
	ui.setupUi(this);
	//setCentralWidget(ui.plainTextEdit);
	QObject::connect(ui.actionAbout, &QAction::triggered, this, &QtPro::ShowAboutDialog);
	QObject::connect(ui.toolButton, &QToolButton::clicked, this, &QtPro::ShowPickProcess);// &QtPro::ShowPickProcess);
	QObject::connect(ui.pushButton, &QPushButton::clicked, this, &QtPro::ShowMemoryView);
}
void QtPro::ShowMemoryView()
{
	MemoryViewer * w = new MemoryViewer(this);
	w->setAttribute(Qt::WA_DeleteOnClose);
	w->show();
}
MemoryViewer::~MemoryViewer()
{

}
QtPro::~QtPro()
{

}
void QtPro::closeDialog()
{
	delete w;
}
About::About(QWidget *parent) : QDialog(parent)
{
	this->setWindowFlags(this->windowFlags() & ~Qt::WindowContextHelpButtonHint);
	QWptr = parent;
	ui.setupUi(this);
}
About::~About()
{
	//QWptr->closeDialog();
}
void OpenProcessClass::SelectProcess()
{
	QString strChosenProcess = this->ui.listView->currentItem()->text();
	DebuggedProc.hwnd = ReturnProcessHandle(strChosenProcess);
	pQtPro->ui.lblProcessName->setText(strChosenProcess);
	this->close();
}
OpenProcessClass::OpenProcessClass(QWidget *parent, QtPro* MainWind) : QDialog(parent)
{
	pQtPro = MainWind;
	this->setWindowFlags(this->windowFlags() & ~Qt::WindowContextHelpButtonHint);
	ui.setupUi(this);
	QObject::connect(ui.pushButton_2, &QPushButton::clicked, this, &OpenProcessClass::SelectProcess);
}
OpenProcessClass::~OpenProcessClass()
{

}

void printError(TCHAR* msg)
{
	DWORD eNum;
	TCHAR sysMsg[256];
	TCHAR* p;

	eNum = GetLastError();
	FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
		NULL, eNum,
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language
		sysMsg, 256, NULL);

	// Trim the end of the line and terminate it with a null
	p = sysMsg;
	while ((*p > 31) || (*p == 9))
		++p;
	do { *p-- = 0; } while ((p >= sysMsg) &&
		((*p == '.') || (*p < 33)));

	// Display the message
	_tprintf(TEXT("\n  WARNING: %s failed with error %d (%s)"), msg, eNum, sysMsg);
}
BOOL GetProcessList(QListWidget * listwidget)
{
	HANDLE hProcessSnap;
	HANDLE hProcess;
	PROCESSENTRY32 pe32;
	DWORD dwPriorityClass;

	// Take a snapshot of all processes in the system.
	hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if (hProcessSnap == INVALID_HANDLE_VALUE)
	{
		printError(TEXT("CreateToolhelp32Snapshot (of processes)"));
		return(FALSE);
	}

	// Set the size of the structure before using it.
	pe32.dwSize = sizeof(PROCESSENTRY32);

	// Retrieve information about the first process,
	// and exit if unsuccessful
	if (!Process32First(hProcessSnap, &pe32))
	{
		printError(TEXT("Process32First")); // show cause of failure
		CloseHandle(hProcessSnap);          // clean the snapshot object
		return(FALSE);
	}

	// Now walk the snapshot of processes, and
	// display information about each process in turn
	do
	{
		_tprintf(TEXT("\n\n====================================================="));
		_tprintf(TEXT("\nPROCESS NAME:  %s"), pe32.szExeFile);
		_tprintf(TEXT("\n-------------------------------------------------------"));
		std::wstring ws(pe32.szExeFile);
		// your new String
		std::string str(ws.begin(), ws.end());
		listwidget->addItem(QString::fromWCharArray(pe32.szExeFile));
		// Retrieve the priority class.
		dwPriorityClass = 0;
		hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pe32.th32ProcessID);
		if (hProcess == NULL)
		{
			printError(TEXT("OpenProcess"));
			//listwidget->addItem("access denied");
			QListWidgetItem * ptr = listwidget->item(listwidget->count()-1);
			QColor h;
			ptr->setBackgroundColor(Qt::red);
		}
		else
		{
			dwPriorityClass = GetPriorityClass(hProcess);
			if (!dwPriorityClass)
				printError(TEXT("GetPriorityClass"));
			CloseHandle(hProcess);
		}

		_tprintf(TEXT("\n  Process ID        = 0x%08X"), pe32.th32ProcessID);
		_tprintf(TEXT("\n  Thread count      = %d"), pe32.cntThreads);
		_tprintf(TEXT("\n  Parent process ID = 0x%08X"), pe32.th32ParentProcessID);
		_tprintf(TEXT("\n  Priority base     = %d"), pe32.pcPriClassBase);
		if (dwPriorityClass)
			_tprintf(TEXT("\n  Priority class    = %d"), dwPriorityClass);

		// List the modules and threads associated with this process
		//ListProcessModules(pe32.th32ProcessID);
		//ListProcessThreads(pe32.th32ProcessID);

	} while (Process32Next(hProcessSnap, &pe32));

	CloseHandle(hProcessSnap);
	return(TRUE);
}
HANDLE  ReturnProcessHandle(QString Qstr)
{
	PROCESSENTRY32 entry;
	entry.dwSize = sizeof(PROCESSENTRY32);

	HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL);

	if (Process32First(snapshot, &entry) == TRUE)
	{
		while (Process32Next(snapshot, &entry) == TRUE)
		{
			std::wstring ws(entry.szExeFile);
			// your new String
			std::string str(ws.begin(), ws.end());
			if (stricmp(str.c_str(), Qstr.toStdString().c_str()) == 0)
			{
				//Give maximum rights to our memory viewer
				ProcessPriv(DebuggedProc.hwnd);
				//Retrieve target process handle
				DebuggedProc.hwnd = OpenProcess(PROCESS_ALL_ACCESS, FALSE, entry.th32ProcessID);
				if (DebuggedProc.hwnd == 0)
					fout << "Could not open process and get the rights." << endl;
				else
					fout << "Opened process with ALL ACCESS." << endl;
				PBOOL pbool = 0;
				BOOL av;
				pbool = &av;
				IsWow64Process(DebuggedProc.hwnd, pbool);
				if (*pbool) // on 64 bit windows, Iswow64Process returns false if process is 64 and true if process is 32
				{
					DebuggedProc.architecture = 0; // 0 for 32 bit : BEA engine doc.
					fout << "32 bit process." << endl;
				}
				else
				{
					DebuggedProc.architecture = 64;
					fout << "64 bit process." << endl;
				}
				// Do stuff..
				//TerminateProcess(*hProcess, 2);
				//CloseHandle(hProcess);
				HMODULE *modarray = new HMODULE[200];
				DWORD o;
				if (0 == EnumProcessModules(DebuggedProc.hwnd, modarray, sizeof(HMODULE) * 200, &o))
					fout << "Enum Process failed" << endl;
				MODULEINFO  lpmod;
				if (0 == GetModuleInformation(DebuggedProc.hwnd,
					modarray[0],
					&lpmod,
					sizeof(lpmod)
				))
					fout << "GetModuleInformation failed." << endl;
				DebuggedProc.addressOfInterest = reinterpret_cast<int64_t>(lpmod.EntryPoint);
				fout << "Entrypoint : " << hex << DebuggedProc.addressOfInterest << endl;
				return DebuggedProc.hwnd;
			}
		}
	}

	CloseHandle(snapshot);
	return DebuggedProc.hwnd;
}