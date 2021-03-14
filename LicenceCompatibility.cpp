// ---------------------------------------------------------------------------

#include <fmx.h>
#ifdef _WIN32
#include <tchar.h>
#endif
#pragma hdrstop
#include <System.StartUpCopy.hpp>
#include <FMX.Types.hpp>
// ---------------------------------------------------------------------------
USEFORM("ufrmMain.cpp", frmMain);
USEFORM("ufrmNodeActions.cpp", frmNodeActions);
USEFORM("ufrmLicenceEditor.cpp", frmLicenceEditor);
USEFORM("ufrmCheckCompatibility.cpp", frmCheckCompatibility);

// ---------------------------------------------------------------------------
extern "C" int FMXmain()
{
	try
	{
		Application->Initialize();
		Application->CreateForm(__classid(TfrmMain), &frmMain);
		Application->CreateForm(__classid(TfrmLicenceEditor), &frmLicenceEditor);
		Application->CreateForm(__classid(TfrmNodeActions), &frmNodeActions);
		Application->CreateForm(__classid(TfrmCheckCompatibility), &frmCheckCompatibility);
		Application->Run();
	}
	catch (Exception &exception)
	{
		Application->ShowException(&exception);
	}
	catch (...)
	{
		try
		{
			throw Exception("");
		}
		catch (Exception &exception)
		{
			Application->ShowException(&exception);
		}
	}
	return 0;
}
// ---------------------------------------------------------------------------
