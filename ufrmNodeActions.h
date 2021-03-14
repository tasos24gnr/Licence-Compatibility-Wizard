#ifndef ufrmNodeActionsH
#define ufrmNodeActionsH
#include <System.Classes.hpp>
#include <FMX.Controls.hpp>
#include <FMX.Forms.hpp>
#include <FMX.Controls.Presentation.hpp>
#include <FMX.Layouts.hpp>
#include <FMX.StdCtrls.hpp>
#include <FMX.Types.hpp>
#include "ufrmMain.h"
#include "ufrmLicenceEditor.h"

class TfrmNodeActions : public TForm
{
__published:	// IDE-managed Components
	TGridPanelLayout *GridPanelLayout1;
	TButton *btnEdit;
	TButton *btnDelete;
	TButton *btnCancel;
	void __fastcall btnEditClick(TObject *Sender);
	void __fastcall btnDeleteClick(TObject *Sender);
	void __fastcall btnCancelClick(TObject *Sender);
private:	// User declarations
public:		// User declarations
	__fastcall TfrmNodeActions(TComponent* Owner);
};

extern PACKAGE TfrmNodeActions *frmNodeActions;

#endif
