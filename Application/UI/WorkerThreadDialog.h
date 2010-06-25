#pragma once

#include "Application/API.h"

#include <wx/dialog.h>

namespace Nocturnal
{
    class DialogWorkerThread;

    /////////////////////////////////////////////////////////////////////////////
    // Class defining a dialog that creates and runs a thread when ShowModal()
    // is called.  Allows the main application to block by showing a modal dialog
    // while the worker thread does its processing.  You must call InitThread()
    // before you can call ShowModal() on this dialog.
    // 
    class APPLICATION_API WorkerThreadDialog : public wxDialog
    {
    private:
        DialogWorkerThread* m_Thread;

    public:
        WorkerThreadDialog( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = wxDEFAULT_DIALOG_STYLE, const wxString& name = wxT( "WorkerThreadDialog" ) );
        virtual ~WorkerThreadDialog();

        void InitThread( DialogWorkerThread* thread );
        virtual int ShowModal();
    };
}
