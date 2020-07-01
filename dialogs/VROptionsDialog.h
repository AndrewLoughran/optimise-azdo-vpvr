#ifndef H_VR_OPTIONS_DIALOG
#define  H_VR_OPTIONS_DIALOG

class VROptionsDialog : public CDialog
{
public:
   VROptionsDialog();

protected:
   virtual BOOL OnInitDialog();
   virtual INT_PTR DialogProc(UINT uMsg, WPARAM wParam, LPARAM lParam);
   virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
   virtual void OnOK();
   virtual void OnClose();

private:
   void AddToolTip(char *text, HWND parentHwnd, HWND toolTipHwnd, HWND controlHwnd);
   void ResetVideoPreferences(); // 0 = default, 1 = lowend PC, 2 = highend PC
   void FillVideoModesList(const eastl::vector<VideoMode>& modes, const VideoMode* curSelMode = 0);
   size_t getBestMatchingAAfactorIndex(float f);

   eastl::vector<VideoMode> allVideoModes;
};

#endif
