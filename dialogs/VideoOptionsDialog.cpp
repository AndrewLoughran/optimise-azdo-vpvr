#include "StdAfx.h"
#include "resource.h"
#include "VideoOptionsDialog.h"

#ifdef ENABLE_VR
#include "openvr.h"
#endif

#define GET_WINDOW_MODES		WM_USER+100
#define GET_FULLSCREENMODES		WM_USER+101
#define RESET_SIZELIST_CONTENT	WM_USER+102

const int rgwindowsize[] = { 640, 720, 800, 912, 1024, 1152, 1280, 1600 };  // windowed resolutions for selection list

VideoOptionsDialog::VideoOptionsDialog() : CDialog(IDD_VIDEO_OPTIONS)
{
}

void VideoOptionsDialog::AddToolTip(char *text, HWND parentHwnd, HWND toolTipHwnd, HWND controlHwnd)
{
   TOOLINFO toolInfo = { 0 };
   toolInfo.cbSize = sizeof(toolInfo);
   toolInfo.hwnd = parentHwnd;
   toolInfo.uFlags = TTF_IDISHWND | TTF_SUBCLASS;
   toolInfo.uId = (UINT_PTR)controlHwnd;
   toolInfo.lpszText = text;
   SendMessage(toolTipHwnd, TTM_ADDTOOL, 0, (LPARAM)&toolInfo);
}

void VideoOptionsDialog::ResetVideoPreferences(const unsigned int profile) // 0 = default, 1 = lowend PC, 2 = highend PC
{
   SendMessage(GetDlgItem(IDC_FULLSCREEN).GetHwnd(), BM_SETCHECK, true ? BST_CHECKED : BST_UNCHECKED, 0);

   const int depthcur = LoadValueIntWithDefault("Player", "ColorDepth", 32);

   const int refreshrate = LoadValueIntWithDefault("Player", "RefreshRate", 0);

   const int widthcur = LoadValueIntWithDefault("Player", "Width", true ? DEFAULT_PLAYER_FS_WIDTH : DEFAULT_PLAYER_WIDTH);

   const int heightcur = LoadValueIntWithDefault("Player", "Height", widthcur * 9 / 16);

   SendMessage(GetHwnd(), true ? GET_FULLSCREENMODES : GET_WINDOW_MODES, widthcur << 16 | refreshrate, heightcur << 16 | depthcur);

   SendMessage(GetDlgItem(IDC_10BIT_VIDEO).GetHwnd(), BM_SETCHECK, false ? BST_CHECKED : BST_UNCHECKED, 0);
   SendMessage(GetDlgItem(IDC_Tex3072).GetHwnd(), BM_SETCHECK, BST_UNCHECKED, 0);
   SendMessage(GetDlgItem(IDC_Tex1024).GetHwnd(), BM_SETCHECK, BST_UNCHECKED, 0);
   SendMessage(GetDlgItem(IDC_Tex2048).GetHwnd(), BM_SETCHECK, profile == 1 ? BST_CHECKED : BST_UNCHECKED, 0);
   SendMessage(GetDlgItem(IDC_TexUnlimited).GetHwnd(), BM_SETCHECK, profile != 1 ? BST_CHECKED : BST_UNCHECKED, 0);
   SendMessage(GetDlgItem(IDC_GLOBAL_REFLECTION_CHECK).GetHwnd(), BM_SETCHECK, profile != 1 ? BST_CHECKED : BST_UNCHECKED, 0);
   SendMessage(GetDlgItem(IDC_GLOBAL_TRAIL_CHECK).GetHwnd(), BM_SETCHECK, true ? BST_CHECKED : BST_UNCHECKED, 0);
   SetDlgItemInt(IDC_ADAPTIVE_VSYNC, 0, FALSE);
   SetDlgItemInt(IDC_MAX_PRE_FRAMES, 0, FALSE);
   float ballAspecRatioOffsetX = 0.0f;
   char tmp[256];
   sprintf_s(tmp, 256, "%f", ballAspecRatioOffsetX);
   SetDlgItemTextA(IDC_CORRECTION_X, tmp);
   float ballAspecRatioOffsetY = 0.0f;
   sprintf_s(tmp, 256, "%f", ballAspecRatioOffsetY);
   SetDlgItemTextA(IDC_CORRECTION_Y, tmp);
   float latitude = 52.52f;
   sprintf_s(tmp, 256, "%f", latitude);
   SetDlgItemTextA(IDC_DN_LATITUDE, tmp);
   float longitude = 13.37f;
   sprintf_s(tmp, 256, "%f", longitude);
   SetDlgItemTextA(IDC_DN_LONGITUDE, tmp);
   float nudgeStrength = 2e-2f;
   sprintf_s(tmp, 256, "%f", nudgeStrength);
   SetDlgItemTextA(IDC_NUDGE_STRENGTH, tmp);
   SendMessage(GetDlgItem(IDC_AA_ALL_TABLES).GetHwnd(), BM_SETCHECK, false ? BST_CHECKED : BST_UNCHECKED, 0);
   SendMessage(GetDlgItem(IDC_DYNAMIC_DN).GetHwnd(), BM_SETCHECK, false ? BST_CHECKED : BST_UNCHECKED, 0);
   SendMessage(GetDlgItem(IDC_DYNAMIC_AO).GetHwnd(), BM_SETCHECK, profile == 2 ? BST_CHECKED : BST_UNCHECKED, 0);
   SendMessage(GetDlgItem(IDC_ENABLE_AO).GetHwnd(), BM_SETCHECK, true ? BST_CHECKED : BST_UNCHECKED, 0);
   SendMessage(GetDlgItem(IDC_GLOBAL_SSREFLECTION_CHECK).GetHwnd(), BM_SETCHECK, profile == 2 ? BST_CHECKED : BST_UNCHECKED, 0);
   SendMessage(GetDlgItem(IDC_GLOBAL_PFREFLECTION_CHECK).GetHwnd(), BM_SETCHECK, profile != 1 ? BST_CHECKED : BST_UNCHECKED, 0);
   SendMessage(GetDlgItem(IDC_OVERWRITE_BALL_IMAGE_CHECK).GetHwnd(), BM_SETCHECK, false ? BST_CHECKED : BST_UNCHECKED, 0);
   SetDlgItemText(IDC_BALL_IMAGE_EDIT, "");
   SetDlgItemText(IDC_BALL_DECAL_EDIT, "");
   if (true)
   {
      ::EnableWindow(GetDlgItem(IDC_BROWSE_BALL_IMAGE).GetHwnd(), FALSE);
      ::EnableWindow(GetDlgItem(IDC_BROWSE_BALL_DECAL).GetHwnd(), FALSE);
      ::EnableWindow(GetDlgItem(IDC_BALL_IMAGE_EDIT).GetHwnd(), FALSE);
      ::EnableWindow(GetDlgItem(IDC_BALL_DECAL_EDIT).GetHwnd(), FALSE);
   }
   SendMessage(GetDlgItem(IDC_FXAACB).GetHwnd(), CB_SETCURSEL, profile == 1 ? 0 : (profile == 2 ? 3 : 2), 0);
   SendMessage(GetDlgItem(IDC_SCALE_FX_DMD).GetHwnd(), BM_SETCHECK, false ? BST_CHECKED : BST_UNCHECKED, 0);
   SendMessage(GetDlgItem(IDC_BG_SET).GetHwnd(), BM_SETCHECK, false ? BST_CHECKED : BST_UNCHECKED, 0);
   SendMessage(GetDlgItem(IDC_3D_STEREO).GetHwnd(), CB_SETCURSEL, 0, 0);
   updateStereoVisibility(0);
   SendMessage(GetDlgItem(IDC_3D_STEREO_Y).GetHwnd(), BM_SETCHECK, false ? BST_CHECKED : BST_UNCHECKED, 0);
   float stereo3DOfs = 0.0f;
   sprintf_s(tmp, 256, "%f", stereo3DOfs);
   SetDlgItemTextA(IDC_3D_STEREO_OFS, tmp);
   float stereo3DMS = 0.03f;
   sprintf_s(tmp, 256, "%f", stereo3DMS);
   SetDlgItemTextA(IDC_3D_STEREO_MS, tmp);
   float stereo3DZPD = 0.5f;
   sprintf_s(tmp, 256, "%f", stereo3DZPD);
   SetDlgItemTextA(IDC_3D_STEREO_ZPD, tmp);
   float vrSlope = 6.5f;
   sprintf_s(tmp, 256, "%f0.1", vrSlope);
   SetDlgItemTextA(IDC_VR_SLOPE, tmp);
   float vrOrientation = 0.0f;
   sprintf_s(tmp, 256, "%f0.1", vrOrientation);
   SetDlgItemTextA(IDC_3D_VR_ORIENTATION, tmp);
   float vrX = 0.0f;
   sprintf_s(tmp, 256, "%f0.1", vrX);
   SetDlgItemTextA(IDC_VR_OFFSET_X, tmp);
   float vrY = 0.0f;
   sprintf_s(tmp, 256, "%f0.1", vrY);
   SetDlgItemTextA(IDC_VR_OFFSET_Y, tmp);
   float vrZ = 80.0f;
   sprintf_s(tmp, 256, "%0.1f", vrZ);
   SetDlgItemTextA(IDC_VR_OFFSET_Z, tmp);
   SendMessage(GetDlgItem(IDC_USE_NVIDIA_API_CHECK).GetHwnd(), BM_SETCHECK, false ? BST_CHECKED : BST_UNCHECKED, 0);
   SendMessage(GetDlgItem(IDC_BLOOM_OFF).GetHwnd(), BM_SETCHECK, false ? BST_CHECKED : BST_UNCHECKED, 0);
   SendMessage(GetDlgItem(IDC_DISABLE_DWM).GetHwnd(), BM_SETCHECK, false ? BST_CHECKED : BST_UNCHECKED, 0);
   SendMessage(GetDlgItem(IDC_FORCE_ANISO).GetHwnd(), BM_SETCHECK, profile != 1 ? BST_CHECKED : BST_UNCHECKED, 0);
   SendMessage(GetDlgItem(IDC_TEX_COMPRESS).GetHwnd(), BM_SETCHECK, false ? BST_CHECKED : BST_UNCHECKED, 0);
   SendMessage(GetDlgItem(IDC_SOFTWARE_VP).GetHwnd(), BM_SETCHECK, false ? BST_CHECKED : BST_UNCHECKED, 0);
   SendMessage(GetDlgItem(IDC_ARASlider).GetHwnd(), TBM_SETPOS, TRUE, profile == 1 ? 5 : 10);
   SendMessage(GetDlgItem(IDC_StretchYes).GetHwnd(), BM_SETCHECK, BST_UNCHECKED, 0);
   SendMessage(GetDlgItem(IDC_StretchMonitor).GetHwnd(), BM_SETCHECK, BST_UNCHECKED, 0);
   SendMessage(GetDlgItem(IDC_StretchNo).GetHwnd(), BM_SETCHECK, BST_CHECKED, 0);
   SendMessage(GetDlgItem(IDC_MonitorCombo).GetHwnd(), CB_SETCURSEL, 1, 0);
   SendMessage(GetDlgItem(IDC_DISPLAY_ID).GetHwnd(), CB_SETCURSEL, 0, 0);
   //AMD Debug
   SendMessage(GetDlgItem(IDC_COMBO_TEXTURE).GetHwnd(), CB_SETCURSEL, 1, 0);
   SendMessage(GetDlgItem(IDC_COMBO_BLIT).GetHwnd(), CB_SETCURSEL, 0, 0);
}

void VideoOptionsDialog::FillVideoModesList(const std::vector<VideoMode>& modes, const VideoMode* curSelMode)
{
   const HWND hwndList = GetDlgItem(IDC_SIZELIST).GetHwnd();
   SendMessage(hwndList, LB_RESETCONTENT, 0, 0);
   int bestMatch = 0; // to find closest matching res
   int bestMatchingPoints = 0; // dto.

   int screenwidth;
   int screenheight;
   int x, y;
   const int display = (int)SendMessage(GetDlgItem(IDC_DISPLAY_ID).GetHwnd(), CB_GETCURSEL, 0, 0);
   getDisplaySetupByID(display, x, y, screenwidth, screenheight);

   for (size_t i = 0; i < modes.size(); ++i)
   {
      char szT[128];

#ifdef ENABLE_SDL
      if (modes[i].depth) // i.e. is this windowed or not
         sprintf_s(szT, "%d x %d (%dHz) %s", modes[i].width, modes[i].height, modes[i].refreshrate, (modes[i].depth == 32) ? "32bit" :
            (modes[i].depth == 30) ? "HDR" :
            (modes[i].depth == 16) ? "16bit" : "");
#else
      if (modes[i].depth)
         sprintf_s(szT, "%d x %d (%dHz)", modes[i].width, modes[i].height, /*modes[i].depth,*/ modes[i].refreshrate);
#endif
      else
         sprintf_s(szT, "%d x %d", modes[i].width, modes[i].height);

      SendMessage(hwndList, LB_ADDSTRING, 0, (LPARAM)szT);
      if (curSelMode) {
         int matchingPoints = 0;
         if (modes[i].width == curSelMode->width) matchingPoints += 100;
         if (modes[i].height == curSelMode->height) matchingPoints += 100;
         if (modes[i].depth == curSelMode->depth) matchingPoints += 50;
         if (modes[i].refreshrate == curSelMode->refreshrate) matchingPoints += 10;
         if (modes[i].width == screenwidth) matchingPoints += 3;
         if (modes[i].height == screenheight) matchingPoints += 3;
         if (modes[i].refreshrate == DEFAULT_PLAYER_FS_REFRESHRATE) matchingPoints += 1;
         if (matchingPoints > bestMatchingPoints) {
            bestMatch = (int)i;
            bestMatchingPoints = matchingPoints;
         }
      }
   }
   SendMessage(hwndList, LB_SETCURSEL, bestMatch, 0);
}

void VideoOptionsDialog::updateStereoVisibility(int stereo3D) {
   if (stereo3D == STEREO_INT || stereo3D == STEREO_SBS) stereo3D = STEREO_TB;
   GetDlgItem(IDC_3D_STEREO_Y).ShowWindow(stereo3D == STEREO_TB ? SW_SHOW : SW_HIDE);
   GetDlgItem(IDC_3D_STEREO_OFS).ShowWindow(stereo3D == STEREO_TB ? SW_SHOW : SW_HIDE);
   GetDlgItem(IDC_3D_STEREO_OFS_LABEL).ShowWindow(stereo3D == STEREO_TB ? SW_SHOW : SW_HIDE);
   GetDlgItem(IDC_3D_STEREO_MS).ShowWindow(stereo3D == STEREO_TB ? SW_SHOW : SW_HIDE);
   GetDlgItem(IDC_3D_STEREO_MS_LABEL).ShowWindow(stereo3D == STEREO_TB ? SW_SHOW : SW_HIDE);
   GetDlgItem(IDC_3D_STEREO_ZPD).ShowWindow(stereo3D == STEREO_TB ? SW_SHOW : SW_HIDE);
   GetDlgItem(IDC_3D_STEREO_ZPD_LABEL).ShowWindow(stereo3D == STEREO_TB ? SW_SHOW : SW_HIDE);

   GetDlgItem(IDC_VR_DISABLE_PREVIEW).ShowWindow(stereo3D == STEREO_VR ? SW_SHOW : SW_HIDE);
   GetDlgItem(IDC_VR_SLOPE).ShowWindow(stereo3D == STEREO_VR ? SW_SHOW : SW_HIDE);
   GetDlgItem(IDC_VR_SLOPE_LABEL).ShowWindow(stereo3D == STEREO_VR ? SW_SHOW : SW_HIDE);
   GetDlgItem(IDC_3D_VR_ORIENTATION).ShowWindow(stereo3D == STEREO_VR ? SW_SHOW : SW_HIDE);
   GetDlgItem(IDC_3D_VR_ORIENTATION_LABEL).ShowWindow(stereo3D == STEREO_VR ? SW_SHOW : SW_HIDE);
   GetDlgItem(IDC_VR_OFFSET_X).ShowWindow(stereo3D == STEREO_VR ? SW_SHOW : SW_HIDE);
   GetDlgItem(IDC_VR_OFFSET_X_LABEL).ShowWindow(stereo3D == STEREO_VR ? SW_SHOW : SW_HIDE);
   GetDlgItem(IDC_VR_OFFSET_Y).ShowWindow(stereo3D == STEREO_VR ? SW_SHOW : SW_HIDE);
   GetDlgItem(IDC_VR_OFFSET_Y_LABEL).ShowWindow(stereo3D == STEREO_VR ? SW_SHOW : SW_HIDE);
   GetDlgItem(IDC_VR_OFFSET_Z).ShowWindow(stereo3D == STEREO_VR ? SW_SHOW : SW_HIDE);
   GetDlgItem(IDC_VR_OFFSET_Z_LABEL).ShowWindow(stereo3D == STEREO_VR ? SW_SHOW : SW_HIDE);

   GetDlgItem(IDC_HEADTRACKING).ShowWindow(stereo3D != STEREO_VR ? SW_SHOW : SW_HIDE);
}

BOOL VideoOptionsDialog::OnInitDialog()
{
   const HWND hwndDlg = GetHwnd();
   const HWND toolTipHwnd = CreateWindowEx(NULL, TOOLTIPS_CLASS, NULL, WS_POPUP | TTS_ALWAYSTIP | TTS_BALLOON, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, hwndDlg, NULL, g_hinst, NULL);
   if (toolTipHwnd)
   {
      SendMessage(toolTipHwnd, TTM_SETMAXTIPWIDTH, 0, 180);
      HWND controlHwnd = GetDlgItem(IDC_USE_NVIDIA_API_CHECK).GetHwnd();
      AddToolTip("Activate this if you get the corresponding error message on table start, or if you experience rendering problems.", hwndDlg, toolTipHwnd, controlHwnd);
      controlHwnd = GetDlgItem(IDC_BLOOM_OFF).GetHwnd();
      AddToolTip("Forces the bloom filter to be always off. Only for very low-end graphics cards.", hwndDlg, toolTipHwnd, controlHwnd);
      controlHwnd = GetDlgItem(IDC_TEX_COMPRESS).GetHwnd();
      AddToolTip("This saves memory on your graphics card but harms quality of the textures.", hwndDlg, toolTipHwnd, controlHwnd);
      controlHwnd = GetDlgItem(IDC_DISABLE_DWM).GetHwnd();
      AddToolTip("Disable Windows Desktop Composition (only works on Windows Vista and Windows 7 systems).\r\nMay reduce lag and improve performance on some setups.", hwndDlg, toolTipHwnd, controlHwnd);
      controlHwnd = GetDlgItem(IDC_SOFTWARE_VP).GetHwnd();
      AddToolTip("Activate this if you have issues using an Intel graphics chip.", hwndDlg, toolTipHwnd, controlHwnd);
      controlHwnd = GetDlgItem(IDC_ADAPTIVE_VSYNC).GetHwnd();
      AddToolTip("1-activates VSYNC for every frame (avoids tearing)\r\n2-adaptive VSYNC, waits only for fast frames (e.g. over 60fps)\r\nor set it to e.g. 60 or 120 to limit the fps to that value (energy saving/less heat)", hwndDlg, toolTipHwnd, controlHwnd);
      controlHwnd = GetDlgItem(IDC_MAX_PRE_FRAMES).GetHwnd();
      AddToolTip("Experiment with 1 or 2 for a chance of lag reduction at the price of a bit of performance.", hwndDlg, toolTipHwnd, controlHwnd);
      controlHwnd = GetDlgItem(IDC_StretchMonitor).GetHwnd();
      AddToolTip("If played in cabinet mode and you get an egg shaped ball activate this.\r\nFor screen ratios other than 16:9 you may have to adjust the offsets.\r\nNormally you have to set the Y offset (around 1.5) but you have to experiment.", hwndDlg, toolTipHwnd, controlHwnd);
      controlHwnd = GetDlgItem(IDC_NUDGE_STRENGTH).GetHwnd();
      AddToolTip("Changes the visual effect/screen shaking when nudging the table.", hwndDlg, toolTipHwnd, controlHwnd);
      controlHwnd = GetDlgItem(IDC_DYNAMIC_DN).GetHwnd();
      AddToolTip("Activate this to switch the table brightness automatically based on your PC date,clock and location.\r\nThis requires to fill in geographic coordinates for your PCs location to work correctly.\r\nYou may use openstreetmap.org for example to get these in the correct format.", hwndDlg, toolTipHwnd, controlHwnd);
      controlHwnd = GetDlgItem(IDC_DN_LATITUDE).GetHwnd();
      AddToolTip("In decimal degrees (-90..90, North positive)", hwndDlg, toolTipHwnd, controlHwnd);
      controlHwnd = GetDlgItem(IDC_DN_LONGITUDE).GetHwnd();
      AddToolTip("In decimal degrees (-180..180, East positive)", hwndDlg, toolTipHwnd, controlHwnd);
      controlHwnd = GetDlgItem(IDC_DYNAMIC_AO).GetHwnd();
      AddToolTip("Activate this to enable dynamic Ambient Occlusion.\r\nThis slows down performance, but enables contact shadows for dynamic objects.", hwndDlg, toolTipHwnd, controlHwnd);
      controlHwnd = GetDlgItem(IDC_FORCE_ANISO).GetHwnd();
      AddToolTip("Activate this to enhance the texture filtering.\r\nThis slows down performance only a bit (on most systems), but increases quality tremendously.", hwndDlg, toolTipHwnd, controlHwnd);
      controlHwnd = GetDlgItem(IDC_ENABLE_AO).GetHwnd();
      AddToolTip("Activate this to enable Ambient Occlusion.\r\nThis enables contact shadows between objects.", hwndDlg, toolTipHwnd, controlHwnd);
      controlHwnd = GetDlgItem(IDC_3D_STEREO).GetHwnd();
      AddToolTip("Activate this to enable 3D Stereo output using the requested format.\r\nSwitch on/off during play with the F10 key.\r\nThis requires that your TV can display 3D Stereo, and respective 3D glasses.", hwndDlg, toolTipHwnd, controlHwnd);
      controlHwnd = GetDlgItem(IDC_3D_STEREO_Y).GetHwnd();
      AddToolTip("Switches 3D Stereo effect to use the Y Axis.\r\nThis should usually be selected for Cabinets/rotated displays.", hwndDlg, toolTipHwnd, controlHwnd);
      controlHwnd = GetDlgItem(IDC_FULLSCREEN).GetHwnd();
      if (IsWindows10_1803orAbove())
      {
         GetDlgItem(IDC_FULLSCREEN).SetWindowText("Force exclusive Fullscreen Mode");
         AddToolTip("Enforces exclusive Fullscreen Mode.\r\nEnforcing exclusive FS can slightly reduce input lag.", hwndDlg, toolTipHwnd, controlHwnd);
      }
      else
         AddToolTip("Enforces exclusive Fullscreen Mode.\r\nDo not enable if you require to see the VPinMAME or B2S windows for example.\r\nEnforcing exclusive FS can slightly reduce input lag though.", hwndDlg, toolTipHwnd, controlHwnd);
      controlHwnd = GetDlgItem(IDC_10BIT_VIDEO).GetHwnd();
      AddToolTip("Enforces 10Bit (WCG) rendering.\r\nRequires a corresponding 10Bit output capable graphics card and monitor.\r\nAlso requires to have exclusive fullscreen mode enforced (for now).", hwndDlg, toolTipHwnd, controlHwnd);
      controlHwnd = GetDlgItem(IDC_BG_SET).GetHwnd();
      AddToolTip("Switches all tables to use the respective Cabinet display setup.\r\nAlso useful if a 270 degree rotated Desktop monitor is used.", hwndDlg, toolTipHwnd, controlHwnd);
      controlHwnd = GetDlgItem(IDC_FXAACB).GetHwnd();
      AddToolTip("Enables post-processed Anti-Aliasing.\r\nThis delivers smoother images, at the cost of slight blurring.\r\n'Quality FXAA' and 'Quality SMAA' are recommended and lead to less artifacts,\nbut will harm performance on low-end graphics cards.", hwndDlg, toolTipHwnd, controlHwnd);
      controlHwnd = GetDlgItem(IDC_AA_ALL_TABLES).GetHwnd();
      AddToolTip("Enables brute-force 4x Anti-Aliasing (similar to DSR).\r\nThis delivers very good quality, but slows down performance significantly.", hwndDlg, toolTipHwnd, controlHwnd);
      controlHwnd = GetDlgItem(IDC_OVERWRITE_BALL_IMAGE_CHECK).GetHwnd();
      AddToolTip("When checked it overwrites the ball image/decal image(s) for every table.", hwndDlg, toolTipHwnd, controlHwnd);
      controlHwnd = GetDlgItem(IDC_DISPLAY_ID).GetHwnd();
      AddToolTip("Select Display for Video output.", hwndDlg, toolTipHwnd, controlHwnd);
      //AMD Debug
      controlHwnd = GetDlgItem(IDC_COMBO_TEXTURE).GetHwnd();
      AddToolTip("Pixel format for VR Rendering.", hwndDlg, toolTipHwnd, controlHwnd);
      controlHwnd = GetDlgItem(IDC_COMBO_BLIT).GetHwnd();
      AddToolTip("Blitting technique for VR Rendering.", hwndDlg, toolTipHwnd, controlHwnd);
      controlHwnd = GetDlgItem(IDC_HEADTRACKING).GetHwnd();
      AddToolTip("Enable BAM Headtracking. See https://www.ravarcade.pl", hwndDlg, toolTipHwnd, controlHwnd);
   }

   const int maxTexDim = LoadValueIntWithDefault("Player", "MaxTexDimension", 0); // default: Don't resize textures
   switch (maxTexDim)
   {
   case 3072:SendMessage(GetDlgItem(IDC_Tex3072).GetHwnd(), BM_SETCHECK, BST_CHECKED, 0);       break;
   case 512: // legacy, map to 1024 nowadays
   case 1024:SendMessage(GetDlgItem(IDC_Tex1024).GetHwnd(), BM_SETCHECK, BST_CHECKED, 0);       break;
   case 2048:SendMessage(GetDlgItem(IDC_Tex2048).GetHwnd(), BM_SETCHECK, BST_CHECKED, 0);       break;
   default:	SendMessage(GetDlgItem(IDC_TexUnlimited).GetHwnd(), BM_SETCHECK, BST_CHECKED, 0);   break;
   }

   const bool reflection = LoadValueBoolWithDefault("Player", "BallReflection", true);
   SendMessage(GetDlgItem(IDC_GLOBAL_REFLECTION_CHECK).GetHwnd(), BM_SETCHECK, reflection ? BST_CHECKED : BST_UNCHECKED, 0);

   const bool trail = LoadValueBoolWithDefault("Player", "BallTrail", true);
   SendMessage(GetDlgItem(IDC_GLOBAL_TRAIL_CHECK).GetHwnd(), BM_SETCHECK, trail ? BST_CHECKED : BST_UNCHECKED, 0);

   const int vsync = LoadValueIntWithDefault("Player", "AdaptiveVSync", 0);

   SetDlgItemInt(IDC_ADAPTIVE_VSYNC, vsync, FALSE);

   const int maxPrerenderedFrames = LoadValueIntWithDefault("Player", "MaxPrerenderedFrames", 0);
   SetDlgItemInt(IDC_MAX_PRE_FRAMES, maxPrerenderedFrames, FALSE);

   char tmp[256];
   const float ballAspecRatioOffsetX = LoadValueFloatWithDefault("Player", "BallCorrectionX", 0.f);
   sprintf_s(tmp, 256, "%f", ballAspecRatioOffsetX);
   SetDlgItemTextA(IDC_CORRECTION_X, tmp);

   const float ballAspecRatioOffsetY = LoadValueFloatWithDefault("Player", "BallCorrectionY", 0.f);
   sprintf_s(tmp, 256, "%f", ballAspecRatioOffsetY);
   SetDlgItemTextA(IDC_CORRECTION_Y, tmp);

   const float latitude = LoadValueFloatWithDefault("Player", "Latitude", 52.52f);
   sprintf_s(tmp, 256, "%f", latitude);
   SetDlgItemTextA(IDC_DN_LATITUDE, tmp);

   const float longitude = LoadValueFloatWithDefault("Player", "Longitude", 13.37f);
   SetDlgItemTextA(IDC_DN_LONGITUDE, tmp);

   const float nudgeStrength = LoadValueFloatWithDefault("Player", "NudgeStrength", 2e-2f);
   SetDlgItemTextA(IDC_NUDGE_STRENGTH, tmp);

   const int useAA = LoadValueIntWithDefault("Player", "USEAA", 0);
   SendMessage(GetDlgItem(IDC_AA_ALL_TABLES).GetHwnd(), BM_SETCHECK, (useAA != 0) ? BST_CHECKED : BST_UNCHECKED, 0);

   const int useDN = LoadValueIntWithDefault("Player", "DynamicDayNight", 0);
   SendMessage(GetDlgItem(IDC_DYNAMIC_DN).GetHwnd(), BM_SETCHECK, (useDN != 0) ? BST_CHECKED : BST_UNCHECKED, 0);

   int useAO = LoadValueIntWithDefault("Player", "DynamicAO", 0);
   SendMessage(GetDlgItem(IDC_DYNAMIC_AO).GetHwnd(), BM_SETCHECK, (useAO != 0) ? BST_CHECKED : BST_UNCHECKED, 0);

   useAO = LoadValueIntWithDefault("Player", "DisableAO", 0);
   SendMessage(GetDlgItem(IDC_ENABLE_AO).GetHwnd(), BM_SETCHECK, (useAO != 0) ? BST_UNCHECKED : BST_CHECKED, 0); // inverted logic

   const int ssreflection = LoadValueIntWithDefault("Player", "SSRefl", 0);
   SendMessage(GetDlgItem(IDC_GLOBAL_SSREFLECTION_CHECK).GetHwnd(), BM_SETCHECK, ssreflection ? BST_CHECKED : BST_UNCHECKED, 0);

   const int pfreflection = LoadValueIntWithDefault("Player", "PFRefl", 1);
   SendMessage(GetDlgItem(IDC_GLOBAL_PFREFLECTION_CHECK).GetHwnd(), BM_SETCHECK, pfreflection ? BST_CHECKED : BST_UNCHECKED, 0);

   const int overwiteBallImage = LoadValueIntWithDefault("Player", "OverwriteBallImage", 0);
   SendMessage(GetDlgItem(IDC_OVERWRITE_BALL_IMAGE_CHECK).GetHwnd(), BM_SETCHECK, (overwiteBallImage != 0) ? BST_CHECKED : BST_UNCHECKED, 0);

   char imageName[MAX_PATH] = { 0 };
   HRESULT hr = LoadValueString("Player", "BallImage", imageName, MAX_PATH);
   if (hr != S_OK)
      imageName[0] = 0;
   SetDlgItemText(IDC_BALL_IMAGE_EDIT, imageName);
   imageName[0] = 0;
   hr = LoadValueString("Player", "DecalImage", imageName, MAX_PATH);
   if (hr != S_OK)
      imageName[0] = 0;
   SetDlgItemText(IDC_BALL_DECAL_EDIT, imageName);
   if (overwiteBallImage == 0)
   {
      ::EnableWindow(GetDlgItem(IDC_BROWSE_BALL_IMAGE).GetHwnd(), FALSE);
      ::EnableWindow(GetDlgItem(IDC_BROWSE_BALL_DECAL).GetHwnd(), FALSE);
      ::EnableWindow(GetDlgItem(IDC_BALL_IMAGE_EDIT).GetHwnd(), FALSE);
      ::EnableWindow(GetDlgItem(IDC_BALL_DECAL_EDIT).GetHwnd(), FALSE);
   }

   const int fxaa = LoadValueIntWithDefault("Player", "FXAA", 0);

   SendMessage(GetDlgItem(IDC_FXAACB).GetHwnd(), CB_ADDSTRING, 0, (LPARAM)"Disabled");
   SendMessage(GetDlgItem(IDC_FXAACB).GetHwnd(), CB_ADDSTRING, 0, (LPARAM)"Fast FXAA");
   SendMessage(GetDlgItem(IDC_FXAACB).GetHwnd(), CB_ADDSTRING, 0, (LPARAM)"Standard FXAA");
   SendMessage(GetDlgItem(IDC_FXAACB).GetHwnd(), CB_ADDSTRING, 0, (LPARAM)"Quality FXAA");
   SendMessage(GetDlgItem(IDC_FXAACB).GetHwnd(), CB_ADDSTRING, 0, (LPARAM)"Fast NFAA");
   SendMessage(GetDlgItem(IDC_FXAACB).GetHwnd(), CB_ADDSTRING, 0, (LPARAM)"Standard DLAA");
   SendMessage(GetDlgItem(IDC_FXAACB).GetHwnd(), CB_ADDSTRING, 0, (LPARAM)"Quality SMAA");
   SendMessage(GetDlgItem(IDC_FXAACB).GetHwnd(), CB_SETCURSEL, fxaa, 0);

   const bool scaleFX_DMD = LoadValueBoolWithDefault("Player", "ScaleFXDMD", false);
   SendMessage(GetDlgItem(IDC_SCALE_FX_DMD).GetHwnd(), BM_SETCHECK, scaleFX_DMD ? BST_CHECKED : BST_UNCHECKED, 0);

   const int bgset = LoadValueIntWithDefault("Player", "BGSet", 0);
   SendMessage(GetDlgItem(IDC_BG_SET).GetHwnd(), BM_SETCHECK, (bgset != 0) ? BST_CHECKED : BST_UNCHECKED, 0);

   int stereo3D;

   //Try to remain compatible with VPX in the registry
   hr = LoadValueInt("Player", "Stereo3DVR", &stereo3D);
   if (hr != S_OK || stereo3D != STEREO_VR) {
      hr = LoadValueInt("Player", "Stereo3D", &stereo3D);
      if (hr != S_OK)
         stereo3D = STEREO_OFF;
      //This should only happens once for users that have used the old VPVR version
      if (stereo3D == STEREO_VR) {
         stereo3D = 0;
         SaveValueInt("Player", "Stereo3D", stereo3D);
         stereo3D = STEREO_VR;
      }
   }

   SendMessage(GetDlgItem(IDC_3D_STEREO).GetHwnd(), CB_ADDSTRING, 0, (LPARAM)"Disabled");
   SendMessage(GetDlgItem(IDC_3D_STEREO).GetHwnd(), CB_ADDSTRING, 0, (LPARAM)"TB (Top / Bottom)");
   SendMessage(GetDlgItem(IDC_3D_STEREO).GetHwnd(), CB_ADDSTRING, 0, (LPARAM)"Interlaced (e.g. LG TVs)");
   SendMessage(GetDlgItem(IDC_3D_STEREO).GetHwnd(), CB_ADDSTRING, 0, (LPARAM)"SBS (Side by Side)");
#ifdef ENABLE_VR
   SendMessage(GetDlgItem(IDC_3D_STEREO).GetHwnd(), CB_ADDSTRING, 0, (LPARAM)"Steam VR");
#endif
   SendMessage(GetDlgItem(IDC_3D_STEREO).GetHwnd(), CB_SETCURSEL, stereo3D, 0);

   const bool stereo3DY = LoadValueBoolWithDefault("Player", "Stereo3DYAxis", false);
   SendMessage(GetDlgItem(IDC_3D_STEREO_Y).GetHwnd(), BM_SETCHECK, stereo3DY ? BST_CHECKED : BST_UNCHECKED, 0);

   const float stereo3DOfs = LoadValueFloatWithDefault("Player", "Stereo3DOffset", 0.f);
   sprintf_s(tmp, 256, "%f", stereo3DOfs);
   SetDlgItemTextA(IDC_3D_STEREO_OFS, tmp);

   int disableVRPreview = LoadValueFloatWithDefault("Player", "VRPreviewDisabled", 0);
   SendMessage(GetDlgItem(IDC_VR_DISABLE_PREVIEW).GetHwnd(), BM_SETCHECK, disableVRPreview ? BST_CHECKED : BST_UNCHECKED, 0);

   float vrSlope = LoadValueFloatWithDefault("Player", "VRSlope", 6.5f);
   sprintf_s(tmp, 256, "%0.2f", vrSlope);
   SetDlgItemTextA(IDC_VR_SLOPE, tmp);

   float vrOrientation = LoadValueFloatWithDefault("Player", "VROrientation", 0.0f);
   sprintf_s(tmp, 256, "%0.1f", vrOrientation);
   SetDlgItemTextA(IDC_3D_VR_ORIENTATION, tmp);

   float vrX = LoadValueFloatWithDefault("Player", "VRTableX", 0.0f);
   sprintf_s(tmp, 256, "%0.1f", vrX);
   SetDlgItemTextA(IDC_VR_OFFSET_X, tmp);

   float vrY = LoadValueFloatWithDefault("Player", "VRTableY", 0.0f);
   sprintf_s(tmp, 256, "%0.1f", vrY);
   SetDlgItemTextA(IDC_VR_OFFSET_Y, tmp);

   float vrZ = LoadValueFloatWithDefault("Player", "VRTableZ", 80.0f);
   sprintf_s(tmp, 256, "%0.1f", vrZ);
   SetDlgItemTextA(IDC_VR_OFFSET_Z, tmp);

   const float stereo3DMS = LoadValueFloatWithDefault("Player", "Stereo3DMaxSeparation", 0.03f);
   sprintf_s(tmp, 256, "%f", stereo3DMS);
   SetDlgItemTextA(IDC_3D_STEREO_MS, tmp);

   const float stereo3DZPD = LoadValueFloatWithDefault("Player", "Stereo3DZPD", 0.5f);
   sprintf_s(tmp, 256, "%f", stereo3DZPD);
   SetDlgItemTextA(IDC_3D_STEREO_ZPD, tmp);

   int bamHeadtracking = LoadValueIntWithDefault("Player", "BAMheadTracking", 0);
   SendMessage(GetDlgItem(IDC_HEADTRACKING).GetHwnd(), BM_SETCHECK, bamHeadtracking ? BST_CHECKED : BST_UNCHECKED, 0);

   const int disableDWM = LoadValueIntWithDefault("Player", "DisableDWM", 0);
   SendMessage(GetDlgItem(IDC_DISABLE_DWM).GetHwnd(), BM_SETCHECK, (disableDWM != 0) ? BST_CHECKED : BST_UNCHECKED, 0);

   const int nvidiaApi = LoadValueIntWithDefault("Player", "UseNVidiaAPI", 0);
   SendMessage(GetDlgItem(IDC_USE_NVIDIA_API_CHECK).GetHwnd(), BM_SETCHECK, (nvidiaApi != 0) ? BST_CHECKED : BST_UNCHECKED, 0);

   const int bloomOff = LoadValueIntWithDefault("Player", "ForceBloomOff", 0);
   SendMessage(GetDlgItem(IDC_BLOOM_OFF).GetHwnd(), BM_SETCHECK, (bloomOff != 0) ? BST_CHECKED : BST_UNCHECKED, 0);

   const bool forceAniso = (LoadValueIntWithDefault("Player", "ForceAnisotropicFiltering", 1) != 0);
   SendMessage(GetDlgItem(IDC_FORCE_ANISO).GetHwnd(), BM_SETCHECK, forceAniso ? BST_CHECKED : BST_UNCHECKED, 0);

   const bool compressTextures = (LoadValueIntWithDefault("Player", "CompressTextures", 0) != 0);
   SendMessage(GetDlgItem(IDC_TEX_COMPRESS).GetHwnd(), BM_SETCHECK, compressTextures ? BST_CHECKED : BST_UNCHECKED, 0);

   const bool softwareVP = (LoadValueIntWithDefault("Player", "SoftwareVertexProcessing", 0) != 0);
   SendMessage(GetDlgItem(IDC_SOFTWARE_VP).GetHwnd(), BM_SETCHECK, softwareVP ? BST_CHECKED : BST_UNCHECKED, 0);

   const bool video10bit = (LoadValueIntWithDefault("Player", "Render10Bit", 0) != 0);
   SendMessage(GetDlgItem(IDC_10BIT_VIDEO).GetHwnd(), BM_SETCHECK, video10bit ? BST_CHECKED : BST_UNCHECKED, 0);

   const int depthcur = LoadValueIntWithDefault("Player", "ColorDepth", 32);

   const int refreshrate = LoadValueIntWithDefault("Player", "RefreshRate", 0);

   int display;
   hr = LoadValueInt("Player", "Display", &display);
   std::vector<DisplayConfig> displays;
   getDisplayList(displays);

   if ((hr != S_OK) || ((int)displays.size() <= display) || (display<-1))
      display = -1;

   SendMessage(GetDlgItem(IDC_DISPLAY_ID).GetHwnd(), CB_RESETCONTENT, 0, 0);

   for (std::vector<DisplayConfig>::iterator dispConf = displays.begin(); dispConf != displays.end(); dispConf++)
   {
      if (display == -1 && dispConf->isPrimary)
         display = dispConf->display;
      char displayName[256];
      sprintf_s(displayName, "Display %d%s %dx%d %s", dispConf->display + 1, (dispConf->isPrimary) ? "*" : "", dispConf->width, dispConf->height, dispConf->GPU_Name);
      SendMessage(GetDlgItem(IDC_DISPLAY_ID).GetHwnd(), CB_ADDSTRING, 0, (LPARAM)displayName);
   }
   SendMessage(GetDlgItem(IDC_DISPLAY_ID).GetHwnd(), CB_SETCURSEL, display, 0);

   const bool fullscreen = LoadValueBoolWithDefault("Player", "FullScreen", IsWindows10_1803orAbove());

   const int widthcur = LoadValueIntWithDefault("Player", "Width", fullscreen ? DEFAULT_PLAYER_FS_WIDTH : DEFAULT_PLAYER_WIDTH);

   const int heightcur = LoadValueIntWithDefault("Player", "Height", widthcur * 9 / 16);

   if (fullscreen)
   {
      SendMessage(hwndDlg, GET_FULLSCREENMODES, widthcur << 16 | refreshrate | (display << 10), heightcur << 16 | depthcur);//Assumtion is that there are less than 64 displays and the refresh rate is lower than 1024
      SendMessage(GetDlgItem(IDC_FULLSCREEN).GetHwnd(), BM_SETCHECK, BST_CHECKED, 0);
   }
   else
   {
      SendMessage(hwndDlg, GET_WINDOW_MODES, widthcur, heightcur);
      SendMessage(GetDlgItem(IDC_FULLSCREEN).GetHwnd(), BM_SETCHECK, BST_UNCHECKED, 0);
   }

   const int alphaRampsAccuracy = LoadValueIntWithDefault("Player", "AlphaRampAccuracy", 10);
   const HWND hwndARASlider = GetDlgItem(IDC_ARASlider).GetHwnd();
   SendMessage(hwndARASlider, TBM_SETRANGE, fTrue, MAKELONG(0, 10));
   SendMessage(hwndARASlider, TBM_SETTICFREQ, 1, 0);
   SendMessage(hwndARASlider, TBM_SETLINESIZE, 0, 1);
   SendMessage(hwndARASlider, TBM_SETPAGESIZE, 0, 1);
   SendMessage(hwndARASlider, TBM_SETTHUMBLENGTH, 5, 0);
   SendMessage(hwndARASlider, TBM_SETPOS, TRUE, alphaRampsAccuracy);

   const int ballStretchMode = LoadValueIntWithDefault("Player", "BallStretchMode", 0);

   switch (ballStretchMode)
   {
   case 0:  SendMessage(GetDlgItem(IDC_StretchNo).GetHwnd(), BM_SETCHECK, BST_CHECKED, 0);      break;
   case 1:  SendMessage(GetDlgItem(IDC_StretchYes).GetHwnd(), BM_SETCHECK, BST_CHECKED, 0);     break;
   case 2:  SendMessage(GetDlgItem(IDC_StretchMonitor).GetHwnd(), BM_SETCHECK, BST_CHECKED, 0); break;
   default: SendMessage(GetDlgItem(IDC_StretchNo).GetHwnd(), BM_SETCHECK, BST_CHECKED, 0);      break;
   }

   // set selected Monitors
   // Monitors: 4:3, 16:9, 16:10, 21:10
   const int selected = LoadValueIntWithDefault("Player", "BallStretchMonitor", 1); // assume 16:9 as standard
   SendMessage(GetDlgItem(IDC_MonitorCombo).GetHwnd(), CB_ADDSTRING, 0, (LPARAM)"4:3");
   SendMessage(GetDlgItem(IDC_MonitorCombo).GetHwnd(), CB_ADDSTRING, 0, (LPARAM)"16:9");
   SendMessage(GetDlgItem(IDC_MonitorCombo).GetHwnd(), CB_ADDSTRING, 0, (LPARAM)"16:10");
   SendMessage(GetDlgItem(IDC_MonitorCombo).GetHwnd(), CB_ADDSTRING, 0, (LPARAM)"21:10");
   SendMessage(GetDlgItem(IDC_MonitorCombo).GetHwnd(), CB_ADDSTRING, 0, (LPARAM)"3:4 (R)");
   SendMessage(GetDlgItem(IDC_MonitorCombo).GetHwnd(), CB_ADDSTRING, 0, (LPARAM)"9:16 (R)");
   SendMessage(GetDlgItem(IDC_MonitorCombo).GetHwnd(), CB_ADDSTRING, 0, (LPARAM)"10:16 (R)");
   SendMessage(GetDlgItem(IDC_MonitorCombo).GetHwnd(), CB_ADDSTRING, 0, (LPARAM)"10:21 (R)");
   SendMessage(GetDlgItem(IDC_MonitorCombo).GetHwnd(), CB_SETCURSEL, selected, 0);

   //AMD Debugging
   SendMessage(GetDlgItem(IDC_COMBO_TEXTURE).GetHwnd(), CB_ADDSTRING, 0, (LPARAM)"RGB 8");
   SendMessage(GetDlgItem(IDC_COMBO_TEXTURE).GetHwnd(), CB_ADDSTRING, 0, (LPARAM)"RGBA 8");
   SendMessage(GetDlgItem(IDC_COMBO_TEXTURE).GetHwnd(), CB_ADDSTRING, 0, (LPARAM)"RGB 16F");
   SendMessage(GetDlgItem(IDC_COMBO_TEXTURE).GetHwnd(), CB_ADDSTRING, 0, (LPARAM)"RGBA 16F");
   int textureModeVR = LoadValueIntWithDefault("Player", "textureModeVR", 1);
   SendMessage(GetDlgItem(IDC_COMBO_TEXTURE).GetHwnd(), CB_SETCURSEL, textureModeVR, 0);

   SendMessage(GetDlgItem(IDC_COMBO_BLIT).GetHwnd(), CB_ADDSTRING, 0, (LPARAM)"Blit");
   SendMessage(GetDlgItem(IDC_COMBO_BLIT).GetHwnd(), CB_ADDSTRING, 0, (LPARAM)"BlitNamed");
   SendMessage(GetDlgItem(IDC_COMBO_BLIT).GetHwnd(), CB_ADDSTRING, 0, (LPARAM)"Shader");
   int blitModeVR = LoadValueIntWithDefault("Player", "blitModeVR", 0);
   SendMessage(GetDlgItem(IDC_COMBO_BLIT).GetHwnd(), CB_SETCURSEL, blitModeVR, 0);

   updateStereoVisibility(stereo3D);

   return TRUE;
}

INT_PTR VideoOptionsDialog::DialogProc(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
   switch (uMsg)
   {
   case GET_WINDOW_MODES:
   {
      size_t indexcur = -1;
      size_t indx = -1;
      int widthcur = (int)wParam, heightcur = (int)lParam;

      SendMessage(GetHwnd(), RESET_SIZELIST_CONTENT, 0, 0);
      HWND hwndList = GetDlgItem(IDC_SIZELIST).GetHwnd();
      //indx = SendMessage(hwndList, LB_GETCURSEL, 0L, 0L);
      //if (indx == LB_ERR)
      //  indx = 0;

      const size_t csize = sizeof(rgwindowsize) / sizeof(int);
      int screenwidth;
      int screenheight;
      int x, y;
      const int display = (int)SendMessage(GetDlgItem(IDC_DISPLAY_ID).GetHwnd(), CB_GETCURSEL, 0, 0);
      getDisplaySetupByID(display, x, y, screenwidth, screenheight);

      //if (indx != -1)
      //  indexcur = indx;

      allVideoModes.clear();
      size_t cnt = 0;

      // test video modes first on list

      // add some (windowed) portrait play modes

      // 16:10 aspect ratio resolutions: 1280*800, 1440*900, 1680*1050, 1920*1200 and 2560*1600
      // 16:9 aspect ratio resolutions:  1280*720, 1366*768, 1600*900, 1920*1080, 2560*1440 and 3840*2160
      // 21:9 aspect ratio resolution:   3440*1440
      // 4:3  aspect ratio resolutions:  1280*1024
      const unsigned int num_portrait_modes = 15;
      const int portrait_modes_width[num_portrait_modes] = { 720, 720, 1024, 768, 800, 900, 900,1050,1050,1080,1200,1440,1440,1600,2160 };
      const int portrait_modes_height[num_portrait_modes] = { 1024,1280, 1280,1366,1280,1440,1600,1600,1680,1920,1920,2560,3440,2560,3840 };

      for (unsigned int i = 0; i < num_portrait_modes; ++i)
         if ((portrait_modes_width[i] <= screenwidth) && (portrait_modes_height[i] <= screenheight))
         {
            VideoMode mode;
            mode.width = portrait_modes_width[i];
            mode.height = portrait_modes_height[i];
            mode.depth = 0;
            mode.refreshrate = 0;

            allVideoModes.push_back(mode);
            if (heightcur > widthcur)
               if ((portrait_modes_width[i] == widthcur) && (portrait_modes_height[i] == heightcur))
                  indx = i;
            cnt++;
         }

      // add landscape play modes

      for (size_t i = 0; i < csize; ++i)
      {
         const int xsize = rgwindowsize[i];
         if ((xsize <= screenwidth) && ((xsize * 3 / 4) <= screenheight))
         {
            if ((xsize == widthcur) && ((xsize * 3 / 4) == heightcur))
               indx = i + cnt;

            VideoMode mode;
            mode.width = xsize;
            mode.height = xsize * 3 / 4;
            mode.depth = 0;
            mode.refreshrate = 0;

            allVideoModes.push_back(mode);
         }
      }

      FillVideoModesList(allVideoModes);

      // set up windowed fullscreen mode
      VideoMode mode;
      mode.width = screenwidth;
      mode.height = screenheight;
      mode.depth = 0;
      mode.refreshrate = 0;
      allVideoModes.push_back(mode);

      char szT[128];
      //if (indexcur == -1)
      //  indexcur = indx;

      if (mode.height < mode.width) // landscape
      {
         sprintf_s(szT, "%d x %d (Windowed Fullscreen)", mode.width, mode.height);
         SendMessage(hwndList, LB_ADDSTRING, 0, (LPARAM)szT);
         if (indx == -1)
            indexcur = SendMessage(hwndList, LB_GETCOUNT, 0, 0) - 1;
         else
            indexcur = indx;
      }
      else { // portrait
         if ((indx == -1) || (indx < num_portrait_modes))
         {
            indexcur = indx;
            if (cnt > 0)
            {
               char szTx[128];
               SendMessage(hwndList, LB_GETTEXT, cnt - 1, (LPARAM)szTx);
               SendMessage(hwndList, LB_DELETESTRING, cnt - 1, 0L);

               if (cnt - 1 < num_portrait_modes)
               {
                  mode.width = portrait_modes_width[cnt - 1];
                  mode.height = portrait_modes_height[cnt - 1];

                  if ((mode.height == screenheight) && (mode.width == screenwidth))
                     sprintf_s(szT, "%d x %d (Windowed Fullscreen)", mode.width, mode.height);
                  else
                     sprintf_s(szT, "%d x %d", mode.width, mode.height);
               }
               else {
                  memset(&szTx, '\x0', sizeof(szTx));
                  strcpy_s(szT, szTx);
               }

               SendMessage(hwndList, LB_INSERTSTRING, cnt - 1, (LPARAM)szT);
            } // end if cnt > 0
         } // end if indx
      } // end if else mode height < width

      SendMessage(hwndList, LB_SETCURSEL, (indexcur != -1) ? indexcur : 0, 0);
      break;
   } // end case GET_WINDOW_MODES
   case GET_FULLSCREENMODES:
   {
      HWND hwndList = GetDlgItem(IDC_SIZELIST).GetHwnd();
      HWND hwndDisplay = GetDlgItem(IDC_DISPLAY_ID).GetHwnd();
      int display = (int)SendMessage(hwndDisplay, CB_GETCURSEL, 0, 0);
      EnumerateDisplayModes(display, allVideoModes);

      VideoMode curSelMode;
      curSelMode.width = (int)wParam >> 16;
      curSelMode.height = (int)lParam >> 16;
      curSelMode.depth = (int)lParam & 0xffff;
      curSelMode.refreshrate = (int)wParam & 0xffff;

      FillVideoModesList(allVideoModes, &curSelMode);

      if (SendMessage(hwndList, LB_GETCURSEL, 0, 0) == -1)
         SendMessage(hwndList, LB_SETCURSEL, 0, 0);
      break;
   }
   case RESET_SIZELIST_CONTENT:
   {
      HWND hwndList = GetDlgItem(IDC_SIZELIST).GetHwnd();
      SendMessage(hwndList, LB_RESETCONTENT, 0, 0);
      break;
   }
   }

   return DialogProcDefault(uMsg, wParam, lParam);
}

BOOL VideoOptionsDialog::OnCommand(WPARAM wParam, LPARAM lParam)
{
   UNREFERENCED_PARAMETER(lParam);

   switch (LOWORD(wParam))
   {
   case IDC_DEFAULTS:
   {
      ResetVideoPreferences(0);
      break;
   }
   case IDC_DEFAULTS_LOW:
   {
      ResetVideoPreferences(1);
      break;
   }
   case IDC_DEFAULTS_HIGH:
   {
      ResetVideoPreferences(2);
      break;
   }
   case IDC_OVERWRITE_BALL_IMAGE_CHECK:
   {
      const bool overwriteEnabled = IsDlgButtonChecked(IDC_OVERWRITE_BALL_IMAGE_CHECK) == BST_CHECKED;
      if (overwriteEnabled)
      {
         ::EnableWindow(GetDlgItem(IDC_BROWSE_BALL_IMAGE).GetHwnd(), TRUE);
         ::EnableWindow(GetDlgItem(IDC_BROWSE_BALL_DECAL).GetHwnd(), TRUE);
         ::EnableWindow(GetDlgItem(IDC_BALL_IMAGE_EDIT).GetHwnd(), TRUE);
         ::EnableWindow(GetDlgItem(IDC_BALL_DECAL_EDIT).GetHwnd(), TRUE);
      }
      else
      {
         ::EnableWindow(GetDlgItem(IDC_BROWSE_BALL_IMAGE).GetHwnd(), FALSE);
         ::EnableWindow(GetDlgItem(IDC_BROWSE_BALL_DECAL).GetHwnd(), FALSE);
         ::EnableWindow(GetDlgItem(IDC_BALL_IMAGE_EDIT).GetHwnd(), FALSE);
         ::EnableWindow(GetDlgItem(IDC_BALL_DECAL_EDIT).GetHwnd(), FALSE);
      }
      break;
   }
   case IDC_BROWSE_BALL_IMAGE:
   {
      char szFileName[MAXSTRING];
      szFileName[0] = '\0';

      OPENFILENAME ofn;
      ZeroMemory(&ofn, sizeof(OPENFILENAME));
      ofn.lStructSize = sizeof(OPENFILENAME);
      ofn.hInstance = g_hinst;
      ofn.hwndOwner = g_pvp->m_hwnd;
      // TEXT
      ofn.lpstrFilter = "Bitmap, JPEG, PNG, TGA, EXR, HDR Files (.bmp/.jpg/.png/.tga/.exr/.hdr)\0*.bmp;*.jpg;*.jpeg;*.png;*.tga;*.exr;*.hdr\0";
      ofn.lpstrFile = szFileName;
      ofn.nMaxFile = MAXSTRING;
      ofn.lpstrDefExt = "png";
      ofn.Flags = OFN_OVERWRITEPROMPT | OFN_HIDEREADONLY;
      const int ret = GetOpenFileName(&ofn);
      if (!ret)
         break;
      SetDlgItemText(IDC_BALL_IMAGE_EDIT, szFileName);

      break;
   }
   case IDC_BROWSE_BALL_DECAL:
   {
      char szFileName[MAXSTRING];
      szFileName[0] = '\0';

      OPENFILENAME ofn;
      ZeroMemory(&ofn, sizeof(OPENFILENAME));
      ofn.lStructSize = sizeof(OPENFILENAME);
      ofn.hInstance = g_hinst;
      ofn.hwndOwner = g_pvp->m_hwnd;
      // TEXT
      ofn.lpstrFilter = "Bitmap, JPEG, PNG, TGA, EXR, HDR Files (.bmp/.jpg/.png/.tga/.exr/.hdr)\0*.bmp;*.jpg;*.jpeg;*.png;*.tga;*.exr;*.hdr\0";
      ofn.lpstrFile = szFileName;
      ofn.nMaxFile = MAXSTRING;
      ofn.lpstrDefExt = "png";
      ofn.Flags = OFN_OVERWRITEPROMPT | OFN_HIDEREADONLY;
      const int ret = GetOpenFileName(&ofn);
      if (!ret)
         break;
      SetDlgItemText(IDC_BALL_DECAL_EDIT, szFileName);
      break;
   }

   case IDC_DISPLAY_ID:
   {
      const size_t checked = SendDlgItemMessage(IDC_FULLSCREEN, BM_GETCHECK, 0, 0);
      size_t index = (int)SendMessage(GetDlgItem(IDC_SIZELIST).GetHwnd(), LB_GETCURSEL, 0, 0);
      if (allVideoModes.size() == 0) {
         HWND hwndList = GetDlgItem(IDC_SIZELIST).GetHwnd();
         HWND hwndDisplay = GetDlgItem(IDC_DISPLAY_ID).GetHwnd();
         int display = SendMessage(hwndDisplay, CB_GETCURSEL, 0, 0);
         EnumerateDisplayModes(display, allVideoModes);

      }
      if (allVideoModes.size() > index) {
         VideoMode * pvm = &allVideoModes[index];
         if (checked)
            SendMessage(GET_FULLSCREENMODES, (pvm->width << 16) | pvm->refreshrate, (pvm->height << 16) | pvm->depth);
         else
            SendMessage(GET_WINDOW_MODES, pvm->width, pvm->height);

      }
      else
         SendMessage(checked ? GET_FULLSCREENMODES : GET_WINDOW_MODES, 0, 0);
      break;
   }
   case IDC_FULLSCREEN:
   {
      const size_t checked = SendDlgItemMessage(IDC_FULLSCREEN, BM_GETCHECK, 0, 0);
      SendMessage(checked ? GET_FULLSCREENMODES : GET_WINDOW_MODES, 0, 0);
      break;
   }
   case IDC_3D_STEREO:
   {
      int stereo3D = SendMessage(GetDlgItem(IDC_3D_STEREO).GetHwnd(), CB_GETCURSEL, 0, 0);
      if (stereo3D>=0) updateStereoVisibility(stereo3D);
      break;
   }
   default:
      return FALSE;
   }
   return TRUE;
}

void VideoOptionsDialog::OnOK()
{
   BOOL nothing = 0;
   size_t fullscreen = SendMessage(GetDlgItem(IDC_FULLSCREEN).GetHwnd(), BM_GETCHECK, 0, 0);
   SaveValueInt("Player", "FullScreen", fullscreen);

   size_t index = SendMessage(GetDlgItem(IDC_SIZELIST).GetHwnd(), LB_GETCURSEL, 0, 0);
   VideoMode* pvm = &allVideoModes[index];
   SaveValueInt("Player", "Width", pvm->width);
   SaveValueInt("Player", "Height", pvm->height);
   if (fullscreen)
   {
      SaveValueInt("Player", "ColorDepth", pvm->depth);
      SaveValueInt("Player", "RefreshRate", pvm->refreshrate);
   }
   size_t display = SendMessage(GetDlgItem(IDC_DISPLAY_ID).GetHwnd(), CB_GETCURSEL, 0, 0);
   SaveValueInt("Player", "Display", display);

   size_t video10bit = SendMessage(GetDlgItem(IDC_10BIT_VIDEO).GetHwnd(), BM_GETCHECK, 0, 0);
   SaveValueInt("Player", "Render10Bit", video10bit);

   int maxTexDim = 0;
   if (SendMessage(GetDlgItem(IDC_Tex3072).GetHwnd(), BM_GETCHECK, 0, 0) == BST_CHECKED)
      maxTexDim = 3072;
   if (SendMessage(GetDlgItem(IDC_Tex1024).GetHwnd(), BM_GETCHECK, 0, 0) == BST_CHECKED)
      maxTexDim = 1024;
   if (SendMessage(GetDlgItem(IDC_Tex2048).GetHwnd(), BM_GETCHECK, 0, 0) == BST_CHECKED)
      maxTexDim = 2048;
   SaveValueInt("Player", "MaxTexDimension", maxTexDim);

   size_t reflection = SendMessage(GetDlgItem(IDC_GLOBAL_REFLECTION_CHECK).GetHwnd(), BM_GETCHECK, 0, 0);
   SaveValueInt("Player", "BallReflection", reflection);

   size_t trail = SendMessage(GetDlgItem(IDC_GLOBAL_TRAIL_CHECK).GetHwnd(), BM_GETCHECK, 0, 0);
   SaveValueInt("Player", "BallTrail", trail);

   int vsync = GetDlgItemInt(IDC_ADAPTIVE_VSYNC, nothing, TRUE);
   SaveValueInt("Player", "AdaptiveVSync", vsync);

   int maxPrerenderedFrames = GetDlgItemInt(IDC_MAX_PRE_FRAMES, nothing, TRUE);
   SaveValueInt("Player", "MaxPrerenderedFrames", maxPrerenderedFrames);

   CString tmpStr;
   tmpStr = GetDlgItemTextA(IDC_CORRECTION_X);
   SaveValueString("Player", "BallCorrectionX", tmpStr.c_str());

   tmpStr = GetDlgItemTextA(IDC_CORRECTION_Y);
   SaveValueString("Player", "BallCorrectionY", tmpStr.c_str());

   tmpStr = GetDlgItemTextA(IDC_DN_LONGITUDE);
   SaveValueString("Player", "Longitude", tmpStr.c_str());

   tmpStr = GetDlgItemTextA(IDC_DN_LATITUDE);
   SaveValueString("Player", "Latitude", tmpStr.c_str());

   tmpStr = GetDlgItemTextA(IDC_NUDGE_STRENGTH);
   SaveValueString("Player", "NudgeStrength", tmpStr.c_str());

   size_t fxaa = SendMessage(GetDlgItem(IDC_FXAACB).GetHwnd(), CB_GETCURSEL, 0, 0);
   if (fxaa == LB_ERR)
      fxaa = 0;
   SaveValueInt("Player", "FXAA", fxaa);

   size_t scaleFX_DMD = SendMessage(GetDlgItem(IDC_SCALE_FX_DMD).GetHwnd(), BM_GETCHECK, 0, 0);
   SaveValueInt("Player", "ScaleFXDMD", scaleFX_DMD);

   size_t BGSet = SendMessage(GetDlgItem(IDC_BG_SET).GetHwnd(), BM_GETCHECK, 0, 0);
   SaveValueInt("Player", "BGSet", BGSet);

   size_t useAA = SendMessage(GetDlgItem(IDC_AA_ALL_TABLES).GetHwnd(), BM_GETCHECK, 0, 0);
   SaveValueInt("Player", "USEAA", useAA);

   size_t useDN = SendMessage(GetDlgItem(IDC_DYNAMIC_DN).GetHwnd(), BM_GETCHECK, 0, 0);
   SaveValueInt("Player", "DynamicDayNight", useDN);

   size_t useAO = SendMessage(GetDlgItem(IDC_DYNAMIC_AO).GetHwnd(), BM_GETCHECK, 0, 0);
   SaveValueInt("Player", "DisableAO", useAO);

   useAO = SendMessage(GetDlgItem(IDC_ENABLE_AO).GetHwnd(), BM_GETCHECK, 0, 0) ? 0 : 1; // inverted logic
   SaveValueInt("Player", "DisableAO", useAO);

   size_t ssreflection = SendMessage(GetDlgItem(IDC_GLOBAL_SSREFLECTION_CHECK).GetHwnd(), BM_GETCHECK, 0, 0);
   SaveValueInt("Player", "SSRefl", ssreflection);

   size_t pfreflection = SendMessage(GetDlgItem(IDC_GLOBAL_PFREFLECTION_CHECK).GetHwnd(), BM_GETCHECK, 0, 0);
   SaveValueInt("Player", "PFRefl", pfreflection);

   size_t stereo3D = SendMessage(GetDlgItem(IDC_3D_STEREO).GetHwnd(), CB_GETCURSEL, 0, 0);
   if (stereo3D == LB_ERR)
      stereo3D = STEREO_OFF;
#ifdef ENABLE_VR
   if ((stereo3D == STEREO_VR) && !vr::VR_IsRuntimeInstalled()) {
      MessageBox("SteamVR Runtime not found. Please install SteamVR.", "SteamVR", MB_OK);
      stereo3D = STEREO_OFF;
   }
#endif
   if (stereo3D != STEREO_VR)
      SaveValueInt("Player", "Stereo3D", stereo3D);
   SaveValueInt("Player", "Stereo3DVR", stereo3D);

   //AMD Debugging
   size_t textureModeVR = SendMessage(GetDlgItem(IDC_COMBO_TEXTURE).GetHwnd(), CB_GETCURSEL, 0, 0);
   SaveValueInt("Player", "textureModeVR", textureModeVR);

   size_t blitModeVR = SendMessage(GetDlgItem(IDC_COMBO_BLIT).GetHwnd(), CB_GETCURSEL, 0, 0);
   SaveValueInt("Player", "blitModeVR", blitModeVR);

   SaveValueInt("Player", "Stereo3DEnabled", stereo3D);

   size_t stereo3DY = SendMessage(GetDlgItem(IDC_3D_STEREO_Y).GetHwnd(), BM_GETCHECK, 0, 0);
   SaveValueInt("Player", "Stereo3DYAxis", stereo3DY);

   size_t forceAniso = SendMessage(GetDlgItem(IDC_FORCE_ANISO).GetHwnd(), BM_GETCHECK, 0, 0);
   SaveValueInt("Player", "ForceAnisotropicFiltering", forceAniso);

   size_t texCompress = SendMessage(GetDlgItem(IDC_TEX_COMPRESS).GetHwnd(), BM_GETCHECK, 0, 0);
   SaveValueInt("Player", "CompressTextures", texCompress);

   size_t softwareVP = SendMessage(GetDlgItem(IDC_SOFTWARE_VP).GetHwnd(), BM_GETCHECK, 0, 0);
   SaveValueBool("Player", "SoftwareVertexProcessing", softwareVP != 0);

   size_t alphaRampsAccuracy = SendMessage(GetDlgItem(IDC_ARASlider).GetHwnd(), TBM_GETPOS, 0, 0);
   SaveValueInt("Player", "AlphaRampAccuracy", alphaRampsAccuracy);

   size_t disableVRPreview = SendMessage(GetDlgItem(IDC_VR_DISABLE_PREVIEW).GetHwnd(), BM_GETCHECK, 0, 0);
   SaveValueInt("Player", "VRPreviewDisabled", disableVRPreview);

   tmpStr = GetDlgItemTextA(IDC_3D_STEREO_OFS);
   SaveValueString("Player", "Stereo3DOffset", tmpStr.c_str());

   tmpStr = GetDlgItemTextA(IDC_VR_SLOPE);
   SaveValueString("Player", "VRSlope", tmpStr.c_str());

   tmpStr = GetDlgItemTextA(IDC_3D_VR_ORIENTATION);
   SaveValueString("Player", "VROrientation", tmpStr.c_str());

   tmpStr = GetDlgItemTextA(IDC_VR_OFFSET_X);
   SaveValueString("Player", "VRTableX", tmpStr.c_str());

   tmpStr = GetDlgItemTextA(IDC_VR_OFFSET_Y);
   SaveValueString("Player", "VRTableY", tmpStr.c_str());

   tmpStr = GetDlgItemTextA(IDC_VR_OFFSET_Z);
   SaveValueString("Player", "VRTableZ", tmpStr.c_str());

   tmpStr = GetDlgItemTextA(IDC_3D_STEREO_MS);
   SaveValueString("Player", "Stereo3DMaxSeparation", tmpStr.c_str());

   tmpStr = GetDlgItemTextA(IDC_3D_STEREO_ZPD);
   SaveValueString("Player", "Stereo3DZPD", tmpStr.c_str());

   size_t bamHeadtracking = SendMessage(GetDlgItem(IDC_HEADTRACKING).GetHwnd(), BM_GETCHECK, 0, 0);
   SaveValueInt("Player", "BAMheadTracking", bamHeadtracking);

   size_t disableDWM = SendMessage(GetDlgItem(IDC_DISABLE_DWM).GetHwnd(), BM_GETCHECK, 0, 0);
   SaveValueInt("Player", "DisableDWM", disableDWM);

   size_t nvidiaApi = SendMessage(GetDlgItem(IDC_USE_NVIDIA_API_CHECK).GetHwnd(), BM_GETCHECK, 0, 0);
   SaveValueInt("Player", "UseNVidiaAPI", nvidiaApi);

   size_t bloomOff = SendMessage(GetDlgItem(IDC_BLOOM_OFF).GetHwnd(), BM_GETCHECK, 0, 0);
   SaveValueInt("Player", "ForceBloomOff", bloomOff);

   int ballStretchMode = 0;
   if (SendMessage(GetDlgItem(IDC_StretchYes).GetHwnd(), BM_GETCHECK, 0, 0) == BST_CHECKED)
      ballStretchMode = 1;
   if (SendMessage(GetDlgItem(IDC_StretchMonitor).GetHwnd(), BM_GETCHECK, 0, 0) == BST_CHECKED)
      ballStretchMode = 2;
   SaveValueInt("Player", "BallStretchMode", ballStretchMode);

   // get selected Monitors
   // Monitors: 4:3, 16:9, 16:10, 21:10
   size_t selected = SendMessage(GetDlgItem(IDC_MonitorCombo).GetHwnd(), CB_GETCURSEL, 0, 0);
   if (selected == LB_ERR)
      selected = 1; // assume a 16:9 Monitor as standard
   SaveValueInt("Player", "BallStretchMonitor", selected);

   const bool overwriteEnabled = IsDlgButtonChecked(IDC_OVERWRITE_BALL_IMAGE_CHECK) == BST_CHECKED;
   if (overwriteEnabled)
   {
      SaveValueInt("Player", "OverwriteBallImage", 1);
      tmpStr = GetDlgItemText(IDC_BALL_IMAGE_EDIT);
      SaveValueString("Player", "BallImage", tmpStr.c_str());
      tmpStr = GetDlgItemText(IDC_BALL_DECAL_EDIT);
      SaveValueString("Player", "DecalImage", tmpStr.c_str());
   }
   else
      SaveValueInt("Player", "OverwriteBallImage", 0);

   CDialog::OnOK();

}

void VideoOptionsDialog::OnClose()
{
   SendMessage(RESET_SIZELIST_CONTENT, 0, 0);
   CDialog::OnClose();
}
