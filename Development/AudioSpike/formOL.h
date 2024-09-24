//---------------------------------------------------------------------------

#ifndef formOLH
#define formOLH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ExtCtrls.hpp>
#include <Inifiles.hpp>
#include <IdBaseComponent.hpp>
#include <IdComponent.hpp>
#include <IdHTTP.hpp>
#include <IdTCPClient.hpp>
#include <IdTCPConnection.hpp>
//---------------------------------------------------------------------------

#ifndef TRYDELETENULL
#  define TRYDELETENULL(p) { if (p!=NULL) delete p; }
#endif

//---------------------------------------------------------------------------
class TfrmOL : public TForm
{
__published: // IDE-verwaltete Komponenten
   TGroupBox *gb;
   TLabeledEdit *edtProductKey;
   TButton *btnOK;
   TButton *btnActivate;
   TLabeledEdit *edtHostId;
   TButton *btnClipboard;
   void __fastcall btnActivateClick(TObject *Sender);
   void __fastcall edtProductKeyChange(TObject *Sender);
   void __fastcall btnClipboardClick(TObject *Sender);
private: // Benutzer-Deklarationen
   TIniFile* m_pIni;
   void UpdateGUI(bool bEnable);
public: // Benutzer-Deklarationen
   __fastcall TfrmOL(TComponent* Owner);
   __fastcall ~TfrmOL();
   void EditOnlineL(UnicodeString usIniFile);
   void ShowError(UnicodeString usAction, UnicodeString usError);
};
//---------------------------------------------------------------------------
extern PACKAGE TfrmOL *frmOL;
//---------------------------------------------------------------------------
#endif
