/*
 * MainScreen.h
 *
 *  Created on: 2 авг. 2023 г.
 *      Author: zhabakaba
 */

#ifndef MAINWINDOW_H_
#define MAINWINDOW_H_

#include <gtkmm.h>
#include <string>
#include <fstream>
#include <iostream>
#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/imgcodecs.hpp>
#include <pango/pango.h>

#include "md5.h"
#include "CVImageArea.h"
#include "SqlConnector.h"
#include "OpenObjectDialog.h"
#include "ExposureChangeDialog.h"
#include "AngaraAboutScreen.h"
#include "ConfigureDialog.h"
#include "ImageDialog.h"
#include "Dispatcher.h"
#include "CommonUtils.h"

typedef enum _SystemMode
{
	Unknown = 0,
	Init,
	Work,
	Settings,
	Error,
	Archive,
	Log
} eSysMode;

typedef enum _ReadyStatus
{
	Ready = 0,
	Wait,
	Err
} ReadyStatus;

class MainWindow : public Gtk::Window, public DispatcherClient
{
public:
	MainWindow(void) {};
	MainWindow(BaseObjectType* cobject, const Glib::RefPtr<Gtk::Builder>& refGlade);
	virtual ~MainWindow(void);

	static gboolean MsgProcess(gpointer data);

private:
	MySQLConnector db;

	Glib::RefPtr<Gtk::Builder> m_refGlade;

	// Global
	Gtk::Button *m_pButtonAbout = nullptr;
	Gtk::Button *m_pButtonExit = nullptr;
	Gtk::Button *m_pButtonArchive = nullptr;
	Gtk::Button *m_pButtonLog = nullptr;
	Gtk::Button *m_pButtonError = nullptr;
	Gtk::Stack *m_pStack = nullptr;
	Gtk::Label *m_pLabelStatus = nullptr;
	Gtk::Box *m_pBoxStatus = nullptr;

	// Init
	Gtk::Button *m_pInitButtonEnter = nullptr;
	Gtk::Entry *m_pInitEntryLogin = nullptr;
	Gtk::Entry *m_pInitEntryPassword = nullptr;
	Gtk::Label *m_pInitLabelError = nullptr;

	// Work
	Gtk::Button *m_pWorkButtonSetting = nullptr;
	Gtk::Button *m_pWorkButtonStart = nullptr;
	Gtk::Button *m_pWorkButtonSaveObject[2] = {nullptr, nullptr};
	Gtk::Button *m_pWorkButtonObjectId[2] = {nullptr, nullptr};
	Gtk::Button *m_pWorkButtonConfiguration = nullptr;
	Gtk::Button *m_pWorkButtonRole = nullptr;
	Gtk::Button *m_pWorkButtonReady = nullptr;
	Gtk::Button *m_pWorkButtonClose = nullptr;
	Gtk::Label *m_pWorkLabelConfiguration = nullptr;
	Gtk::Label *m_pWorkLabelRole = nullptr;
	Gtk::Label *m_pWorkLabelReady = nullptr;

	Gtk::Label *m_pWorkLabelStatus[2] = {nullptr, nullptr};

	int m_pImageCount[2] = {0, 0};

	long long int m_pObjectId[2] = {0, 0};

	bool m_pIsObjectSaved[2] = {true, true};

	CVImageArea *m_pWorkDrawingArea[2][8];

	cv::Mat m_pWorkImages[2][8];

	Gtk::Button *m_pWorkButtons[2][8];

	Gtk::Frame *m_pWorkFrames[2][8];

	bool m_pObjectStatus[2][8];

	// Settings
	Gtk::Button *m_pSettingsButtonWork = nullptr;
	Gtk::Button *m_pSettingsButtonExposure = nullptr;
	Gtk::Button *m_pSettingsButtonConfiguration = nullptr;

	ExposureChangeDialog *m_pExpChangeDlg = nullptr;

	// Log
	Gtk::Button *m_pLogButtonBack = nullptr;
	Gtk::TextView *m_pLogTextView = nullptr;
	Gtk::ComboBox *m_pLogComboBox = nullptr;

	// Error
	Gtk::Button *m_pErrorButtonBack = nullptr;
	Gtk::Button *m_pErrorButtonReset = nullptr;
	Gtk::TreeView *m_pErrorTreeView = nullptr;

	// Archive
	Gtk::Button *m_pArchiveButtonBack = nullptr;
	Gtk::TreeView *m_pArchiveTreeView= nullptr;

	// Other
	ReadyStatus m_pCurrentReadyStatus;

	int m_pCurrentImageWidth;
	int m_pCurrentImageHeight;

	void SetStatus(std::string strStatus);
protected:
	virtual void MsgProc(DispMessage sMessage);

	void SwitchMode(eSysMode);

	// TODO в будущем явно понадобится
	// Запуск с текущей конфигурацией
	bool LoadConfiguration(void);

	// Установление связи и синхронизация конфигурации с контроллером
	bool ConnectController(void);

	void on_window_hide(void);

	void on_button_about(void);

	void on_button_quit(void);

	void init_on_button_enter(void);

	void init_on_entry_changed(void);

	bool init_check_login(string login);

	bool init_check_password(string password);

	void work_on_button_object_save(int post);

	void work_on_button_configuration(void);

	void work_on_click_image(int post, int index);

	void work_change_id(int);

	void work_switch_image_status(int post, int index, bool status);

	void work_switch_object_status(void);

	bool work_check_object_status(int post);

	bool work_check_object_id(int post);

	bool work_check_is_object_saved(void);

	void work_set_image(int post, int index);

	void work_hide_object(int post);

	void work_show_object(int post);

	void work_save_object(int post);

	void work_on_button_start(void);

	void work_change_ready_status(void);

	void work_change_user(void);

	void work_on_close_button(void);

	void work_load_images(void);

	void settings_on_button_exposure_change(void);

	void log_update_log(string path);

	void log_on_changed_combo_box(void);

	std::string m_strLastStatus = "";

	bool m_bCtrlCommStatus = false;
	bool m_bKrnlCommStatus = false;

};

MainWindow* Load_Glade(std::string strFile);

#endif /* MAINWINDOW_H_ */
